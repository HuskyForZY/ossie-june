#! /bin/env python

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
from wx.lib import ogl

try:   # mac framework
    from wavedev import ComponentClass as CC
    from wavedev import WaveformClass
    import CF, CF__POA
    import customInterfaces
    import standardInterfaces
    import importResource
    import importIDL

except ImportError:  # 0.6.2
    from WaveDev.wavedev import ComponentClass as CC
    from WaveDev.wavedev import WaveformClass
    import ossie.standardinterfaces.standardInterfaces as standardInterfaces
    import ossie.custominterfaces.customInterfaces as customInterfaces
    import ossie.cf.CF as CF
    import ossie.cf.CF__POA as CF__POA
    import WaveDev.wavedev.importResource as importResource
    import WaveDev.wavedev.importIDL as importIDL

from omniORB import CORBA, PortableServer
import CosNaming
import sys
import importWaveform
import ALFshapes, ALFtiming

import xml.dom.minidom
from xml.dom.minidom import Node

import os
import ALFutils

import connectTool
from namingserviceDialog import NamingserviceDialog
import compform
import shutil
import shlex
import subprocess
import ConfigParser
from ALFNodeBooterUtils import ALFNodeBooterUtils
from ALFNSChoiceDialog import ALFNSChoiceDialog
from ALFNodeBooterDialog import ALFNodeBooterDialog

import wx.lib.foldpanelbar as fpb

#----------------------------------------------------------------------
# Create unique IDs for menu items and toolbar items for associating
# events with a particular action
[wxID_TOOLBAR_TIMING_TOOL, wxID_TOOLBAR_REFRESH_TOOL,
wxID_TOOLBAR_TIMING_DISPLAY_TOOL, wxID_TOOLBAR_CONNECT_TOOL,
wxID_TOOLBAR_NAMINGSERVICE_DIALOG, wxID_TOOLBAR_LAUNCH_WAVEDASH,
wxID_TOOLBAR_START_NODEBOOTER] = [wx.NewId() for x in range(7)]

[wxID_NSBOX_POPUP_DISPLAY, wxID_NSBOX_POPUP_START, 
 wxID_NSBOX_POPUP_STOP, wxID_NSBOX_POPUP_UNINSTALL] = [
                                                 wx.NewId() for x in range(4)]

[wxID_INSTALLBOX_POPUP_INSTALL_START, wxID_COMPONENTSBOX_POPUP_INSTALL_START, 
 wxID_INSTALLBOX_POPUP_INSTALL, wxID_COMPONENTSBOX_POPUP_INSTALL] = [wx.NewId() for x in range(4)]

OSSIE_CONFIG_FILE = '.ossie.cfg'
#----------------------------------------------------------------------
class alfApp(wx.App):
    def OnInit(self):
        self.main = alfFrame(None)
        self.main.Show()
        self.SetTopWindow(self.main)
        return True

