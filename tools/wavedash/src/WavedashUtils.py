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

from ossie.cf import CF
from omniORB import CORBA
import sys
import CosNaming
import WaveDev.wavedev.importResource as importResource
import wx
import xml.dom.minidom
from xml.dom.minidom import Node
from namingserviceDialog import NamingserviceDialog

[NON_FATAL, FATAL, INFO] = [1,2,3]
[SUCCESS, FAILURE] = [True, False]
LOCAL_HOST = '127.0.0.1'

def showMessage(msg, sev, enableGUI = True):
    if(not enableGUI):
        print msg
        return
    if (sev == FATAL or sev == NON_FATAL ):
        dlg = wx.MessageDialog(None, msg, 'Error', wx.OK | wx.ICON_ERROR)
    elif sev == INFO: 
        dlg = wx.MessageDialog(None, msg, 'Info', wx.OK | wx.ICON_INFORMATION)
        
    try:
       dlg.ShowModal()
    finally:
       dlg.Destroy()
    if sev == FATAL:
        sys.exit()
    return

class ConfigureWidget(wx.Dialog):
    """ This utility class launches a dialog box to configure the min and max values of
    a slider or spinctrl"""
    def __init__(self, parent, title, propCurVal):
        wx.Dialog.__init__(self, parent, -1, title, size = (250,200))
        self.minValue = 0
        self.maxValue = 10000
        self.propCurVal = propCurVal
        panel = wx.Panel(self, -1)
        vbox = wx.BoxSizer(wx.VERTICAL)
        #wx.StaticBox(panel, -1, 'Colors', (5, 5), (240, 150))
        minHBox = wx.BoxSizer(wx.HORIZONTAL)
        minLabel = wx.StaticText(self, -1, 'Min Value', name = 'minValue')
        self.minText = wx.TextCtrl(self, -1, str(self.minValue))
        minHBox.Add(minLabel, 1, wx.ALIGN_LEFT | wx.TOP | wx.BOTTOM, 5)
        minHBox.Add(self.minText, 1, wx.ALIGN_RIGHT | wx.TOP | wx.BOTTOM, 5)
        
        maxHBox = wx.BoxSizer(wx.HORIZONTAL)
        maxLabel = wx.StaticText(self, -1, 'Max Value', name = 'maxValue')
        self.maxText = wx.TextCtrl(self, -1, str(self.maxValue))
        maxHBox.Add(maxLabel, 1, wx.ALIGN_LEFT | wx.TOP | wx.BOTTOM, 5)
        maxHBox.Add(self.maxText, 1, wx.ALIGN_RIGHT | wx.TOP | wx.BOTTOM, 5)

        btnHBox = wx.BoxSizer(wx.HORIZONTAL)
        OkBtn = wx.Button(self, -1, 'Ok')
        CancelBtn = wx.Button(self, -1, 'Cancel')
        self.Bind(wx.EVT_BUTTON, self.OnOk, OkBtn)
        self.Bind(wx.EVT_BUTTON, self.OnCancel, CancelBtn)
        
        btnHBox.Add(OkBtn, 1,  wx.ALIGN_LEFT | wx.TOP | wx.BOTTOM, 10)
        btnHBox.AddSpacer((10,10))
        btnHBox.Add(CancelBtn, 1, wx.ALIGN_RIGHT | wx.TOP | wx.BOTTOM, 10)
        
        vbox.Add(minHBox, 1, wx.ALIGN_CENTER | wx.TOP | wx.BOTTOM, 5)
        vbox.Add(maxHBox, 1,  wx.ALIGN_CENTER | wx.TOP | wx.BOTTOM, 5)
        vbox.Add(btnHBox, 1, wx.ALIGN_CENTER | wx.TOP | wx.BOTTOM , 5)
        
        self.SetSizer(vbox)
        
    
    def OnOk(self, event):
        try:
            self.minValue = int(self.minText.GetValue())
            self.maxValue = int(self.maxText.GetValue())
            
            if ( self.propCurVal < self.minValue or self.propCurVal > self.maxValue ):
                warningMsg = "Current Value " + str(self.propCurVal) + " is not in the newly configured range ("
                warningMsg = warningMsg + str(self.minValue) + "," + str(self.maxValue) + ")"
                warningMsg = warningMsg + "The current value will be rest accordingly. \nDo you wish to continue?" 
                optDialog = wx.MessageDialog(self, warningMsg, "Warning" , wx.YES_NO | wx.ICON_QUESTION)
                userOption = optDialog.ShowModal()
                if ( userOption == wx.ID_NO ):
                    return
                else:
                    pass
            self.Destroy()
        except(ValueError):
            msg = 'Invalid values. Please enter only numeric(integer) values'
            showMessage(msg, NON_FATAL)
    
    def OnCancel(self, event):
        self.Destroy()
    
    def getMin(self):
        #return int(self.minText.GetValue())
        return self.minValue
    
    def getMax(self):
        #return int(self.maxText.GetValue())
        return self.maxValue
    
    def setMin(self, newVal):
        self.minText.SetValue(str(newVal))
    
    def setMax(self, newVal):
        self.maxText.SetValue(str(newVal))
        

