#! /usr/bin/env python

from distutils.core import setup
import sys

install_location = '/sdr/dom/'

if len(sys.argv) != 2:
	sys.exit(1)

sys.argv.append('--install-lib='+install_location)

setup(name='OSSIETalk', 
      description='OSSIETalk',
      data_files=[
                  (install_location+'bin/OSSIETalk',
                       ['OSSIETalk.py', 
                        'WorkModules.py',
                        'wx_inits.py',   
                        'port_impl.py',
                        'cvsd.py']),
                  (install_location+'xml/OSSIETalk',
                       ['OSSIETalk.prf.xml',
                        'OSSIETalk.scd.xml', 
                        'OSSIETalk.spd.xml'])
                 ]
     )
