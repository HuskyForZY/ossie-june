#!/usr/bin/env python

# audio_cvsd_test.py
#
# This script tests capturing audio from the sound device, running
# it through the CVSD codec, and then playing it back.
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
import cvsd


if __name__ == '__main__':
    # define properties
    Fs = 16000   # sampling frequency
    num_channels = 2
    T = 10      # number of seconds to capture

    # generate CVSD objects
    enc = cvsd.encoder()
    dec = cvsd.decoder()

    # open the playback device and set properties
    try:
        dp = oss.open('w')
    except IOError:
        print "[audio_cvsd_test] Failed to open playback device for writing"
    dp.setfmt(oss.AFMT_S16_LE)
    dp.channels(num_channels)
    dp.speed(Fs)

    # open the capture device and set properties
    try:
        dc = oss.open('r')
    except IOError:
        print "[audio_cvsd_test] Failed to open playback device for reading"
    dc.setfmt(oss.AFMT_S16_LE)
    dc.channels(num_channels)
    dc.speed(Fs)
    
    # try to record audio, delay, and output to speakers
    s = ''
    buf_len = 512*num_channels*2
    # NOTE: delay is approximately buf_len/Fs seconds
    N = int(2*T*Fs/buf_len)
    for i in range(0, N):
        # capture
        s = dc.read(buf_len)

        # unpack and calculate energy
        v = unpack_audio(s, num_channels)
        print "energy = " + str(int(energy(v)))

        # encode audio into bits
        cvsd_bits = enc.Encode(v)

        # decode bits
        cvsd_audio = dec.Decode(cvsd_bits)

        # pack audio data and play it through the speakers
        s = pack_audio(cvsd_audio, num_channels)
        dp.writeall(s)
    
    print "audio delay is " + str(float(buf_len)/Fs) + "s"

    # close the devices
    dp.close()
    dc.close()

