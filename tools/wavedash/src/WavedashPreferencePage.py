#! /bin/env python

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

DEFAULT_DEVICE_MANAGER_FILE = 'DeviceManager.dcd.xml'
DEFAULT_DEVICE_MANAGER_DIR = 'dev/nodes/default_GPP_node'
DEFAULT_DOMAIN_MANAGER_FILE = 'DomainManager.dmd.xml'
DEFAULT_DOMAIN_MANAGER_DIR = 'dom/domain'

class WavedashPreferencePage(wx.Frame):
    def __init__(self, parent, controller):
        wx.Frame.__init__(self, 
                          parent = parent, 
                          title='Wavedash Preference Page')
        self.devChanged = False
        self.domChanged = False

        self.controller = controller
        panel = wx.Panel(self, -1)
        managerSizer = wx.GridBagSizer(3, 4)
        
       
        self.devLabel = wx.StaticText(panel, -1, 'Default Device Manager:')
        self.deviceManager = wx.TextCtrl(panel, -1, 'DeviceManager.dcd.xml', size=wx.Size(120, -1))
        self.deviceBrowseButton = wx.Button(panel, -1, 'Browse')
        self.Bind(wx.EVT_BUTTON, self.OnDeviceBrowseClick, self.deviceBrowseButton)
        self.domLabel = wx.StaticText(panel, -1, 'Default Domain Manager:')
        self.domainManager = wx.TextCtrl(panel, -1, 'DomainManager.dmd.xml', size=wx.Size(120, -1))
        self.domainBrowseButton = wx.Button(panel, -1, 'Browse')
        self.Bind(wx.EVT_BUTTON, self.OnDomainBrowseClick, self.domainBrowseButton)
        self.okButton = wx.Button(panel, -1, 'OK')
        self.Bind(wx.EVT_BUTTON, self.OnOKClick, self.okButton)
        self.restoreDefaultsButton = wx.Button(panel, -1, 'Restore Defaults')
        self.Bind(wx.EVT_BUTTON, self.OnRestoreDefaultsClick, self.restoreDefaultsButton)
       
        managerSizer.Add(self.devLabel, (0, 0), flag=wx.LEFT, border=5)
        managerSizer.Add(self.deviceManager, (0, 1))
        managerSizer.Add(self.deviceBrowseButton, (0, 2), flag=wx.RIGHT | wx.LEFT, border=5)
        managerSizer.Add(self.domLabel, (1, 0), flag=wx.LEFT, border=5)
        managerSizer.Add(self.domainManager, (1, 1))
        managerSizer.Add(self.domainBrowseButton, (1, 2), flag=wx.RIGHT | wx.LEFT, border=5)
        managerSizer.Add(self.restoreDefaultsButton, (2, 1), flag=wx.LEFT, border=2)
        managerSizer.Add(self.okButton, (2, 2), flag=wx.LEFT, border=5)
        
        
        panel.SetSizerAndFit(managerSizer)
        
        self.Center()
    
    def OnDeviceBrowseClick(self, event):
        dlg = wx.FileDialog(self, "Choose a Device Manager", '/sdr/dev/nodes', "", "*.dcd.xml", wx.OPEN)
        temp = dlg.ShowModal()
        if temp == wx.ID_OK:
            self.devChanged = True
            self.devFileName=dlg.GetFilename()
            self.devDirName=dlg.GetDirectory()
            self.deviceManager.SetValue(self.devFileName)
            
    def OnDomainBrowseClick(self, event):
        dlg = wx.FileDialog(self, "Choose a Domain Manager", '/sdr/dom/domain', "", "*.dmd.xml", wx.OPEN)
        temp = dlg.ShowModal()
        if temp == wx.ID_OK:
            self.domChanged = True
            self.domFileName=dlg.GetFilename()
            self.domDirName=dlg.GetDirectory()
            self.domainManager.SetValue(self.domFileName)
            
    def OnOKClick(self, event):
        self.setProperties()  
        self.Close()
        
    def OnRestoreDefaultsClick(self, event):
        self.devDirName = DEFAULT_DEVICE_MANAGER_DIR
        self.devFileName = DEFAULT_DEVICE_MANAGER_FILE
        self.domDirName = DEFAULT_DOMAIN_MANAGER_DIR
        self.domFileName = DEFAULT_DOMAIN_MANAGER_FILE 
        self.deviceManager.SetValue(self.devFileName)
        self.domainManager.SetValue(self.domFileName)  
        self.devChanged = True
        self.domChanged = True
        self.setProperties()
        
    def setProperties(self):
        if self.devChanged:
            devMan = self.devDirName + '/' + self.devFileName
            if devMan.startswith('/sdr/'):
                devMan = devMan[5:]
            self.controller.setOSSIEProperty('ossie', 'default.device.manager', devMan)
        if self.domChanged:
            domMan = self.domDirName + '/' + self.domFileName
            if domMan.startswith('/sdr/'):
                domMan = domMan[5:]
            self.controller.setOSSIEProperty('ossie', 'default.domain.manager', domMan)  
        


    

        
    