from WavedashController import Controller
from BaseHTTPServer import HTTPServer
import WaveformModel

class WavedashServer(HTTPServer):
    
    def initializeWavedashController(self):
        self.ctrlr = Controller(False)
        self.ctrlr.createWidgetContainer()
        self.ctrlr.CORBAutils.init_CORBA(False)
        self.ctrlr.buildModel()
        
    def handleRequest(self, request):
        requestSplit = request.split()
        
        command = requestSplit[0]
        if command == "getWaveformSadFile":
            self.getWaveformSadFile(requestSplit[1])
        else:
            methodToRun = getattr(self.ctrlr, requestSplit[0])
        
            if methodToRun.__name__ == "installWaveform":
                methodToRun(requestSplit[1], True)
            else:
                methodToRun(requestSplit[1])
            
    def getWaveformSadFile(self, waveformName):
        waveform = self.ctrlr.model.getWaveform(waveformName, 1)
        return waveform.sadFile
    def getWaveformDasFile(self, waveformName):
        waveform = self.ctrlr.model.getWaveform(waveformName, 1)
        return waveform.dasFile
    
    def configure(self, params):
        waveName = params['waveform']
        compName = params['component']
        propName = params['property']
        newValue = params['value']
        wave = self.ctrlr.model.getWaveform(waveName, WaveformModel.INSTANCE_WAVEFORM)
        comp = wave.getComponent(compName)
        prop = comp.findPropertyByName(propName)
            
        propType = prop.getType()[1]
        if propType in ['short', 'ushort', 'int', 'uint'] :
            self.ctrlr.model.configure(waveName, compName, propName, int(newValue))
        elif propType in ['long', 'ulong']:
            self.ctrlr.model.configure(waveName, compName, propName, long(newValue))
        elif propType in ['double', 'float']:
            self.ctrlr.model.configure(waveName, compName, propName, float(newValue))
        elif propType in ['char', 'string']:
            self.ctrlr.model.configure(waveName, compName, propName, str(newValue))
        elif propType in ['boolean']:
            if str(newValue).lower() in trueValues:
                self.ctrlr.model.configure(waveName, compName, propName, True)
            else:
                self.ctrlr.model.configure(waveName, compName, propName, False)
                    
    def query(self, params):
        waveName = params['waveform']
        compName = params['component']
        propProvided = True
        try:
            propName = params['property']
        except KeyError:
            propProvided = False
            
        wave = self.ctrlr.model.getWaveform(waveName, WaveformModel.INSTANCE_WAVEFORM)
        comp = wave.getComponent(compName)
            
        if not propProvided:
            result = ''
            for prop in comp.getAllProperties():
                #list = []
                #list = ctrlr.CORBAutils.query(waveName, args2[0], list)
                #for l in list:
                 #   prop = comp.findPropertyByID(l.id)
                result += 'name:  ' + prop.getName() + '<br>'
                result +=  'id:    ' + prop.getID() + '<br>'
                result += 'type:  ' + prop.getType()[1] + '<br>'
                result += 'value: ' + str(prop.getValue()) + '<br>'
                result += '\n'
            return result
            
        else:
            prop = comp.findPropertyByName(propName)
            if prop:
                return prop.getValue()
            else:
                return 'Error: could not find property: ', args2[1]  
    
    def getSystemWaveforms(self):
        list = self.ctrlr.model.getSystemWaveforms()
        return list
    
    def getInstanceWaveforms(self, parent):
        list = self.ctrlr.model.getInstanceWaveforms(parent)
        return list
    
    def installWaveform(self, params):
        self.ctrlr.installWaveform(params['waveform'], True)
        
    def uninstallWaveform(self, params):
        self.ctrlr.uninstallWaveform(params['waveform'])
    
    def startWaveform(self, params):
        self.ctrlr.startWaveform((params['waveform']))
        
    def stopWaveform(self, params):
        self.ctrlr.stopWaveform((params['waveform']))
        
        
    