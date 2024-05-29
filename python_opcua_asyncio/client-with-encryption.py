# based on https://github.com/FreeOpcUa/opcua-asyncio/blob/master/examples/client-with-encryption.py

# Zertifikat des Servers mit UAExpert runterladen und hier als certificate-nodered-example.der einbinden.

# Client Zertifikate mit shell Skript generieren.

import asyncio
import logging
import sys
import socket
from pathlib import Path
from cryptography.x509.oid import ExtendedKeyUsageOID

# Fügen Sie das übergeordnete Verzeichnis zum Pfad hinzu, um asyncua zu importieren
sys.path.insert(0, "..")

# Importieren Sie die benötigten Module aus asyncua
from asyncua import Client
from asyncua.crypto.security_policies import SecurityPolicyBasic256Sha256
from asyncua.crypto.cert_gen import setup_self_signed_certificate
from asyncua.crypto.validator import CertificateValidator, CertificateValidatorOptions
from asyncua.crypto.truststore import TrustStore
from asyncua import ua

# Konfigurieren Sie das Logging
logging.basicConfig(level=logging.INFO)
_logger = logging.getLogger(__name__)

# Konstante für die Nutzung eines Trust Stores
USE_TRUST_STORE = False

# Indizes und Dateipfade für Zertifikate und private Schlüssel
cert_idx = 4
cert_base = Path(__file__).parent
cert = Path(cert_base / f"my_generated_cert.der")
private_key = Path(cert_base / f"my_generated_private_key.pem")

# Definieren Sie die asynchrone Hauptaufgabe
async def task(loop):
    # Bestimmen Sie den Hostnamen und die Client-Anwendungs-URI
    host_name = socket.gethostname()
    client_app_uri = f"urn:{host_name}:foobar:myselfsignedclient"
    url = "opc.tcp://nodered:53880/"

    # Erstellen Sie ein selbstsigniertes Zertifikat
    await setup_self_signed_certificate(private_key,
                                        cert,
                                        client_app_uri,
                                        host_name,
                                        [ExtendedKeyUsageOID.CLIENT_AUTH],
                                        {
                                            'countryName': 'DE',
                                            'stateOrProvinceName': 'NRW',
                                            'localityName': 'Wuerselen',
                                            'organizationName': "Proto",
                                        })

    # Initialisieren Sie den OPC UA Client
    client = Client(url=url)
    client.application_uri = client_app_uri
    
    # Konfigurieren Sie die Sicherheitsrichtlinien und Zertifikate
    await client.set_security(
        SecurityPolicyBasic256Sha256,
        certificate=str(cert),
        private_key=str(private_key),
        server_certificate="certificate-nodered.der"
    )

    # Konfigurieren Sie den Zertifikat-Validator basierend auf der Nutzung des Trust Stores
    if USE_TRUST_STORE:
        trust_store = TrustStore([Path('examples') / 'certificates' / 'trusted' / 'certs'], [])
        await trust_store.load()
        validator = CertificateValidator(CertificateValidatorOptions.TRUSTED_VALIDATION | CertificateValidatorOptions.PEER_SERVER, trust_store)
    else:
        validator = CertificateValidator(CertificateValidatorOptions.EXT_VALIDATION | CertificateValidatorOptions.PEER_SERVER)
    client.certificate_validator = validator

    try:
        # Verbindungsaufbau mit dem OPC UA Server
        async with client:
            _logger.info("Root node is: %r", client.nodes.root)
            _logger.info("Objects node is: %r", client.nodes.objects)

            # Auflisten der Kinder des Wurzelknotens
            _logger.info("Children of root are: %r", await client.nodes.root.get_children())
            uri = "urn:nodered:NodeOPCUA-Server"
            idx = await client.get_namespace_index(uri)
            _logger.info("index of our namespace is %s", idx)

            # Lesen Sie einen spezifischen Knotenwert
            var = client.get_node("ns=0;i=17634")
            value = await var.read_data_value()
            print(value)

    except ua.UaError as exp:
        _logger.error(exp)

# Hauptfunktion zur Ausführung der asynchronen Aufgabe
def main():
    loop = asyncio.get_event_loop()
    loop.set_debug(True)
    loop.run_until_complete(task(loop))
    loop.close()

# Starten der Hauptfunktion, wenn das Skript direkt ausgeführt wird
if __name__ == "__main__":
    main()