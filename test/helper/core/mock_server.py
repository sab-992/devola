import os
import sys
import ssl
import signal
import tempfile
import threading
import subprocess
from http.server import HTTPServer, BaseHTTPRequestHandler
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--ready-fd", type=int, default=None)
args = parser.parse_args()

PORT = 8000

tmp = tempfile.mkdtemp()
key_file = f"{tmp}/key.pem"
cert_file = f"{tmp}/cert.pem"
subprocess.run(["openssl", "req", "-x509", "-newkey", "rsa:2048", "-nodes", "-keyout", key_file, "-out", cert_file, "-days", "1", "-subj", "/CN=localhost"], check=True, capture_output=True)

class Handler(BaseHTTPRequestHandler):
    protocol_version = "HTTP/1.1"

    def do_GET(self):
        if self.path == "/":
            self.send_response(200, "OK")
            self.end_headers()

    def log_message(self, format, *args):
        print(f"[{self.address_string()}] {format % args}")

ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
ctx.load_cert_chain(cert_file, key_file)
ctx.set_alpn_protocols(["h2", "http/1.1"])

server = HTTPServer(("0.0.0.0", PORT), Handler)
server.socket = ctx.wrap_socket(server.socket, server_side=True)

signal.signal(signal.SIGTERM, lambda *_: server.shutdown())

if args.ready_fd is not None:
    try:
        os.write(args.ready_fd, b"READY")
        os.close(args.ready_fd)
    except OSError as e:
        print(f"fd {args.ready_fd} error: {e}", file=sys.stderr, flush=True)

print(f"Listening on https://localhost:{PORT}", flush=True)
server_thread = threading.Thread(target=server.serve_forever)
server_thread.start()

signal.signal(signal.SIGTERM, lambda *_: server.shutdown())
signal.pause()