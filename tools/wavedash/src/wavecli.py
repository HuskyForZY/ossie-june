#! /usr/bin/env python

## Copyright 2005, 2006, 2007, 2008 Virginia Polytechnic Institute and State University
##
## This file is part of the OSSIE Waveform Application Visualization Environment
##
## WaveDash is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## WaveDash is distributed in the hope that it will be useful, but WITHOUT ANY
## WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with OSSIE WaveDash; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
import sys
from optparse import OptionParser
from wavedash.src.WavedashController import Controller
import wavedash.src.WaveformModel 

[ SYSTEM_WAVEFORM, INSTANCE_WAVEFORM, RUNNING_WAVEFORM ] = [1, 2, 3]

def main(argv):
    try:
        ctrlr = Controller(False)
        ctrlr.createWidgetContainer()
        ctrlr.CORBAutils.init_CORBA(False)
        ctrlr.buildModel()  
    except:
         print 'Something went wrong connecting to the naming service or building the model.'
    else:
    
        parser = OptionParser()
        parser.add_option('-i', '--interactive', action='store_true', help='Run wavecli.py in interactive mode')
        parser.add_option('--install', dest='waveformToInstall', help='install a waveform')
        parser.add_option('--start', dest='waveformToStart', help='start a waveform')
        parser.add_option('--stop', dest='waveformToStop', help='stop a waveform')
        parser.add_option('--uninstall', dest='waveformToUninstall', help='uninstall a waveform')
        parser.add_option('--listSystemWaveforms', action='store_true', help='list all waveforms installed on the system')
        parser.add_option('--listWaveformInstances', dest='waveformToList', help='list all running instances of of a particular waveform')
        parser.add_option('--listAllInstances', action='store_true', help='list all waveform instances currently running')
        parser.add_option('--configure', dest='waveformToConfigure', help='Configure a waveform. You must supply a waveform, a component, a property, and a value for that property.')
        parser.add_option('--query', dest='waveformToQuery', help='Query a waveform. You must supply a waveform and a component')
        (options, args) = parser.parse_args()
        
        if options.waveformToInstall:
            if options.waveformToInstall in [wave.getName() for wave in ctrlr.model.getSystemWaveforms()]:
                installed = ctrlr.installWaveform(options.waveformToInstall, False)
                if installed:
                    print 'Successfully installed ' + installed
            else:
                print options.waveformToInstall + ' not found.'
        
        elif options.waveformToStart:
            ctrlr.startWaveform(options.waveformToStart)
        
        elif options.waveformToStop:
            ctrlr.stopWaveform(options.waveformToStop)
        
        elif options.waveformToUninstall:
            ctrlr.uninstallWaveform(options.waveformToUninstall)
        
        elif options.listSystemWaveforms:
            list = ctrlr.model.getSystemWaveforms()
            for waveform in list:
                print str(waveform.getName())
        
        elif options.waveformToList:
            list = ctrlr.model.getInstanceWaveforms(options.waveformToList)
            if list != None:
                for waveform in list:
                    print str(waveform.getName())
        
        elif options.listAllInstances:
            systemWaveforms = ctrlr.model.getSystemWaveforms()
            for waveform in systemWaveforms:
                instances = ctrlr.model.getInstanceWaveforms(waveform.getName())
                if instances != None:
                    for instance in instances:
                        print str(instance.getName())
        
        elif options.waveformToConfigure:
            waveName = options.waveformToConfigure
            compName = args[0]
            propName = args[1]
            newValue = args[2]
            wave = ctrlr.model.getWaveform(waveName, INSTANCE_WAVEFORM)
            comp = wave.getComponent(compName)
            prop = comp.findPropertyByName(propName)
            
            trueValues = ["t", "true", "yes", "on", "1"]
            
            propType = prop.getType()[1]
            if propType in ['short', 'ushort', 'int', 'uint'] :
                ctrlr.model.configure(waveName, compName, propName, int(newValue))
            elif propType in ['long', 'ulong']:
                ctrlr.model.configure(waveName, compName, propName, long(newValue))
            elif propType in ['double', 'float']:
                ctrlr.model.configure(waveName, compName, propName, float(newValue))
            elif propType in ['char', 'string']:
                ctrlr.model.configure(waveName, compName, propName, str(newValue))
            elif propType in ['boolean']:
                if str(newValue).lower() in trueValues:
                    ctrlr.model.configure(waveName, compName, propName, True)
                else:
                    ctrlr.model.configure(waveName, compName, propName, False)
                    
        
        elif options.waveformToQuery:
            waveName = options.waveformToQuery
            compName = args[0] 
            wave = ctrlr.model.getWaveform(waveName, INSTANCE_WAVEFORM)
            comp = wave.getComponent(compName)
            
            #if no property name is provided then look up and print all of the 
            #component's properties
            if len(args) == 1:
                for prop in comp.getAllProperties():
                #list = []
                #list = ctrlr.CORBAutils.query(waveName, args[0], list)
                #for l in list:
                 #   prop = comp.findPropertyByID(l.id)
                    print 'name:  ', prop.getName()
                    print 'id:    ', prop.getID()
                    print 'type:  ', prop.getType()[1]
                    print 'value: ', prop.getValue()
                    print
           
            #if a property name was provided, only print that property's value        
            elif len(args) == 2:
                prop = comp.findPropertyByName(args[1])
                if prop:
                    print prop.getValue()
                else:
                    print 'Error: could not find property: ', args[1]
                
        elif options.interactive:
            interactiveMode(ctrlr)
        else:
            print 'No option provided. Exiting.'
            
def interactiveMode(ctrlr):
        availableWaveforms = ctrlr.model.getSystemWaveforms()
        numAvailableWaveforms = len(availableWaveforms)
        print 'Found ' + str(numAvailableWaveforms) + ' available applications\n'
        i = 1
        for w in availableWaveforms:
            print str(i) + '. ' + str(w.getName()) 
            i = i + 1
            
        print
        print 'n - Install application number n'
        print 'x - exit'
        
        valid = False
        while not valid:
            choice = raw_input('Selection: ')
            try:
                if choice == 'x':
                    valid = True
                elif int(choice) > 0 and int(choice) <= numAvailableWaveforms:
                    valid = True
                else:
                    print 'Invalid selection.'
                    valid = False
            except:
                print 'Invalid selection.'
                valid = False
    
        if choice == 'x':
            pass
        else:
            choice = int(choice)
            instance = ctrlr.installWaveform(availableWaveforms[choice -1].getName(), False)
            if instance:
                print 'Application installed. Ready to run or uninstall'
                while not (choice == 's' or choice == 'u'):
                    print 's - start application'
                    print 'u - uninstall application'
                    choice = raw_input('Selection: ')
                    if choice == 's':
                        ctrlr.startWaveform(instance)
                        while choice != 't':
                            print 't - stop application'
                            choice = raw_input('Selection: ')
                            if choice == 't':
                                ctrlr.stopWaveform(instance)
                    elif choice == 'u':
                        ctrlr.uninstallWaveform(instance)
                        interactiveMode(ctrlr)
        
def usage():
    print "usage here"

if __name__ == '__main__':
    main(sys.argv[1:]) 
    