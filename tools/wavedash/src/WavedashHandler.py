import string,cgi,time, urlparse
from os import curdir, sep
from BaseHTTPServer import BaseHTTPRequestHandler
from WavedashServer import WavedashServer

class WavedashHandler(BaseHTTPRequestHandler):
                
    def do_GET(self):
        #convert path into a dictionary of (parameter, value) pairs
        self.query = urlparse.urlparse(self.path).query
        self.query = self.query.split("&");
        self.params = {}
        for entry in self.query:
            tmp = entry.split("=")
            self.params[tmp[0]] = tmp[1]
        
        command = self.params['command']
        
        #commands that return something
        if(command.startswith("get")):
            if(command == "getWaveformSadFile"):
                file = self.server.getWaveformSadFile(self.params['waveform'])
            elif(command == "getWaveformDasFile"):
                file = self.server.getWaveformDasFile(self.params['waveform'])
            else:
                self.send_error(901, "Invalid get command")
                return
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            f = open("/sdr/dom" + file)
            self.wfile.write(f.read())
            f.close()
        elif(command == "query"):
            propertyList = self.server.query(self.params)
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            self.wfile.write(str(propertyList))
        elif(command == "listSystemWaveforms"):
            list = self.server.getSystemWaveforms()
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            for w in list:
                self.wfile.write(str(w.getName()) + '<br>')
        elif(command == "listInstanceWaveforms"):
            list = self.server.getInstanceWaveforms(self.params['waveform'])
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            for w in list:
                self.wfile.write(str(w.getName()) + '<br>')
        elif(command == "listAllInstances"):
            systemWaveforms = self.server.getSystemWaveforms()
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            for waveform in systemWaveforms:
                instances = self.server.getInstanceWaveforms(waveform.getName())
                if instances != None:
                    for instance in instances:
                        self.wfile.write(str(instance.getName()) + '<br>')
                
        #commands that perform an action on the model but don't return
        #anything
        elif(command == "configure"):
            self.server.configure(self.params)
        elif(command == "installWaveform"):
            self.server.installWaveform(self.params)
        elif(command == "startWaveform"):
            self.server.startWaveform(self.params)
        elif(command == "stopWaveform"):
            self.server.stopWaveform(self.params)
        elif(command == "uninstallWaveform"):
            self.server.uninstallWaveform(self.params)
        
        return  

def main():
    try:
        #Change port number here to run server on a different port
        #When using port 80, the WavedashHandler must be run as root
        server = WavedashServer(('', 4443), WavedashHandler)
        server.initializeWavedashController()
        print 'started httpserver...'
        server.serve_forever()
    except KeyboardInterrupt:
        print '^C received, shutting down server'
        server.socket.close()

if __name__ == '__main__':
    main()