class WaveAppStatusBar(wx.StatusBar):
    def __init__(self, parent):
        wx.StatusBar.__init__(self, parent)
        self.SetFieldsCount(2)
        #self.SetStatusText("Welcome to OSSIE WaveApp", 0)
        self.SetStatusWidths([-5,-1])
              
        startIconPath = "../resources/start.png"
        stopIconPath = "../resources/stop.png"
        self.startIcon = wx.Image(startIconPath, wx.BITMAP_TYPE_ANY).ConvertToBitmap()
        self.stopIcon = wx.Image(stopIconPath, wx.BITMAP_TYPE_ANY).ConvertToBitmap()
        
        self.startBtn = wx.BitmapButton(self, -1, self.startIcon)
        self.uninstall = wx.Button(self, -1, 'U')
        
        self.startBtn.SetSize((23,23))
        self.uninstall.SetSize((23,23))
        
        self.Bind(wx.EVT_SIZE, self.OnResize)
        self.Bind(wx.EVT_BUTTON, self.OnStart, self.startBtn)
        self.running = True
        self.placeButtons()
    
    def placeButtons(self):
        rect = self.GetFieldRect(1)
        print rect.x, rect.y
        self.startBtn.SetPosition((rect.x+5, rect.y))
        stSize = self.startBtn.GetSize()
        self.uninstall.SetPosition((rect.x + stSize[0]+5, rect.y))
    
    def OnStart(self, event):
        if (self.running):
            self.startBtn.SetBitmapLabel(self.stopIcon)
            self.running = False
        else:
            self.startBtn.SetBitmapLabel(self.startIcon)
            self.running = True
    def OnResize(self, event):
        self.placeButtons()



