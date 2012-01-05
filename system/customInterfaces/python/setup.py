#! /usr/bin/env python

from distutils.core import setup
import sys

if len(sys.argv) != 2:
    sys.exit(1)

ossiepyidl = ['ossie', 'ossie/custominterfaces', 'ossie/custominterfaces/customInterfaces', 'ossie/custominterfaces/customInterfaces__POA', 'ossie/custominterfaces/PortTypes', 'ossie/custominterfaces/PortTypes__POA']

setup(
        name='ossiepyidl',
        version='0.8.0',
        description='OSSIE Python IDL bindings',
        packages=ossiepyidl,
    )
