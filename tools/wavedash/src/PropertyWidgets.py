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

import copy
import xml.dom.minidom
from xml.dom.minidom import Node

class Widget(object):
    def __init__(self):
        self.type=None
        self.parameters = {}
    
    def getMin(self):
        try:
            val = self.parameters["min"]
            return val
        except(KeyError):
            return None
    
    def getMax(self):
        try:
            val = self.parameters["max"]
            return val
        except(KeyError):
            return None
    
    def setMin(self, newVal):
        try:
            self.parameters["min"] = newVal
        except(KeyError):
            return None
    
    def setMax(self, newVal):
        try:
            self.parameters["max"] = newVal
        except(KeyError):
            return None
    
    
        
class PropertyWidgetMap(object):
    def __init__(self):
        self.property = () #tuple of (xmltype, datatype) e.g. ('simple', 'int')
        self.default = None
        self.optional = [] 
    
class WidgetContainer(object):
    def __init__(self, widgetDom):
        self.widgetCollection = []
        self.propertyWidgetMap = []
        
        self.buildWidgetCollection(widgetDom)
        self.buildPropertyWidgetMap(widgetDom)

        
    def buildWidgetCollection(self, widgetDom):
        """ Gets all the <widget> nodes from the widget config file and makes
        a collection of it """
        widgetNodes = widgetDom.getElementsByTagName('widgets')[0].childNodes
        for widget in widgetNodes:
            if (widget.nodeType == Node.ELEMENT_NODE):
                #Create a new widget of the type as specified in the xml file 
                newWidget = Widget()
                newWidget.type = widget.attributes['type'].value
                parameterList = widget.getElementsByTagName('parameter')
                #Look into the parameter list and build parameter dictionary
                for parameter in parameterList:
                    if parameter.attributes != None:
                        name = parameter.attributes['name'].value
                        value = parameter.attributes['value'].value
                        newWidget.parameters[name] = value
                self.widgetCollection.append(newWidget)
                
    def buildPropertyWidgetMap(self, widgetDom):
        prpNodes = widgetDom.getElementsByTagName('properties')[0].childNodes
        for prp in prpNodes:
            if (prp.nodeType == Node.ELEMENT_NODE):
                newWidgetMap = PropertyWidgetMap()
                xmlType = prp.attributes['xmltype'].value
                dataType = prp.attributes['datatype'].value        
                              
                newWidgetMap.property = (xmlType, dataType)
                preferredWidgets = prp.getElementsByTagName('preferred-widgets')[0]
                preferredWidgetList = preferredWidgets.getElementsByTagName('preferred-widget')
                
                for pw in preferredWidgetList:
                    type = pw.attributes['type'].value
                    widget = self.getWidgetByType(type)
                    isDefault = pw.attributes.get('default')
                    
                    #if default attribute is found and is set to True, set the widget as
                    #default widget. Else, add it to the optional widget list.
                    
                    if ( isDefault == None):
                        newWidgetMap.optional.append(widget)
                    elif ( isDefault.value == "true" ):
                        newWidgetMap.default = widget
                        
                self.propertyWidgetMap.append(newWidgetMap)
        
    def getWidgetByType(self, type):
        retWidget = None 
        for widget in self.widgetCollection:
            if widget.type == type:
                retWidget = widget
        return copy.deepcopy(retWidget)
    
    def getDefaultWidget(self, property):
        for pwmap in self.propertyWidgetMap:
            if ( (pwmap.property == property)  ):
                return copy.deepcopy(pwmap.default)
    
    def getOptionalWidgets(self, propertyType):
        for pwmap in self.propertyWidgetMap:
            if ( pwmap.property == propertyType ):
                return pwmap.optional
    
    def printPropertyWidgetMap(self):
        for pwmap in self.propertyWidgetMap:
            print pwmap.property
            print pwmap.default.type
            
             
    def printWidgets(self):
        for widget in self.widgetCollection:
            print widget.type,
            for name in widget.parameters.keys():
                print name,widget.parameters[name] 
    
                
            
    