#----------------------------------------------------------------------
class alfFrame(wx.Frame):
    def _init_ctrls(self, parent):
        wx.Frame.__init__(self, id=-1, name='CompFrame',
            parent=parent, pos=wx.DefaultPosition, size=wx.Size(1100, 770),
            style=wx.DEFAULT_FRAME_STYLE, title=u'ALF Waveform Debugger - OSSIE Waveform Workshop')
        
        # Initialize graphics
        ogl.OGLInitialize()
        ALFshapes.initializeColours()

        # Create the sash window and layout
        self._leftWindow = wx.SashLayoutWindow(self, 101, wx.DefaultPosition,
            wx.Size(240, 600), wx.NO_BORDER | wx.SW_3D | wx.CLIP_CHILDREN)
            
        self._leftWindow.SetDefaultSize(wx.Size(300, 400))
        self._leftWindow.SetOrientation(wx.LAYOUT_VERTICAL)
        self._leftWindow.SetAlignment(wx.LAYOUT_LEFT)
        self._leftWindow.SetSashVisible(wx.SASH_RIGHT, True)
        self._leftWindow.SetExtraBorderSize(10)

        self.ID_WINDOW_TOP = 100
        self.ID_WINDOW_LEFT = 101
        self.ID_WINDOW_RIGHT = 102
        self.ID_WINDOW_BOTTOM = 103

        self._leftWindow.Bind(wx.EVT_SASH_DRAGGED_RANGE, self.OnFoldPanelBarDrag,
            id=100, id2=103)
        self.Bind(wx.EVT_SIZE, self.OnFoldPanelSize)
        
        # Instantiate the main canvas
        self.canvas = MainWindow(self, self)

        # Create the fold panel and add its windows
        self._fpb_pnl = None
        self.CreateFoldPanel()
        
        # Create the status and toolbars
        self.CreateStatusBar(1, wx.ST_SIZEGRIP)
        self.CreateToolBar()

        # Make a popup box for the Naming Service / Manage Waveforms functionality
        self.nsBoxPopup = wx.Menu(title=u'')
        self.init_nsBoxPopup_Items(self.nsBoxPopup)
        
        # Make a popup box for the Launch Waveforms functionality
        self.installBoxPopup = wx.Menu(title=u'')
        self.init_installBoxPopup_Items(self.installBoxPopup)

        # Make a popup box for the Launch Components asWaveforms functionality
        self.componentsBoxPopup = wx.Menu(title=u'')
        self.init_componentsBoxPopup_Items(self.componentsBoxPopup)

        self.setupToolbar()
        
        self.Bind(wx.EVT_CLOSE, self.OnClose)
        self.connections = {'127.0.0.1': []}
        self.namingservice = ['127.0.0.1', '']
        self.nodeBooterProcess = None
        self.nbUtils = ALFNodeBooterUtils()
        if not self.nbUtils.namingServiceIsRunning():
            anscd = ALFNSChoiceDialog(self)
        

    def __init__(self,parent):
        self.active_wave = None
        self.timing_display = None
        self.tools = None
        self.connect_frame = None
        self.waveform_displays = {}

        # WARNING: if alf is restarted and waveforms are left running
        # counters will overlap and cause a crash :/
        self.compform_counter = 0

        self._init_ctrls(parent)
        self.Available_Ints = ALFutils.importStandardIdl(self)
        self.rootContext = None
        self.domMgr = None
        self.fileMgr = None
        
        
        self.init_CORBA()
        
        ALFutils.LoadConfiguration(self)

        self.waveformData = {}
        self.tool_frames = []
        self.last_waveform_data_update = None
        self.dasXML_list = []
        self.availableWaveforms = {}
        self.availableComponents = {}
        self.connections = {'127.0.0.1': []}

        if self.rootContext != None and self.fileMgr != None:
            self.DisplayInstalledWaveforms()
            self.DisplayAvailableWaveforms()
            self.DisplayAvailableComponents()

    def init_CORBA(self):
        """Initialize an orb and try to connect to the DomainManager"""

        try:
            sys.argv.index('-ORBInitRef')
        except ValueError:
            sys.argv.append('-ORBInitRef')
            sys.argv.append('NameService=corbaname::'+self.namingservice[0])
            
        #find the host Ip from the command line args
        isNSonLocalHost = False
        for i in range(len(sys.argv)):
            arg = sys.argv[i]
            if arg.find("corbaname::") != -1:
                hostIP = arg[arg.index("::")+2:]
                if hostIP == '127.0.0.1':
                    isNSonLocalHost = True
                    break

        try:
            self.orb.destroy()
        except:
            pass

        self.orb = CORBA.ORB_init(sys.argv, CORBA.ORB_ID)
        self.obj = self.orb.resolve_initial_references("NameService")
        
        try:
            self.rootContext = self.obj._narrow(CosNaming.NamingContext)
        except:
            newNS = False
            while not newNS:
                ts = "Failed to narrow the root naming context.\n"
                ts += "Are the Naming Service and nodeBooter running on "+self.namingservice[0]+"?"
                errorMsg(self, ts)
                self.namingservice_dialog = NamingserviceDialog(self)
                if self.namingservice_dialog.GetReturnCode() == 1:
                    newNS = True
            return
        
        #comp form feature is enabled only when NamingService is running on localhost.
        if ( isNSonLocalHost is True ):
            self.componentsBox.Enable(True)
        else:
            self.componentsBox.Enable(False)
            wx.MessageBox("Warning: Comp Form feature will be disabled when naming service is running remotely")
        
        if self.rootContext is None:
            errorMsg(self,"Failed to narrow the root naming context")
            self.domMgr = None
            return

        name = [CosNaming.NameComponent("DomainName1",""),
            CosNaming.NameComponent("DomainManager","")]
            
        try:
            self.obj = self.rootContext.resolve(name)
        except:
            errorMsg(self,"DomainManager name not found")
            self.domMgr = None
            return

        self.domMgr = self.obj._narrow(CF.DomainManager)
         #get the reference to FileManager to perform all file operations (list, open etc.. )
        try:
            self.fileMgr = self.domMgr._get_fileMgr()
        except:
            msg = "init_CORBA(): FATAL Error: Could not get file Manager from Domain manager\n"
            msg = msg + "Is nodeBooter Running??"
            errorMsg (self, msg)
            self.fileMgr = None
            return
            
 
    #---------------------------------------------------------------
    # Setup the display
    #---------------------------------------------------------------
    def init_nsBoxPopup_Items(self, parent):
        """Setup the popup menu for the Naming Service / Manage Waveforms box"""

        parent.Append(help='', id=wxID_NSBOX_POPUP_DISPLAY, kind=wx.ITEM_NORMAL, 
                      text=u'Display')
        self.nsBox.Bind(wx.EVT_MENU, self.OnNsBoxPopupDisplayMenu, 
                        id=wxID_NSBOX_POPUP_DISPLAY)
        
        parent.Append(help='', id=wxID_NSBOX_POPUP_START, kind=wx.ITEM_NORMAL, 
                      text=u'Start')
        self.nsBox.Bind(wx.EVT_MENU, self.OnNsBoxPopupStartMenu, 
                        id=wxID_NSBOX_POPUP_START)

        parent.Append(help='', id=wxID_NSBOX_POPUP_STOP, kind=wx.ITEM_NORMAL, 
                      text=u'Stop')
        self.nsBox.Bind(wx.EVT_MENU, self.OnNsBoxPopupStopMenu, 
                        id=wxID_NSBOX_POPUP_STOP)

        parent.Append(help='', id=wxID_NSBOX_POPUP_UNINSTALL, kind=wx.ITEM_NORMAL, 
                      text=u'Uninstall')
        self.nsBox.Bind(wx.EVT_MENU, self.OnNsBoxPopupUninstallMenu, 
                        id=wxID_NSBOX_POPUP_UNINSTALL)
    
    def init_installBoxPopup_Items(self, parent):
        """Setup the popup menu for the Launch Waveforms box"""

        parent.Append(help='', id=wxID_INSTALLBOX_POPUP_INSTALL_START, 
                        kind=wx.ITEM_NORMAL, text=u'Install and Start')
        self.installBox.Bind(wx.EVT_MENU, self.OnInstallBoxPopupInstallStartMenu, 
                        id=wxID_INSTALLBOX_POPUP_INSTALL_START)

        parent.Append(help='', id=wxID_INSTALLBOX_POPUP_INSTALL, 
                        kind=wx.ITEM_NORMAL, text=u'Install')
        self.installBox.Bind(wx.EVT_MENU, self.OnInstallBoxPopupInstallMenu, 
                        id=wxID_INSTALLBOX_POPUP_INSTALL)
       
    def init_componentsBoxPopup_Items(self, parent):
        """Setup the popup menu for the Launch Components as Waveforms box"""

        parent.Append(help='', id=wxID_COMPONENTSBOX_POPUP_INSTALL_START, 
                        kind=wx.ITEM_NORMAL, text=u'Install and Start')
        self.componentsBox.Bind(wx.EVT_MENU, self.OnComponentsBoxPopupInstallStartMenu, 
                        id=wxID_COMPONENTSBOX_POPUP_INSTALL_START)

        parent.Append(help='', id=wxID_COMPONENTSBOX_POPUP_INSTALL, 
                        kind=wx.ITEM_NORMAL, text=u'Install')
        self.componentsBox.Bind(wx.EVT_MENU, self.OnComponentsBoxPopupInstallMenu, 
                        id=wxID_COMPONENTSBOX_POPUP_INSTALL)
 
    def CreateFoldPanel(self):
        self._fpb_pnl = fpb.FoldPanelBar(self._leftWindow, -1, wx.DefaultPosition,
            wx.Size(-1,-1), 0)
	
        # Create the image list for the fold button icons
        Images = wx.ImageList(16,16)
        Images.Add(ALFutils.GetExpandedIconBitmap())
        Images.Add(ALFutils.GetCollapsedIconBitmap())
            
        # Add the Launch Waveforms box
        item = self._fpb_pnl.AddFoldPanel("Launch Waveform Applications", 
                                          collapsed=False,
                                          foldIcons=Images)
        self.installBox = wx.TreeCtrl(name=u'installBox', 
                                      parent=item,
                                      size = wx.Size(-1,200), 
                                      style = wx.TR_HIDE_ROOT | 
                                              wx.TR_HAS_BUTTONS | 
                                              wx.SIMPLE_BORDER)
        self._fpb_pnl.AddFoldPanelWindow(item, self.installBox, 
                                         fpb.FPB_ALIGN_WIDTH, 4) 
        self.installBox.Bind(wx.EVT_RIGHT_UP, self.OnInstallBoxRightUp)
        self.installBox.Bind(wx.EVT_LEFT_DCLICK, self.OnInstallBoxLeftDclick)

        # Add the Launch Components as Waveforms box
        item = self._fpb_pnl.AddFoldPanel("Launch Components as Applications", 
                                          collapsed=False,
                                          foldIcons=Images)
        self.componentsBox = wx.TreeCtrl(name=u'componentsBox', 
                                         parent=item,
                                         size = wx.Size(-1,200), 
                                         style = wx.TR_HIDE_ROOT | 
                                                 wx.TR_HAS_BUTTONS | 
                                                 wx.SIMPLE_BORDER)
        self._fpb_pnl.AddFoldPanelWindow(item, self.componentsBox, 
                                         fpb.FPB_ALIGN_WIDTH, 4) 
        self.componentsBox.Bind(wx.EVT_RIGHT_UP, self.OnComponentsBoxRightUp)
        self.componentsBox.Bind(wx.EVT_LEFT_DCLICK, 
                                self.OnComponentsBoxLeftDclick)

        # Add the Manage Waveforms box
        item = self._fpb_pnl.AddFoldPanel("Manage Applications", 
                                          collapsed=False, 
                                          foldIcons=Images)
        self.nsBox = wx.TreeCtrl(name=u'nsBox', parent=item,
                                 size = wx.Size(-1,200), 
                                 style = wx.TR_HIDE_ROOT | 
                                         wx.TR_HAS_BUTTONS | 
                                         wx.SIMPLE_BORDER)
        self.nsBox.Bind(wx.EVT_RIGHT_UP, self.OnNsBoxRightUp)
        self.nsBox.Bind(wx.EVT_LEFT_DCLICK, self.OnNsBoxLeftDclick)
        self._fpb_pnl.AddFoldPanelWindow(item, self.nsBox, 
                                         fpb.FPB_ALIGN_WIDTH, 4) 

        self._leftWindow.SizeWindows()
       
        
    def setupToolbar(self):
        toolbar = self.GetToolBar()
        tsize = (20,20)
        test_bmp = wx.ArtProvider.GetBitmap(wx.ART_REDO, wx.ART_TOOLBAR, tsize)
        toolbar.AddSimpleTool(wxID_TOOLBAR_REFRESH_TOOL,test_bmp,shortHelpString="Refresh")
        toolbar.AddSeparator()

        root = __file__
        if os.path.islink (root):
            root = os.path.realpath (root)
        root = os.path.dirname (os.path.abspath (root))

        time_img = wx.Image(root + '/images/timing.png',type=wx.BITMAP_TYPE_PNG)
        time_img.Rescale(24,24)
        time_bmp = wx.BitmapFromImage(time_img)
        toolbar.AddCheckTool(wxID_TOOLBAR_TIMING_TOOL,time_bmp,shortHelp="Enable Timing")
        self.timing_view_state = False

        time_img = wx.Image(root + '/images/timing_display.png',type=wx.BITMAP_TYPE_PNG)
        time_img.Rescale(24,24)
        time_bmp = wx.BitmapFromImage(time_img)
        toolbar.AddCheckTool(wxID_TOOLBAR_TIMING_DISPLAY_TOOL,time_bmp,shortHelp="Toggle Timing Display")
        
        toolbar.AddSeparator()


        connect_bmp = wx.ArtProvider.GetBitmap(wx.ART_COPY, wx.ART_TOOLBAR, tsize)
        toolbar.AddSimpleTool(wxID_TOOLBAR_CONNECT_TOOL,connect_bmp,shortHelpString="Connect Tool")

        toolbar.AddSeparator()

        net_img = wx.Image(root + '/images/network.png',type=wx.BITMAP_TYPE_PNG)
        net_img.Rescale(24,24)
        net_bmp = wx.BitmapFromImage(net_img)
        toolbar.AddSimpleTool(wxID_TOOLBAR_NAMINGSERVICE_DIALOG,net_bmp,shortHelpString="Select Naming Service")
        
        toolbar.AddSeparator()
        wavedash_img = wx.Image(root + '/images/launch_wavedash.png', type=wx.BITMAP_TYPE_PNG)
        wavedash_img.Rescale(24,24)
        wavedash_bmp = wx.BitmapFromImage(wavedash_img)
        toolbar.AddSimpleTool(wxID_TOOLBAR_LAUNCH_WAVEDASH, wavedash_bmp, shortHelpString="Launch Wavedash to configure waveforms")

        toolbar.AddSeparator()
        nodeBooter_img = wx.Image(root + '/images/nbstart.png', type = wx.BITMAP_TYPE_PNG)
        nodeBooter_img.Rescale(24, 24)
        nodeBooter_bmp = wx.BitmapFromImage(nodeBooter_img)
        toolbar.AddSimpleTool(wxID_TOOLBAR_START_NODEBOOTER, nodeBooter_bmp, shortHelpString="Start NodeBooter")
        toolbar.Bind(wx.EVT_TOOL, self.OnToolBarClick)
        
        toolbar.Realize()

    # ---------------------------------------------------
    # Event handling for toolbar
    # ---------------------------------------------------
    def OnToolBarClick(self,event):
        tb = self.GetToolBar()
        if event.GetId() == wxID_TOOLBAR_TIMING_TOOL:
            if tb.GetToolState(wxID_TOOLBAR_TIMING_TOOL):
                if self.active_wave == None:
                    errorMsg(self,"Please select and display a waveform first!")        
                    tb.ToggleTool(wxID_TOOLBAR_TIMING_TOOL, False)
                    return
                tb.EnableTool(wxID_TOOLBAR_TIMING_DISPLAY_TOOL, True)
                tb.SetToolShortHelp(wxID_TOOLBAR_TIMING_TOOL, "Disable Timing")
                self.timing_display = ALFtiming.TimingDisplay(self.active_wave.naming_context, self)
            else:
                self.timing_display = None
                tb.EnableTool(wxID_TOOLBAR_TIMING_DISPLAY_TOOL, False)
                tb.SetToolShortHelp(wxID_TOOLBAR_TIMING_TOOL, "Enable Timing")
                self.refreshDisplay()

        if event.GetId() == wxID_TOOLBAR_TIMING_DISPLAY_TOOL:
            if tb.GetToolState(wxID_TOOLBAR_TIMING_DISPLAY_TOOL):
                tb.SetToolShortHelp(wxID_TOOLBAR_TIMING_DISPLAY_TOOL, "Disable Timing Graphics")
                self.timing_view_state = True
            else:
                tb.SetToolShortHelp(wxID_TOOLBAR_TIMING_DISPLAY_TOOL, "Enable Timing Graphics")
                self.timing_view_state = False

        elif event.GetId() == wxID_TOOLBAR_CONNECT_TOOL:
            if self.connect_frame:
                self.connect_frame.Raise()
            else:
                self.connect_frame = connectTool.create(self)

        elif event.GetId() == wxID_TOOLBAR_NAMINGSERVICE_DIALOG:
            self.namingservice_dialog = NamingserviceDialog(self)
            
        elif event.GetId() == wxID_TOOLBAR_LAUNCH_WAVEDASH:
            namingService = "-ORBInitRef NameService=corbaname::" + self.namingservice[0]
            if (os.path.exists("/usr/local/bin/WAVEDASH")):
                os.spawnl(os.P_NOWAIT, "/usr/local/bin/WAVEDASH", namingService, "")
            elif (os.path.exists("/usr/bin/WAVEDASH")):
                os.spawnl(os.P_NOWAIT, "/usr/bin/WAVEDASH", namingService, "")
            else:
                errorMsg(self, "Failed to launch WAVEDASH. Executable(WAVEDASH) not found in /usr/local/bin/ and /usr/bin/ !!!")
                return


        # Refresh the display: Naming Service, Waveform List, and Canvas
        elif event.GetId() == wxID_TOOLBAR_REFRESH_TOOL:
            self.refreshDisplay(True)
            self.DisplayInstalledWaveforms()
            self.DisplayAvailableWaveforms()
            self.DisplayAvailableComponents()
            tb.ToggleTool(wxID_TOOLBAR_REFRESH_TOOL, False)
        
        elif event.GetId() == wxID_TOOLBAR_START_NODEBOOTER:
            anbd = ALFNodeBooterDialog(self)
            
            if anbd.GetReturnCode() == 0:
                self.namingservice[0] = '127.0.0.1'
                self.init_CORBA()
                self.refreshDisplay(True)
                self.DisplayInstalledWaveforms()
                self.DisplayAvailableWaveforms()
                self.DisplayAvailableComponents()
            
    def processTimingEvent(self, component_name, port_name, function_name, description, time_s, time_us, number_samples):
        if self.active_wave == None or (self.active_wave.naming_context not in component_name):
            errorMsg(self,"Cannot find waveform containing: " + component_name)

        cname = component_name[component_name.rfind('/')+1:]
        if self.active_wave != None:
            for comp in self.active_wave.components:
                if comp.name == cname:
                    comp.shape.processTimingEvent(port_name, function_name, description, time_s, time_us, number_samples)


        
    # ---------------------------------------------------------
    # Event handling for the FoldPanel
    # --------------------------------------------------------
    def OnFoldPanelSize(self, event):
        wx.LayoutAlgorithm().LayoutWindow(self, self.canvas)
        event.Skip()
    
    def OnFoldPanelBarDrag(self, event):
        if event.GetDragStatus() == wx.SASH_STATUS_OUT_OF_RANGE:
            return
                
        if event.GetId() == self.ID_WINDOW_LEFT:
            self._leftWindow.SetDefaultSize(wx.Size(event.GetDragRect().width, 400))
                
        # Leaves bits of itself behind sometimes
        wx.LayoutAlgorithm().LayoutWindow(self, self.canvas)
        self.canvas.Refresh()
            
        event.Skip()

    # ---------------------------------------------------------
    # Event handling for Naming Service / Manage Waveforms box
    # --------------------------------------------------------
    def OnNsBoxRightUp(self, event):
        self.nsBox.PopupMenu(self.nsBoxPopup)
        event.Skip()

    def OnNsBoxLeftDclick(self, event):
        self.DisplayWaveform()
        event.Skip()

    def OnNsBoxPopupDisplayMenu(self, event):
        self.DisplayWaveform()
        event.Skip()

    def OnNsBoxPopupStartMenu(self, event):
        self.StartApplication()
        event.Skip()

    def OnNsBoxPopupStopMenu(self, event):
        self.StopApplication()
        event.Skip()

    def OnNsBoxPopupUninstallMenu(self, event):
        self.UninstallWaveform()
        event.Skip()

    def DisplayWaveform(self):
        sn = self.nsBox.GetSelection()
        if sn == self.nsBox.GetRootItem():
            errorMsg(self,'Please select a waveform!')
            return
        snPrnt = self.nsBox.GetItemParent(sn)
        if snPrnt != self.nsBox.GetRootItem():
            errorMsg(self,'Please select a waveform!')
            return

        wave_name = self.nsBox.GetItemText(sn)
        app = self.nsBox.GetPyData(sn)
        if app is None:
            errorMsg(self,'No application associated with this entry!')
            return
            
        # Check to see if a waveform is already active
        if self.active_wave != None:
            self.refreshDisplay(True)

        # Set item bold and all others set to plain text
        troot = self.nsBox.GetRootItem()
        if self.nsBox.GetChildrenCount(troot) <= 0:
            return
        cid1,cookie1 = self.nsBox.GetFirstChild(troot)
        self.nsBox.SetItemBold(cid1, False)
        for x in range(self.nsBox.GetChildrenCount(troot,recursively=False)-1):
            cid2,cookie2 = self.nsBox.GetNextChild(troot,cookie1)
            self.nsBox.SetItemBold(cid2, False)
            cid1 = cid2
            cookie1 = cookie2

        self.nsBox.SetItemBold(sn, True)
        self.nsBox.SelectItem(sn, False)

        # Check to see if this waveform has previously been displayed
        # If so, then update the display and return
        if self.waveform_displays.has_key(wave_name):
            # Clear the canvas and get ready to display the waveform
            #print "already imported this waveform .... displaying..."
            tmpdisplay = self.waveform_displays[wave_name]
