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
import copy
import WaveformModel
from WaveformModel import Waveform
from WaveformModel import WaveformEvent
from ComponentModel import ComponentEvent
from PropertyModel import PropertyEvent
import xml.dom.minidom as minidom
import WavedashUtils as utils

[SYSTEM_WAVEFORM_ADDED,
 SYSTEM_WAVEFORM_REMOVED,
 WAVEFORM_INSTANCE_ADDED,
 WAVEFORM_INSTANCE_REMOVED,
 WAVEFORM_INSTANCE_SELECTED,
 WAVEFORM_REFRESH ] = [wx.NewId() for x in range(6)]
COMPONENT_STATE_CHANGE = wx.NewId()
PROPERTY_STATE_CHANGE = wx.NewId()
PROPERTY_WIDGET_CHANGE = wx.NewId()
PROPERTY_WIDGET_CONFIGURE = wx.NewId()

class WavedashModel:
    """ This class is the top level model handling the sub-models
    WaveformModel, ComponentModel and PropertyModel. It also takes
    care of delegating the listeners to each sub model """
    
    def __init__(self, controller):
        self.controller = controller
        self.systemWaveforms = []
        self.instanceWaveforms = {}
        self.waveformListeners = []
        self.componentListeners = []
        self.propertyListeners = []
        self.activeWaveform = None
        #self.buildModels()
    
    def getSystemWaveforms(self):
        sysWaveforms = self.systemWaveforms[:]
        return sysWaveforms
    
    def getInstanceWaveforms(self, parentWaveform):
        for wform in self.instanceWaveforms.keys():
             if wform.getName() == parentWaveform:
                 return self.instanceWaveforms[wform]
        return None
    
    def getWaveform(self, waveform, type):
        if waveform is None:
            return None
        if type == WaveformModel.SYSTEM_WAVEFORM:
            for wform in self.systemWaveforms:
                if wform.getName() == waveform:
                    return wform
        elif type == WaveformModel.INSTANCE_WAVEFORM:
            for wform in self.instanceWaveforms.keys():
                for instWform in self.instanceWaveforms[wform]:
                    if instWform.getName() == waveform:
                        return instWform
        return None
    
    def getParentWaveform(self, child):
        for pWform in self.instanceWaveforms.keys():
            for wform in self.instanceWaveforms[pWform]:
                if ( wform.getName() == child ):
                    return pWform
                
    def getActiveWaveform(self):
        return self.activeWaveform
    
    def setActiveWaveform(self, wformName):
        wform = self.getWaveform(wformName, WaveformModel.INSTANCE_WAVEFORM)
        self.activeWaveform = wform
        wsEvent = WaveformEvent(WAVEFORM_INSTANCE_SELECTED)
        wsEvent.waveform = self.activeWaveform
        self.fireModelChange(wsEvent)
    
    def moveComponentUp(self, wformName, compName):
        wform = self.getWaveform(wformName, WaveformModel.INSTANCE_WAVEFORM)
        status = wform.moveComponentUp(compName)
        if status is True:
            cEvent = ComponentEvent(COMPONENT_STATE_CHANGE)
            cEvent.component = wform.getComponent(compName)
            self.fireModelChange(cEvent)
        else:
            return
    
    def moveComponentDown(self, wformName, compName):
        wform = self.getWaveform(wformName, WaveformModel.INSTANCE_WAVEFORM)
        status = wform.moveComponentDown(compName)
        if status is True:
            cEvent = ComponentEvent(COMPONENT_STATE_CHANGE)
            cEvent.component = wform.getComponent(compName)
            self.fireModelChange(cEvent)
        else:
            return
    
    def updateComponentState(self, cName, cState):
        wform = self.getActiveWaveform()
        wform.updateComponentState(cName, cState)
                
        cEvent = ComponentEvent(COMPONENT_STATE_CHANGE)
        cEvent.component = wform.getComponent(cName)
        self.fireModelChange(cEvent)
    
    def updatePropertyState(self, cName, pName, visible):
        component = self.activeWaveform.getComponent(cName)
        status = component.updatePropertyState(pName, visible)
        #The above method returns True if property is updated succesfully
        #If the new state and old state are same, it simply returns False.
        #In such case, this method will not fire any model change. just returns
        if status is True:
            pEvent = PropertyEvent(PROPERTY_STATE_CHANGE)
            pEvent.property = component.findPropertyByName(pName)
            self.fireModelChange(pEvent)
            
            
        
        
    def pollAppFactory(self):
        pass
        #self.simulateWaveformAdd('ossie_demo')
        #self.simulateWaveformAdd('pass_data_waveform')
    
    def addInstanceWaveform(self, wform, instance, isRunning = False):
        
        #newWaveform = copy.deepcopy(self.getWaveform(wform, WaveformModel.SYSTEM_WAVEFORM))
        parent = self.getWaveform(wform, WaveformModel.SYSTEM_WAVEFORM)
        if parent is None:
            errMsg = "Unable to add new application, " + instance + ". \nWaveform  " + wform + "."
            errMsg = errMsg + "\nPlease check /sdr/waveforms or /sdr/_tmp_alf_waveforms directory for valid waveforms"
            #errMsg = errMsg + "Warning! This version of Wavedash do not support component as waveform applications "
            utils.showMessage ( errMsg, utils.NON_FATAL)
            return
        newWaveform = Waveform(parent.id, instance ,parent.sadFile, parent.dasFile, WaveformModel.INSTANCE_WAVEFORM, self.controller, isRunning )
        #newWaveform.type = WaveformModel.INSTANCE_WAVEFORM
        #newWaveform.name = instance
        if (self.instanceWaveforms.has_key(parent)):
            self.instanceWaveforms[parent].append(newWaveform)
        else:
            self.instanceWaveforms[parent] = [newWaveform]
            
        wEvent = WaveformEvent(WAVEFORM_INSTANCE_ADDED)
        wEvent.waveform = newWaveform
        self.fireModelChange(wEvent)
    
    def removeWaveform(self, wform):
        """ Removes the currently active waveform application from the list and udpates the 
        listeners. User can perform 'Unisntall' operation only on the current
        active waveform. """
        
        #uninstall the waveform
        success = self.controller.uninstallWaveform(wform.getName())
        if not success:
            return False
        
        #update the model data
        parent = self.getParentWaveform(wform.getName())
