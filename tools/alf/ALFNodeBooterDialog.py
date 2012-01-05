## Copyright 2005, 2006, 2007, 2008, 2009 Virginia Polytechnic Institute and State University
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
from ALFNodeBooterUtils import ALFNodeBooterUtils


class ALFNodeBooterDialog(wx.Dialog):
    def __init__(self, parent):
        wx.Dialog.__init__(self,
                           parent=None,
                           id=-1,
                           title='NodeBooter Dialog',
                           pos=wx.DefaultPosition)
        self.parent = parent
        
        try:
            temp = self.parent.getOSSIEProperty('ossie', 'default.domain.manager')
            if not temp is None:
                self.domFileName = temp[temp.rfind('/') + 1:]
                self.domDirName = temp[:temp.rfind('/') + 1]
            else:
                self.domFileName = 'DomainManager.dmd.xml'
                self.domDirName = 'dom/domain/'
        
            temp = self.parent.getOSSIEProperty('ossie', 'default.device.manager')
            if not temp is None:
                self.devFileName = temp[temp.rfind('/') + 1:]
                self.devDirName = temp[:temp.rfind('/') + 1]
            else:
                self.devFileName = 'DeviceManager.dcd.xml'
                self.devDirName = 'dev/nodes/default_GPP_node/'
                
            
        except AttributeError:
            self.domFileName = 'DomainManager.dmd.xml'
            self.domDirName = 'dom/domain/'
            self.devFileName = 'DeviceManager.dcd.xml'
            self.devDirName = 'dev/nodes/default_GPP_node/'
            
        panel = wx.Panel(self)
    
        hbox2 = wx.BoxSizer(wx.HORIZONTAL)
        hbox3 = wx.BoxSizer(wx.HORIZONTAL)
        hbox4 = wx.BoxSizer(wx.HORIZONTAL)
        
        vbox = wx.BoxSizer(wx.VERTICAL)
        
        self.domCheckBox = wx.CheckBox(panel)
        self.domCheckBox.SetValue(True)
        domLabel = wx.StaticText(panel, label='Start Domain Manager:')
        self.domText = wx.TextCtrl(panel)
        self.domText.SetValue(self.domFileName)
        domBrowseButton = wx.Button(panel, label='Browse')
        domBrowseButton.Bind(wx.EVT_BUTTON, self.OnDomBrowse)
        
        self.devCheckBox = wx.CheckBox(panel)
        self.devCheckBox.SetValue(True)
        devLabel = wx.StaticText(panel, label='Start Device Manager:')
        self.devText = wx.TextCtrl(panel)
        self.devText.SetValue(self.devFileName)
        devBrowseButton = wx.Button(panel, label='Browse')
        devBrowseButton.Bind(wx.EVT_BUTTON, self.OnDevBrowse)
        
        cancelButton = wx.Button(panel, label='Cancel')
        cancelButton.Bind(wx.EVT_BUTTON, self.OnCancel)
        startButton = wx.Button(panel, label = 'Start')
        startButton.Bind(wx.EVT_BUTTON, self.OnStart)
        
        hbox2.Add(self.domCheckBox, 0, wx.Left, 5)
        hbox2.Add(domLabel, 1, wx.ALL, 5)
        hbox2.Add(self.domText, 1, wx.ALL, 5)
        hbox2.Add(domBrowseButton, 1, wx.ALL, 5)
        
        hbox3.Add(self.devCheckBox, 0    , wx.Left, 5)
        hbox3.Add(devLabel, 1, wx.ALL, 5)
        hbox3.Add(self.devText, 1, wx.ALL, 5)
        hbox3.Add(devBrowseButton, 1, wx.ALL, 5)
        
        hbox4.Add(cancelButton, 1, wx.ALL, 5)
        hbox4.Add(startButton, 1, wx.ALL, 5)

        vbox.Add(hbox2, 0, wx.EXPAND, 5)
        vbox.Add(hbox3, 0, wx.EXPAND, 5)
        vbox.Add(hbox4, 0, wx.EXPAND, 5)
        
        panel.SetSizer(vbox)
        
        vbox.Fit(self)
        startButton.SetFocus()
        self.ShowModal()
        self.Destroy()
        
    def OnDomBrowse(self, event):
        dlg = wx.FileDialog(self, "Choose a Domain Manager", '/sdr/dom/domain', "", "*.dmd.xml", wx.OPEN)
        temp = dlg.ShowModal()
        if temp == wx.ID_OK:
            self.domChanged = True
            self.domFileName=dlg.GetFilename()
            self.domDirName=dlg.GetDirectory() + '/'
            if self.domDirName.startswith('/sdr/'):
                self.domDirName = self.domDirName[5:]
            self.domText.SetValue(self.domFileName)
            
        
    def OnDevBrowse(self, event):
        dlg = wx.FileDialog(self, "Choose a Device Manager", '/sdr/dev/nodes', "", "*.dcd.xml", wx.OPEN)
        temp = dlg.ShowModal()
        if temp == wx.ID_OK:
            self.devChanged = True
            self.devFileName=dlg.GetFilename()
            self.devDirName=dlg.GetDirectory() + '/'
            if self.devDirName.startswith('/sdr/'):
                self.devDirName = self.devDirName[5:]
            self.devText.SetValue(self.devFileName)
            
            
    def OnCancel(self, event):
        self.EndModal(-1)
        
    def OnStart(self, event):
        command = '/usr/local/bin/nodeBooter'

        if self.domCheckBox.GetValue():
            command += ' -D'
            command += ' ' + self.domDirName + self.domFileName 
        if self.devCheckBox.GetValue():
            command += ' -d'
            command += ' ' + self.devDirName + self.devFileName
            
        nbUtils = ALFNodeBooterUtils()
        nbUtils.startNodeBooterWithCommand(command)
        if self.domCheckBox.GetValue():
            self.EndModal(0)
        else:
            self.EndModal(1)
    
def main():
    app = wx.App()
    frame = wx.Frame(None, -1, "test")
    frame.Show()
    nbd = NodeBooterDialog(frame)
    app.MainLoop()
    
        
if __name__ == '__main__':
    main()        
        