#            self.refreshDisplay(True)
            self.active_wave = tmpdisplay.waveform
            self.canvas.updateDisplay(tmpdisplay)
            return

# NOTE: use CF::FileManager list to find SAD file
#sadfile wud resemble like "/waveforms/ossie_demo/ossie_demo.sad.xml
        sadfile = app._get_profile()
        sadfile = sadfile.replace('//','')
        wav_name = sadfile.replace('.sad.xml','')
# No need to prepend /sdr/dom to sad file. CF:FileManager takes the relative path.
        #sadpath = '/sdr/dom/' + sadfile
        sadpath = sadfile

        self.active_wave = importWaveform.getWaveform(sadpath, self, self.Available_Ints)
        self.active_wave.naming_context = str(wave_name)

        tmpdisplay = self.AddWaveformShape(self.active_wave)
        self.waveform_displays[wave_name] = tmpdisplay 

        self.canvas.updateDisplay(tmpdisplay)

            
    def DisplayInstalledWaveforms(self):
        self.nsBox.DeleteAllItems()
        nsRoot = self.nsBox.AddRoot("ns_root")
        
        if self.domMgr == None or self.rootContext == None:
            return

        dom_obj = self.rootContext.resolve([CosNaming.NameComponent("DomainName1","")])
        dom_context = dom_obj._narrow(CosNaming.NamingContext) 
        if dom_context is None:
            return

        try:
            appSeq =  self.domMgr._get_applications()
        except:
            newNS = False
            while not newNS:
                ts = 'Could not got a list of applications from the domain manager. Is nodeBooter running on this system?'
                errorMsg(self, ts)
                self.namingservice_dialog = NamingserviceDialog(self)
                if self.namingservice_dialog.GetReturnCode() == 1:
                    newNS = True
            return


        members = dom_context.list(1000) 
        for m in members[0]:
            wav_name = str(m.binding_name[0].id)
            wav_obj = dom_context.resolve([CosNaming.NameComponent(wav_name,"")])
            wav_context = wav_obj._narrow(CosNaming.NamingContext)
            if wav_context is None:
                continue

            contextApp = None
            foundApp = False
            for app in appSeq:
                compNameCon = app._get_componentNamingContexts()
                for compElementType in  compNameCon:
                    if wav_name in compElementType.elementId:
                        waveformApp = app
                        foundApp = True
                        #print compElementType.componentId + " " + compElementType.elementId
                        break

            if not foundApp:
                print "Could not find associated application for: " + wav_name
                continue

            t1 = self.nsBox.AppendItem(nsRoot,wav_name)
            self.nsBox.SetPyData(t1,waveformApp)
            
            # Set item bold if it is the active waveform
            if self.active_wave is not None:
                if self.active_wave.naming_context == wav_name:
                    self.nsBox.SetItemBold(t1, True)
                    
        self.nsBox.SortChildren(nsRoot)

    # ---------------------------------------------------------
    # Event handling and control for Launch Waveforms box
    # --------------------------------------------------------
    def DisplayAvailableWaveforms(self):
        self.installBox.DeleteAllItems()
        self.availableWaveforms.clear()
        installRoot = self.installBox.AddRoot("install_root")
        
        
        sadFileObjList = self.fileMgr.list("/*.sad.xml")
        dasFileObjList = self.fileMgr.list("/*_DAS.xml")
        
        #if SAD and DAS file count differ, then report error
        if len(sadFileObjList) != len(dasFileObjList):
            errorMsg(self, "Mismatch in SAD and DAS files")
            

        for fIndex in range(len(sadFileObjList)):
            sadObj = sadFileObjList[fIndex]
            dasObj = dasFileObjList[fIndex]
            
            #get SAD and DAS file names
            #the file name is stored as an absolute path to the FileManager file system
            #e.g file name = dom/waveforms/ossie_demo/ossie_demo.sad.xml
            #throughout ALF,we store the filenames in the model as relative to the
            #FileManager root dir (/sdr/dom). Hence strip off dom/ from the object filename
        
            sad_file = sadObj.name[sadObj.name.find("/"):]
            das_file = dasObj.name[dasObj.name.find("/"):]
            #e.g. sad_file = /xml/waveforms/ossie_demo/ossie_demo.sad.xml
            #we need to extract the waveform name part from the above file name
            #find the position of the rightmost "/" and extract the substring till
            #last 8 chars (that corresponds to .spd.xml).
            wavename = sad_file[sad_file.rfind("/")+1:-8]
            
            #make sure that both SAD and DAS files are in the same directory
            #remove the file name alone and compare the directory paths of SAD and DAS file
            if ( sad_file[:sad_file.rfind("/")] != das_file[:das_file.rfind("/")] ):
                errorMsg(self, "Invalid SAD or DAS file - " + sad_file + " " + das_file )
                continue
            if ( das_file.find(wavename) == -1 ):
                errorMsg (self, "SAD and DAS files mismatch")
                continue
            
            #self.installpath is set to /sdr/dom by default
            full_sad_file = self.installPath + sad_file
            full_das_file = self.installPath + das_file
      
            
            if (not self.availableWaveforms.has_key(wavename)):
                self.availableWaveforms[wavename] = (sad_file, full_sad_file, das_file, full_das_file)
            else:
                #duplicate waveforms found
                errorMsg(self, "Duplicate " + wavename + " waveform found\n")
                continue
            
        # Populate the display at the Domain level
        for waveform in self.availableWaveforms.keys():
            t1 = self.installBox.AppendItem(installRoot,waveform)
            self.installBox.SetPyData(t1,self.availableWaveforms[waveform])
            self.installBox.SetItemBold(t1,False)

        self.installBox.SortChildren(installRoot)


    # -----------------------------------------------------------------
    # Event handling and control for Launch Components as Waveforms box
    # -----------------------------------------------------------------
    def DisplayAvailableComponents(self):
        self.componentsBox.DeleteAllItems()
        self.availableComponents.clear()
        installRoot = self.componentsBox.AddRoot("install_root")
        
        #get the list of spd files under /sdr/dom/xml
        spdFiles = ALFutils.getFileList(self.fileMgr, "dom/xml", "*.spd.xml")
        for spd_file in spdFiles:   
             compname = spd_file[spd_file.rfind("/")+1:-8]
             #installPath is set to /sdr/dom by default
             compNameAndDir = spd_file[:spd_file.rfind("/")]

             if self.availableComponents.has_key(compname):
                 errorMsg(self, "Conflicting component name: " + compname)
                 continue
            
             self.availableComponents[compname] = (compname, compNameAndDir)
    
        # Populate the display at the Domain level
        for component in self.availableComponents.keys():
            t1 = self.componentsBox.AppendItem(installRoot,component)
            self.componentsBox.SetPyData(t1,self.availableComponents[component])
            self.componentsBox.SetItemBold(t1,False)

        self.componentsBox.SortChildren(installRoot)


    def OnInstallBoxRightUp(self, event):
        self.installBox.PopupMenu(self.installBoxPopup)
        event.Skip()
    
    def OnInstallBoxPopupInstallMenu(self, event):
        self.InstallWaveformFromBox(False)
        event.Skip()
       
    def OnInstallBoxPopupInstallStartMenu(self, event):
        self.InstallWaveformFromBox(True)
        event.Skip()

    def OnInstallBoxLeftDclick(self, event):
        self.InstallWaveformFromBox(True)
        event.Skip()

    def OnComponentsBoxRightUp(self, event):
        self.componentsBox.PopupMenu(self.componentsBoxPopup)
        event.Skip()

    def OnComponentsBoxPopupInstallMenu(self, event):
        ''' Install but do not start compform '''
        self.InstallCompform(False)
        event.Skip()

    def OnComponentsBoxPopupInstallStartMenu(self, event):
        ''' Install and start the selected compform '''
        self.InstallCompform(True)
        event.Skip()

    def OnComponentsBoxLeftDclick(self, event):
        ''' Install and start the selected compform '''
        self.InstallCompform(True)
        event.Skip()

    def InstallWaveformFromBox(self, start_flag):
        selection = self.installBox.GetSelection()
        name_SAD, absolute_name_SAD, name_DAS, absolute_name_DAS = self.installBox.GetPyData(selection)

        self.InstallWaveform(name_SAD, absolute_name_SAD, 
                             name_DAS, absolute_name_DAS, start_flag)


    def InstallWaveform(self,name_SAD, absolute_name_SAD, 
                        name_DAS, absolute_name_DAS, start_flag = True):
        ''' Installs waveform application as an (SCA) application.  By default
            the (SCA) application will be started '''
         
        #name_SAD - contains the path of the SAD file relative the framework filesystem
        #e.g. the framework file system is mounted at /sdr/dom. then the value of name_SAD
        #would be /waveforms/ossie_demo/ossie_demo.sad.xml
           
        #doc_sad = xml.dom.minidom.parse(absolute_name_SAD)
        doc_sad = ALFutils.getDOM (self.fileMgr, name_SAD)
        if doc_sad is None:
            return
        app_name = doc_sad.getElementsByTagName("softwareassembly")[0].getAttribute("name")
        _appFacProps = []
        devMgrSeq = self.domMgr._get_deviceManagers()
        available_dev_seq = []
        for devmgr in range(len(devMgrSeq)):
            devMgr = devMgrSeq[devmgr]
            curr_devSeq = devMgr._get_registeredDevices()
            for dev in range(len(curr_devSeq)):
                curr_dev = curr_devSeq[dev]
                available_dev_seq.append(curr_dev._get_identifier())
                #print curr_dev._get_identifier()

        self.domMgr.installApplication(name_SAD)
        
        # Parse the device assignment sequence, ensure
        #Use FileManager to open files.
        
        doc_das = ALFutils.getDOM(self.fileMgr, name_DAS)
        deviceassignmenttypeNodeList = doc_das.getElementsByTagName("deviceassignmenttype")

        for deviceassignmenttypeNode in deviceassignmenttypeNodeList:
            # look for assigndeviceid nodes
            assigndeviceidNodeList = deviceassignmenttypeNode.getElementsByTagName("assigndeviceid")
            if len(assigndeviceidNodeList) == 0:
                ts = "Could not find \"assigndeviceid\" tag\nAborting install"
                errorMsg(self, ts)
                return

            # get assigndeviceid tag value (DCE:xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx)
            assigndeviceid = assigndeviceidNodeList[0].firstChild.data

            # ensure assigndeviceid is in list of available devices
            if assigndeviceid not in available_dev_seq:
                ts = "Could not find the required device: " + str(assigndeviceid)
                ts += "\nAborting install"
                errorMsg(self, ts)
                return
                
        _devSeq = self.BuildDevSeq(name_DAS)
        _applicationFactories = self.domMgr._get_applicationFactories()

        # attempt to match up the waveform application name to 
        # a application factory of the same name
        app_factory_num = -1
        for app_num in range(len(_applicationFactories)):
            if _applicationFactories[app_num]._get_name()==app_name:
                app_factory_num = app_num
                break
    
        if app_factory_num == -1:
            print "Application factory not found"
            sys.exit(-1)

        # use the application factor I found above to create an instance
        # of an application
        try:
            app = _applicationFactories[app_factory_num].create(_applicationFactories[app_factory_num]._get_name(),_appFacProps,_devSeq)
        except:
            print "Unable to create application - make sure that all appropriate nodes are installed"
            return(None)
        
        if start_flag:
            # start the application
            app.start()
        
        naming_context_list = app._get_componentNamingContexts()
        naming_context = naming_context_list[0].elementId.split("/")
        application_name = app._get_name()
        waveform_name = naming_context[1]
        
        
