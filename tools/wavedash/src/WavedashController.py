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

import os
import sys
import xml.dom.minidom as minidom
from WavedashModel import WavedashModel
import WaveformModel 
import PropertyWidgets
from PropertyWidgets import WidgetContainer
import WavedashUtils as utils
import time
import shlex
import subprocess
import ConfigParser
from NodeBooterUtils import NodeBooterUtils

SDR_DIR = '/sdr/dom/'
SDR_WAVEFORMS_DIR = '/waveforms'
COMP_WAVEFORMS_DIR = '/_tmp_alf_waveforms'
SAD_XML = '*.sad.xml'
DAS_XML = '*_DAS.xml'
PROPERTY_WIDGET_CONFIG_FILE = '/resources/widgetconf.xml'
OSSIE_CONFIG_FILE = '.ossie.cfg'

class Controller:
    def __init__(self, enableGUI = True):
        self.nodeBooterProcess = None
        self.nbUtils = NodeBooterUtils(self)
        
        self.enableGUI = enableGUI
        self.model = WavedashModel(self)
        self.views = []
        self.widgetContainer = None
        self.CORBAutils = utils.WaveAppCORBA()
        
    def buildModel(self):
        """ This functions walks through the system directories in /sdr/, construct a
        list of .sad.xml files and pass them to the model to build the waveforms. 
        Waveforms in turn build their components and each component in turn build their
        properties """     
        try:
            mgr = self.CORBAutils.domMgr._get_fileMgr()
        except:
            return
        #Find the list of SAD and DAS files for all waveforms in standard SDR root directory (/sdr/)
        (sdrWformsSADList, sdrWformsDASList) = (self.getFileList(SDR_WAVEFORMS_DIR, SAD_XML), self.getFileList(SDR_WAVEFORMS_DIR, DAS_XML))
        #Find the list of SAD and DAS files for component running as waveforms. Those waveforms are installed in /sdr/_tmp_alf_waveforms 
        (tmpWformsSADList, tmpWformsDASList) = (self.getFileList(COMP_WAVEFORMS_DIR, SAD_XML, quiet=True), self.getFileList(COMP_WAVEFORMS_DIR, DAS_XML, quiet=True))
        
        if (sdrWformsSADList is None or sdrWformsDASList is None):
                print "buildModel(): Error in reading SAD/DAS files for SDR waveforms in " + SDR_DIR
                sys.exit(-1)
        
        if (tmpWformsSADList is None or tmpWformsDASList is None):
            #We need not exit if no temporary waveforms are not found in /sdr/_tmp_alf_waveforms
            tmpWformsSADList = []
            tmpWformsDASList = []
            
        
        sadFileList = sdrWformsSADList + tmpWformsSADList
        dasFileList = sdrWformsDASList + tmpWformsDASList
             
        self.model.buildWaveforms(sadFileList, dasFileList)
        
        #update the model with currently installed waveforms in framework
        installedWformDict = self.CORBAutils.getApplications()
        for wform in installedWformDict.keys():
            wformName = wform[wform.find("::") + 2 :]
            for instance in installedWformDict[wform]:
                instanceName = instance[instance.find("::") + 2 :]
                self.model.addInstanceWaveform(wformName, instanceName)
    
    def selectNS(self):
        isNewNS = self.CORBAutils.selectNamingService()
        #if new namign service is selected, refresh the model and views.
        if isNewNS:
            self.refresh()
    
    def refresh(self):
        #First, update the SDR waveforms installed
        #model.buildWaveforms(..) will build a new object for every waveform if it is not there already
        #in its list
        #Find the list of SAD and DAS files for all waveforms in standard SDR root directory (/sdr/)
        (sdrWformsSADList, sdrWformsDASList) = (self.getFileList(SDR_WAVEFORMS_DIR, SAD_XML), self.getFileList(SDR_WAVEFORMS_DIR, DAS_XML))
        #Find the list of SAD and DAS files for component running as waveforms. Those waveforms are installed in /sdr/_tmp_alf_waveforms 
        (tmpWformsSADList, tmpWformsDASList) = (self.getFileList(COMP_WAVEFORMS_DIR, SAD_XML, quiet=True), self.getFileList(COMP_WAVEFORMS_DIR, DAS_XML, quiet = True))
        
        if (sdrWformsSADList is None or sdrWformsDASList is None):
                print "buildModel(): Error in reading SAD/DAS files for SDR waveforms in " + SDR_DIR
                sys.exit(-1)
        
        if (tmpWformsSADList is None or tmpWformsDASList is None):
            #We need not exit if no temporary waveforms are not found in /sdr/_tmp_alf_waveforms
            tmpWformsSADList = []
            tmpWformsDASList = []
        
        sadFileList = sdrWformsSADList + tmpWformsSADList
        dasFileList = sdrWformsDASList + tmpWformsDASList
        
        #sadFileList = self.getFileList(SDR_WAVEFORMS_DIR, SAD_XML) #list holding sad files of all the system waveforms           
        #dasFileList = self.getFileList(SDR_WAVEFORMS_DIR, DAS_XML)
        self.model.buildWaveforms(sadFileList, dasFileList)
        
        #get the new list of installed applications
        installedWformDict = self.CORBAutils.getApplications()
        self.model.refreshModel(installedWformDict)         
        
            
    def createWidgetContainer(self):
        root = __file__
        if os.path.islink (root):
            root = os.path.realpath (root)
        root = os.path.dirname (os.path.abspath (root))

        widgetsDom = minidom.parse(root + PROPERTY_WIDGET_CONFIG_FILE)#self.getDOM(PROPERTY_WIDGET_CONFIG_FILE)
        self.widgetContainer = WidgetContainer(widgetsDom)
        
    def getFileList(self, dir, filetype, quiet=False):
        fileList = []
        
        fileList = self.CORBAutils.getFileList(dir,filetype)
        
        #If no file found?
        if fileList == None:
            if not quiet: #Ignore the error for _tmp_alf_waveforms. 
                utils.showMessage("Could not find " + filetype + " files in this directory:\n " + dir, utils.FATAL, self.enableGUI)
            return None
        
        return fileList
    
    def getDOM(self, xmlfile):
        """ This method gets a file name as string and returns a DOM object built
        using minidom
        """
        return self.CORBAutils.getDOM(xmlfile)
    
    def installWaveform(self, wformName, start):
        wform = self.model.getWaveform(wformName, WaveformModel.SYSTEM_WAVEFORM)
        if wform is None:
            utils.showMessage ("Unable to install " + wformName, utils.FATAL, self.enableGUI)
            return False
        else:
            instance = self.CORBAutils.installWaveform(wform.sadFile, wform.dasFile, start)
            if instance is not None:
                instanceName = instance[instance.find("::") + 2 :]
                self.model.addInstanceWaveform(wformName, instanceName, start)
                return instanceName
            else:
                utils.showMessage("Unable to find applications", utils.NON_FATAL, self.enableGUI)
                return False
    def uninstallWaveform(self, wformName):
        status = self.CORBAutils.uninstallWaveform(wformName)
        return status
    
    def startWaveform(self, wformName):
        status = self.CORBAutils.startWaveform(wformName)
        return status
    
    def stopWaveform(self, wformName):
        status = self.CORBAutils.stopWaveform(wformName)
        return status
    
    def getDefaultWidget(self, property):
        return self.widgetContainer.getDefaultWidget(property)
    
    def getWidgetByType(self, type):
        return self.widgetContainer.getWidgetByType(type)
    
    def getOptionalWidgets(self, propertyType):
        return self.widgetContainer.getOptionalWidgets(propertyType)
    
    def getWidgetContainer(self):
        return self.widgetContainer
    
    def getUtilRef(self):
        return self.CORBAutils
    
    def setOSSIEProperty(self, sectionName, propertyName, propertyValue):
        configFile = os.getenv('HOME') + '/' + OSSIE_CONFIG_FILE
        configParser = ConfigParser.SafeConfigParser()
        #check to see if a config file already exists
        if os.path.exists(configFile):
            #if so, read in its properties so they don't get lost
            configParser.read(configFile)
        if not configParser.has_section(sectionName):
            configParser.add_section(sectionName)
        configParser.set(sectionName, propertyName, propertyValue)
        f = open(configFile, 'w')
        configParser.write(f)
        f.close()
            
    def getOSSIEProperty(self, sectionName, propertyName):
        configFile = os.getenv('HOME') + '/' + OSSIE_CONFIG_FILE
        if os.path.exists(configFile):
            configParser = ConfigParser.SafeConfigParser()
            configParser.read(configFile)
            if configParser.has_option(sectionName, propertyName):
                return configParser.get(sectionName, propertyName)
        return None
    
    def OSSIEPropertyFileExists(self):
        configFile = os.getenv('HOME') + '/' + OSSIE_CONFIG_FILE
        return os.path.exists(configFile)
    
    def namingServiceIsRunning(self):
        return self.nbUtils.namingServiceIsRunning()
    
    def startNamingService(self):
        self.nbUtils.startNamingService()
        
    def nodeBooterIsRunning(self):
        return self.nbUtils.nodeBooterIsRunning()
    
    def startNodeBooter(self):
        self.nodeBooterProcess = self.nbUtils.startNodeBooter()
    
    
    
    
       


