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

class NSChoiceDialog(wx.Dialog):
    def __init__(self, parent):
        wx.Dialog.__init__(self,
                parent=None,
                id=-1,
                title='Warning!',
                pos=wx.DefaultPosition,
                size=wx.Size(400, 100))
        self.parent = parent

        panel = wx.Panel(self)
        vbox = wx.BoxSizer(wx.VERTICAL)
        hbox = wx.BoxSizer(wx.HORIZONTAL)
        

        text = wx.StaticText(panel, -1, "The naming service is not running locally!")
        startLocallyButton = wx.Button(panel, -1, 'Start Locally')
        connectRemoteButton = wx.Button(panel, -1, 'Connect Remotely')
        cancelButton = wx.Button(panel, -1, 'Cancel')
        startLocallyButton.Bind(wx.EVT_BUTTON, self.startLocally)
        connectRemoteButton.Bind(wx.EVT_BUTTON, self.connectRemote)
        cancelButton.Bind(wx.EVT_BUTTON, self.OnExit)

        hbox.Add(startLocallyButton, 1, wx.LEFT, 5)
        hbox.Add(connectRemoteButton, 1, wx.LEFT | wx.RIGHT, 5)
        hbox.Add(cancelButton, 1, wx.RIGHT, 5)

        vbox.Add(text, 0, wx.ALIGN_CENTER| wx.TOP, 10)
        vbox.Add(hbox, 1, wx.ALIGN_CENTER | wx.TOP | wx.BOTTOM, 10)
        
        panel.SetSizer(vbox)
        vbox.Fit(self)
     
        self.ShowModal()
        
        self.Destroy()

  
    def startLocally(self, event):
        self.parent.controller.startNamingService()
        self.EndModal(-1)
        
    def connectRemote(self, event):
        self.parent.controller.selectNS()
        self.EndModal(-1)

    def OnExit(self, event):
        self.EndModal(-2)
        #self.GetParent().Destroy()
        
def main():
    app = wx.App()
    frame = wx.Frame(None, -1, "test")
    frame.Center()
    frame.Show()
    nbcd = NSChoiceDialog(frame)
    app.MainLoop()
    
        
if __name__ == '__main__':
    main()  
        
  