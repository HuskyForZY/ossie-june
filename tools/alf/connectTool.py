## Copyright 2005, 2006, 2007, 2008 Virginia Polytechnic Institute and State University
##
## This file is part of the OSSIE ALF Waveform Application Visualization Environment
##
## ALF is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## ALF is distributed in the hope that it will be useful, but WITHOUT ANY
## WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with OSSIE Waveform Developer; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

import wx

import re
import sys       # for system commands (e.g., argv and argc stuff)
import CosNaming   # narrowing naming context stuff
import random   # for generating random connection id

try:   #mac / older OSSIE versions
    import standardInterfaces__POA
    import CF, CF__POA    # core framework stuff
except ImportError:   #0.6.2
    import ossie.standardinterfaces.standardInterfaces__POA as standardInterfaces__POA
    import ossie.cf.CF as CF
    import ossie.cf.CF__POA as CF__POA    

from wx.lib.anchors import LayoutAnchors  # used by splitter window

import loadAutomationFile
import importWaveform

def create(parent):
    ''' returns a wx frame representing the connect tool'''
    win = MainFrame(parent, -1)
    win.CenterOnParent()
    return win



def errorMsg(self,msg):
    ''' Brings up a wx error message that says msg (first argument)'''
    dlg = wx.MessageDialog(self,msg,'Error', wx.OK | wx.ICON_INFORMATION)
    try:
        dlg.ShowModal()
    finally:
        dlg.Destroy()
    return



