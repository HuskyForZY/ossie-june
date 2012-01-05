## Copyright 2005, 2006, 2007 Virginia Polytechnic Institute and State University
##
## This file is part of the OSSIE Waveform Developer.
##
## OSSIE Waveform Developer is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## OSSIE Waveform Developer is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with OSSIE Waveform Developer; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

##-------------------------------------------------------------------------
## This file defines basic methods for retrieving information from the
## wavedev.cfg file.
##-------------------------------------------------------------------------
import xml.dom.minidom
import os
import sys
import traceback
from WaveDev.wavedev.errorMsg import *
##-------------------------------------------------------------------------
## Doc
##-------------------------------------------------------------------------
"""
This module provides functions for accessing configuration settings
stored in the wavedev.cfg file (in xml).  The primary functions provided
for use in other modules are defined in the "public" section of the
module below.

The configuration file is a simple XML file where values are stored
in specialized tags.  Each value has a name, and is encoded in the
XML file as a child tag of the top-level <owdconfiguration> tag that
looks like this:
    <name>value</name>

To look up a configuration value, simply provide the corresponding name
as an argument to ossieCfgValue().  The XML in the config file is parsed
only once, and results are cached internally within this module.  Provisions
are also made to override configuration settings loaded from the config
file using the overrideCfgValue() function.  Special methods for retrieving
the version() and ossieInstallDir() are provided as separate methods, since
they are used more frequently.  These methods are shallow wrappers around
ossieCfgValue().

Names in this file that begin with double-underscores ("__") are intended
to be private and should not be referenced outside this file.
"""

##-------------------------------------------------------------------------
## Public functions
##-------------------------------------------------------------------------

def version():
    """
    Get the current OSSIE version by looking up the "version" key.
    """
    result = ossieCfgValue('version')
    if result == None:
        result = 'unknown'
    return result


def ossieInstallDir():
    """
    Get the location of the OSSIE installation directory by looking
    up the "installpath" key.  This function ensures that the return
    value always ends in a trailing slash.
    """
    return ensureDirHasTrailingSlash(ossieCfgValue('installpath'))


def ossieCfgValue(key):
    """
    Look up a key in the configuration file and return its associated value.
    """
    global __overrides
    if __overrides.has_key(key):
        return __overrides[key]
    else:
        val = __keyFromXml(key)
        # cache it
        if val != None:
            setCfgValueIfNecessary(key, val)
        return val


def overrideCfgValue(key, value):
    """
    Override a configuration value from the config file for the current
    process, without rewriting the configuration file.
    """
    global __overrides
    __overrides[key] = value


def setCfgValueIfNecessary(key, value):
    """
    Set an override value for a configuration setting, if no override
    value is already recorded.
    """
    global __overrides
    if not __overrides.has_key(key):
        overrideCfgValue(key, value)


def ensureDirHasTrailingSlash(dir):
    """
    Add a trailing slash to the dir, if there isn't one there already.
    """
    if dir != None and dir[len(dir) - 1] != '/':
        dir = dir + '/'
    return dir


def commentLine():
    """
    Generate an XML comment line containing the current version number
    for use in automatically generated XML files.
    """
    global __commentLine
    if __commentLine == None:
        __commentLine = u'<!--Created with OSSIE WaveDev ' + version() \
            + u'-->\n<!--Powered by Python-->\n'
    return __commentLine

