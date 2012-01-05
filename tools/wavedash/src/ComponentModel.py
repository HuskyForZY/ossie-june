from xml.dom import minidom
from xml.dom.minidom import Node
from PropertyModel import Property
from PropertyWidgets import WidgetContainer
import sys

class ComponentEvent:
    """ serves as a call back object for methods of ComponentListener
    interface"""
    
    def __init__(self,type):
        self.component = None
        self.type = type
    def getSource(self):
        return self.component
    def getEventType(self):
        return self.type

class ComponentListener:
    """ An abstract class acting as an interface to update the views about the 
    changes happening in ComponentModel. All classes wish to interface with 
    Component model must inherit this class and can implement their
    own logic by overriding the methods of this class """
    
    def componentStateChanged(self, cEvent):
        pass
    def componentHidden(self, cEvent):
        pass
    def componentShown(self, cEvent):
        pass
    
class Component:
    """ The Model class for Component objects. Encapsulates the
    components' attributes and methods """
     
    def __init__(self, parent, id, name, pos, spd_file, controller, visible = True):
        self.id = id
        self.parent = parent
        self.name = str(name)
        self.position = pos
        self.spdFile = str(spd_file)
        self.controller = controller
        self.visible = visible
        self.properties = []
        self.buildProperties(controller)
    
    def __str__(self):
        return "Component: %s, pos: %d, parent: %s" % (self.name, self.position, self.parent.getName())
    
    def getName(self):
        return self.name
    
    def getPosition(self):
        return self.position
    
    def setPosition(self, newPos):
        self.position = newPos
    
    def getPanel(self):
        return self.panel
    
    def setPanel(self, newPanel):
        self.panel = newPanel
        
    def setPosition(self, newPos):
        self.position = newPos
    
    def hide(self):
        self.visible = False
    
    def show(self):
      self.visible = True
    
    def isVisible(self):
        return self.visible
    
    def getParent(self):
        return self.parent
    
    def getAllProperties(self):
        plist = self.properties[0:]
        return plist
    
    def findPropertyByName(self, name):
        for prop in self.properties:
            if prop.name == name:
                return prop
    
    def findPropertyByID(self, id):
        for prp in self.properties:
            if prp.id == id:
                return prp
    
    def updatePropertyState(self, pName, visible):
        property = self.findPropertyByName(pName)
        if property.isVisible() == visible:
            return False
        if visible:
            property.show()
        else:
            property.hide()
        return True
    
    def queryProperties(self):
        #This method queries the list of properties and their values of a component, 
        #update the values to the property objects in the model.
        utilRef = self.controller.getUtilRef()
        newPrpList = []
        newPrpList = utilRef.query(self.parent.getName(), self.name, newPrpList)
        #newPrpList is a lsit of corba objects of type
        #ossie.cf.CF.DataType(id='DCE:a337c5f0-8245-11dc-860f-00123f63025f', 
        #                    value=CORBA.Any(CORBA.TC_short, 2502))
        if newPrpList is None:
            return
        for newprp in newPrpList:
            prpRef = self.findPropertyByID(newprp.id)
            if (prpRef is not None):
                if prpRef.corbaObj is None:
                    prpRef.corbaObj = newprp    
                else: 
                   #print "setting old corbaObj to " + str(newprp.value._v)
                    prpRef.setValue(newprp.value._v)
            
       # print newPrpList
        return
    
    def configureProperty(self, pName, newVal):
        prpRef = self.findPropertyByName(pName)
        #storing the old value in a temp varaible to revert the changes if configure() fails.
        curVal = prpRef.getValue()
    
        prpRef.setValue(newVal)
        #configure() method expects the argument to be passed as a LIST object.
        #hence convert the corbaObj to a list
        prpList = [prpRef.corbaObj]
        utilRef = self.controller.getUtilRef()
        
        
        status = utilRef.configure(self.parent.getName(), self.name, prpList)
        if status is False:
            prpRef.setValue(curVal)
        return status
        
    def buildProperties(self, controller):
        #Read the path of prf file from spd file of the component
        spddom = controller.getDOM(self.spdFile)
        
        prfnode = spddom.getElementsByTagName('propertyfile')[0]
        
        # propertyfile tag is of type
        # <propertyfile type = "PRF"> 
        #    <localfile name = <path of prf file> />
        # </propertyfile>
        
        # Extracting the path of the prf file from childNodes[1], i.e. <localfile> tag
        # the file name may be written in unicode format. convert to string for internal use
        prf_file = str(prfnode.childNodes[1].attributes['name'].value)
                
        #Now parsing .prf file to build property model
        prfdom = controller.getDOM(prf_file)
        if prfdom is None:
            print "buildProperties(): Failed to create DOM for PRF file for ", self.parent.getName(), "/", self.name
            sys.exit(-1)
       
        prpNode = prfdom.getElementsByTagName('properties')[0]
        prpList = prpNode.childNodes
        
        try:
            for prp in prpList:
                if ( prp.nodeType != Node.ELEMENT_NODE or str(prp.tagName) == 'description' ):
                    continue
                #populate property fields and create the property object
                
                id = prp.attributes['id'].value
                name = prp.attributes['name'].value
                type = (str(prp.tagName), str(prp.attributes['type'].value))
                mode = prp.attributes['mode'].value
                
                desc = ''
                value = None
                range = (0,10000)
                actionType = ''
                kindType = ''
                
                #Iterate through children of <simple> tag to update desc, value and kindType
                for child in prp.childNodes:
                    if (child.nodeType == Node.ELEMENT_NODE):
                        if (child.tagName == 'description'):
                            desc = child.firstChild.data
                        elif (child.tagName == 'value'):
                            value = child.firstChild.data
                        elif (child.tagName == 'kind'):
                            kindType = child.attributes['kindtype'].value
                        elif (child.tagName == 'range'):
                            min = child.childNodes[1].firstChild.data
                            max = child.childNodes[3].firstChild.data
                            range = (min, max)
                        elif (child.tagName == 'action'):
                            actionType = child.attributes['type'].value
                
                
                widget = controller.getDefaultWidget(type) 

                
                newProp = Property(self, id, name, type, mode, desc, value, 
                                   range, kindType, actionType, widget, prf_file)
                self.properties.append(newProp)
        except:
            print "Error in reading PRF files for ", self.parent.getName(), "/", self.name
            errorMsg = sys.exc_info()
            print errorMsg
            sys.exit(-1)
            
    def changePropWidget(self, property, newWidgetType):
        prpRef = self.findPropertyByName(property)
        curWidget = prpRef.getWidget()
        if curWidget.type == newWidgetType:
            return None
        newWidget = self.controller.getWidgetByType(newWidgetType)
        prpRef.widget = newWidget
        return prpRef
        
    
    def fireModelChange(self, event):
        pass
