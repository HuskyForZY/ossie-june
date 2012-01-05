'''
/****************************************************************************

Copyright 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIETalk.

OSSIE OSSIETalk is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE OSSIETalk is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIETalk; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/

'''

from ossie.standardinterfaces import standardInterfaces__POA
from ossie.cf import CF__POA
import threading


#------------------------------------------------------------------
# dataIn_realChar_i class definition
#------------------------------------------------------------------
# from radio
class dataIn_realChar_i(standardInterfaces__POA.realChar):
    def __init__(self, parent, name):
        self.parent = parent
        self.name = name

    def pushPacket(self, data):
        self.parent.rx_work_mod.AddData(data)
       



#------------------------------------------------------------------
# dataOut_realChar_i class definition
#------------------------------------------------------------------
# to radio
class dataOut_realChar_i(CF__POA.Port):
    def __init__(self, parent, name):
        self.parent = parent
        self.outPorts = {}
        self.name = name
        
        self.data_buffer = []
        self.data_event = threading.Event()
        self.data_buffer_lock = threading.Lock()
        
        self.is_running = True
        self.process_thread = threading.Thread(target = self.Process)
        self.process_thread.start()

    def connectPort(self, connection, connectionId):
        port = connection._narrow(standardInterfaces__POA.realChar)
        self.outPorts[str(connectionId)] = port
        self.parent.to_radio_port_active = True

    def disconnectPort(self, connectionId):
        self.outPorts.pop(str(connectionId))
        if len(self.outPorts)==0:
            self.parent.to_radio_port_active = False

    def releasePort(self):
        # shut down the Process thread
        self.is_running = False
        self.data_event.set()

#    def send_data(self, data, metadata):
    def send_data(self, data):
        self.data_buffer_lock.acquire()
#        self.data_buffer.insert(0, (data, metadata))
        self.data_buffer.insert(0, data)
        self.data_buffer_lock.release()
        self.data_event.set()

    def Process(self):
        while self.is_running:

            self.data_event.wait()

            while len(self.data_buffer) > 0:
                self.data_buffer_lock.acquire()
                # get data and metadata from the stack
                data = self.data_buffer.pop()
                self.data_buffer_lock.release()
                
                # send the data to all outPort connections 
                for port in self.outPorts.values():
                    #print "tx packet " + str(metadata.packet_id) + \
                    #      ", len = " + str(len(data)) + \
                    #      "  app: " + str(metadata.app_id)
                    port.pushPacket(data)
            
                self.data_event.clear()