#        remWform = wform.getName()
#        if parent is None:
#            utils.showError ("Unable to find parent waveform for " + wform.getName())
#        
        #remove instance waveform from the aprent list
        self.instanceWaveforms[parent].remove(wform)
        wform = None
        self.activeWaveform = None
        wEvent = WaveformEvent(WAVEFORM_INSTANCE_REMOVED)
        wEvent.waveform = None
        self.fireModelChange(wEvent)
         
    def buildWaveforms(self, waveformSADList, waveformDASList):
        """ This method gets a list of sad files of waveforms passed from the 
        controller and builds the model of every Waveform.  
        """
        
        #self.systemWaveforms = []
        #self.instanceWaveforms = {}
        #self.activeWaveform = None
        wformsToBeRemoved = []
        
        for fIndex in range(len(waveformSADList)):
            sadFile = waveformSADList[fIndex]
            dasFile = waveformDASList[fIndex]
           # print "sadFile = ", sadFile
            wavedom = self.controller.getDOM(sadFile)
            #if error in getting the DOM?
            if ( wavedom is None ):
                print "Error! buildWaveforms(): Could not build DOM for " , sadFile 
                continue
            swassemblyNode = wavedom.getElementsByTagName('softwareassembly')[0]
            wname = swassemblyNode.attributes['name'].value
            
            #stripping off 'OSSIE::' from waveform names
            wname = wname[wname.find('::') +2 :]
            wid = swassemblyNode.attributes['id'].value
            existingWform = self.getWaveform(wname, WaveformModel.SYSTEM_WAVEFORM)
            #Build a new waveform only if there is no other waveoform exists in the same name
            #This block prevents the duplicate waveform entries while doing Refresh 
            if (existingWform is None):
                  
                #create a new waveform and add it to systemWaveforms list
                #since this method builds only system waveform, it always pass the
                #state parameter of Waveform as SYSTEM_WAVEFORM
                
                newWaveform = Waveform(wid, wname,sadFile, dasFile, WaveformModel.SYSTEM_WAVEFORM, self.controller )
                #if(newWaveform.name == "w1"):
                 #   print "after waveform building"
                  #  for comp in newWaveform.components:
                   #     for prop in comp.properties:
                    #        print str(prop.id) + ": "  + str(prop.getValue())
                self.systemWaveforms.append(newWaveform)
                wEvent = WaveformEvent(SYSTEM_WAVEFORM_ADDED)
                wEvent.waveform = newWaveform
                
                self.fireModelChange(wEvent)
            #Check if sadFile of the current waveform and existing waveform matches or not.
            #If it matches, no need to create the waveform. else, there report to user that
            #there is name conflict
            else:
                if ( sadFile == existingWform.sadFile and dasFile == existingWform.dasFile):
                    continue
                else:
                    errormsg = "Conflicting names for waveform - " + wname
                    errormsg = errormsg + "\nCheck the sad files - " + sadFile
                    errormsg = errormsg + "\n" + existingWform.sadFile 
                    utils.showMessage ( errormsg, utils.NON_FATAL )
                    continue
                
        #When this method is called from Refresh, we need to remove the waveforms from the systemWaveforms dict
        #that are not present in the given sadFile and DASFile list.
        wformFound = False
        for sysWform in self.systemWaveforms:
            wformFound = False
            for curSADFile in waveformSADList:
                if curSADFile == sysWform.sadFile:
                    #wform match found
                    wformFound = True
                    
            #if no match found? then remove the wform from system waveforms list.
            #since we are iterating on self.systemWaveforms, we cannot remvoe items
            #from the same list now. collect the lsit of waveforms to be removed and
            #remove them from hte list once for all.
            if wformFound is False:
                wformsToBeRemoved.append(sysWform)
        
        for wform in wformsToBeRemoved:
            print "removing waveform ", wform.getName()
            self.systemWaveforms.remove(wform)
            wEvent = WaveformEvent(SYSTEM_WAVEFORM_REMOVED)
            wEvent.waveform = wform
            self.fireModelChange(wEvent)

    
    def waveformExists(self, wformName, type):
        if (type == WaveformModel.SYSTEM_WAVEFORM ):
            for wform in self.systemWaveforms:
                if (wform.getName() == wformName):
                    return True
        return False
    
    def refreshModel(self, installedWformDict):
        
        #installedWformDict = self.CORBAutils.getApplications()
        newWaveforms = []
        wformsTobeRemoved = []
        for wform in installedWformDict.keys():
            wformName = wform[wform.find("::") + 2 :]
            for instance in installedWformDict[wform]:
                instanceName = instance[instance.find("::") + 2 :]
                wformRef = self.getWaveform(instanceName, WaveformModel.INSTANCE_WAVEFORM) 
                if ( wformRef is None):
                    self.addInstanceWaveform(wformName, instanceName, False)
                else:
                    wformRef.updateComponentValues()
                    
                newWaveforms.append(instanceName)
        
        for wform in self.instanceWaveforms.keys():
                for instWform in self.instanceWaveforms[wform]:
                    if (instWform.getName() in newWaveforms):
                        continue
                    else:
                        wformsTobeRemoved.append(instWform.getName())
                        #self.instanceWaveforms[wform].remove(instWform)
        
        for wform in wformsTobeRemoved:
            instWformRef = self.getWaveform(wform, WaveformModel.INSTANCE_WAVEFORM)
            parentWform = self.getParentWaveform(wform)
            self.instanceWaveforms[parentWform].remove(instWformRef)
            
        
        wEvent = WaveformEvent(WAVEFORM_REFRESH)
        wEvent.waveform = wformsTobeRemoved
        self.fireModelChange(wEvent)
        
    
    def configure(self, wformName, cName, pName, pValue):
        wformRef = self.getWaveform(wformName, WaveformModel.INSTANCE_WAVEFORM)
        status = wformRef.configure(cName, pName, pValue)
        return status
            
        
    
    def changePropertyWidget(self, componentName, propertyName, widget):
        wformRef = self.getActiveWaveform()
        compRef = wformRef.getComponent(componentName)
        
        #changePropWidget returns the property reference if newWidget is set
        #succesfully. If the new and old widgets are same, it returns None
        prp = compRef.changePropWidget(propertyName, widget)
        #If prp widget changed succesfully, fire widgetChange event to property listeners    
        if prp != None:
            pEvent = PropertyEvent(PROPERTY_WIDGET_CHANGE)
            pEvent.property = prp
            self.fireModelChange(pEvent)
        
        return
      
    def configurePropertyWidget(self, componentName, propertyName, newMin, newMax):
        #Changes are always made to the active waveform. Hence taking the active
        #waveform reference directly
        
        wformRef = self.getActiveWaveform()
        compRef = wformRef.getComponent(componentName)
        propRef = compRef.findPropertyByName(propertyName)
        widget = propRef.getWidget()
        
        oldMin, oldMax = propRef.getRange()
        #if min and max are not in valid range, make no changes to the model
        success = propRef.setRange((newMin, newMax))
        if not success:
            return False
        
        #if newMin is greater than old min value or newMax is less the previous min value
        #then setting the min value first will cause an assertion failure (min < max).
        #Hence, check the condition and invoke setMin and setMax in the appropriate order
        if (newMin > oldMax or newMax < oldMin):
            widget.setMax(newMax)
            widget.setMin(newMin)
        else:
            widget.setMin(newMin)
            widget.setMax(newMax)
        
        pEvent = PropertyEvent(PROPERTY_WIDGET_CONFIGURE)
        pEvent.property = propRef
        self.fireModelChange(pEvent)
        
        
    
    def addWaveformListener(self, listener):
        self.waveformListeners.append(listener)
    def addComponentListener(self, listener):
        self.componentListeners.append(listener)
    def addPropertyListener(self, listener):
        self.propertyListeners.append(listener)
    def removeWaveformListener(self, listener):
        self.waveformListeners.remove(listener)
    def removeComponentListener(self, listener):
        self.componentListeners.remove(listener)
    def removePropertyListener(self, listener):
        self.propertyListeners.remove(listener)
    def fireModelChange(self, event):
            
        if event.type == WAVEFORM_INSTANCE_ADDED \
         or event.type == SYSTEM_WAVEFORM_ADDED:
            for listener in self.waveformListeners:
                listener.waveformAdded(event)
        if event.type == SYSTEM_WAVEFORM_REMOVED:
            for listener in self.waveformListeners:
                listener.waveformRemoved(event)
        if event.type == WAVEFORM_INSTANCE_SELECTED:
            for listener in self.waveformListeners:
                listener.waveformSelected(event)        
        if event.type == COMPONENT_STATE_CHANGE:
            for listener in self.componentListeners:
                listener.componentStateChanged(event)
        if event.type == WAVEFORM_INSTANCE_REMOVED:
            for listener in self.waveformListeners:
                listener.waveformRemoved(event)
        if event.type == WAVEFORM_REFRESH:
            for listener in self.waveformListeners:
                listener.waveformRefresh(event)
        if event.type == PROPERTY_WIDGET_CHANGE:
            for listener in self.propertyListeners:
                listener.propertyWidgetChanged(event)
        if event.type == PROPERTY_STATE_CHANGE:
            for listener in self.propertyListeners:
                listener.propertyStateChanged(event)
        if event.type == PROPERTY_WIDGET_CONFIGURE:
            for listener in self.propertyListeners:
                listener.propertyRangeChanged(event)
                    
    def printTextualData(self):
        wformList = self.systemWaveforms  
        for wform in wformList:
            print wform
            #print "Components:"
            cmplist = wform.getAllComponents()
            for cmp in cmplist:
                print cmp
                prplist = cmp.getAllProperties()
                print "Properties:"
                for prp in prplist:
                    print "\t", prp
    
    def waveformToXML(self, wformName = None):
        wformRef = self.getActiveWaveform()
        if ( wformRef is None ):
            return
        wformDom = minidom.Document()
        wformElem = wformDom.createElement("waveform")
        wformElem.setAttribute("name", wformRef.getName())
        wformDom.appendChild(wformElem)
        compsElem = wformDom.createElement("components")
        wformElem.appendChild(compsElem)
        components = wformRef.getAllComponents()
        for comp in components:
            compElem = wformDom.createElement("component")
            compElem.setAttribute("name", comp.getName())
            posElem = wformDom.createElement("pos")
            posElem.appendChild(wformDom.createTextNode(str( comp.getPosition())) )
            compElem.appendChild(posElem)
            visibleElem = wformDom.createElement("visible")
            visibleElem.appendChild(wformDom.createTextNode(str(comp.isVisible())))
            compElem.appendChild(visibleElem)
            compsElem.appendChild(compElem)
        
        print wformDom.toprettyxml("  ", "\n", "UTF-8")
        
            
