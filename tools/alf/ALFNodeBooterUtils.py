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


class ALFNodeBooterUtils:
    def __init__(self):
        pass
    
    def nodeBooterIsRunning(self):
        #run ps and check for nodebooter
        processes = os.popen('ps x | grep nodeBooter').read()
        processes = processes.split()
        return ('/usr/local/bin/nodeBooter' in processes or '/usr/bin/nodeBooter' in processes)

    def startNodeBooter(self, domainManager = 'dom/domain/DomainManager.dmd.xml', deviceManager = 'dev/nodes/default_GPP_node/DeviceManager.dcd.xml'):
        configFile = os.getenv('HOME') + '/.ossie.cfg'
        if os.path.exists(configFile):
            configParser = ConfigParser.SafeConfigParser()
            configParser.read(configFile)
            domainManager = configParser.get("ossie", "default.domain.manager")
            deviceManager = configParser.get("ossie", "default.device.manager")
        command = '/usr/local/bin/nodeBooter -D ' + domainManager + ' -d ' + deviceManager
        args = shlex.split(command)
        print "NodeBooter not running. Starting now..."
        nodeBooterProcess = subprocess.Popen(args, cwd='/sdr')
        #pause for a couple seconds to let nodeBooter start up
        #otherwise occasionally alf or wavedash will start up
        #before nodebooter finishes and give errors
        time.sleep(2)
        return nodeBooterProcess
    
    def startNodeBooterWithCommand(self, command):
        args = shlex.split(str(command))
        print "NodeBooter not running. Starting now..."
        nodeBooterProcess = subprocess.Popen(args, cwd='/sdr')
        #pause for a couple seconds to let nodeBooter start up
        #otherwise occasionally alf or wavedash will start up
        #before nodebooter finishes and give errors
        time.sleep(2)
            
            
    def namingServiceIsRunning(self):
        namecltResult = os.popen('nameclt list').read()
        if namecltResult.startswith('DomainName'):
            return True
        return False;
    
    def startNamingService(self):
        configFile = os.getenv('HOME') + '/.ossie.cfg'
        if os.path.exists(configFile):
            configParser = ConfigParser.SafeConfigParser()
            configParser.read(configFile)
            nsCommand = configParser.get('ossie', 'naming.service.start.command')
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
            
