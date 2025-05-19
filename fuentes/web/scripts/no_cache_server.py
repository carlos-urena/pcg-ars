## servidor sencillo en HTTP que le dice a los navegadores que no usen nunca las copias 
## que tengan en sus caches de los archivos servidos por este servidor.
## además, lanza el servidor en la carpeta ../public_html y escucha en el puerto 8000

from http.server import HTTPServer, SimpleHTTPRequestHandler
import os 

class NoCacheHTTPRequestHandler( SimpleHTTPRequestHandler ):
    
    def __init__(self, *args, **kwargs):
        # Set the directory to serve from ../public_html
        super().__init__(*args, directory=os.path.abspath("../public_html"), **kwargs)

    def end_headers(self):
        self.send_no_cache_headers()
        super().end_headers()

    def send_no_cache_headers(self):
        self.send_header("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0")
        self.send_header("Pragma", "no-cache")
        self.send_header("Expires", "0")

if __name__ == '__main__':
    port = 8000
    server_address = ('', port)
    httpd = HTTPServer( server_address, NoCacheHTTPRequestHandler )
    print(f"Serving on port {port} with no-cache headers...")
    httpd.serve_forever()
