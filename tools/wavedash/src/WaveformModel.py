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

import xml.dom.minidom as minidom
from ComponentModel import Component

[ SYSTEM_WAVEFORM, INSTANCE_WAVEFORM, RUNNING_WAVEFORM ] = [1, 2, 3]

class WaveformEvent:
    """This is the call back object passed in the methods of
    WaveformListener class"""
    def __init__(self,type):
        self.type = type
        self.waveform = None
    def getSource(self):
        return self.waveform
    def getEventType(self):
        return self.type
    
class WaveformListener:
    """ An abstract class providing the interface that fires the changes
    happening in Waveform model. Classes wish to interface with
    Waveform Model must inherit this class and can override the 
    methods of this listener to accomplish their task."""
    def waveformAdded(self, wEvent):
        pass
    def waveformRemoved(self, wEvent):
        pass
    def waveformSelected(self, wEvent):
        pass
    def waveformRefresh(self, wEvent):
        pass

class Waveform:
    """ This is the model class for Waveform."""
    def __init__(self, id, name, sad_file, das_file, type, controller, isRunning = False):
        self.id = str(id)
        self.name = str(name)
        self.sadFile = str(sad_file) #filenames are read in unicode. converting them to string here
        self.dasFile = str(das_file)
        self.type = type
        self.components = []
        self.buildComponents(controller)
        if ( self.type == INSTANCE_WAVEFORM ):
            for cmp in self.components:
                cmp.queryProperties()
        self.isRunning = isRunning
    
    def getName(self):
        return self.name
    
    def getType(self):
        return self.type
    
    def __str__(self):
        wStr = ""
        wStr = wStr + "Name: " + self.name + "\n"
        wStr = wStr + "Id: " + self.id + "\n"
        wStr = wStr + "SAD File: " + self.sadFile + "\n"
        wStr = wStr + "DAS File: " + self.dasFile + "\n"
        wStr = wStr + "Components: " + "\n\t"
        for comp in self.components:
            wStr = wStr + comp.name + "\n\t"
        wStr = wStr + "\n"
        return wStr
        
    def getComponent(self, name):
        for component in self.components:
            if component.name == name:
                return component
            
    def getAllComponents(self):
        #clist = self.components[0:]
        return self.components
    
    def moveComponentUp(self, compName):
        #Indices of components in the self.component list reflects the order of components
        #moveUp will decrease the index of the component in steps on 1 until it is placed
        #on top a visible component.
        compRef = self.getComponent(compName)
        oldIndex = self.components.index(compRef)
        if oldIndex > 0:
            temp = self.components.pop(oldIndex)
            tIndex = oldIndex - 1
            while (tIndex > 0 and not self.components[tIndex].isVisible()):
                tIndex = tIndex - 1
            
            newIndex = tIndex
            self.components.insert(newIndex, compRef)
            #compRef.setPosition(newIndex)
            return True
        else:
            return False
    
    def moveComponentDown(self, compName):
        compRef = self.getComponent(compName)
        oldIndex = self.components.index(compRef)
#===============================================================================
#        NOTE : This function does not work properly when there are hidden
#                components between two components that needs to be reordered.
#                Needs a fix before it goes for release
#===============================================================================
        if oldIndex < (len(self.components) - 1):
            temp = self.components.pop(oldIndex)
            tIndex = oldIndex + 1
            
            while (tIndex < (len(self.components) ) and not self.components[tIndex].isVisible()):
                tIndex = tIndex + 1
            
            newIndex = tIndex 
            self.components.insert(newIndex, compRef)
            #compRef.setPosition(newIndex)
            return True
        else:
            return False
        
        return True
        
        
    def buildComponents(self, controller):
        
        wavedom = controller.getDOM(self.sadFile)
        cmplist = [] #temp list to hold component name and id
        pos = 0 #value that determines ordering of components
           
        componentPlacementList = wavedom.getElementsByTagName('componentplacement')
        
        for cmp in componentPlacementList:
            nsName = cmp.getElementsByTagName('namingservice')[0].attributes['name'].value
            id = cmp.getElementsByTagName('componentinstantiation')[0].attributes['id'].value
            fileRef = cmp.getElementsByTagName('componentfileref')[0].attributes['refid'].value
            cmplist.append((id, nsName, fileRef))
        
        componentFilesList = wavedom.getElementsByTagName('componentfile')
        for compFile in componentFilesList:
            fileRefId = compFile.attributes['id'].value
            tspd = compFile.getElementsByTagName('localfile')[0].attributes['name'].value
            cname = tspd
            #strip off componenet name from spd file name
            while 1:
                index = cname.find('/')
                if index == -1:
                    cname = cname[:cname.find('.spd.xml')]
                    break
                else:
                    cname = cname[index+1:]
            #cname is the component name stripped off from the spd file name
            for c in cmplist:
                #c is of the form (id, nsName, fileRef)
                #check if component name c[1] and spd file name 'cname' are equal? and
                #their associated file reference ids. The file ref id check is required
                #to avoid duplicates if a component name is a substring of another componet
                #name. for e.g. Channel is a substring of ChannelDemo1. if u spd is Channel.spd.xml
                #and your component name is ChannelDemo1, then this will hit a match. hence 
                #compare the file reference id as well to avoid this situation.
                if ( c[1].find(cname) != -1 ) and (c[2] == fileRefId):
                    newComponent = Component(self, id = c[0], name = c[1], pos = pos,
                                            spd_file = tspd, controller = controller )
                    self.components.append(newComponent)
                    pos = pos + 1
    
    def updateComponentState(self, cName, visible):
        component = self.getComponent(cName)
        if visible:
            component.show()
        else:
            component.hide()
    
    def configure(self, cName, pName, pValue):
        component = self.getComponent(cName)
        status = component.configureProperty(pName, pValue)
        return status
    
    def updateComponentValues(self):
        for cmp in self.components:
            cmp.queryProperties()
    
    def reOrderComponents(self, component, oldPos, newPos):
        pass 
    def commitWaveform(self):
        pass