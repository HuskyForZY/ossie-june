import wx

class WavedashButton(wx.BitmapButton):
    
    def setParentName(self, newParentName):
        self.parentName = newParentName
        
    def getParentName(self):
        return self.parentName
    
    def setGrandParentName(self, newGrandParentName):
        self.grandParentName = newGrandParentName
        
    def getGrandParentName(self):
        return self.grandParentName
    
    def setIsComponentButton(self):
        self.isComponentButton = True
        self.isWaveformButton = False
        
    def setIsWaveformButton(self):
        self.isWaveformButton = True
        self.isComponentButton = False