#        self.refreshDisplay()
        self.DisplayInstalledWaveforms()
        if self.connect_frame:
            self.connect_frame.refreshDisplay()

        return app

    def InstallCompform(self, start_flag = False):
        ''' This method will take the component selected from the GUI
            and create the necessary files to define a Waveform Application
            consisting of a single instance of this component.  The files will
            then be passed to the InstallWaveform method which will create
            an (SCA) application from the Waveform Application.   '''


        print "WARNING: if your node's GPP UUID is not DCE:5ba336ee-aaaa-aaaa-aaaa-00123f573a7f this will not work...\n"
        # get component name from the list
        selection = self.componentsBox.GetSelection()
        compName, compNameAndDir = self.componentsBox.GetPyData(selection)

        self.compform_counter = self.compform_counter + 1

        tmp_dir_name = "/_tmp_alf_waveforms/"  # this is where I put my temporary
                                              # xml files
        tmp_wave_name = "_" + compName + str(self.compform_counter)


        #make the directory to put the XML
        if self.fileMgr.exists(tmp_dir_name) == False:
            try:
                #for some weird reasons, fileMgr.exists(...) looks by default in /sdr/dom/
                #but mkdir(...) creates a directory under /sdr/dev :-(
                #as a temp fix, use "dom/_tmp_alf_waveforms"
                print "creating _tmp_alf_waveforms directory\n"
                self.fileMgr.mkdir("dom" + tmp_dir_name)   
            except:
                errorMsg(self,"Cannot create temporary directory in the waveform directory.\n"
                              "You may need to change the temporary directory to one that you have write permissions to")
        
        #Ticket 272:could not create a second instance of a component as a waveform if ALF is restarted.   
        #Fix: loop into _tmp_alf_waveforms/ directory and see if tmp waveforms exists already
        #with the same name. If found, increment the counter and form a new name for the temp
        #waveform. Repeat this until you find a new name.
        while (self.fileMgr.exists(tmp_dir_name + tmp_wave_name) == True ):
            print "InstallCompform(): ", tmp_wave_name + " exists already...Regenerating new name ", "at ", tmp_dir_name
            self.compform_counter = self.compform_counter + 1
            tmp_wave_name = "_" + compName + str(self.compform_counter)

        if self.fileMgr.exists(tmp_dir_name + tmp_wave_name) == False:   
            try:
                #Add "dom" to the directory because of the above said weird reasons
                print tmp_dir_name, tmp_wave_name
                self.fileMgr.mkdir("dom" + tmp_dir_name + tmp_wave_name)
                
            except:
                errorMsg(self,"Cannot create temporary directory in the waveform directory.\n"
                              "You may need to change the temporary directory to one that you have write permissions to")
                return
                
                
        #till this place, all file related operations are done through framework fileManager
        #generating temp compform files is done by Wavedev code. hence, passing the sdr install
        #path of the component files that should be in config/alf.cfg under installpath
        my_compform = compform.compform(compName, self.installPath + '/' +compNameAndDir, 
                                        self.installPath + '/' +tmp_dir_name, tmp_wave_name)
        my_compform.create()


        print "WARNING: tmp files generated in " + tmp_dir_name

        tmp_dir_name =  tmp_dir_name +  tmp_wave_name + "/"
        
        self.DisplayAvailableWaveforms()
        #the second and fourth arguments are kind of redundant. but passing to keep inline
        #with the previous version of this code. can be removed in future.
        self.InstallWaveform(tmp_dir_name + tmp_wave_name + ".sad.xml", 
                             tmp_dir_name + tmp_wave_name + ".sad.xml",
                             tmp_dir_name + tmp_wave_name + "_DAS.xml",
                             tmp_dir_name + tmp_wave_name + "_DAS.xml", 
                             start_flag)
        
    def StartApplication(self):
        selection = self.nsBox.GetSelection()
        app_ref = self.nsBox.GetPyData(selection)
        app_ref.start()
        self.DisplayInstalledWaveforms()


    def StopApplication(self):
        selection = self.nsBox.GetSelection()
        app_ref = self.nsBox.GetPyData(selection)
        app_ref.stop()
        self.DisplayInstalledWaveforms()

    def UninstallWaveform(self):
        selection = self.nsBox.GetSelection()
        waveform_name = self.nsBox.GetItemText(selection)
        if self.active_wave is not None:
            if self.active_wave.naming_context == waveform_name:
                self.refreshDisplay(True)
        if self.waveform_displays.has_key(waveform_name):
            # close any tool frames associated with waveform display
            tmp_display = self.waveform_displays[waveform_name]
            while len(tmp_display.tool_frames) > 0:
                tf = tmp_display.tool_frames.pop()
                tf.Close()
            self.waveform_displays.pop(waveform_name)
        app_ref = self.nsBox.GetPyData(selection)
