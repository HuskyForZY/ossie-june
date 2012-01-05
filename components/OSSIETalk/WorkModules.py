
'''
/****************************************************************************

Copyright 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE OSSIETalk.

OSSIE OSSIETalk is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE OSSIETalk is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE OSSIETalk; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/

'''

#!/usr/bin/env python
import threading
from ossie.standardinterfaces import standardInterfaces__POA
from ossie.standardinterfaces import standardInterfaces as SI

import math
import random

import time
import ossaudiodev
import struct   # for audio
import copy

import cvsd     # 

import pprint

def pack_audio(v, num_channels=1):
    '''packs a list of signed shorts into a string which can be read by
    the audio device'''
    s = ''
    for i in v:
        s += struct.pack('h', i)
        if num_channels==2:
            s+= '\0\0'
    return s

def unpack_audio(s, num_channels=1):
    '''unpacks a string into a vector'''
    v = []
    if num_channels==2:
        d = 4
    else:
        d = 2

    for i in range(0,len(s),d):
        v.append( struct.unpack('h', s[i:(i+2)])[0] )
    return v

def energy(x):
    e = 0
    for i in x:
        e += i*i
    return math.sqrt(e/len(x))

# global sound card variables
audio_sampling_frequency = 16000 #16000
audio_channels = 1
audio_buffer_length = 1024 #1024

def CreateMetadataObject():
    modulation_scheme = SI.ModulationScheme(SI.ModulationScheme.PSK, 2)

    packet_id = 0
    sampling_frequency = 0.0
    carrier_frequency = 0.0
    signal_bandwidth = 0.0
    signal_strength = 0.0
    eom = True
    src_id = 0
    dst_id = 0
    app_id = 0
    port_id = 0
    packet_type = 0
    default_metadata = SI.MetaData(modulation_scheme,
                                   packet_id,
                                   sampling_frequency,
                                   carrier_frequency,
                                   signal_bandwidth,
                                   signal_strength,
                                   eom,
                                   src_id,
                                   dst_id,
                                   app_id,
                                   port_id,
                                   packet_type)
    return default_metadata


