#! /usr/bin/env python

from distutils.core import setup
import sys

if len(sys.argv) != 2:
    sys.exit(1)

ossiepyidl = ['ossie', 'ossie/standardinterfaces', 'ossie/standardinterfaces/standardInterfaces', 'ossie/standardinterfaces/CF', 'ossie/standardinterfaces/CF__POA', 'ossie/standardinterfaces/PortTypes', 'ossie/standardinterfaces/PortTypes__POA', 'ossie/standardinterfaces/standardInterfaces__POA']

setup(
        name='ossiepyidl',
        version='0.8.0',
        description='OSSIE Python IDL bindings',
        packages=ossiepyidl,
    )
