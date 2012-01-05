## Copyright 2007 Virginia Polytechnic Institute and State University
##
## This file is part of the OSSIE CVSD Library.
##
## OSSIE CVSD Library is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## OSSIE CVSD Library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with OSSIE CVSD Library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


class cvsd:
    '''Base class for continuously variable slope delta codec'''

    def __init__(self):
        '''Initialization'''
        # signal processing variables
        self.N = 3
        self.num_ones = 0
        self.num_zeros = 0
        self.ref = 0
        self.delta = 4096
        self.delta_min = 64
        self.delta_max = 16384
        self.output_limit = 16000
        
class encoder(cvsd):
    '''CVSD Encoder class'''

    def __init__(self):
        cvsd.__init__(self)

    def Encode(self, vect):
        '''Encode a vector of audio samples into a bit stream'''
        return self.EncodeAudioVector(vect)
        

    def EncodeAudioVector(self, vect):
        '''Encodes audio vector into bit stream of same length'''
        bits = []
        for v in vect:
            bits.append(self.EncodeAudioSample(v))
        return bits


    def EncodeAudioSampleBasic(self, v):
        '''Encodes single audio sample using simple threshold'''
        if v > 0:
            return 1
        else:
            return 0

    def EncodeAudioSample(self, v):
        '''Encodes single audio sample using true CVSD codec'''
        if self.ref > v:
            b = 0
            self.num_ones = 0
            self.num_zeros += 1
        else:
            b = 1
            self.num_ones += 1
            self.num_zeros = 0

        # update delta
        if self.num_zeros >= self.N or self.num_ones >= self.N:
            self.delta *= 2
            if self.delta > self.delta_max:
                self.delta = self.delta_max
        else:
            self.delta /= 2
            if self.delta < self.delta_min:
                self.delta = self.delta_min

        # update reference value
        if b==1:
            self.ref += self.delta
            if self.ref > self.output_limit:
                self.ref = self.output_limit
        else:
            self.ref -= self.delta
            if self.ref < -self.output_limit:
                self.ref = -self.output_limit

        return b

class decoder(cvsd):
    '''CVSD Decoder class'''

    def __init__(self):
        cvsd.__init__(self)

    def Decode(self, bits):
        '''Decode bit sequence into audio stream of same length'''
        return self.DecodeAudioVector(bits)

    def DecodeAudioVector(self, bits):
        '''Decodes bits tream into audio vector of same length'''
        vect = []
        for b in bits:
            vect.append(self.DecodeAudioSample(b))
        return vect

    def DecodeAudioSampleBasic(self, b):
        '''Decodes single audio sample using simple threshold'''
        if b==0:
            return -10000
        else:
            return 10000


    def DecodeAudioSample(self, b):
        '''Decodes single audio sample using true CVSD codec'''
        if b == 1:
            self.num_ones += 1
            self.num_zeros = 0
        else:
            self.num_ones = 0
            self.num_zeros += 1

        # update delta
        if self.num_zeros >= self.N or self.num_ones >= self.N:
            # double delta value
            self.delta *= 2
            if self.delta > self.delta_max:
                self.delta = self.delta_max
        else:
            # halve delta
            self.delta /= 2
            if self.delta < self.delta_min:
                self.delta = self.delta_min

        # update reference value
        if b == 0:
            self.ref += self.delta
            if self.ref > self.output_limit:
                self.ref = self.output_limit
        else:
            self.ref -= self.delta
            if self.ref < -self.output_limit:
                self.ref = -self.output_limit

        return self.ref






        return ref