class WaveAppCORBA:
    def __init__(self):
        self.rootContext = None
        self.availableWforms = {}
        self.__namingservice = LOCAL_HOST
        #self.init_CORBA()
            
    def selectNamingService(self):
        newNS = NamingserviceDialog(self)
        #return TRUE if new naming service is selected
        if newNS.GetReturnCode() == 1:
            return True
        else:
            return False
    
    def init_CORBA(self, enableGUI = True):
        self.enableGUI = enableGUI
        self.domMgr = None
        #Add naming service address to sys.argv
        try:
            sys.argv.index('-ORBInitRef')
        except ValueError:
            sys.argv.append('-ORBInitRef')
            sys.argv.append('NameService=corbaname::'+self.__namingservice)
        
        #orb object has to be destroyed before moving to new naming service
        try:
            self.orb.destroy()
        except:
            pass
         
        self.orb = CORBA.ORB_init(sys.argv, CORBA.ORB_ID)
        self.obj = self.orb.resolve_initial_references("NameService")
        if self.obj is None:
            print "init_CORBA(): FATAL Error: Could not resolve initial references"
            showMessage('Could not resolve initial references' , FATAL, self.enableGUI)
            return
        try:
            self.rootContext = self.obj._narrow(CosNaming.NamingContext)
        except(Exception), val:
            #print "init_CORBA(): FATAL Error: Failed to get naming context"
            #showMessage('Could not initialize CORBA. Failed to get naming context \n The application will now exit.', FATAL)
            
            newNS = False
            while not newNS:
                ts = "Failed to narrow the root naming context.\n"
                ts += "Are the Naming Service and nodeBooter running on "+self.__namingservice+"?"
               
                showMessage(ts, NON_FATAL, self.enableGUI)
                nsDialog = NamingserviceDialog(self)
                if nsDialog.GetReturnCode() == 1:
                    newNS = True
                elif nsDialog.GetReturnCode() == -2:
                        sys.exit(-1)
                
            return
        
        if self.rootContext is None:
            print "init_CORBA(): FATAL Error: Failed to get root context" 
            
            showMessage('Could not initialize CORBA.\nFailed to get root context', FATAL, self.enableGUI)
            return
        name = [CosNaming.NameComponent("DomainName1",""), CosNaming.NameComponent("DomainManager","")]
        try:
            self.obj = self.rootContext.resolve(name)
        except:
            print "init_CORBA(): FATAL Error: Could not find domain manager"
            showMessage('Could not find Domain Manager', FATAL, self.enableGUI)
            return
        self.domMgr = self.obj._narrow(CF.DomainManager)
        if self.domMgr is None:
            print "init_CORBA(): FATAL Error: Could not resolve domain manager"
            showMessage('Could not resolve Domain Manager', FATAL, self.enableGUI)
        
        #get the reference to FileManager to perform all file operations (list, open etc.. )
        try:
            self.fileMgr = self.domMgr._get_fileMgr()
        except:
            print "init_CORBA(): FATAL Error: Could not get file Manager from Domain manager"
            showMessage('Could not get file manager reference.\n' + 
                        'Has a domain manager been started with NodeBooter?', INFO, self.enableGUI)
    
    def setNamingService (self, newNS):
        self.__namingservice = newNS
    
    def getNamingService(self):
        return self.__namingservice
        
    def getApplications(self):
        dom_obj = self.rootContext.resolve([CosNaming.NameComponent("DomainName1","")])
        dom_context = dom_obj._narrow(CosNaming.NamingContext)
        
        if dom_context is None:
            print 'In getApplications() : dom_context not found'
            return
        self.availableWforms.clear()
        
        try:
            appSeq = self.domMgr._get_applications()
        except(CORBA.TRANSIENT), val:
            #print "In getApplications():",  str(val)
            showMessage("Exception! " + str(val) + "\n Could not get applications from Domain Manager \n Exiting now!!!" , FATAL, self.enableGUI) 
            sys.exit(-1)
        members = dom_context.list(100)
        for m in members[0]:
            wformName = str(m.binding_name[0].id)
            wformObj = dom_context.resolve([CosNaming.NameComponent(wformName,"")])
            wformContext = wformObj._narrow(CosNaming.NamingContext)
            if wformContext is None:
                #print wformName
                continue
            
                       
            foundApp = False
            for app in appSeq:
                appName = app._get_name()
                if appName in wformName:
                    if self.availableWforms.has_key(appName):
                        if wformName not in self.availableWforms[appName]:
                            self.availableWforms[appName].append(wformName)
                    else:
                        self.availableWforms[appName] = [wformName]
        #returns a dictionary of applications and theri corresponding installed waveforms
        return self.availableWforms
    
    def installWaveform(self, wformSAD, wformDAS, start):
        
        #sadxml = importResource.stripDoctype(wformSAD)
        #doc_sad = xml.dom.minidom.parse(wformSAD)
        doc_sad = self.getDOM(wformSAD)
        if doc_sad is None:
            return
        app_name = doc_sad.getElementsByTagName("softwareassembly")[0].getAttribute("name")
        _appFacProps = []
        devMgrSeq = None
        try:
            devMgrSeq = self.domMgr._get_deviceManagers()
        except(CORBA.COMM_FAILURE, CORBA.TRANSIENT), val:
            showMessage("Exception: " + str(val) + "\n - Could not get device managers!" , NON_FATAL, self.enableGUI)
            return 
            
        available_dev_seq = []
        for devmgr in range(len(devMgrSeq)):
            devMgr = devMgrSeq[devmgr]
            curr_devSeq = devMgr._get_registeredDevices()
            for dev in range(len(curr_devSeq)):
                curr_dev = curr_devSeq[dev]
                available_dev_seq.append(curr_dev._get_identifier())
                #print curr_dev._get_identifier()

        #clean_SAD = wformSAD.split("/sdr/dom")
        #rel_wformSAD = clean_SAD[1]
        self.domMgr.installApplication(wformSAD)

        
        # Parse the device assignment sequence, ensure
        #doc_das = xml.dom.minidom.parse(wformDAS)
        doc_das = self.getDOM(wformDAS)
        deviceassignmenttypeNodeList = doc_das.getElementsByTagName("deviceassignmenttype")

        for deviceassignmenttypeNode in deviceassignmenttypeNodeList:
            # look for assigndeviceid nodes
            assigndeviceidNodeList = deviceassignmenttypeNode.getElementsByTagName("assigndeviceid")
            if len(assigndeviceidNodeList) == 0:
                ts = "Could not find \"assigndeviceid\" tag\nAborting install"
                showMessage(ts, NON_FATAL, self.enableGUI)
                #errorMsg(self, ts)
                return

            # get assigndeviceid tag value (DCE:xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx)
            assigndeviceid = assigndeviceidNodeList[0].firstChild.data

            # ensure assigndeviceid is in list of available devices
            if assigndeviceid not in available_dev_seq:
                ts = "Could not find the required device: " + str(assigndeviceid)
                ts += "\nAborting install"
                showMessage(ts, NON_FATAL, self.enableGUI)
                #errorMsg(self, ts)
                return
                
        _devSeq = self.BuildDevSeq(wformDAS)
        _applicationFactories = self.domMgr._get_applicationFactories()

        # attempt to match up the waveform application name to 
        # a application factory of the same name
        app_factory_num = -1
        for app_num in range(len(_applicationFactories)):
            if _applicationFactories[app_num]._get_name()==app_name:
                app_factory_num = app_num
                break
    
        if app_factory_num == -1:
            showMessage ("Application factory not found", NON_FATAL, self.enableGUI)
            

        # use the application factor I found above to create an instance
        # of an application
        try:
            app = _applicationFactories[app_factory_num].create(_applicationFactories[app_factory_num]._get_name(),_appFacProps,_devSeq)
        except:
            showMessage("Unable to create application\nMake sure that all appropriate nodes are installed", NON_FATAL, self.enableGUI)
            return(None)
        
        if start:
            # start the application
            app.start()
        
        naming_context_list = app._get_componentNamingContexts()
        naming_context = naming_context_list[0].elementId.split("/")
        application_name = app._get_name()
        newInstance = naming_context[1]
        return newInstance
    
    def uninstallWaveform(self, wformName):
        try:
            appRef = self.getAppRef(wformName)
            if appRef is None:
                showMessage("Error: Failed to get application reference for " + wformName, NON_FATAL, self.enableGUI)
                return False
            appRef.releaseObject()
        except(CORBA.COMM_FAILURE, CORBA.INV_OBJREF), val:
            showMessage("Exception: " + str(val) + "\n - Could not uninstall " + wformName, NON_FATAL, self.enableGUI)
            return False
        return True
    
    def startWaveform(self, wformName):
        appRef = self.getAppRef(wformName)
        appRef.start()
        return True
    
    def stopWaveform(self, wformName):
        appRef = self.getAppRef(wformName)
        appRef.stop()
        return True
    
    def BuildDevSeq(self, dasXML):
        #doc_das = xml.dom.minidom.parse(dasXML)
        doc_das = self.getDOM(dasXML)
         
        # create node list of "deviceassignmenttype"
        deviceassignmenttypeNodeList = doc_das.getElementsByTagName("deviceassignmenttype")

        ds = []
        for n in deviceassignmenttypeNodeList:
            componentid = n.getElementsByTagName("componentid")[0].firstChild.data
            assigndeviceid = n.getElementsByTagName("assigndeviceid")[0].firstChild.data
            ds.append(CF.DeviceAssignmentType(str(componentid),str(assigndeviceid)))

        return ds
    
    def getAppRef(self, selWform):
        dom_obj = self.rootContext.resolve([CosNaming.NameComponent("DomainName1","")])
        dom_context = dom_obj._narrow(CosNaming.NamingContext)
        try:
            if dom_context is None:
                showMessage('Could not reference for DomainName1', NON_FATAL, self.enableGUI)
                return
            appSeq = self.domMgr._get_applications()
            members = dom_context.list(100)
            for m in members[0]:
                wformName = str(m.binding_name[0].id)
                
                wformObj = dom_context.resolve([CosNaming.NameComponent(wformName,"")])
                wformContext = wformObj._narrow(CosNaming.NamingContext)
                if wformContext is None:
                    #print wformName
                    continue
                
                wformName = wformName[wformName.index("::") + 2:] #strip off OSSIE:: from wform naem           
                foundApp = False
                wformApp = None
                for app in appSeq:
                    compNameCon = app._get_componentNamingContexts()
                    for compElementType in compNameCon:
                        if (wformName in compElementType.elementId) and (wformName == selWform):
                            wformApp = app
                            break
                
                if wformApp is not None:
                    break
            return wformApp
        except:
            #errorMsg = sys.exc_info()[1]
            #showMessage(str(errorMsg), NON_FATAL)
            return None
            
    def query(self, wformName, compName, prpList):
        wformName = "OSSIE::" + wformName
        
        try:
            prp = [CosNaming.NameComponent("DomainName1", ''), 
                   CosNaming.NameComponent(wformName,''),
                   CosNaming.NameComponent(compName,'')]
    
            prpRsrcRef = self.rootContext.resolve(prp)
            if prpRsrcRef is None:
                showMessage(("Unable to find rootContext for %s/%s" % (wformName,compName)), NON_FATAL, self.enableGUI)
                return None
        
            prpRsrcHandle = prpRsrcRef._narrow(CF.Resource)
            prpSetHandle = prpRsrcRef._narrow(CF.PropertySet)
        
            if prpSetHandle is None:
                showMessage(("Unable to get PropertySet reference for %s/%s" % (wformName, compName)), NON_FATAL, self.enableGUI)
    
            if len(prpList) == 0:
                prpList = prpSetHandle.query(prpList)
                        
        except:
            errorMsg = sys.exc_info()[1]
            showMessage(str(errorMsg), NON_FATAL, self.enableGUI)
            return None
        return prpList
    
    def configure(self, wformName, compName, prpList):
        wformName = "OSSIE::" + wformName
        try:
            prpRef = [CosNaming.NameComponent("DomainName1", ''), 
                      CosNaming.NameComponent(wformName,''),
                      CosNaming.NameComponent(compName,'')]
            prpRsrcRef = self.rootContext.resolve(prpRef)
            if prpRsrcRef is None:
                showMessage(("Unable to find rootContext for %s/%s" % (wformName,compName)), NON_FATAL, self.enableGUI)
                return False
            prpRsrcHandle = prpRsrcRef._narrow(CF.Resource)
            prpSetHandle = prpRsrcRef._narrow(CF.PropertySet)
            if prpSetHandle is None:
                showMessage(("Unable to get PropertySet reference for %s/%s" % (wformName, compName)), NON_FATAL, self.enableGUI)
                return False
            prpSetHandle.configure(prpList)
            
            return True
        except:
            errorMsg = str(sys.exc_info()[1])
            showMessage("Exception! " + errorMsg + "\n Connection to Domain Manager Failed", NON_FATAL, self.enableGUI)
            return False
        
    def getDOM(self, fileName):
        #get the file descriptor from the framework
        #file path should be absolute to the Framework file system
        #e.g. if framework is mounted on /sdr/dom, then the path of a
        #sad file would like "/waveforms/ossie_demo/ossie_demo.sad.xml
        
        try:
            #print "getDOM() - fileName = ", fileName 
            fd = self.fileMgr.open(fileName, True)
            domObj = xml.dom.minidom.parse(fd)
            fd.close()
            return domObj
        except:
            errorMsg = str(sys.exc_info()[1])
            errorMsg = errorMsg + "\n getDOM(): Could not create DOM for file '" + fileName + "'"
            showMessage(str(errorMsg), NON_FATAL, self.enableGUI)
            return None
    
    def getFileList(self, dir=None, filetype= ""):
        """ This function searches for the given filetype (e.g. *.sad.xml)
        under the dir through the FileManager. the directory dir is relative 
        to the FileManager mount directory (/sdr/dom)
        """
        
        fileNames = []
        try:
            fileObjList = self.fileMgr.list("/"+filetype) #FileMgr list func demands a /
            #fileObjList is a CORBA sequence of CF:File objects. extract the file name 
            
            if fileObjList is not None:
                for fileObj in fileObjList:
                    #the file name is stored as an absolute path to the FileManager file system
                    #e.g file name = dom/waveforms/ossie_demo/ossie_demo.sad.xml
                    #throughout wavedash,we store the filenames in the model as relative to the
                    #FileManager root dir (/sdr/dom). Hence strip off dom/ from the returned filename
                    if dir is not None:
                        if ( fileObj.name.find(dir) != -1 ):
                            fName = fileObj.name[fileObj.name.find("/"):]
                            fileNames.append(fName)
        except:
            errorMsg = str(sys.exc_info()[1])
            errorMsg = errorMsg + "\n getFileList(): Could not get list of files from FileManager '"
            showMessage(str(errorMsg), NON_FATAL, self.enableGUI)
            return None
        
        return fileNames
