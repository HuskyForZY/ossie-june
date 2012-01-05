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

import wx
import os
import WaveformModel
import WavedashUtils as utils
from wx.lib import scrolledpanel
from WaveformModel import WaveformListener
from ComponentModel import ComponentListener
from PropertyModel import PropertyListener
from WavedashButton import WavedashButton
from WavedashController import Controller
from WavedashPreferencePage import WavedashPreferencePage
from NSChoiceDialog import NSChoiceDialog
from NodeBooterDialog import NodeBooterDialog

OSSIE_WAVEAPP_DIMENSION = (600,600)                                                    
OSSIE_TITLE = 'WaveDash Waveform Dashboard - OSSIE Waveform Workshop'
START_ICON_FILE = "/resources/start.png"
STOP_ICON_FILE = "/resources/stop.png"
UNINSTALL_ICON_FILE = "/resources/LET-U.jpg"
NAMINGSERVICE_ICON_FILE = "/resources/network.png"
REFRESH_ICON_FILE = "/resources/refresh.png"
NODEBOOTER_ICON_FILE = "/resources/nbstart.png"


class WavedashView(wx.Frame, WaveformListener, 
                   ComponentListener, PropertyListener):
    """ This is top level frame that acts as primary UI """
    def __init__(self, controller, amodel):
        wx.Frame.__init__(self, parent=None, id=-1, title=OSSIE_TITLE )
#                          size = OSSIE_WAVEAPP_DIMENSION)
        #Create the model and register the frame to the listeners of 
        #Waveform, Component and Property Models.
        
        self.controller = controller
        self.model = amodel
        self.model.addWaveformListener(self)
        self.model.addComponentListener(self)
        self.model.addPropertyListener(self)
        
        self.toolbar = None
        self.createToolBar()
        
        #notebook to hold the waveform panels
        self.mainPanel = wx.Panel(self)
        self.notebook = wx.Notebook(self.mainPanel)
        #bind notebook to page changed event
        self.notebook.Bind(wx.EVT_NOTEBOOK_PAGE_CHANGED, self.OnNbPageChange, self.notebook)
        
        #sizers to manage the notebook
        self.nbSizer = wx.BoxSizer()
        self.nbSizer.Add(self.notebook, 1, wx.EXPAND)
        self.mainPanel.SetSizer(self.nbSizer)
        
        self.statusBar = None
        self.createMenus()
        
        
        
        
        self.wpanels = [] # this list holds the panel objects of the installed waveforms
        self.prpPopupMenus = {} #this dict is of the form {'wPanelRef':{window, popUpMenu}}
        self.compPopupMenus = {}
        self.tempProp = None
        self.tempComp = None
        
        self.configureButtons = []
        
        self.trueValues = ["t", "true", "yes", "on", "1"]
        
            
        self.Bind(wx.EVT_CLOSE, self.OnClose)
        
        self.SetSize(OSSIE_WAVEAPP_DIMENSION)
        self.Center()
        self.Show()
        
        if not self.controller.namingServiceIsRunning():
            nscd = NSChoiceDialog(self)        
    
          
    def createToolBar(self):
        self.toolbar = self.CreateToolBar()
        rsize = (20, 20)
        refreshImg = wx.ArtProvider.GetBitmap(wx.ART_REDO, wx.ART_TOOLBAR, rsize)
        refresh = self.toolbar.AddSimpleTool(-1,refreshImg ,shortHelpString="Refresh")
        self.Bind(wx.EVT_TOOL, self.OnRefresh, refresh)
        
        root = self.getRoot()
        
        selectNSIcon = wx.Image(root + NAMINGSERVICE_ICON_FILE, wx.BITMAP_TYPE_ANY).ConvertToBitmap()
        selectNS = self.toolbar.AddSimpleTool(-1, selectNSIcon, shortHelpString="Configure Naming Service address")
        
        startIcon = wx.Image(root + START_ICON_FILE, wx.BITMAP_TYPE_ANY).ConvertToBitmap()
        stopIcon = wx.Image(root + STOP_ICON_FILE, wx.BITMAP_TYPE_ANY).ConvertToBitmap()
        uninstallIcon = wx.Image(root + UNINSTALL_ICON_FILE, wx.BITMAP_TYPE_ANY).ConvertToBitmap()
        start = self.toolbar.AddSimpleTool(-1, startIcon, shortHelpString="Start waveform")
        stop = self.toolbar.AddSimpleTool(-1, stopIcon, shortHelpString="Stop Waveform")
        uninstall = self.toolbar.AddSimpleTool(-1, uninstallIcon, shortHelpString="Uninstall waveform")
        
        startNodeBooterIcon = wx.Image(root + NODEBOOTER_ICON_FILE, wx.BITMAP_TYPE_ANY).ConvertToBitmap()
        startNodeBooter = self.toolbar.AddSimpleTool(-1, startNodeBooterIcon, shortHelpString="Start NodeBooter")
        
        
        self.Bind(wx.EVT_TOOL, self.OnStart, start)
        self.Bind(wx.EVT_TOOL, self.OnStop, stop)
        self.Bind(wx.EVT_TOOL, self.OnUninstall, uninstall)
        self.Bind(wx.EVT_TOOL, self.OnSelectNS, selectNS)
        self.Bind(wx.EVT_TOOL, self.OnStartNodeBooter, startNodeBooter)
    
    def OnStart(self, event):
        activeWform = self.model.getActiveWaveform()
        if activeWform is None:
                utils.showMessage("Start Failed!!! No waveform is running now",
                                    utils.NON_FATAL)
                return
        status = self.controller.startWaveform(activeWform.getName())
    
    def OnStop(self, event):
        activeWform = self.model.getActiveWaveform()
        if activeWform is None:
            utils.showMessage("Stop Failed!!! No waveform is running now", 
                               utils.NON_FATAL)
            return
        status = self.controller.stopWaveform(activeWform.getName())
    
    def OnUninstall(self, event):    
        selWform = self.model.getActiveWaveform()
        if selWform is None:
                utils.showMessage("Uninstall Failed!!! No waveform is running now",
                                   utils.NON_FATAL)
                return

        #iterate through a slice of the whole list rather than
        #the list itself so prevent errors
        for button in self.configureButtons[:]:
            if(button.isWaveformButton == True and button.getParentName() == selWform.getName()):
                self.configureButtons.remove(button)

        for button in self.configureButtons[:]:
            if(button.isComponentButton == True and button.getGrandParentName() == selWform.getName()):
                self.configureButtons.remove(button)
                 
        success = self.model.removeWaveform(selWform)
            
    def OnSelectNS(self, event):
        self.controller.selectNS()
        
    def OnStartNodeBooter(self, event):
        nbd = NodeBooterDialog(self)
        
        if nbd.GetReturnCode() == 0:
            self.controller.CORBAutils.setNamingService('127.0.0.1')
            self.controller.CORBAutils.init_CORBA()
            self.controller.refresh()

    def createMenus(self):
        menuBar = wx.MenuBar()
        #These features are not yet implemented in 0.7.3
