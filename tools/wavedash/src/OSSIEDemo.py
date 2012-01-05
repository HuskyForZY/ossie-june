from wavedash.src.WavedashController import Controller
import os


def main():
    
    #These first four lines build the backend and initialize the controller. 
    #The false parameter here...
    ctrlr = Controller(False)
    ctrlr.createWidgetContainer() 
    ctrlr.CORBAutils.init_CORBA(False)
    #                            ... and here tell the controller not to use 
    #a GUI and to redirect any errors to the console.
    ctrlr.buildModel()
    
    #The rest of this program demonstrates the main capabilities of
    #the Wavedash Controller and Model backend. It shows to how install,
    #start, query, configure, stop, and uninstall a waveform, using raw_input()
    #calls to allow the user to follow what the program is doing with 
    #ALF, a graphical waveform debugging tool.
    
    raw_input('Welcome to the ossie demo.\n\nPlease start the ALF Graphical Waveform Debugger and then press enter to install the ossie_demo waveform.')
    #The installWaveform method takes a waveform name and a boolean
    #to indicate whether or not to start the waveform after
    #installing.
    #It returns the name of the instance that gets installed
    #or False if there was an error.
    instance = ctrlr.installWaveform('ossie_demo', False)
    os.system("clear")
    
    raw_input('ossie_demo waveform installed.\n\nRefresh ALF to see. Open a plot on the Channel\'s output port then press enter to start the waveform.')
    #The startWaveform method takes the name of a running waveform instance
    ctrlr.startWaveform(instance)
    os.system("clear")
    
    raw_input('Waveform started.\n\nPress enter to double rate.')
    #To retrieve information about a waveform, you can consult
    #the controller's model. The model stores information about
    #waveforms available on the system and waveforms that are currently
    #running. The getWaveform method takes the name of a waveform
    #and an integer: 1 for a system waveform, 2 for an instance waveform
    wave = ctrlr.model.getWaveform(instance, 2)
    #Once you have retrieved the waveform you can get its components by name
    comp = wave.getComponent('TxDemo1')
    #Once you have retrieved a component you can get its properties by name
    prop = comp.findPropertyByName('packet_delay_ms')
    #Once you have a property you can get its current value
    currentRate = int(prop.getValue())
    ctrlr.model.configure(instance, 'TxDemo1', 'packet_delay_ms', int(.5 * currentRate))
    os.system("clear")
    
    raw_input('Rate doubled. \n\nSwitch to I/Q plotting and press enter to rotate the constellation.')
    #You can change property values by calling configure. You
    #need to specify the waveform instance name, component name,
    #property name, and a correclty casted value
    ctrlr.model.configure(instance, 'ChannelDemo1', 'phase_offset', int(45))
    os.system("clear")
    
    raw_input('Constellation rotated. \n\nPress enter to stop waveform.')
    #Stop works just like start. It takes the name of a running 
    #instance waveform
    ctrlr.stopWaveform(instance)
    os.system("clear")
    
    raw_input('Waveform stopped. \n\nPress enter to uninstall.')
    #Uninstall works just like stop and start. I takes the name
    #of a running instance waveform.
    ctrlr.uninstallWaveform(instance)
    os.system("clear")
    print 'Thank you for using OSSIE.'
    
    

if __name__ == '__main__':
    main() 