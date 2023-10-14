import http.server
import socketserver
import cgi
import subprocess

PORT = 8000

class VideoProcessorHandler(http.server.SimpleHTTPRequestHandler):
    def do_POST(self):
        if self.path == '/process':
            form = cgi.FieldStorage(
                fp=self.rfile,
                headers=self.headers,
                environ={'REQUEST_METHOD': 'POST'}
            )
            video_file = form['video']

            # Save the uploaded video to a file
            with open('uploaded_video.mp4', 'wb') as output_file:
                output_file.write(video_file.file.read())

            # Execute the processing script (predict.py)
            process = subprocess.Popen(['python', 'predict.py'], stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
            output, error = process.communicate()

            self.send_response(200)
            self.send_header('Content-type', 'text/plain')
            self.end_headers()
            self.wfile.write(output.encode())

        else:
            self.send_response(404)
            self.end_headers()

if __name__ == '__main__':
    with socketserver.TCPServer(("", PORT), VideoProcessorHandler) as httpd:
        print(f"Serving at port {PORT}")
        httpd.serve_forever()