#        fileMenu = wx.Menu()
#        openLayout = fileMenu.Append(-1, '&Open Layout')
#        saveLayout = fileMenu.Append(-1, '&Save Layout')
#        saveLayoutAs = fileMenu.Append(-1, 'S&ave Layout As')
#        exit = fileMenu.Append(-1, 'E&xit\tCtrl-Q')
#        
#        self.Bind(wx.EVT_MENU, self.OnOpenLayout, openLayout)
#        self.Bind(wx.EVT_MENU, self.OnSaveLayout, saveLayout)
#        self.Bind(wx.EVT_MENU, self.OnSaveLayoutAs, saveLayoutAs)
#        self.Bind(wx.EVT_MENU, self.OnExit, exit)    
#        
        waveformsMenu = wx.Menu()      
        componentsMenu = wx.Menu()
        optionsMenu = wx.Menu()
        self.updateOnRefresh = optionsMenu.Append(501, 'Update Properties on Refresh', 'Update Properties on Refresh', kind=wx.ITEM_CHECK)
        self.preferences = optionsMenu.Append(502, 'Wavedash Preferences', 'Wavedash Preferences', kind=wx.ITEM_NORMAL)
        self.updateOnRefresh.Check(False)
        self.Bind(wx.EVT_MENU, self.OnUpdateOnRefreshCheck, self.updateOnRefresh)
        self.Bind(wx.EVT_MENU, self.OpenPreferences, self.preferences)
        
#        preferencesMenu = wx.Menu()
#        widgetSettings = preferencesMenu.Append(-1, '&Widget Settings')
#        self.Bind(wx.EVT_MENU, self.OnWidgetSettings,widgetSettings)
#        
        helpMenu = wx.Menu()
        about = helpMenu.Append(-1,'&About')
        self.Bind(wx.EVT_MENU, self.OnAbout, about)
        
