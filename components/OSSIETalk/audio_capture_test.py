#!/usr/bin/env python

# audio_capture_test.py
#
# This script tests capturing audio from the sound device, calculating
# a frame of energy, and then printing the energy to the screen.  It
# does not test playback in any way.
#
# Run this script and blow into the microphone to see the energy change.
#
# This file does NOT need to be installed with the GUI component.
# It is strictly for testing

import ossaudiodev as oss
import math
import struct
import random
import pprint

from WorkModules import pack_audio, unpack_audio, energy

if __name__ == '__main__':
    # define properties
    Fs = 8000   # sampling frequency
    num_channels = 2
    T = 10      # number of seconds to capture

    # open the device and set properties
    try:
        d = oss.open('r')
    except IOError:
        print "[audio_capture_test] Failed to open playback device for reading"
    d.setfmt(oss.AFMT_S16_LE)
    d.channels(num_channels)
    d.speed(Fs)

    # try to record audio, calculate energy, and dump to screen
    s = ''
    buf_len = 4000
    N = int(2*T*Fs/buf_len)
    for i in range(0, N):
        s = d.read(buf_len)
        v = unpack_audio(s, num_channels)
        print "energy = " + str(int(energy(v)))

    # close the device
    d.close()

