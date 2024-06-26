import asyncio

import typer

from OPCUAClient import OPCUAClient

import logging
logging.basicConfig(level=logging.INFO)
_logger = logging.getLogger(__name__)

def data_change_callback(node, val):
    _logger.info('Data change callback: %r %s', node, val)


app = typer.Typer()

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
    loop.run_until_complete(opcua_client.setup())
    loop.run_until_complete(opcua_client.subscribe("ns=1;s=A", data_change_callback))
    loop.run_until_complete(opcua_client.subscribe("ns=1;s=B", data_change_callback))
    loop.run_until_complete(opcua_client.subscribe("ns=1;s=C", data_change_callback))
    loop.run_until_complete(opcua_client.write_Boolean_value("ns=1;s=A", True))
    loop.run_until_complete(opcua_client.write_Boolean_value("ns=1;s=A", False))
    loop.run_until_complete(opcua_client.write_Boolean_value("ns=1;s=B", True))
    
    loop.run_forever()

    loop.run_until_complete(opcua_client.disconnect())
    loop.close()

if __name__ == "__main__":
    app()