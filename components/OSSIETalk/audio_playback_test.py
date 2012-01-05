#!/usr/bin/env python

# audio_playback_test.py
#
# This script tests the audio device playback by generating a tone
# and sending it to the speakers.  It does not test audio capture
# in any way.
#
# This file does NOT need to be installed with the GUI component.
# It is strictly for testing

import ossaudiodev as oss
import math
import struct
import random
import pprint
from math import *

from WorkModules import pack_audio, unpack_audio

if __name__ == '__main__':
    # define properties
    Fs = 8000   # sampling frequency
    Fc = 440    # audio tone frequency
    Ac = 10000  # amplitude: -32767 < Ac < 32767
    T = 2       # time (seconds) of playback
    num_channels = 2

    # open the device and set properties
    try:
        d = oss.open('w')
    except IOError:
        print "[audio_playback_test] Failed to open playback device for writing"
    d.setfmt(oss.AFMT_S16_LE)
    d.channels(num_channels)
    d.speed(Fs)

    # create audio data; approximately one second long
    #a = [random.randint(-10000, 10000) for x in range(Fs)]
    a = []
    for t in range(Fs):
        a.append( Ac*sin(2*pi*t*Fc/Fs) )
    s = pack_audio(a, num_channels)

    # play audio
    for i in range(0, T+1):
        d.writeall(s)

    # close the device
    d.close()