#        menuBar.Append(fileMenu, '&File')
        menuBar.Append(waveformsMenu, '&Waveforms')
        #waveformsMenu.AppendSeparator()
        menuBar.Append(componentsMenu, '&Components')
        #menuBar.Append(preferencesMenu, '&Preferences')
        menuBar.Append(optionsMenu, '&Options')
        menuBar.Append(helpMenu, '&Help')
        
        self.SetMenuBar(menuBar)
    
    def createWaveformPanel(self, parent, waveform, preview = False):
       # if(waveform.name == "w1_1"):
            #print "in createWaveform Panel"
            #for comp in waveform.components:
             #   for prop in comp.properties:
              #      print str(prop.id) + ": "  + str(prop.getValue())
        
        wPanel = scrolledpanel.ScrolledPanel(parent, -1, name=waveform.getName())
        wSizer = wx.BoxSizer(wx.VERTICAL)
        componentList = waveform.getAllComponents()
        
        
                    
        for component in componentList:
            cPanel = self.createComponentPanel(wPanel,component, preview)
            wSizer.Add(cPanel, 0, wx.ALIGN_LEFT | wx.GROW, 10)
            
        wSizer.AddSpacer(10)
        
        root = self.getRoot()
        refreshIcon = wx.Image(root + REFRESH_ICON_FILE, wx.BITMAP_TYPE_ANY).ConvertToBitmap()

        updateWaveformButton = WavedashButton(wPanel, wx.NewId(), refreshIcon)
        updateWaveformButton.setParentName(waveform.getName())
        updateWaveformButton.setGrandParentName("none")
        updateWaveformButton.setIsWaveformButton()
        updateWaveformButton.Enable(self.updateOnRefresh.IsChecked())
        wPanel.Bind(wx.EVT_BUTTON, self.OnUpdateWaveformClicked, updateWaveformButton)

        self.configureButtons.append(updateWaveformButton)
        wSizer.Add(updateWaveformButton, flag=wx.ALIGN_RIGHT)
            
        wPanel.SetSizer(wSizer)
        wPanel.SetupScrolling(True, True, 20, 20)
        
        #No need to add the panels to the wpanels list when preview is On
        if preview is False:
            self.wpanels.append(wPanel)
        
        wSizer.Fit(wPanel)
        return wPanel
               
    def createComponentPanel(self, wPanel, component, preview = False):
        cPanel = wx.Panel(wPanel, -1, name=component.getName())
        component.setPanel(cPanel)
        sbox = wx.StaticBox(cPanel, -1, component.getName())
        sboxSizer = wx.StaticBoxSizer(sbox, wx.VERTICAL)
        prpWidgetList = []

        root = self.getRoot()

        if not component.isVisible():
            cPanel.Hide()
        
        propertyList = component.getAllProperties()
        #properites of a component are laid on the component panel using a gridbag sizer
        #Each widget takes one cell except a Slider which spans across multiple columns
       # if(component.parent.name == "w1_1"):
        #    print "in addComponentPanel"
         #   for prop in component.properties:
          #      print str(prop.id) + ": "  + str(prop.getValue())
        
        for property in propertyList:
            widget = property.getWidget()
            
            plabel = wx.StaticText(cPanel, -1, property.getName(), name = property.getName())
            pWindow = self.mapWidgetToWindow(cPanel, property, widget)
            pWindow.SetToolTipString(property.getDesc())
            property.setWindow(pWindow)
            
            if not property.isVisible():
                plabel.Hide()
                pWindow.Hide()
            
            if preview is True:
                #Don't allow any manipulation on properties in Preview form.
                pWindow.Enable(False)
                
            #Create a popup menu for this property 
            self.attachPopupMenuToProp(wPanel, pWindow, property)
            
            windowType = pWindow.__class__
            eventType = wx.EVT_KILL_FOCUS
            
            #Find the type of event corresponding to the window in use
            if windowType is wx.TextCtrl or windowType is wx.SpinCtrl:
                pWindow.Bind(wx.EVT_TEXT_ENTER, self.OnPropValueChanged, pWindow)
                pWindow.Bind(wx.EVT_KILL_FOCUS, self.OnPropValueChanged, pWindow)
            elif windowType is wx.Slider:
                pWindow.Bind(wx.EVT_COMMAND_SCROLL, self.OnPropValueChanged, pWindow)
            elif windowType is wx.CheckBox:
                pWindow.Bind(wx.EVT_CHECKBOX, self.OnPropValueChanged, pWindow)
            
            #checkbox has a label associated with itself. so no need to add separate label 
            if pWindow.__class__ is wx.CheckBox:
                plabel.SetLabel("")
                
    
            
            prpWidgetList.append(plabel)
            prpWidgetList.append(pWindow)
        
        #prpWidgetList contains the label and widgets for all the properties of this component
        #addPropToCompSizer creates a new gridbagsizer and adds the labels and widgets contained
        #in the prpWidgetList
        
        gbagSizer = self.addPropToCompSizer(prpWidgetList)
        
        
        
        sboxSizer.SetMinSize(gbagSizer.GetMinSize())
        sboxSizer.Add(gbagSizer, 1, wx.ALIGN_CENTER | wx.EXPAND, 10)
        
        refreshIcon = wx.Image(root + REFRESH_ICON_FILE, wx.BITMAP_TYPE_ANY).ConvertToBitmap()
        
        updateComponentButton = WavedashButton(cPanel, wx.NewId(), refreshIcon )
        updateComponentButton.setParentName(component.getName())
        updateComponentButton.setGrandParentName(component.getParent().getName())
        updateComponentButton.setIsComponentButton()
        updateComponentButton.Enable(self.updateOnRefresh.IsChecked())
        cPanel.Bind(wx.EVT_BUTTON, self.OnUpdateComponentClicked, updateComponentButton)
        self.configureButtons.append(updateComponentButton)
        sboxSizer.Add(updateComponentButton, flag=wx.ALIGN_RIGHT)  
        
        
        
        cPanel.SetSizer(sboxSizer)
     
        #After layingout the widgets (labels & controls) for all the properties, 
        #attach a popupmenu at component level. i.e. for each component panel
        
        self.attachPopupToComponent(component, cPanel)
        return cPanel
    
    def addPropToCompSizer(self, propList):
        #IMPORTANT NOTE: proplist should have items in the following way.
        #[label, window, label, window, label, window, .... ]
        index = 0
        gridSizer = wx.GridSizer(2,4,5,5)
       
        while (index < len(propList) ):
            plabel = propList[index]
            pWindow = propList[index+1]
            
            #if the windows are hidden, don't add them to the sizer.
            #They will be added when the user select the properties again.
            if (not plabel.IsShown()) or (not pWindow.IsShown()):
                index = index + 2
                continue
            gridSizer.Add(plabel, 0)
            gridSizer.Add(pWindow, 0)
            index = index + 2
        return gridSizer
     
    def attachPopupToComponent(self, component, cPanel):
        cPopup = wx.Menu(title='')
        moveUp = cPopup.Append(wx.NewId(), 'Move up')
        moveDown = cPopup.Append(wx.NewId(), 'Move Down')
        save = cPopup.Append(wx.NewId(), 'Save')
        saveAs = cPopup.Append(wx.NewId(), 'Save As')
        
        #save and saveAs will be implemented in next release.disable for now
        save.Enable(False)
        saveAs.Enable(False)
        
        prpMenu = wx.Menu()
        #All properties are enabled by default.
        prpList = component.getAllProperties()
        for prp in prpList:
            chkMenuItem = prpMenu.AppendCheckItem(wx.NewId(), prp.getName())
            chkMenuItem.Check(True)
            self.Bind(wx.EVT_MENU, self.OnPropertySelect, chkMenuItem)
            
        cPopup.AppendMenu(wx.NewId(), 'Properties', prpMenu)
        
        wPanel = cPanel.GetParent()
        
        if self.compPopupMenus.has_key(wPanel):
            cPanelDict = self.compPopupMenus[wPanel]
            if cPanelDict.has_key(cPanel.GetName()) is False:
                cPanelDict[str(cPanel.GetName())] = cPopup
        else:
            self.compPopupMenus[wPanel] = {cPanel.GetName():cPopup}
        
        self.Bind(wx.EVT_MENU, self.OnComponentMoveUp, moveUp)
        self.Bind(wx.EVT_MENU, self.OnComponentMoveDown, moveDown)
        
        cPanel.Bind(wx.EVT_RIGHT_DOWN, self.OnComponentRightClick)
        
    
    def OnComponentRightClick(self, event):
        src = event.GetEventObject()
        curWPanel = self.getCurrentPanel()
        #Storing the reference to current component panel in a temp variable
        #This temp will be used to map the context menu and the component menu in
        #the event handler for PropertySelect and other items in Context menu also.
        
        self.tempComp = src
        for wpanel in self.compPopupMenus.keys():
            if wpanel is curWPanel:
                windowDict = self.compPopupMenus[wpanel]
                src.PopupMenu(windowDict[str(src.GetName())])
    
    def attachPopupMenuToProp(self, wPanel, window, property):
        pSubMenu = wx.Menu(title='')
        #default = property.getWidget()
        default = self.controller.getDefaultWidget(property.getType())
        optList = self.controller.getOptionalWidgets(property.getType())
        
        mItem = pSubMenu.AppendRadioItem(wx.NewId(), default.type)
        self.Bind(wx.EVT_MENU, self.PropPopupHandler, id = mItem.GetId())
        for optWidget in optList:
            mItem = pSubMenu.AppendRadioItem(wx.NewId(), optWidget.type)
            self.Bind(wx.EVT_MENU, self.PropPopupHandler, id = mItem.GetId())
            if property.getWidget().type == optWidget.type:
                mItem.Check(True)
            
        pSubMenu.AppendSeparator()
        configure = pSubMenu.Append(wx.NewId(), 'Configure')
        self.Bind(wx.EVT_MENU, self.PropConfigureHandler, id = configure.GetId())
        
        #Configure shuold be enabled only for slider and spin buttons.
        #If other options are enabled, then disable the Configure Menu Item
        self.propPopUpUpdate(pSubMenu)
        
        chgMenu = wx.Menu(title='')
        chgMenuItem = wx.MenuItem(chgMenu, wx.NewId(), 'Change To', subMenu = pSubMenu)
        chgMenu.AppendItem(chgMenuItem)
        
        #Associate the pop Menu with the window
        cpRef = property.getParent().getName() + '/' + property.getName()
        
        if self.prpPopupMenus.has_key(wPanel):
            subDict = self.prpPopupMenus[wPanel]
            if subDict.has_key(cpRef) is False:
                subDict[cpRef] = chgMenu
        else:
            self.prpPopupMenus[wPanel] = {cpRef:chgMenu}
        
        window.Bind(wx.EVT_RIGHT_DOWN, self.OnPropRightClick)
        
    def OnPropRightClick(self, event):
        src = event.GetEventObject()
        curPanel = self.getCurrentPanel()
        #The widget(actual wxPython windows) on which the Popupmenu is invoked is
        #stored in a temp variable to access it later during the events generated 
        #by PopUpMenu
        
        self.tempProp = src
        for wpanel in self.prpPopupMenus.keys():
            if wpanel is curPanel:
 
        #===============================================================================
        #  A pop-up menu is associated with each property. the member variable prpPopupMenus
        #  is a hash object indexed by waveform names. Value of a hash object points to another
        #  hash which indexes the popup menus by component_name/property_name.
        #=============================================================================== 

                windowDict = self.prpPopupMenus[wpanel]
                cpRef = src.GetParent().GetName() + '/' + src.GetName()
                src.GetParent().PopupMenu(windowDict[cpRef])
            
    
    def PropPopupHandler(self, event):
        srcId = event.GetId()
        mItems = event.GetEventObject().GetMenuItems()
        
        #Enable the configure option only slider and spin box to set their min and
        #max values.
        
        self.propPopUpUpdate(event.GetEventObject())
        
        for mItem in mItems:
            if mItem.GetId() == srcId:        
                #self.tempProp is set to identify the property when Popupmenu is invoked.    
                property = self.tempProp
                component = property.GetParent()
                wform = component.GetParent()
                wformRef = self.model.getActiveWaveform()
                compRef = wformRef.getComponent(component.GetName())
                propRef = compRef.findPropertyByName(property.GetName())
                self.model.changePropertyWidget(component.GetName(), 
                                                property.GetName(), mItem.GetItemLabel())
                

    def PropConfigureHandler(self, event):
        propertyCtrl = self.tempProp
        cPanel = propertyCtrl.GetParent()
        wPanel = cPanel.GetParent()
        
        #Get reference to the property thruogh its compnent and waveform.
        #Then, get the widget reference from the property
        wform = self.model.getActiveWaveform()
        component = wform.getComponent(cPanel.GetName())
        property = component.findPropertyByName(propertyCtrl.GetName())
        
        widget = property.getWidget()
        
        if propertyCtrl.__class__ is wx.SpinCtrl \
            or propertyCtrl.__class__ is wx.Slider:
            curMin, curMax = property.getRange()
            curValue = property.getValue()
            #curMin = widget.getMin()
            #curMax = widget.getMax()
            title = 'Widget Configuration'
            confDialog = utils.ConfigureWidget(self, title, curValue)
            confDialog.setMin(curMin)
            confDialog.setMax(curMax)
            
            confDialog.ShowModal()
            
            #Get the new min and max once the dialog is destroyed in the
            #event handlers of Ok and Cancel defined in ConfigureWidget
            newMin = confDialog.getMin() 
            newMax = confDialog.getMax()
                        
            #No need to udpate the model if the values are not changed
            if (newMin == curMin) and (newMax == curMax):
                return
            else:
                # the newMin and newMax values will be updated to the range attribute of the property
                status = self.model.configurePropertyWidget(cPanel.GetName(), propertyCtrl.GetName(), newMin, newMax)
                if status is False:
                    utils.showMessage("Invalid range of values (" + str(newMin) + "," + str(newMax) + ")" , utils.NON_FATAL)
                return
    
    def propPopUpUpdate(self, menu):     
        menuItems = menu.GetMenuItems()
        confItemId = menu.FindItem('Configure')
        confItem = menu.FindItemById(confItemId)
        confItem.Enable(False)
        
        for mItem in menuItems:
            if mItem.GetItemLabel() == "spin" or mItem.GetItemLabel() == "slider":
                if mItem.IsChecked():
                    confItem.Enable(True) 
        
    def mapWidgetToWindow(self, parent, property, widget):
        window = None
        label = None
        pValue = property.getValue()

        widgetType = widget.type
        
        if widgetType == "text":                    
            window = wx.TextCtrl(parent, -1, str(pValue), style = wx.TE_PROCESS_ENTER)
        elif widgetType == "spin":
            minV, maxV = property.getRange()
            window = wx.SpinCtrl(parent, -1, str(pValue), min = minV, max = maxV, style = wx.TE_PROCESS_ENTER)
        elif widgetType == "slider":
            minV, maxV = property.getRange()
            if widget.parameters["style"] == "HORIZONTAL":
                styleV = wx.SL_HORIZONTAL | wx.SL_LABELS
            elif widget.parameters["style"] == "VERTICAL":
                styleV = wx.SL_VERTICAL | wx.SL_LABELS
            window = wx.Slider(parent, -1, int(pValue), minValue = minV, maxValue = maxV, style = styleV, size=(120,35))
        elif widgetType == "checkbox":
            window = wx.CheckBox(parent, -1, property.getName())
        elif widgetType == "listbox":
            if (pValue is not None) and (type(pValue) is list):
                window = wx.TextCtrl(parent, -1, str(pValue))
            else:
                window = wx.TextCtrl(parent, -1, str(None))
        else:
            window = wx.TextCtrl(parent, -1, str(pValue))
        
        if window is not None:
            window.SetName(property.getName())    
        return window
    
    def OnPropValueChanged(self, event):
        if(self.updateOnRefresh.IsChecked()):
            return
        src = event.GetEventObject()
        prp = src.GetName()
        comp = src.GetParent().GetName()
        wName = src.GetParent().GetParent().GetName()
        wform = self.model.getWaveform(wName, WaveformModel.INSTANCE_WAVEFORM)
        #This event is triggered whenever a property widget loses its focus.
        #A widget loses its focus when the user switch to another widget in the same window
        #or select a different waveform.
        if wform is None:
            #if event triggered as a result of object destroy or from Waveform Preview Dialog
            #then no need to update the value. Just return
            return
        
        compRef = wform.getComponent(comp)
        prpRef = compRef.findPropertyByName(prp)
        
        curVal = self.formatWidgetToModel(src.GetValue(), prpRef.getType())
        preVal = prpRef.getValue()
        
     
        if curVal is not None:
            
            #if both the values are equal then no need to call configure()
            if preVal == curVal:
                #in cases where a boolean property was changed to a different
                #text value but the same boolean value (ie True changed to 1)
                #the model won't configure since the actual value didnt change
                #so we have to call formatModelToWidget to change the text back
                # to True or False
                if(prpRef.type[1] == "boolean"):
                    self.formatModelToWidget(src, curVal)
                return
            else:
                #print "configuring: wName = " + str(wName) + ", comp = " + str(comp) + ", prp = " + str(prp) + ", curVal = " + str(curVal)
                status = self.model.configure(wName, comp, prp, curVal)
                
                #leave the text box as it is if status is True
                #if the configure operation failed, revert the text value to the value stored in 
                #the model.
                if status is False:
                    msg = "Could not configure " + prp
                    utils.showMessage(msg, utils.NON_FATAL)
                    self.formatModelToWidget(src, preVal)
                    #src.SetValue(str(preVal))
                elif prpRef.type[1] == "boolean":
                    self.formatModelToWidget(src, curVal)
                
        else:
            msg = "Invalid value for %s/%s" % (comp, prp) + "\nEnter only '" + prpRef.getType()[1] + "' values"
            msg = msg + "\nFor sequence types, enter values separated by comma" 
            msg = msg + " and enclosed within [] e.g. [10,20,30]"
            
            utils.showMessage(msg, utils.NON_FATAL)
            self.formatModelToWidget(src, preVal)     
            self.Layout()   
            
    
    def formatModelToWidget(self, widget, value):
        if widget.__class__ is wx.TextCtrl:
            widget.SetValue(str(value))
        elif widget.__class__ is wx.SpinCtrl:           
            widget.SetValue(int(value))
        elif widget.__class__ is wx.Slider:
            widget.SetValue(int(value))
        elif widget.__class__ is wx.CheckBox:
            if value == "True" or value == 1:
                widget.SetValue(True)
            else:
                widget.SetValue(False)
        
                
         
    def formatWidgetToModel(self, curVal, propType):
        """ Converts the string literal into the suitable data type
        as configured for the property"""
        val = None
        #propType is a tuple containing the (xmlType,datatype)
        #e.g. ('simple','short')
        xmlType = propType[0]
        dtype = propType[1]

        try:
            if (xmlType == "simple"):
                if dtype == "short" or dtype == "ushort":
                    val = int(curVal)
                elif dtype == "int" or dtype == "uint":
                    val = int(curVal)
                elif dtype == "long" or dtype == "ulong":
                    val = long(curVal)
                elif dtype == "double" or dtype == "float":
                    val = float(curVal)
                elif dtype == "char":
                    val = str(curVal) 
                elif dtype == "boolean":
                    if str(curVal).lower() in self.trueValues:
                        val = True
                    else:
                        val = False
                elif dtype == "string":
                    val = str(curVal)
                else:
                    val = None
            #sequence values are represented as list of values separated
            #by a comma. e.g. '[10,20,30]'
            
            elif (xmlType == "simplesequence"):
                if dtype == "float":
                    val = []
                    #strip the open and close bracket
                    listVal = curVal.lstrip('[')
                    listVal = listVal.rstrip(']')
                    for lVal in listVal.split(','):
                        val.append(float(lVal))
                if dtype == "string":
                    val = []
                    #strip the open and close bracket
                    listVal = curVal.lstrip('[')
                    listVal = listVal.rstrip(']')
                    for lVal in listVal.split(','):
                        val.append(str(lVal))
                if dtype == "short" or dtype == "int":
                    val = []
                    #strip the open and close bracket
                    listVal = curVal.lstrip('[')
                    listVal = listVal.rstrip(']')
                    for lVal in listVal.split(','):
                        val.append(int(lVal))
                    
        except(ValueError):
            val = None
        
        return val
        
    def AddToWaveformsMenu(self, newWaveform, type):
        wMenu = self.FindMenuInMenuBar('Waveforms')
        
        if type == WaveformModel.SYSTEM_WAVEFORM:
            wSubMenu = wx.Menu()
            install = wSubMenu.Append(-1, 'Install')
            installAndStart = wSubMenu.Append(-1, 'Install and Start')
            viewWform = wSubMenu.Append(-1, 'Preview')
            properties = wSubMenu.Append(-1, 'Properties')
            
            wMenu.InsertMenu(0,-1, newWaveform, wSubMenu) #insert at the start of the list
            
            self.Bind(wx.EVT_MENU, self.OnWaveformInstall, install)
            self.Bind(wx.EVT_MENU, self.OnWaveformInstallAndStart, installAndStart)
            self.Bind(wx.EVT_MENU, self.OnWaveformPreview, viewWform)
            self.Bind(wx.EVT_MENU, self.OnWaveformProperties, properties)
                
    def AddToComponentsMenu(self, newComponent, isVisible):
        cMenu = self.FindMenuInMenuBar('Components')
        citem = cMenu.AppendCheckItem(-1, newComponent)
        self.Bind(wx.EVT_MENU, self.OnComponentMenuClick, citem)
        
        if isVisible:
            citem.Check(True)
    
    def OnWaveformInstall(self, event):
        chosenMenu = event.GetEventObject()
        parent = chosenMenu.GetParent()
        
        for mItem in parent.GetMenuItems():
            if mItem.GetSubMenu() is chosenMenu:
                self.controller.installWaveform(mItem.GetItemLabel(), False)
                break
    
    def OnWaveformInstallAndStart(self, event):
        chosenMenu = event.GetEventObject()
        parent = chosenMenu.GetParent()
        
        for mItem in parent.GetMenuItems():
            if mItem.GetSubMenu() is chosenMenu:
                self.controller.installWaveform(mItem.GetItemLabel(), True)
                break
    
    def OnPropertySelect(self, event):
        srcId = event.GetId()
        pMenu = event.GetEventObject()
        mItemList = pMenu.GetMenuItems()
        for mItem in mItemList:
            if srcId == mItem.GetId():
                propName = mItem.GetItemLabel()
                #self.tempComp has the reference to the component panel on which
                #the context menu is invoked in ComponentRightClick Handler
                compName = self.tempComp.GetName()
                self.model.updatePropertyState(compName, propName, mItem.IsChecked())
                break
    
    def OnComponentMoveUp(self, event):       
        compName = self.tempComp.GetName()
        wformName = self.tempComp.GetParent().GetName()
        self.model.moveComponentUp(wformName, compName)
    
    def OnComponentMoveDown(self, event):
        compName = self.tempComp.GetName()
        wformName = self.tempComp.GetParent().GetName()
        self.model.moveComponentDown(wformName, compName)
            
    
    def OnWaveformPreview(self, event):
        chosenMenu = event.GetEventObject()
        parent = chosenMenu.GetParent()
        
        for mItem in parent.GetMenuItems():
            if mItem.GetSubMenu() is chosenMenu:
                wformName = mItem.GetItemLabel()
                waveform = self.model.getWaveform(wformName, WaveformModel.SYSTEM_WAVEFORM)
                title = 'Preview - ' + wformName 
                
                previewFrame = wx.Frame(self, -1, title, OSSIE_WAVEAPP_DIMENSION)
                wPanel = self.createWaveformPanel(previewFrame, waveform, preview = True)
                wPanel.Show()
                previewFrame.Show()
                break
    
    def OnWaveformProperties(self, event):
        chosenMenu = event.GetEventObject()
        parent = chosenMenu.GetParent()
        
        for mItem in parent.GetMenuItems():
            if mItem.GetSubMenu() is chosenMenu:
                wformName = mItem.GetItemLabel()
                wformObj = self.model.getWaveform(wformName, WaveformModel.SYSTEM_WAVEFORM)
                print str(wformObj)
                title = wformName + " - Properties"
                wformProperties = str(wformObj)
                dlg = wx.MessageDialog(None, wformProperties, title, wx.OK | wx.ICON_INFORMATION)
                try:
                    dlg.ShowModal()
                finally:
                    dlg.Destroy()
    
    #Event Handler for page change. Update the active waveform on each page change
    def OnNbPageChange(self, event):
        pageId = event.GetSelection()
        #GetPage() returns the reference to waveform panel object
        wPanel = self.notebook.GetPage(pageId)
        #update active waveform in the model
        self.model.setActiveWaveform(wPanel.GetName())
    
    def OnComponentMenuClick(self, event):
        cMenu = event.GetEventObject()
        mItemList = cMenu.GetMenuItems()
        for mItem in mItemList:
            self.model.updateComponentState(mItem.GetItemLabel(), mItem.IsChecked())
        
    def UpdateComponentMenu(self, wform):
        
        componentList = wform.getAllComponents()
        compMenu = self.FindMenuInMenuBar('Components')
        #clear the existing items in Component Menu 
        for mItem in compMenu.GetMenuItems():
            compMenu.DeleteItem(mItem)
            
        #Now update the component Menu wit the components of currently selected waveform
        for component in componentList:
            self.AddToComponentsMenu(component.getName(), component.isVisible())
        
    #listeners of WaveformListener interface
    def waveformAdded(self, event):
        wform = event.getSource()
        #if(wform.name == "w1"):
         #   print "in waveform Added"
          #  for comp in wform.components:
           #     for prop in comp.properties:
                #        print str(prop.id) + ": "  + str(prop.getValue())
                    
                    
        
        #create the panel only if it is a instance waveform.
        #For system waveform, just add it to the menu
        wType = wform.getType()
        if wType == WaveformModel.INSTANCE_WAVEFORM:
            wPanel = self.createWaveformPanel(self.notebook, wform)
            self.notebook.AddPage(wPanel, wform.getName())
            
        self.AddToWaveformsMenu(wform.getName(), wType)
    
    def waveformRemoved(self, event):
        
        srcWform = event.getSource()
        #if waveform removed is a SYSTEM_WAVEFORM, i.e. the one found in /sdr/dom/waveforms,
        #remove the waveform from the Waveforms Menu
        if srcWform is not None and srcWform.getType() == WaveformModel.SYSTEM_WAVEFORM:
            wformsMenu = self.FindMenuInMenuBar('Waveforms')
            menuItems = wformsMenu.GetMenuItems()
            for mItem in menuItems:
                if mItem.GetItemLabel() == srcWform.getName():
                    wformsMenu.DeleteItem(mItem)
                    break
            return
     
        curPanelId = self.notebook.GetSelection()
        curPanel = self.notebook.GetCurrentPage()
        wformName = curPanel.GetName()
        self.wpanels.remove(curPanel)
        #remove the panel from the notebook
        self.notebook.RemovePage(curPanelId)
        #destroy the waveform panel object
        curPanel.Destroy()
        
        msg = ("%s Uninstalled !" % wformName)
        utils.showMessage(msg, utils.INFO)
        
        #AdvanceSelection() sets the next available page active
        #and trigger EVT_NOTEBOOK_PAGE_CHANGED event. The event
        #handler OnNbPageChange() would be called that sets the
        #active waveform thruogh model.SetActiveWaveform()
        
        self.notebook.AdvanceSelection(True)
        
        #if no running waveforms on the system?
        if self.notebook.GetPageCount() == 0:
            compMenu = self.FindMenuInMenuBar('Components')
            #clear the existing items in Component Menu 
            for mItem in compMenu.GetMenuItems():
                compMenu.DeleteItem(mItem)
            self.model.setActiveWaveform(None)
            self.SetSize(OSSIE_WAVEAPP_DIMENSION)
        #notebook.AdvanceSelection() will not trigger PAGE_CHANGED event if there is 
        #only one panel left. Hence, set the active form explicitly.
        elif self.notebook.GetPageCount() == 1:
            nextActiveWform = self.notebook.GetCurrentPage().GetName()
            self.model.setActiveWaveform(nextActiveWform)
        
    def waveformSelected(self, event):
        wform = event.getSource()
        #If no waveform is there, just update the title and status Bar and return
        if wform is None:
            self.SetTitle(OSSIE_TITLE)
            #self.updateStatusBar()
            return   
        self.SetTitle(OSSIE_TITLE + ' (' + wform.getName() + ')')
        #Component menu needs to be updated per waveform basis.        
        self.UpdateComponentMenu(wform)
        self.Layout()
        
    def componentStateChanged(self, cEvent):
        """ A listener method of ComponentListener class that is triggerred
        whenever a component is selected/deselected in the Component Menu"""

        wPanel = self.getCurrentPanel()
        cPanelList = wPanel.GetChildren()
        wPanelSizer = wPanel.GetSizer()     
        componentList = self.model.getActiveWaveform().getAllComponents()
        
        #Detach all the existing panels from the sizer 
        for cPanel in cPanelList:
            if wPanelSizer.GetItem(cPanel) != None:
                wPanelSizer.Detach(cPanel)
                cPanel.Hide()
        
        #Update the sizer to be inline with the order of components in Waveform Model
        for cmp in componentList:
            if cmp.isVisible():
                #print cmp.getPosition()
                for cPanel in cPanelList:
                    if cPanel.GetName() == cmp.getName():
                        wPanelSizer.Add(cPanel, 0, wx.ALL | wx.EXPAND, 5)
                        cPanel.Show()
                        break
        
        wPanelSizer.Layout()
        wPanel.Refresh()
    
    def propertyStateChanged(self, event):
        property = event.getSource()
        component = property.getParent()
        wPanel = self.getCurrentPanel()
        cPanel = wPanel.FindWindowByName(component.getName())
        cPanelSizer = cPanel.GetSizer() 
        #Component Panel sizer is a static box sizer. ACtual widgets are contained in a
        #gridbag sizer which in turn contained in this static box sizer
        
        cgsizer = cPanelSizer.GetChildren()[0].GetSizer()
        propWidgetsList = cgsizer.GetChildren()
        
        #propWidgetsList stores the list of labels and widgets of properties in SizerItem format.
        #Get the actual window from the SizerItem object before acting on the window
        
        #removing all the widgets contained in the sizer
        for prpWid in propWidgetsList:
            #prpWid is of type wx.SizerItem. Use GetWindow() to get the window tracked by this SizerItem
            actW = prpWid.GetWindow()
            if cgsizer.GetItem(actW) != None:
                cgsizer.Detach(actW)
                actW.Hide()
        
        prpList = component.getAllProperties()
        propWidgetsList = cPanel.GetChildren()
     
        #NOTE: The while loop inside the for loop iterates over the children of 
        #the component panel. Since StaticBoxSizer is used to layout the properties,
        #the first child of the component panel (cPanel) will always be the StaticBox
        #object which should not be added to the gridbagsizer in addPropToCompSizer 
        #method. Hence, iterating from 1 to length of the propWidgetsList.
        # (see the value of index )
        
        newPrpList = []
        index = 1
        for prp in prpList:
            if prp.isVisible():
                index = 1
                while ( index < (len(propWidgetsList)) ):
                    actW = propWidgetsList[index]
                    
                    if actW.GetName() == prp.getName():
                        newPrpList.append(actW)
                        actW.Show()
                    index = index + 1    
        
        newCgSizer = self.addPropToCompSizer(newPrpList)
        cPanelSizer.Remove(cgsizer)
        cPanelSizer.Add(newCgSizer, 1, wx.ALIGN_CENTER | wx.EXPAND, 10)
        cPanelSizer.Layout()
        wPanel.GetSizer().Layout()
                      
            
    def propertyWidgetChanged(self, event):
        property = event.getSource()
        compRef = property.getParent()
        wPanel = self.getCurrentPanel()
        
        
        # DO NOT CALL replaceComponentPanel() DIRECTLY FROM HERE AS IT INVOLVES
        # A CALL TO window.Destroy() METHOD. THIS METHOD IS STILL IN THE CONTEXT
        # OF EVENT HANDLER INVOKED AS A RESULT OF RIGHT CLICK ON PROPERTY WIDGET.
        # CALLING replaceComponentPanel() DIRECTLY FROM HERE WOULD **CRASH** THE 
        # APPLICATION.
        
        wx.CallAfter(self.replaceComponentPanel, wPanel, compRef )
    
    def propertyRangeChanged(self, event):
        propertyCtrl = self.tempProp
        propRef = event.getSource()
        newMin, newMax = propRef.getRange()
        propertyCtrl.SetRange(newMin, newMax)
        
        #if GUI widget's current value is changed because new min, max values
        #we have to update the new value to the model as well.
        curWidVal = self.formatWidgetToModel(propertyCtrl.GetValue(), propRef.getType())
        preVal = propRef.getValue()
        if preVal == curWidVal:
            pass
        else:
            pName = propertyCtrl.GetName()
            cName = propertyCtrl.GetParent().GetName()
            wName = propertyCtrl.GetParent().GetParent().GetName()
        
            status = self.model.configure(wName, cName, pName, curWidVal)
            #leave the text box as it is if status is True
            #if the configure operation failed, revert the text value to the value stored in 
            #the model.
            if status is False:
                msg = "Could not configure " + pName
                utils.showMessage(msg, utils.NON_FATAL)
                self.formatModelToWidget(propertyCtrl, preVal)
            
        return
