import socket
import json
import asyncio
from pathlib import Path
from asyncua import Client, Node, ua
from cryptography.x509.oid import ExtendedKeyUsageOID
from asyncua.crypto.security_policies import SecurityPolicyBasic256Sha256
from asyncua.crypto.cert_gen import setup_self_signed_certificate
from asyncua.crypto.validator import CertificateValidator, CertificateValidatorOptions
from asyncua.crypto.truststore import TrustStore
import logging
from asyncua import ua
# Konfigurieren Sie das Logging

logging.basicConfig(level=logging.INFO)
_logger = logging.getLogger(__name__)

class SubscriptionHandler:
    """
    The SubscriptionHandler is used to handle the data that is received for the subscription.
    """
    def datachange_notification(self, node: Node, val, data):
        """
        Callback for asyncua Subscription.
        This method will be called when the Client received a data change message from the Server.
        """
        _logger.info('datachange_notification %r %s', node, val)


class OPCUAClient:
    def __init__(self, config_file: str):
        self.config_file = Path(config_file)
        self._load_or_create_config()

    def _load_or_create_config(self):
        default_config = {
            'cert_base': str(Path(__file__).parent),
            'cert': 'my_generated_cert.der',
            'private_key': 'my_generated_private_key.pem',
            'host_name': socket.gethostname(),
            'client_app_uri': f"urn:{socket.gethostname()}:foobar:myselfsignedclient",
            'url': "opc.tcp://nodered:53880/",
            'server_certificate': 'certificate-server.der',
            'countryName': 'DE',
            'stateOrProvinceName': 'NRW',
            'localityName': 'Wuerselen',
            'organizationName': "Proto",
            'use_trust_store': False,
            'trust_store_path': 'certificates/trusted/certs'
        }

        if self.config_file.exists():
            with open(self.config_file, 'r') as file:
                self.config = json.load(file)
        else:
            self.config = default_config
            with open(self.config_file, 'w') as file:
                json.dump(self.config, file, indent=4)

    async def setup_certificate(self):
        cert_base = Path(self.config['cert_base'])
        cert = cert_base / self.config['cert']
        private_key = cert_base / self.config['private_key']
        host_name = self.config['host_name']
        client_app_uri = self.config['client_app_uri']

        await setup_self_signed_certificate(
            private_key,
            cert,
            client_app_uri,
            host_name,
            [ExtendedKeyUsageOID.CLIENT_AUTH],
            {
                'countryName': self.config['countryName'],
                'stateOrProvinceName': self.config['stateOrProvinceName'],
                'localityName': self.config['localityName'],
                'organizationName': self.config['organizationName'],
            }
        )

    async def run(self):
        cert_base = Path(self.config['cert_base'])
        cert = cert_base / self.config['cert']
        private_key = cert_base / self.config['private_key']
        url = self.config['url']
        client_app_uri = self.config['client_app_uri']
        server_certificate = self.config['server_certificate']

        await self.setup_certificate()

        client = Client(url=url)
        client.application_uri = client_app_uri

        await client.set_security(
            SecurityPolicyBasic256Sha256,
            certificate=str(cert),
            private_key=str(private_key),
            server_certificate=server_certificate
        )

        if self.config['use_trust_store']:
            trust_store = TrustStore([Path(self.config['trust_store_path'])], [])
            await trust_store.load()
            validator = CertificateValidator(CertificateValidatorOptions.TRUSTED_VALIDATION | CertificateValidatorOptions.PEER_SERVER, trust_store)
        else:
            validator = CertificateValidator(CertificateValidatorOptions.EXT_VALIDATION | CertificateValidatorOptions.PEER_SERVER)
        client.certificate_validator = validator

        try:
            async with client:
                handler = SubscriptionHandler()
                #_logger.info("Root node is: %r", client.nodes.root)
                #_logger.info("Objects node is: %r", client.nodes.objects)

                #_logger.info("Children of root are: %r", await client.nodes.root.get_children())
                #uri = "urn:nodered:NodeOPCUA-Server"
                #idx = await client.get_namespace_index(uri)
                #_logger.info("index of our namespace is %s", idx)
                subscription = await client.create_subscription(500, handler)
                nodes = [
                    client.get_node("ns=1;s=A"),
                    client.get_node("ns=1;s=B"),
                    client.get_node("ns=1;s=C")
                ]
                await subscription.subscribe_data_change(nodes)
                # We let the subscription run for ten seconds
                await asyncio.sleep(100)
                # We delete the subscription (this un-subscribes from the data changes of the two variables).
                # This is optional since closing the connection will also delete all subscriptions.
                await subscription.delete()
                # After one second we exit the Client context manager - this will close the connection.
                await asyncio.sleep(1)

        except ua.UaError as exp:
            _logger.error(exp)