def LoadConfiguration(frame_obj):
    """
    This function used to be located in MainFrame.py, but is used by
    both MainFrame and ComponentFrame to load configuration settings
    into fields within the provided frame_obj.  It was moved here, since
    it is used by both of those other modules.
    """
    frame_obj.version = version()

    # install path
    frame_obj.installPath = ossieInstallDir()

    # standard IDL path
    frame_obj.stdIdlPath = ensureDirHasTrailingSlash(ossieCfgValue('stdidlpath'))
    if frame_obj.stdIdlPath == None or not os.path.isdir(frame_obj.stdIdlPath):
        if frame_obj.stdIdlPath == None:
            print "warning: wavedev.cfg stdidl path is not set"
        else:
            print "warning: wavedev.cfg stdidl path " + frame_obj.stdIdlPath + " does not exist"
        print "  => using default standard idl path instead"

        # Attempt to use a default value
        if os.path.isdir('/usr/include/standardinterfaces'):
            frame_obj.stdIdlPath = '/usr/include/standardinterfaces/'
        elif os.path.isdir('/usr/local/include/standardinterfaces'):
            frame_obj.stdIdlPath = '/usr/local/include/standardinterfaces/'
        else:
            tmpstr = "StandardInterfaces does not appear to be installed!\n"
            tmpstr += "You will not be able to add ports to components.\n\n"
            tmpstr += "If you have standardinterfaces installed in a location\n"
            tmpstr += "other than the default, please specify that location\n"
            tmpstr += "in the wavedev.cfg file located in the top directory."
            errorMsg(frame_obj,tmpstr)

    # custom IDL path
    frame_obj.customIdlPath = ensureDirHasTrailingSlash(ossieCfgValue('customidlpath'))
    if frame_obj.customIdlPath == None or not os.path.isdir(frame_obj.customIdlPath):
        if frame_obj.customIdlPath == None:
            print "warning: wavedev.cfg customidl path is not set"
        else:
            print "warning: wavedev.cfg customidl path " + frame_obj.customIdlPath + " does not exist"
        print "  => using default custom idl path instead"

        # Attempt to use a default value
        if os.path.isdir('/usr/include/custominterfaces'):
            frame_obj.customIdlPath = '/usr/include/custominterfaces/'
        elif os.path.isdir('/usr/local/include/custominterfaces'):
            frame_obj.customIdlPath = '/usr/local/include/custominterfaces/'
        else:
            tmpstr = "CustomInterfaces does not appear to be installed!\n"
            tmpstr += "You will not be able to add ports to components.\n\n"
            tmpstr += "If you have standardinterfaces installed in a location\n"
            tmpstr += "other than the default, please specify that location\n"
            tmpstr += "in the wavedev.cfg file located in the top directory."
            errorMsg(frame_obj,tmpstr)

    # ossie include path
    frame_obj.ossieIncludePath = ensureDirHasTrailingSlash(ossieCfgValue('ossieincludepath'))
    if frame_obj.ossieIncludePath == None or not os.path.isdir(frame_obj.ossieIncludePath):
        if os.path.isdir('/usr/include/ossie'):
            frame_obj.ossieIncludePath = '/usr/include/ossie/'
        elif os.path.isdir('/usr/local/include/ossie'):
            frame_obj.ossieIncludePath = '/usr/local/include/ossie/'
        else:
            tmpstr = "OSSIE does not appear to be installed!\n"
            tmpstr += "You will not be able to add ports to components\n"
            tmpstr += "using CF interfaces.\n\n"
            tmpstr += "If you have ossie installed in a location other than\n"
            tmpstr += "the default please specify that location in the\n"
            tmpstr += "wavedev.cfg file located in the top directory."
            errorMsg(frame_obj,tmpstr)

    # home directory
    frame_obj.homeDir = ensureDirHasTrailingSlash(ossieCfgValue('homedir'))

    # generate source preamble
    frame_obj.sourcepreamble = ossieCfgValue('sourcepreamble')
    if frame_obj.sourcepreamble == None or not os.path.exists(frame_obj.sourcepreamble):
        #search for generate/generic_preamble in Wavedev/wavedev
        frame_obj.sourcepreamble = __getPath('/WaveDev/wavedev/generate/generic_preamble')

    # generate license file
    frame_obj.licensefile = ossieCfgValue('licensefile')
    if frame_obj.licensefile == None or not os.path.exists(frame_obj.licensefile):
        frame_obj.licensefile = ''

    # developer
    frame_obj.developer = ossieCfgValue('developer')
    if frame_obj.developer == None or not os.path.exists(frame_obj.developer):
        frame_obj.developer = "your_name_or_organization"

##-------------------------------------------------------------------------
## Internal
##-------------------------------------------------------------------------

def __relativeRoot():
    """
    Locate the OSSIE WaveDev root directory by looking in the system path.
    """
    for i in sys.path:
        root = i + "/WaveDev/"
        if os.path.exists(root):
            return root
    print "Root install directory not found ... check sys.path selections"
    return None

def __getPath(toFile):
    """Search for the given file in list of directories available in sys.path
       and returns the absolute path
    """
    for dir in sys.path:
        filePath = dir + toFile
        if os.path.exists(filePath):
            return filePath
    return None

__overrides = {'rootpath' : __relativeRoot() }
__cachedXml = None
__commentLine = None


def __xml():
    global __cachedXml
    if __cachedXml == None:
        fileName = __overrides['rootpath'] + 'wavedev.cfg'
        if os.path.exists(fileName):
            __cachedXml = xml.dom.minidom.parse(fileName)
    return __cachedXml


def __keyFromXml(key):
    result = None
    xml = __xml()
    if xml != None:
        try:
            result = str(xml.getElementsByTagName(key)[0].firstChild.data)
        except:
            # ignore it--the config file is missing the given setting
            pass
    return result