#        if propertyCtrl.__class__ is wx.SpinCtrl:
#            propertyCtrl.SetRange(newMin, newMax)
#        if propertyCtrl.__class__ is wx.Slider:
#            propertyCtrl.SetMin(newMin)
#            propertyCtrl.SetMax(newMax)
    
    def FindMenuInMenuBar(self, title):
        pos = self.GetMenuBar().FindMenu(title)
        return self.GetMenuBar().GetMenu(pos)
    
    def getCurrentPanel(self):
        return self.notebook.GetCurrentPage()
    
    def replaceComponentPanel(self, wPanel, component):
        
#        Find the component to be replaced from the children list of current
#        active waveform panel. Replace the Old component panel in the waveform
#        panel's sizer with the compnent panel created new and then destroy the 
#        old component panel
#        
        for button in self.configureButtons:
            if button.isComponentButton and button.getParentName() == component.getName():
                self.configureButtons.remove(button)
        
        oldCPanel = wPanel.FindWindowByName(component.getName())
        newCPanel = self.createComponentPanel(wPanel, component)
        wSizer = wPanel.GetSizer()         
        wSizer.Replace(oldCPanel, newCPanel)
      
        wSizer.Fit(wPanel)        
        wPanel.SetSizer(wSizer)
        wSizer.Layout()
        oldCPanel.Destroy()
        
    def OnRefresh(self, event):
        self.controller.refresh()
        
    def waveformRefresh(self, event):
        wformsTobeRemoved = event.getSource()
        wformMenu = self.FindMenuInMenuBar('Waveforms')
            
        for wformName in wformsTobeRemoved:
            wPanel = self.getWaveformPanel(wformName)
            self.wpanels.remove(wPanel)
            self.compPopupMenus.pop(wPanel)
            self.prpPopupMenus.pop(wPanel)
            wPanel.Destroy()
                    
            #update the menu display
            menuItems = wformMenu.GetMenuItems()
                
            #remove the selected waveform from the menu list 
            for mItem in menuItems:
                if mItem.GetItemLabel() == wformName :
                    wformMenu.RemoveItem(mItem)
                    mItem.Destroy()
                    break
        
        for wPanel in self.wpanels:
            cPanelList = wPanel.GetChildren()
            wformRef = self.model.getWaveform(wPanel.GetName(), WaveformModel.INSTANCE_WAVEFORM)
            for cPanel in cPanelList:
                compRef = wformRef.getComponent(cPanel.GetName())
                prpList = compRef.getAllProperties()
                prpWidgetList = cPanel.GetChildren()
                
                for prpW in prpWidgetList:
                    if type(prpW) is wx.TextCtrl: 
                        prpW.SetValue(str(compRef.findPropertyByName(prpW.GetName()).getValue()))
                    elif type(prpW) is wx.SpinCtrl:
                        prpW.SetValue(int(compRef.findPropertyByName(prpW.GetName()).getValue()))
                    elif type(prpW) is wx.Slider:
                        prpW.SetValue(int(compRef.findPropertyByName(prpW.GetName()).getValue()))
            
        if (self.model.getActiveWaveform() is None):
            nextActiveWform = None
            #make the first available waveform active
            menuItems = wformMenu.GetMenuItems()
            for mItem in menuItems:
                if mItem.IsCheckable():
                    mItem.Check()
                    nextActiveWform = mItem.GetItemLabel()
                    break
            
            #no running waveforms on the system
            if nextActiveWform is None:
                self.SetSize(OSSIE_WAVEAPP_DIMENSION)
                return
            self.model.setActiveWaveform(nextActiveWform)
                    
    def getWaveformPanel(self, wformName):
        for wPanel in self.wpanels:
            if (wformName == wPanel.GetName()):
                return wPanel
                
      
    def OnOpenLayout(self, event):
        pass
    def OnSaveLayout(self, event):
        self.model.waveformToXML()
        pass
    def OnSaveLayoutAs(self, event):
        pass
    def OnClose(self, event):
        if self.controller.nodeBooterProcess != None:
            dlg = wx.MessageDialog(self,
                                   "NodeBooter was started by WaveDash.\n" +
                                   "Would you like to terminate it?",
                                   "Confirm NodeBooter Termination", wx.YES | wx.NO | wx.ICON_QUESTION)
            result = dlg.ShowModal()
            if result == wx.ID_YES:
                print "Terminating nodeBooter"
                self.controller.nodeBooterProcess.terminate()
        self.Destroy()

    def OnWidgetSettings(self, event):
        pass
    def OnAbout(self, event):
        dlg = wx.MessageDialog(self, "WaveDash\nPart of the OSSIE Toolset",
                               "About WaveDash")
        dlg.ShowModal()
        
    def OnUpdateOnRefreshCheck(self, event):
        for button in self.configureButtons:
            button.Enable(self.updateOnRefresh.IsChecked())
            
    def OpenPreferences(self, event):
        PreferencePage = WavedashPreferencePage(self, self.controller)
        PreferencePage.Show()
            
    def OnUpdateComponentClicked(self, event):
        src = event.GetEventObject()
        compName = src.GetParent().GetName()
        wName = src.GetParent().GetParent().GetName()
        self.updateComponent(compName, wName)
        
    def OnUpdateWaveformClicked(self, event):
        src = event.GetEventObject()
        wName = src.GetParent().GetName()
        self.updateWaveform(wName)
        
    def updateWaveform(self, waveformName):
        wform = self.model.getWaveform(waveformName, WaveformModel.INSTANCE_WAVEFORM)
        for component in wform.getAllComponents():
            self.updateComponent(component.getName(), wform.getName())
                
    def updateComponent(self, componentName, waveformName):
        wform = self.model.getWaveform(waveformName, WaveformModel.INSTANCE_WAVEFORM)
        compRef = wform.getComponent(componentName)
        for property in compRef.getAllProperties():
            curVal = self.formatWidgetToModel(property.getWindow().GetValue(), property.getType())
            preVal = property.getValue()
          
    
            if curVal is not None:
            
            #if both the values are equal then no need to call configure()
                if preVal == curVal:
                    pass
                else:
                    status = self.model.configure(waveformName, componentName, property.getName(), curVal)
                    #leave the text box as it is if status is True
                    #if the configure operation failed, revert the text value to the value stored in 
                    #the model.
                    if status is False:
                        msg = "Could not configure " + property.getName()
                        utils.showMessage(msg, utils.NON_FATAL)
                        self.formatModelToWidget(property.getWindow(), preVal)
                        #src.SetValue(str(preVal))
                    elif property.type[1] == "boolean":
                        self.formatModelToWidget(property.getWindow(), curVal)
                
            else:
                msg = "Invalid value for %s/%s" % (componentName, property.getName()) + "\nEnter only '" + property.getType()[1] + "' values"
                msg = msg + "\nFor sequence types, enter values separated by comma" 
                msg = msg + " and enclosed within [] e.g. [10,20,30]"
            
                utils.showMessage(msg, utils.NON_FATAL)
                self.formatModelToWidget(property.getWindow(), preVal)     
                self.Layout()
            
    def getRoot(self):
        root = __file__
        if os.path.islink (root):
            root = os.path.realpath (root)
        root = os.path.dirname (os.path.abspath (root))
        return root

def main():
    app = wx.App()
    ctrlr = Controller()
    ctrlr.createWidgetContainer()
   
    frame = WavedashView(ctrlr, ctrlr.model)

    ctrlr.CORBAutils.init_CORBA()
    ctrlr.buildModel()

    app.MainLoop()
        
if __name__ == '__main__':
    main() 

    
