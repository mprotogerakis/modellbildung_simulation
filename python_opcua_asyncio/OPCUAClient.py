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
    def __init__(self, callback):
        self.callback = callback

    def datachange_notification(self, node: Node, val, data):
        """
        Callback for asyncua Subscription.
        This method will be called when the Client received a data change message from the Server.
        """
        _logger.info('datachange_notification %r %s', node, val)
        self.callback(node, val)


class OPCUAClient:
    def __init__(self, config_file: str):
        self.config_file = Path(config_file)
        self._load_or_create_config()
        self.client = None

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
            'trust_store_path': 'certificates/trusted/certs',
            'username': 'testuser',
            'password': 'testuser_pass'
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

    async def some_old_stuff(self):
        try:    
                _logger.info("Root node is: %r", self.client.nodes.root)
                _logger.info("Objects node is: %r", self.client.nodes.objects)
                _logger.info("Children of root are: %r", await self.client.nodes.root.get_children())
                #uri = "urn:nodered:NodeOPCUA-Server"
                #idx = await client.get_namespace_index(uri)
                #_logger.info("index of our namespace is %s", idx)
                
                
                #while True: #This is to keep connection open
                 #   await asyncio.sleep(1)

        except ua.UaError as exp:
            _logger.error(exp)

    async def setup(self):
        _logger.info("Doing the setup")
        url = self.config['url']
        self.client = Client(url=url)
        self.client.set_user(self.config['username'])
        self.client.set_password(self.config['password'])
        cert_base = Path(self.config['cert_base'])
        cert = cert_base / self.config['cert']
        private_key = cert_base / self.config['private_key']
        client_app_uri = self.config['client_app_uri'] # should match the certificate
        server_certificate = self.config['server_certificate']
        try:
                await self.setup_certificate()
                self.client.application_uri = client_app_uri

                await self.client.set_security(
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
                self.client.certificate_validator = validator
                await self.client.connect()
        except ua.UaError as exp:
            _logger.error(exp)

    async def disconnect(self):
        await self.client.disconnect()

    async def subscribe(self, node_string: str, callback):
        try:
            handler = SubscriptionHandler(callback)
            subscription = await self.client.create_subscription(500, handler)
            node = self.client.get_node(node_string)
            await subscription.subscribe_data_change(node)
            _logger.info(f"Subscribed to {node_string}")
        except ua.UaError as exp:
            _logger.error(exp)

    async def write_Double_value(self, node_string: str, value):
        try:
            node = self.client.get_node(node_string)
            variant = ua.Variant(value, ua.VariantType.Double)  # Assuming value is a double
            await node.write_value(variant)
            _logger.info(f"Wrote value {value} to {node_string}")
        except ua.UaError as exp:
            _logger.error(exp)
    
    async def write_Boolean_value(self, node_string: str, value):
        try:
            node = self.client.get_node(node_string)
            variant = ua.Variant(value, ua.VariantType.Boolean)  # Assuming value is a double
            await node.write_value(variant)
            _logger.info(f"Wrote value {value} to {node_string}")
        except ua.UaError as exp:
            _logger.error(exp)