class MainFrame(wx.Frame):
    ''' The main window where all of the connect tool's wx stuff resides'''

    def __init__(self, parent, id):
        self.alfFrameRef = parent

        self._init_ctrls(self.alfFrameRef)   # initialize the wx stuff
       
        if __name__ != '__main__':      # this statement allows me to run the tool
                                        # as a standalone without OSSIE to do 
                                        # design work
         self.refreshDisplay()

        # This argument sets a default value for the automations file
        #self.automationFileEditor.write('/sdr/automation_files/AM_FM_router.xml')
        self.automationFileEditor.write('automationFileExamples')

        self.parent = parent

        if False:        
         # Now Create the menu bar and items
         self.mainmenu = wx.MenuBar()

         menu = wx.Menu()
         menu.Append(205, 'E&xit', 'Enough of this already!')
         self.Bind(wx.EVT_MENU, self.OnFileExit, id=205)
         self.mainmenu.Append(menu, '&File')
        
         menu = wx.Menu()
         menu.Append(300, '&About', 'About this thing...')
         self.Bind(wx.EVT_MENU, self.OnHelpAbout, id=300)
         self.mainmenu.Append(menu, '&Help')

         self.SetMenuBar(self.mainmenu)

         # Bind the close event so we can disconnect the ports
         self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)

        self.Show(True)
        
    def _init_ctrls(self, prnt):
        ''' Initialize the wx controls'''

        frame_size = wx.Size(770, 350)

        wx.Frame.__init__(self, id=-1, name='', parent=prnt,
                          pos=wx.Point(10, 20), size=frame_size,
                          style=wx.DEFAULT_FRAME_STYLE, 
                          title='Connection Tool')


        panel = wx.Panel(self, -1)


        # --------------------------------------------------------------------
        # create the top sizer


        # Static Texts (labels)
        self.blankLabel  = wx.StaticText(id=-1, label='',
                                        name='blankLabel', parent=panel, 
                                        size=wx.Size(150, 30), style=0)
        self.usesLabel = wx.StaticText(id=-1, label='Select Uses Port:',
                                        name='usesLabel', parent=panel, 
                                        size=wx.Size(250, 30), 
                                        style=wx.ALIGN_CENTER)
        self.providesLabel = wx.StaticText(id=-1, label='Select Provides Port:',
                                        name='providesLabel', parent=panel, 
                                        size=wx.Size(250, 30), 
                                        style = wx.ALIGN_CENTER)
        treeSize = (350,200)
        
        #Changing to Tree style listing of waveforms/components/ports from previously used Choice style
        self.usesTree = wx.TreeCtrl(parent=panel, id=-1, size=treeSize , style=wx.TR_HIDE_ROOT|wx.TR_HAS_BUTTONS)
        self.providesTree = wx.TreeCtrl(parent=panel, id=-1, size=treeSize , style=wx.TR_HIDE_ROOT|wx.TR_HAS_BUTTONS)
        
        # connect button
        self.ConnectBtn = wx.Button(id = -1, label='Connect',
                                    parent=panel)
        self.ConnectBtn.Bind(wx.EVT_BUTTON, self.OnConnectBtn, id=-1)
        
        blankLabel2 = wx.StaticText(label = '', parent = panel)
        blankLabel3 = wx.StaticText(label = '', parent = panel)
        blankLabel4 = wx.StaticText(label = '', parent = panel)


        flexSizer1 = wx.FlexGridSizer(cols=3, hgap=6, vgap = 6)
        flexSizer1.AddMany([blankLabel2, self.usesLabel, self.providesLabel, 
                            blankLabel3, self.usesTree, self.providesTree,
                            blankLabel4, self.ConnectBtn])
        # --------------------------------------------------------------------
        # second sizer
        vSizer1 = wx.BoxSizer(wx.VERTICAL)
        self.connectionsLabel = wx.StaticText(parent = panel,
                                            label = "Connections:")
        self.connectionsListBox = wx.ListBox(parent=panel,
                                            size=wx.Size(700,150),
                                            style=wx.LB_MULTIPLE,
                                            )
        self.DisconnectBtn = wx.Button(parent=panel,
                                            label='Disconnect')
        self.DisconnectBtn.Bind(wx.EVT_BUTTON, self.OnDisconnectBtn)
        vSizer1.Add(self.connectionsLabel, 0, wx.ALL, 5)
        vSizer1.Add(self.connectionsListBox, 0, wx.ALL, 5)
        vSizer1.Add(self.DisconnectBtn, 0, wx.ALL, 5)
        # --------------------------------------------------------------------


        # --------------------------------------------------------------------
        # third sizer
        hSizer1 = wx.BoxSizer(wx.HORIZONTAL)
        self.automationFileEditor = wx.TextCtrl(id=-1,
                                        name=u'automationFileEditor', 
                                        parent=panel, 
                                        size=wx.Size(350, 30), 
                                        style=0, value=u'')
        self.browseAutomationBtn = wx.Button(id = -1, 
                                           label='Browse',
                                    parent=panel, 
                                    size=wx.Size(80, 30))
        self.browseAutomationBtn.Bind(wx.EVT_BUTTON, 
                                    self.onBrowseAutomationButton,
                                    id=-1)
        self.loadAutomationBtn = wx.Button(id = -1, 
                                           label='Load Automation File',
                                    parent=panel, 
                                    size=wx.Size(180, 30))
        self.loadAutomationBtn.Bind(wx.EVT_BUTTON, 
                                    self.onLoadAutomationFileButton,
                                    id=-1)
        hSizer1.Add(self.automationFileEditor, 0, wx.ALL, 5)
        hSizer1.Add(self.browseAutomationBtn, 0, wx.ALL, 5)
        hSizer1.Add(self.loadAutomationBtn, 0, wx.ALL, 5)
        # --------------------------------------------------------------------



        # --------------------------------------------------------------------
        # assemble main sizer from other sizers

        # topLabel = wx.StaticText(label = 'Connect Tool.  OSSIE 0.6.2', 
        #                         parent = panel)

        mainSizer = wx.BoxSizer(wx.VERTICAL)
        
        mainSizer.Add(flexSizer1, 0 , wx.EXPAND|wx.ALL, 10)
        mainSizer.Add(wx.StaticLine(panel), 0 , wx.EXPAND|wx.TOP|wx.BOTTOM, 5)
        mainSizer.Add(vSizer1, 0, wx.EXPAND|wx.ALL, 10)
        mainSizer.Add(wx.StaticLine(panel), 0 , wx.EXPAND|wx.TOP|wx.BOTTOM, 5)
        mainSizer.Add(hSizer1, 0, wx.EXPAND|wx.ALL, 10)

        panel.SetSizer(mainSizer)
        mainSizer.Fit(self)
        # --------------------------------------------------------------------
        

    def refreshDisplay(self):
        self.getAvailableConnections()
        self.setAvailableApplications()
        self.connectionsListBox.SetItems([x['display'] for x in self.alfFrameRef.connections[self.alfFrameRef.namingservice[0]]])


    def getAvailableConnections(self):
        ''' Get a list of objects I can potentially connect to'''

        # Initialize my dictionary 
        self.avail_connects = {}

        dom_obj = self.alfFrameRef.rootContext.resolve(
                           [CosNaming.NameComponent("DomainName1","")])
        dom_context = dom_obj._narrow(CosNaming.NamingContext) 
        if dom_context is None:
            return

        # get a list of applications running in the domain
        appSeq =  self.alfFrameRef.domMgr._get_applications()
        apps = {}
        for app in appSeq:
            sadfile = app._get_profile()
            sadfile = sadfile.replace('//','')
            # NOTE:  Use CF::FileManager to obtain location

            waveform = importWaveform.getWaveform(sadfile, self.alfFrameRef, self.alfFrameRef.Available_Ints)
            for comp in waveform.components:
                for port in comp.ports:
                    if apps.has_key(waveform.name):
                        if apps[waveform.name].has_key(comp.name):
                            apps[waveform.name][comp.name][port.name] = port.type
                        else:
                            apps[waveform.name][comp.name] = {port.name: port.type}
                    else:
                        apps[waveform.name] = {comp.name: {port.name: port.type}}

        p = re.compile("^(OSSIE::.+)_\d+$")
        members = dom_context.list(1000)
        for m in members[0]:
            wav_name = str(m.binding_name[0].id)

            wav_obj = dom_context.resolve(
                                [CosNaming.NameComponent(wav_name,"")])
            wav_context = wav_obj._narrow(CosNaming.NamingContext)
            if wav_context is None:
                continue

            m = p.match(wav_name)
            if m:
                if apps[m.group(1)]:
                    self.avail_connects[wav_name] = apps[m.group(1)]
                else:
                    print "Could not find associated application for: " + wav_name
                    continue

    def setAvailableApplications(self):
        ''' Assumes that I have a list of available applications. Populates
            the Uses and Provides tree with those applications/components/ports.
        '''

        usesRoot = self.usesTree.AddRoot("Uses")
        providesRoot = self.providesTree.AddRoot("Provides")
         
        for w in self.avail_connects.keys():
            uApp = self.usesTree.AppendItem(usesRoot, w )
            pApp = self.providesTree.AppendItem(providesRoot, w)
            for c in self.avail_connects[w].keys():
                uComp = self.usesTree.AppendItem(uApp, c)
                pComp = self.providesTree.AppendItem(pApp, c)
                for p,type in self.avail_connects[w][c].iteritems():
                    if type == "Uses":
                        uPort = self.usesTree.AppendItem(uComp, p)
                        self.usesTree.SetPyData(uPort, "Uses")
                    elif type == "Provides":
                        pPort = self.providesTree.AppendItem(pComp, p)
                        self.providesTree.SetPyData(pPort, "Provides")         

    def OnPAppSelection(self, event):
        ''' Should occur when the user has selected a provides application.
            Adds a list of the available components in the selected application
            to the wx.choice for provides components.'''

        choice = str(self.pAppChoice.GetStringSelection())
        self.pCompChoice.Clear()
        self.pPortChoice.Clear()
        if choice != '(Choose Application)':
            self.pCompChoice.Append('(Choose Component)')
            for c in self.avail_connects[choice].keys():
                self.pCompChoice.Append(c)
            self.pCompChoice.SetSelection(0)

        event.Skip()
    
    def OnUAppSelection(self,event):
        ''' Should occur when the user has selected a uses application.
            Adds a list of the available components in the selected application
            to the wx.choice for uses components.'''

        choice = str(self.uAppChoice.GetStringSelection())
        self.uCompChoice.Clear()
        self.uPortChoice.Clear()
        if choice != '(Choose Application)':
            self.uCompChoice.Append('(Choose Component)')
            for c in self.avail_connects[choice].keys():
                self.uCompChoice.Append(c)
            self.uCompChoice.SetSelection(0)

        event.Skip()

    def OnPCompSelection(self, event):
        ''' Should occur when the user has selected a provides component.
            Adds a list of the available ports in the selected proviedes
            component to the wx.choice for provides ports.'''

        app_choice = str(self.pAppChoice.GetStringSelection())
        comp_choice = str(self.pCompChoice.GetStringSelection())
        self.pPortChoice.Clear()
        if comp_choice != '(Choose Component)':
            self.pPortChoice.Append('(Choose Port)')
            for p,type in self.avail_connects[app_choice][comp_choice].iteritems():
                if type == "Provides":
                    self.pPortChoice.Append(p)
            self.pPortChoice.SetSelection(0)

        event.Skip()

    def OnUCompSelection(self, event):
        ''' Should occur when the user has selected a uses component.
            Adds a list of the usesavailable ports in the selected 
            component to the wx.choice for uses ports.'''

        app_choice = str(self.uAppChoice.GetStringSelection())
        comp_choice = str(self.uCompChoice.GetStringSelection())
        self.uPortChoice.Clear()
        if comp_choice != '(Choose Component)':
            self.uPortChoice.Append('(Choose Port)')
            for p,type in self.avail_connects[app_choice][comp_choice].iteritems():
                if type == "Uses":
                    self.uPortChoice.Append(p)
            self.uPortChoice.SetSelection(0)

        event.Skip()

    def OnConnectBtn(self,event):
        ''' This is what happens when you press the connect button.
            Retrieves the selected apps, components, and ports.
            Then calls connect method.'''