class txWorkClass:

    def __init__(self, OSSIETalk_ref):
        '''Initialization.  Sets a reference to parent.  
        Initializes the buffer.  Starts the Process data
        thread, which waits for data to be added to the buffer'''

        self.OSSIETalk_ref = OSSIETalk_ref
    
        # open the capture device and set properties
        self.Fs = audio_sampling_frequency   # sampling frequency
        self.num_channels = audio_channels

        # open the playback device and set properties
        try:
            self.mic_driver = ossaudiodev.open('r')
        except IOError:
            print "[WorkModules] Failed to open playback device for reading"
        self.mic_driver.setfmt(ossaudiodev.AFMT_S16_LE)
        self.mic_driver.channels(self.num_channels)
        self.mic_driver.speed(self.Fs)


        self.buf_len = audio_buffer_length

        self.voice_data_queue = []
        self.voice_data_queue_lock = threading.Lock()
        self.voice_data_signal = threading.Event()
        self.is_running = True 

        # create meta data objects for audio and text
        #HVself.audio_metadata = CreateMetadataObject()
        #HVself.text_metadata = CreateMetadataObject()

        self.my_encoder = cvsd.encoder()
        self.my_decoder = cvsd.decoder()
        self.CVSD_e_thread = threading.Thread(target=self.CVSD_encode)
        self.CVSD_e_thread.start()
        
        self.process_thread = threading.Thread(target=self.Process)
        self.process_thread.start()
    
    def __del__(self):
        '''Destructor'''
        self.mic_driver.close()
        self.speaker_driver.close()
        pass
    
    def CVSD_encode(self):
        # get voice from mic
        while self.is_running:
            raw_voice = self.mic_driver.read(self.buf_len)
            voice_data = unpack_audio(raw_voice, self.num_channels)
            self.AddVoiceData(voice_data)
            
            # generate sine wave
            #voice_data = [];
            #Fc = 440
            #Ac = 10000
            #for t in range(512):
            #    voice_data.append( Ac*math.sin(2*math.pi*t*Fc/self.Fs) );
            #self.AddVoiceData(voice_data)

            # For now, I need to sleep for a little bit so that 
            # this thread does not take up all of my processor.
            # This will effectively slow my data rate
            #time.sleep(.1)

    def AddVoiceData(self, data):
        '''Generally called by parent.  Adds data to a buffer.
        The Process() method will wait for this buffer to be set.
        '''
        self.voice_data_queue_lock.acquire()
        self.voice_data_queue.insert(0, data)
        self.voice_data_queue_lock.release()
        self.voice_data_signal.set()
    
    def SendTextData(self,text):
        # TODO: configure meta data here
        #HVself.text_metadata.app_id = 2
        #HVself.text_metadata.packet_id += 1
        #HVself.OSSIETalk_ref.to_radio_port_servant.send_data(
        #        text,
        #        copy.copy(self.text_metadata))
        self.OSSIETalk_ref.to_radio_port_servant.send_data(text)
    
    def Release(self):
        self.is_running = False
        self.voice_data_signal.set()
        
    def Process(self):
        # assume talk button is not pressed when I first start
        previous_talk_flag = False

        while self.is_running:
            self.voice_data_signal.wait()  # wait for data to be aded to the 
                                           # buffer in self.AddVoiceData()
            while len(self.voice_data_queue) > 0:
                # get the data from the buffer:
                self.voice_data_queue_lock.acquire()
                data = self.voice_data_queue.pop()
                self.voice_data_queue_lock.release()

                # save a copy of the talk_flag to look at
                # because I can't have this changing on me half-
                # way through                
                current_talk_flag = self.OSSIETalk_ref.talk_flag

                # forwarding voice data:
                if current_talk_flag:
                    # if the push to talk button is depressed.  does not 
                    # matter what the previous state was
                    # sending a packet with eom = False

                    # encode audio data using CVSD codec
                    data_int = self.my_encoder.Encode(data)
                    # pack integers into string
                    # NOTE: this does NOT mean that an integer zero is
                    #   converted into an ASCII '0' character, but rather
                    #   the '\x00' character
                    data = ''
                    for b in data_int:
                        data += struct.pack('B', b);
                    # tmp: true for voice
                    # TODO: configure meta data here
                    #HV self.audio_metadata.app_id = 1
                    #HV self.audio_metadata.packet_id += 1
                    #HV self.audio_metadata.eom = False

                    if self.OSSIETalk_ref.to_radio_port_active:
                        #print "audio length: " + str(len(data))
                        self.OSSIETalk_ref.to_radio_port_servant.send_data(data)

                if current_talk_flag == False and previous_talk_flag == True:
                    # button was just released
                    # send eom = True with one last packet
                    
                    # encode audio data using CVSD codec
                    data_int = self.my_encoder.Encode(data)
                    # pack integers into string
                    # NOTE: this does NOT mean that an integer zero is
                    #   converted into an ASCII '0' character, but rather
                    #   the '\x00' character
                    data = ''
                    for b in data_int:
                        data += struct.pack('B', b);
                    # tmp: true for voice
                    # TODO: configure meta data here
                    #HV self.audio_metadata.app_id = 1
                    #HV self.audio_metadata.packet_id += 1
                    #HV self.audio_metadata.eom = True

                    if self.OSSIETalk_ref.to_radio_port_active:
                        #print "audio length: " + str(len(data))
                        #HV self.OSSIETalk_ref.to_radio_port_servant.send_data(
                        #HV        data, copy.copy(self.audio_metadata))
                          self.OSSIETalk_ref.to_radio_port_servant.send_data(data)        
                                

                # keep track of previous flag state
                # always do this
                previous_talk_flag = current_talk_flag


            self.voice_data_signal.clear()  # done reading the buffer
                


