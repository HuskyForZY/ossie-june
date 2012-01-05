## Copyright 2009 Virginia Polytechnic Institute and State University
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

import os

##-------------------------------------------------------------------------
## Doc
##-------------------------------------------------------------------------
"""
This module provides a function that wraps the os.chmod function.  It is
here primarily to make it much easier to replace calls to chmod on
platforms where it is not supported or where it is provided via a
different operation.
"""

##-------------------------------------------------------------------------
## Public functions
##-------------------------------------------------------------------------

def chmod(file, permissions):
    # chmod is not available in Jython
    pass
