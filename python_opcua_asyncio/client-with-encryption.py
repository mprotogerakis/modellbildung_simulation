import asyncio
import logging
import sys
import socket
import json
from pathlib import Path
from cryptography.x509.oid import ExtendedKeyUsageOID
from asyncua import Client
from asyncua.crypto.security_policies import SecurityPolicyBasic256Sha256
from asyncua.crypto.cert_gen import setup_self_signed_certificate
from asyncua.crypto.validator import CertificateValidator, CertificateValidatorOptions
from asyncua.crypto.truststore import TrustStore
from asyncua import ua
import typer

# Konfigurieren Sie das Logging
logging.basicConfig(level=logging.INFO)
_logger = logging.getLogger(__name__)

app = typer.Typer()

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
            'server_certificate': 'certificate-nodered.der',
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
                _logger.info("Root node is: %r", client.nodes.root)
                _logger.info("Objects node is: %r", client.nodes.objects)

                _logger.info("Children of root are: %r", await client.nodes.root.get_children())
                uri = "urn:nodered:NodeOPCUA-Server"
                idx = await client.get_namespace_index(uri)
                _logger.info("index of our namespace is %s", idx)

                var = client.get_node("ns=0;i=17634")
                value = await var.read_data_value()
                print(value)

        except ua.UaError as exp:
            _logger.error(exp)

def override_config(config, **kwargs):
    for key, value in kwargs.items():
        if value is not None:
            config[key] = value
    return config

@app.command()
def main(
    config_file: str = typer.Option('config.json', help="Path to the configuration file"),
    cert_base: str = typer.Option(None, help="Base path for certificates"),
    cert: str = typer.Option(None, help="Certificate file name"),
    private_key: str = typer.Option(None, help="Private key file name"),
    host_name: str = typer.Option(None, help="Host name"),
    client_app_uri: str = typer.Option(None, help="Client application URI"),
    url: str = typer.Option(None, help="OPC UA server URL"),
    server_certificate: str = typer.Option(None, help="Server certificate file name"),
    use_trust_store: bool = typer.Option(None, help="Use trust store"),
    trust_store_path: str = typer.Option(None, help="Path to the trust store")
):
    opcua_client = OPCUAClient(config_file)
    opcua_client.config = override_config(
        opcua_client.config,
        cert_base=cert_base,
        cert=cert,
        private_key=private_key,
        host_name=host_name,
        client_app_uri=client_app_uri,
        url=url,
        server_certificate=server_certificate,
        use_trust_store=use_trust_store,
        trust_store_path=trust_store_path
    )
    loop = asyncio.get_event_loop()
    loop.set_debug(True)
    loop.run_until_complete(opcua_client.run())
    loop.close()

if __name__ == "__main__":
    app()