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

#only spinbox and slider are configurable in Wavedash. Prevent users from
#configuring a value beyond these limits.

SPIN_SLIDER_MIN_VAL = 0
SPIN_SLIDER_MAX_VAL = 2147483647 #Max value of signed integer -1 (2^31 -1 )
SPIN_SLIDER_RANGE = ( SPIN_SLIDER_MIN_VAL, SPIN_SLIDER_MAX_VAL )

class PropertyEvent:
    def __init__(self, type):
        self.type = type
        self.property = None
        self.value = 0
        self.range = ()
    def getSource(self):
        return self.property
    def getValue(self):
        return self.value
    def getRange(self):
        return self.range

class PropertyListener:
    """ This is an abstract class providing the interface methods to
    fire the changes happening in Property Model to those views who
    has inherited this listener """
    
    def propertyWidgetChanged(self, pEvent):
        pass
    
    def propertyValueChanged(self, pEvent):
        pass
    
    def propertyRangeChanged(self, pEvent):
        pass
    
    def propertyStateChanged(self, pEvent):
        pass

class Property:
    """ This is the model class encapsulating the attributes and
    methods of Properties as defined the prf files """
    def __init__(self, parent, id, name, type, mode, desc="",value=0,
                 range = (0,0) ,kindType = '', actionType='', widget = None, prf_file = ''):
        self.parent = parent
        self.id = str(id)
        self.name = str(name)
        self.type = type
        self.mode = mode
        self.desc = desc
        self.value = value #default value read from SAD file, not used for configuration
        self.kindType = kindType
        self.actionType = actionType
        self.range = range
        self.widget = widget
        self.prfFile = prf_file
        
        #reference object to CF::Properties, used to avoid query() everytime before invoking
        #configure(). the object "value" of the corbaObj is used to store the actual value of the property.
        #corbaObj is of type ossie.cf.CF.DataType(id='DCE:a337c5f0-8245-11dc-860f-00123f63025f', 
        #                                        value=CORBA.Any(CORBA.TC_short, 2502))
        self.corbaObj = None 
        
        
        #all properties are visible by default.
        self.visible = True
    
    def __str__(self):
        return "name: %s, type = (%s,%s), value = %s, widget = %s" % (
             self.name, self.type[0], self.type[1], self.value, self.widget.type)
    
    def getID(self):
        return self.id
    
    def getName(self):
        return self.name
    
    def getParent(self):
        return self.parent
    
    def getType(self):
        return self.type
    
    def getDesc(self):
        return self.desc
    
    def getWidget(self):
        return self.widget
    
    def getValue(self):
        #print self.name, self.value
        if self.corbaObj is not None:
            #print "from corba"
            return self.corbaObj.value._v
        else:
            #if value is not retrieved from the nodeBooter, return the default value
            #read from the SAD file.
            #print "from self"
            return self.value
        
    def setValue(self, newValue):
        if self.corbaObj is not None:
            self.corbaObj.value._v = newValue
            
    def setWindow(self, newWindow):
        self.window = newWindow
        
    def getWindow(self):
        return self.window
    
    def setRange(self, range):
        #range[0] = min, range[1] = max
        
        if ( ( range[0] >= SPIN_SLIDER_MIN_VAL and range[0] <= SPIN_SLIDER_MAX_VAL ) and
             ( range[1] >= SPIN_SLIDER_MIN_VAL and range[1] <= SPIN_SLIDER_MAX_VAL ) and 
             ( range[0] < range[1]) ):
            self.range = range
            return True
        else:
            return False
    
    def getRange(self):
        return self.range
    
    def isReadOnly(self):
        if self.mode == "readonly":
            return True
        else:
            return False
    
    def show(self):
        self.visible = True
    
    def hide(self):
        self.visible = False
        
    def isVisible(self):
        return self.visible
    
    def fireModelChange(self):
        pass