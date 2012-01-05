#! /usr/bin/env python

from distutils.core import setup
import sys

install_location = '/sdr/dom/'

if len(sys.argv) != 2:
	sys.exit(1)

sys.argv.append('--install-lib='+install_location)

setup(name='pass_data', 
      description='pass_data',
      data_files=[
                  (install_location+'bin',['pass_data.py']),
                  (install_location+'xml/pass_data',
                               ['pass_data.prf.xml',
                                'pass_data.scd.xml', 
                                'pass_data.spd.xml'])
                 ]
     )