#        self.domMgr.uninstallApplication(app_ref._get_identifier()) # not sure if we need this or not

        # Remove any connections we made with the connect tool to/from this waveform
        for connection in reversed(self.connections[self.namingservice[0]]):
            if waveform_name in connection['appNames']:
                try:
                    resourceRef = self.rootContext.resolve(connection['name'])
                    resourceHandle = resourceRef._narrow(CF.Resource)
                    portReference = resourceHandle.getPort(connection['port'])
                    portHandle = portReference._narrow(CF.Port)
                    portHandle.disconnectPort(connection['id'])
                    self.connections[self.namingservice[0]].remove(connection)
                except e:
                    print e
                    dial = wx.MessageDialog(self, 'Disconnect failed.', 'Failed', wx.OK)
                    dial.ShowModal()


        app_ref.releaseObject()
#        self.refreshDisplay()
        self.DisplayInstalledWaveforms()

        if self.connect_frame:
            self.connect_frame.refreshDisplay()


    def BuildDevSeq(self, dasXML):
        doc_das = ALFutils.getDOM(self.fileMgr, dasXML)
        if doc_das is None:
            return None
        # create node list of "deviceassignmenttype"
        deviceassignmenttypeNodeList = doc_das.getElementsByTagName("deviceassignmenttype")

        ds = []
        for n in deviceassignmenttypeNodeList:
            componentid = n.getElementsByTagName("componentid")[0].firstChild.data
            assigndeviceid = n.getElementsByTagName("assigndeviceid")[0].firstChild.data
            ds.append(CF.DeviceAssignmentType(str(componentid),str(assigndeviceid)))

        return ds
    

    #----------------------------------------------------------------------------
    # Waveform level controls and functions
    #----------------------------------------------------------------------------
    def refreshDisplay(self, init = False):
