#! /usr/bin/env python

'''
/****************************************************************************

Copyright 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE OSSIETalk.

OSSIE OSSIETalk is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE OSSIETalk is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE OSSIETalk; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/

'''


from omniORB import CORBA
from omniORB import URI
import CosNaming
from ossie.standardinterfaces import standardInterfaces__POA
from ossie.custominterfaces import customInterfaces__POA
from ossie.cf import CF, CF__POA
import sys

#from omniORB import CORBA
#from omniORB import URI
#import CosNaming
#from ossie.cf import CF, CF__POA
#import sys

import threading
import time        # primarily availble for time.sleep() statements

import wx          # gui stuff
import wx_inits    # my frame init code: specific to this application
import WorkModules
import port_impl

#-------------------------------------------------------------
# OSSIETalk_i class definition (main component class)
#-------------------------------------------------------------
class OSSIETalk_i(CF__POA.Resource):
    def __init__(self, prnt_orb, uuid, label, poa):
        CF._objref_Resource.__init__(self._this())
        print "OSSIETalk_i __init__: " + label
        self.naming_service_name = label
        self.poa = poa

        self.prnt_orb = prnt_orb

        self._declare_ports()

        self.talk_flag = False

        self.propertySet = []

        self.prntAppReferenced = False
        self.txWorkModule_created = False
        self.rxWorkModule_created = False

        self.tx_work_mod = None
        self.rx_work_mod = None


    def _declare_ports(self):
        '''declare my 2 provides ports and 2 uses ports'''

        # provides ports:
        self.from_CVSD_port_servant = port_impl.dataIn_realChar_i(
                                                              self, "from_CVSD")
        self.from_CVSD_port_var = self.from_CVSD_port_servant._this()

        self.from_radio_port_servant = port_impl.dataIn_realChar_i(
                                                          self, "from_radio")
        self.from_radio_port_var = self.from_radio_port_servant._this()

        # uses ports
        self.to_radio_port_servant =port_impl.dataOut_realChar_i(
                                                          self, "to_radio")
        self.to_radio_port_var = self.to_radio_port_servant._this()
        self.to_radio_port_active = False

        self.to_CVSD_port_servant = port_impl.dataOut_realChar_i(
                                                         self, "to_CVSD")
        self.to_CVSD_port_var = self.to_CVSD_port_servant._this()
        self.to_CVSD_port_active = False


    def start(self):
        print "OSSIETalk start called"
 
    def stop(self):
        print "OSSIETalk stop called"
        
    def getPort(self, id):
        if str(id) == "from_CVSD":
            return self.from_CVSD_port_var
        if str(id) == "to_radio":
            return self.to_radio_port_var
        if str(id) == "from_radio":
            return self.from_radio_port_var
        if str(id) == "to_CVSD":
            return self.to_CVSD_port_var

        return None  #port not found in available ports list
        
    def initialize(self):
        print "OSSIETalk initialize called"
    
    def configure(self, props):
        ''' The configure method is called twice by the framework:
        once to read the default properties in the component.prf
        file, and once to read the component instance properties
        storred in the waveform.sad file.  This method should be
        called before the start method.  This method is where
        the properties are read in by the component.  
        ''' 
       
        print "OSSIETalk configure called"
        
        if not self.prntAppReferenced:
            self.prnt_app = app
            self.parentAppReferenced = True

        # make sure that only one WorkModule thread is started, 
        # even if configure method is called more than once    


        if not self.txWorkModule_created:
            self.tx_work_mod = WorkModules.txWorkClass(self)
            self.txWorkModule_created = True   
 
        # make sure that only one WorkModule thread is started, 
        # even if configure method is called more than once    
        if not self.rxWorkModule_created:
            self.rx_work_mod = WorkModules.rxWorkClass(self)
            self.rxWorkModule_created = True 

    def query(self, props):
        return self.propertySet
    
    def releaseObject(self):
        # release the main work module
        if self.txWorkModule_created:
            self.tx_work_mod.Release()
        if self.rxWorkModule_created:
            self.rx_work_mod.Release()
 
        # release the main process threads for the ports
        self.to_radio_port_servant.releasePort()
                
        # deactivate the ports
        iid0 = self.poa.reference_to_id(self.from_CVSD_port_var)
        iid1 = self.poa.reference_to_id(self.from_radio_port_var)
        oid0 = self.poa.reference_to_id(self.to_radio_port_var)
        oid1 = self.poa.reference_to_id(self.to_CVSD_port_var)

        self.poa.deactivate_object(iid0)
        self.poa.deactivate_object(iid1)
        self.poa.deactivate_object(oid0)
        self.poa.deactivate_object(oid1)




#-------------------------------------------------------------------
# ORB_Init class definition
#-------------------------------------------------------------------
class ORB_Init:
    def __init__(self, uuid, label):
        # initialize the orb
        self.orb = CORBA.ORB_init()
        
        

def ORB_start_fun():
    orb_ref = orb

    # get the POA
    obj_poa = orb_ref.orb.resolve_initial_references("RootPOA")
    poaManager = obj_poa._get_the_POAManager()
    poaManager.activate()
        
    ns_obj = orb_ref.orb.resolve_initial_references("NameService")
    rootContext = ns_obj._narrow(CosNaming.NamingContext)
        
    # create the main component object
    orb_ref.OSSIETalk_Obj = OSSIETalk_i(orb_ref, uuid, label, obj_poa)
    OSSIETalk_var = orb_ref.OSSIETalk_Obj._this()
        
    name = URI.stringToName(label)
    rootContext.rebind(name, OSSIETalk_var)

    orb_ref.orb.run()


#-------------------------------------------------------------------
# wx.App for the GUI
#-------------------------------------------------------------------
class App(wx.App):
    def OnInit(self):
        self.frame = wx_inits.MainFrame(orb)
        self.frame.Show()
        self.SetTopWindow(self.frame)

        return True


class my_threads:
    def __init__(self):

        # start the orb that has already been initialized in a thread
        self.orb_thread = threading.Thread(target=ORB_start_fun)
        self.orb_thread.start()

        self.app_thread = threading.Thread(target=app.MainLoop)
        self.app_thread.start()
         
        
#-------------------------------------------------------------------
# Code run when this file is executed
#-------------------------------------------------------------------
if __name__ == "__main__":
    if len(sys.argv) != 3:
        print sys.argv[0] + " <id> <usage name> "
    
    uuid = str(sys.argv[1])
    label = str(sys.argv[2])
    
    print "Identifier - " + uuid + "  Label - " + label
    
    # initialize the orb, but do not start it
    orb = ORB_Init(uuid, label)
    
    # initialize the wx stuff
    app = App()

    # start up my threads
    threads_running = my_threads() 

