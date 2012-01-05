#!/usr/bin/env python

# audio_loopback_test.py
#
# This script tests capturing audio from the sound device and then
# playing it back with some delay.
#
# This file does NOT need to be installed with the GUI component.
# It is strictly for testing

import ossaudiodev as oss
import math
import struct
import random
import pprint
from math import *

from WorkModules import pack_audio, unpack_audio, energy


if __name__ == '__main__':
    # define properties
    Fs = 8000   # sampling frequency
    num_channels = 2
    T = 10      # number of seconds to capture

    # open the playback device and set properties
    try:
        dp = oss.open('w')
    except IOError:
        print "[audio_loopback_test] Failed to open playback device for writing"
    dp.setfmt(oss.AFMT_S16_LE)
    dp.channels(num_channels)
    dp.speed(Fs)

    # open the capture device and set properties
    try:
        dc = oss.open('r')
    except IOError:
        print "[audio_loopback_test] Failed to open playback device for reading"
    dc.setfmt(oss.AFMT_S16_LE)
    dc.channels(num_channels)
    dc.speed(Fs)
    
    # try to record audio, delay, and output to speakers
    s = ''
    buf_len = 2000
    # NOTE: delay is approximately buf_len/Fs seconds
    N = int(2*T*Fs/buf_len)
    for i in range(0, N):
        # capture
        s = dc.read(buf_len)

        # unpack and calculate energy
        v = unpack_audio(s, num_channels)
        print "energy = " + str(int(energy(v)))

        # play audio
        dp.writeall(s)
    
    print "audio delay is " + str(float(buf_len)/Fs) + "s"

    # close the devices
    dp.close()
    dc.close()