#        self.DisplayInstalledWaveforms()
        
        dc = wx.ClientDC(self.canvas)
        self.canvas.PrepareDC(dc)

        if init:
            if self.active_wave != None and self.timing_view_state:
                for comp in self.active_wave.components:
                    for gauge in comp.shape.gauge_shapes:
                        gauge.gauge.Show(False)
                    
            self.active_wave = None
            self.timing_display = None

            self.canvas.diagram.RemoveAllShapes()
            self.canvas.shapes = []
           
            tb = self.GetToolBar()
            tb.ToggleTool(wxID_TOOLBAR_TIMING_DISPLAY_TOOL, False)
            self.timing_view_state = False
            tb.SetToolShortHelp(wxID_TOOLBAR_TIMING_DISPLAY_TOOL, "Enable Timing Graphics")
            tb.ToggleTool(wxID_TOOLBAR_TIMING_TOOL, False)
            tb.SetToolShortHelp(wxID_TOOLBAR_TIMING_TOOL, "Enable Timing")
            #self.canvas.diagram = ogl.Diagram()
            #self.canvas.SetDiagram(self.canvas.diagram)
            #self.canvas.diagram.SetCanvas(self.canvas)
        
        self.canvas.Refresh()

    def AddWaveformShape(self, waveform):
        tmpdisplay = ALFshapes.WaveformShapes(waveform, self.canvas)
        for comp in waveform.components:
            tmpdisplay.AddComponentShape(comp)
        tmpdisplay.ConnectComponents()

        return tmpdisplay


    def updateWaveformData(self, data):
        for d in data:
            self.waveformData[d[0]] = d[1]

        self.last_waveform_data_update = self.waveformData.copy()

        for frame in self.tool_frames:
            if hasattr(frame, 'updateWaveformData'):
                frame.updateWaveformData(self.waveformData)

    def removeToolFrame(self, frame):
        if frame not in self.tool_frames:
            return
        else:
            index = self.tool_frames.index(frame)
            del self.tool_frames[index]
            
    def OnClose(self, event):
        if self.nodeBooterProcess != None:
            dlg = wx.MessageDialog(self,
                                   "NodeBooter was started by ALF.\n" +
                                   "Would you like to terminate it?",
                                   "Confirm NodeBooter Termination", wx.YES | wx.NO | wx.ICON_QUESTION)
            result = dlg.ShowModal()
            if result == wx.ID_YES:
                print "Terminating nodeBooter"
                self.nodeBooterProcess.terminate()
        self.Destroy()
    
    
    