#get the selected uses and provides port from teh trees
        selUPort = self.usesTree.GetSelection()
        selPPort = self.providesTree.GetSelection()
        
        #All uses ports are attached with a string metadata "Uses" and all 
        #provides ports are attached with "Provides". If user selects a component
        #or a waveform from the tree, the PyData would be None.
        
        if ( self.usesTree.GetItemPyData(selUPort) != "Uses" or
             self.providesTree.GetItemPyData(selPPort) != "Provides" ):
            dial = wx.MessageDialog(self, 'Please select a valid uses port and provides port to make a connection.', 'Ports Not Selected', wx.OK)
            dial.ShowModal()
            return
        
        selUComp = self.usesTree.GetItemParent(selUPort)
        selUApp = self.usesTree.GetItemParent(selUComp)
        
        selPComp = self.providesTree.GetItemParent(selPPort)
        selPApp = self.providesTree.GetItemParent(selPComp)
        
        uPortName = str(self.usesTree.GetItemText(selUPort))
        pPortName = str(self.usesTree.GetItemText(selPPort))
        uCompInstName = str(self.usesTree.GetItemText(selUComp))
        pCompInstName = str(self.providesTree.GetItemText(selPComp))
        uAppInstName = str(self.providesTree.GetItemText(selUApp))
        pAppInstName = str(self.providesTree.GetItemText(selPApp))
        
        self.Connect( uAppInstName, uCompInstName, uPortName,
                          pAppInstName, pCompInstName, pPortName)
    
        event.Skip()


    def Connect(self, uAppInstName, uCompInstName, uPortName,
                      pAppInstName, pCompInstName, pPortName):
 
        rootContext = self.GetParent().rootContext
        if rootContext is None:
            print "Failed to narrow the root naming context"
            sys.exit(1)

        # get a reference to the provides port:
        # don't forget OSSIE:: in waveform name
        pname = [CosNaming.NameComponent('DomainName1',''),
                 CosNaming.NameComponent(pAppInstName,''),
                 CosNaming.NameComponent(pCompInstName,'')]
        
        try:
            pResourceRef = rootContext.resolve(pname)
        except:
            print "provides resource not found"
            sys.exit(1)
     
        pResourceHandle = pResourceRef._narrow(CF.Resource)
        pPortReference = pResourceHandle.getPort(pPortName)
        

        # get a reference to the uses port:
        # don't forget OSSIE:: in waveform name
        uname = [CosNaming.NameComponent('DomainName1',''),
                 CosNaming.NameComponent(uAppInstName,''),
                 CosNaming.NameComponent(uCompInstName,'')]
        try:
            uResourceRef = rootContext.resolve(uname)
     
        except:
            print "uses resource not found"
            sys.exit(1)
     
        uResourceHandle = uResourceRef._narrow(CF.Resource)

        # get a reference to the uses port
        uPortReference = uResourceHandle.getPort(uPortName)
        if uPortReference is None:
            print "Failed to get Port reference"
            return
        uPortHandle = uPortReference._narrow(CF.Port)
         

        # connect to the Uses port by passing a ref to my provides port
        # make up some arbitrary connectionid that will be used for 
        # disconnectPort later
        try:
            connection_id = str(random.randint(100000,999999)) + "_" + uPortName
            uPortHandle.connectPort(pPortReference, connection_id)
            
            connection_display = uAppInstName + ":" + uCompInstName + ":" + uPortName + "-->" + pAppInstName + ":" + pCompInstName + ":" + pPortName
            connection = {
                    'name': uname,
                    'port': uPortName,
                    'id': connection_id,
                    'appNames': [uAppInstName, pAppInstName],
                    'display': connection_display,
                }
            self.alfFrameRef.connections[self.alfFrameRef.namingservice[0]].append(connection)
            self.connectionsListBox.Append(connection['display'])
        except e:
            print e
            dial = wx.MessageDialog(self, 'Connection failed.', 'Failed', wx.OK)
            dial.ShowModal()

    def OnDisconnectBtn(self,event):
        for x in reversed(self.connectionsListBox.GetSelections()):
            connection = self.alfFrameRef.connections[self.alfFrameRef.namingservice[0]][x]
            try:
                rootContext = self.GetParent().rootContext
                resourceRef = rootContext.resolve(connection['name'])
                resourceHandle = resourceRef._narrow(CF.Resource)
                portReference = resourceHandle.getPort(connection['port'])
                if portReference is None:
                    print "Failed to get port reference for disconnect."
                    return
                portHandle = portReference._narrow(CF.Port)
                portHandle.disconnectPort(connection['id'])
                del self.alfFrameRef.connections[self.alfFrameRef.namingservice[0]][x]
                self.connectionsListBox.Delete(x)
            except e:
                print e
                dial = wx.MessageDialog(self, 'Disconnect failed.', 'Failed', wx.OK)
                dial.ShowModal()

    def onBrowseAutomationButton(self,event):
        ''' This is what happens when the Browse button is pressed.
            Will load a file dialog and allow the user to browse for an
            automation file.'''
        dlg = wx.FileDialog(self, "Choose automation file", "", "", "XML Files (*.xml)|*.xml|All Files|*", wx.FD_OPEN)
        if dlg.ShowModal() == wx.ID_OK:
            self.automationFileEditor.Clear()
            self.automationFileEditor.write(dlg.GetPath())
        dlg.Destroy()

    def onLoadAutomationFileButton(self,event):
        ''' This is what happens when the Load Automation File button is pressed.
            Will get the filename from the GUI and then start up the loadAutomationFile
            modue '''

        # get the name of the file
        automations_file = self.automationFileEditor.GetLineText(0)
        self.automation = loadAutomationFile.automation(self, automations_file)
        event.Skip()

    def pushPacket(self, I , Q):
        ''' Other ports in the Domain can call push packet on me.  I will forward the
            packet to the instance of loadAutomationFile'''
        self.automation.pushPacket(I,Q)


    def pushPacketMetaData(self, I, Q, metadata):
        ''' Other ports in the Domain can call push packet on me.  I will forward the
            packet to the instance of loadAutomationFile'''

        # depeding on how metadata is defined, this method might change !!

        self.automation.pushPacketMetaData(I,Q, metadata)



    # --------------------------------------------------------------
    # Wx details:

    def OnFileExit(self, event):
        ''' This is what will happen when you select File -> Exit 
            in the menu bar'''
        self.Close()      #close the frame
  
    def OnHelpAbout(self, event):
        '''Stuff that gets displayed when you select Help -> About in the menu bar'''
        from wx.lib.dialogs import ScrolledMessageDialog
        about = ScrolledMessageDialog(self, "Connection Tool.\nA product of Wireless@VT.\n\nOSSIE 0.6.2.", "About...")
        about.ShowModal()

    def OnCloseWindow(self,event):
        '''This is what happens when you close the GUI'''
        if hasattr(self.parent, 'removeToolFrame'):
            self.parent.removeToolFrame(self)
        self = None
        event.Skip()


class MyApp(wx.App):
    '''for development purposes only'''
    def OnInit(self):
        frame = create(None)
        self.SetTopWindow(frame)
        return True


if __name__ == '__main__':
    '''for development and debugging purposes only'''
    print "WARNING:  Running this tool in the command line is intended for testing new GUI layouts only.  For this tool to function properly, it must be called from ALF"
    app= MyApp(0)
    app.MainLoop()







