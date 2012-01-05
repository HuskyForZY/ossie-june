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

import os
import subprocess
import shlex
import time
import ConfigParser
import wx

DEFAULT_DOMAIN_MANAGER = 'dom/domain/DomainManager.dmd.xml'
DEFAULT_DEVICE_MANAGER = 'dev/nodes/default_GPP_node/DeviceManager.dcd.xml'

class NodeBooterUtils:
    def __init__(self, controller):
        self.controller = controller
    
    def nodeBooterIsRunning(self):
        processes = os.popen('ps x | grep nodeBooter').read()
        processes = processes.split()
        return ('/usr/local/bin/nodeBooter' in processes or '/usr/bin/nodeBooter' in processes)

    def startNodeBooter(self):
        if self.controller.OSSIEPropertyFileExists():
            domainManager = self.controller.getOSSIEProperty('ossie', 'default.domain.manager')
            deviceManager = self.controller.getOSSIEProperty('ossie', 'default.device.manager')
        if domainManager == None:
            domainManager = DEFAULT_DOMAIN_MANAGER
        if deviceManager == None:
            deviceManager = DEFAULT_DEVICE_MANAGER
        command = '/usr/local/bin/nodeBooter -D ' + domainManager + ' -d ' + deviceManager
        return self.startNodeBooterWithCommand(command)
    
    def startNodeBooterWithCommand(self, command):
        args = shlex.split(str(command))
        print "NodeBooter not running. Starting now with command: " + command
        nodeBooterProcess = subprocess.Popen(args, cwd='/sdr')
        time.sleep(2)
        return nodeBooterProcess
    
    def namingServiceIsRunning(self):
        namecltResult = os.popen('nameclt list').read()
        if namecltResult.startswith('DomainName'):
            return True
        return False;
    
    def startNamingService(self):
        nsCommand = self.controller.getOSSIEProperty('ossie', 'naming.service.start.command')
        if not nsCommand == None:
            if nsCommand.startswith('sudo'):
                dialog = wx.TextEntryDialog(None, 'Please enter your root password to restart the naming service:', 'Restart Naming Service', '', style = wx.TE_PASSWORD | wx.OK | wx.CANCEL)
                if dialog.ShowModal() == wx.ID_OK:
                    password = dialog.GetValue()
                    command = 'echo ' + '\"' + password + '\"' + ' | sudo -kS ' + nsCommand[5:]
                    subprocess.Popen(command, shell=True)
                    time.sleep(2)
            else:
                subprocess.Popen(nsCommand, shell=True)
        else:
            #default case
            nsCommand = 'sudo service omniorb4-nameserver restart'
            dialog = wx.TextEntryDialog(None, 'Please enter your root password to restart the naming service:', 'Restart Naming Service', '', style = wx.TE_PASSWORD | wx.OK | wx.CANCEL)
            if dialog.ShowModal() == wx.ID_OK:
                password = dialog.GetValue()
                command = 'echo ' + '\"' + password + '\"' + ' | sudo -kS ' + nsCommand[5:]
                subprocess.Popen(command, shell=True)
                time.sleep(2)
                

            