class rxWorkClass:

    def __init__(self, OSSIETalk_ref):
        '''Initialization.  Sets a reference to parent.  
        Initializes the buffer.  Starts the Process data
        thread, which waits for data to be added to the buffer'''

        self.OSSIETalk_ref = OSSIETalk_ref
   
        # new method: 
        self.num_channels = audio_channels
        self.Fs = audio_sampling_frequency
        try:
            self.speaker_driver = ossaudiodev.open('w')
        except IOError:
            print "[WorkModules] Failed to open playback device for writing"
        self.speaker_driver.setfmt(ossaudiodev.AFMT_S16_LE)
        self.speaker_driver.channels(self.num_channels)
        self.speaker_driver.speed(self.Fs)

        self.data_queue = []
        self.data_queue_lock = threading.Lock()
        self.data_signal = threading.Event()

        self.is_running = True

        # old method:
        #self.channels = 2
        #self._setup_sound()

        self.my_decoder = cvsd.decoder()

        self.process_thread = threading.Thread(target=self.Process)
        self.process_thread.start()

    def _setup_sound(self):
        self.timing_diff = 1000
        self.ossspeed = 100000
        self.osschannels = 1
        self.ossfmt = ossaudiodev.AFMT_S16_LE
        self.CORBA_being_used = False

        try:
            self.sound_driver = ossaudiodev.open('w')
        except IOError:
            print "[WorkModules] Failed to open playback device for writing"
        self.osschannels = self.sound_driver.channels(2)
        self.ossfmt = self.sound_driver.setfmt(ossaudiodev.AFMT_S16_LE)
        self.ossspeed = self.sound_driver.speed(50000)
        self.sound_driver.nonblock()
    
    def __del__(self):
        '''Destructor'''
        pass
    
    def AddMetaData(self, data, meta_data):
        '''Generally called by parent.  Adds data to a buffer.
        The Process() method will wait for this buffer to be set.
        '''
        self.data_queue_lock.acquire()
        self.data_queue.insert(0, (data, meta_data) )
        self.data_queue_lock.release()
        self.data_signal.set()
    
    def AddData(self, data):
        '''Generally called by parent.  Adds data to a buffer.
        The Process() method will wait for this buffer to be set.
        '''
        self.data_queue_lock.acquire()
        self.data_queue.insert(0, (data) )
        self.data_queue_lock.release()
        self.data_signal.set()
    
    def CVSDDecode(self, data):
        # data is real char
        unpacked_audio = self.my_decoder.Decode(data)

        # the new method
        packed_audio = pack_audio(unpacked_audio, self.num_channels)
        self.speaker_driver.writeall(packed_audio)        

    def Release(self):
        self.is_running = False
        self.data_signal.set()
        
    def Process(self):
        while self.is_running:

            self.data_signal.wait()  # wait for data to be aded to the 
                                     # buffer in self.AddVoiceData()
            while len(self.data_queue) > 0:
                # get the data from the buffer:
                self.data_queue_lock.acquire()
                #HV (data, metadata) = self.data_queue.pop()
                (data) = self.data_queue.pop()
                self.data_queue_lock.release()
        
                # Use metadata application identifier to forward
                # data appropriately
                #HV if metadata.app_id==1:
                    # voice data
                    # convert string to list of integers
                data_int = []
                for c in data:
                    data_int.append( int(struct.unpack('B', c)[0]) )

                self.CVSDDecode(data_int)
                #HV
                #elif metadata.app_id==2:
                #    # text data
                #    self.OSSIETalk_ref.prnt_app.frame.DisplayText(data)
                #
                #else:
                    # unknown application identifier
                #print "ERROR! OSSIETalk: unknown app_id: " + str(metadata.app_id)

            self.data_signal.clear()  # done reading the buffer