#----------------------------------------------------------------------
class MainWindow(ogl.ShapeCanvas):
    def __init__(self, parent, frame):
        ogl.ShapeCanvas.__init__(self, parent)

        maxWidth  = 3000
        maxHeight = 1500
        self.SetScrollbars(20, 20, maxWidth/20, maxHeight/20)

        self.frame = frame
        self.SetBackgroundColour("LIGHT BLUE") #wxWHITE)
        self.diagram = ogl.Diagram()
        self.SetDiagram(self.diagram)
        self.diagram.SetCanvas(self)
        self.shapes = []

        dsBrush = wx.Brush("WHEAT", wx.SOLID)
        
    def updateDisplay(self, waveformdisplay):
        dc = wx.ClientDC(self)
        self.PrepareDC(dc)
        for compshape in waveformdisplay.shapes:
            self.AddShape(compshape)
            compshape.Show(True)
        
        self. diagram.Redraw(dc)
        self.Refresh()
        								
    def OnDestroy(self, evt):
        # Do some cleanup
        for shape in self.diagram.GetShapeList():
            if shape.GetParent() == None:
                shape.SetCanvas(None)
                shape.Destroy()
        self.diagram.Destroyparent
        
    def setOSSIEProperty(self, sectionName, propertyName, propertyValue):
        configFile = os.getenv('HOME') + '/' + OSSIE_CONFIG_FILE
        configParser = ConfigParser.SafeConfigParser()
        #check to see if a config file already exists
        if os.path.exists(configFile):
            #if so, read in its properties so they don't get lost
            configParser.read(configFile)
        if not configParser.has_section(sectionName):
            configParser.add_section(sectionName)
        configParser.set(sectionName, propertyName, propertyValue)
        f = open(configFile, 'w')
        configParser.write(f)
        f.close()
            
    def getOSSIEProperty(self, sectionName, propertyName):
        configFile = os.getenv('HOME') + '/' + OSSIE_CONFIG_FILE
        if os.path.exists(configFile):
            configParser = ConfigParser.SafeConfigParser()
            configParser.read(configFile)
            if configParser.has_option(sectionName, propertyName):
                return configParser.get(sectionName, propertyName)
        return None
#----------------------------------------------------------------------

def errorMsg(self,msg):
    dlg = wx.MessageDialog(self,msg,'Error', wx.OK | wx.ICON_INFORMATION)
    try:
        dlg.ShowModal()
    finally:
        dlg.Destroy()
    return
                                

def main():
    app = alfApp(0)
    app.MainLoop()


# # If not profiling:
if __name__ == '__main__':
    main()


'''
# A script created by Drew Cormier for profiling
# code developed for his thesis 
if __name__ == "__main__":

    import profile
    import pstats

    # bias the profile to remove profiling overhead
    # note that the number being passed to 
    # bias varies based on what system I am using
    #
    # Aquire this number for my linux machine using
    # >>> p.calibrate(10000000)
    profile.bias = 1.6989519401052302e-05

    profile.run("main()", "alf_profile_1")

    my_stats = pstats.Stats("alf_profile_1")

    my_stats.sort_stats('cumulative')
    my_stats.print_stats(10)

    my_stats.sort_stats('time')
    my_stats.print_stats(10)
'''







