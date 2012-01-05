/****************************************************************************

Copyright 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE Demodulator.

OSSIE Demodulator is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE Demodulator is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE Demodulator; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/


#include <iostream>
#include "DigitalModem.h"
#include <math.h>

using namespace SigProc;


// Default constructor
DigitalModem::DigitalModem()
{
    SetModulationScheme( BPSK );
}

// Destructor
DigitalModem::~DigitalModem()
{
}

// Set modulation scheme
void DigitalModem::SetModulationScheme( SigProc::ModulationScheme _ms )
{
    switch ( _ms ) {
    case BPSK:
        Demodulate = &DemodulateBPSK;
        Modulate = &ModulateBPSK;
        bitsPerSymbol = 1;
        differentialMode = false;
        break;
    case QPSK:
        Demodulate = &DemodulateQPSK;
        Modulate = &ModulateQPSK;
        bitsPerSymbol = 2;
        differentialMode = false;
        break;
    case PSK8:
        Demodulate = &Demodulate8PSK;
        Modulate = &Modulate8PSK;
        bitsPerSymbol = 3;
        differentialMode = false;
        break;
    case QAM4:
        Demodulate = &DemodulateQAM4;
        Modulate = &ModulateQAM4;
        bitsPerSymbol = 2;
        differentialMode = false;
        break;
    case QAM16:
        Demodulate = &Demodulate16QAM;
        Modulate = &Modulate16QAM;
        bitsPerSymbol = 4;
        differentialMode = false;
        break;
    case PAM4:
        Demodulate = &Demodulate4PAM;
        Modulate = &Modulate4PAM;
        bitsPerSymbol = 2;
        differentialMode = false;
        break;
    case DBPSK:
        state_i = 0;
        state_q = 0;
        Demodulate = &DemodulateBPSK;
        Modulate = &ModulateBPSK;
        bitsPerSymbol = 1;
        differentialMode = true;
        break;
    case DQPSK:
        state_i = 0;
        state_q = 0;
        Demodulate = &DemodulateQPSK;
        Modulate = &ModulateQPSK;
        bitsPerSymbol = 2;
        differentialMode = true;
        break;
    case DPSK8:
        state_i = 0;
        state_q = 0;
        Demodulate = &Demodulate8PSK;
        Modulate = &Modulate8PSK;
        bitsPerSymbol = 3;
        differentialMode = true;
        break;
    default:
        std::cerr << "ERROR: DigitalModem::SetModulationScheme(): "
                  << "unknown mod. scheme " << _ms << std::endl;
        std::cerr << "  => Using BPSK instead" << std::endl;
        SetModulationScheme( BPSK );
        //throw 0;
    }

    mod_scheme = _ms;
}

// Modulates sequence of bits
void DigitalModem::ModulateSequence(
    char * bits_in,
    unsigned int N_in,
    short * I_out,
    short * Q_out)
{
    unsigned short s;
    unsigned int j(0);

    if ( differentialMode ) {
        ///\todo implement differential PSK modulation
        std::cerr << "ERROR: DigitalModem::DemodulateSequence(): "
                  << "differential mode not yet supported!" << std::endl;
        throw 0;
    } else {
        for (unsigned int i=0; j<N_in; i++) {
            ConvertBitsToSymbol(bits_in+j, s);
            Modulate(s, I_out[i], Q_out[i]);
            j += bitsPerSymbol;
        }
    }

}


//
void DigitalModem::DemodulateSequence(
    short * I_in,
    short * Q_in,
    unsigned int N_in,
    char * bits_out)
{
    short s;
    unsigned int j(0);

// If QAM or PAM correct for amplitude
    if ( mod_scheme == QAM16 || mod_scheme == PAM4 ) {
        ScaleSignalAmplitude(I_in, Q_in, N_in, TARGET_SIGNAL_ENERGY);
    }

    if ( differentialMode ) {
        ///\todo implement differential PSK demodulation
        std::cerr << "ERROR: DigitalModem::DemodulateSequence(): "
                  << "differential mode not yet supported!" << std::endl;
        throw 0;
    } else {
        for (unsigned int i=0; i<N_in; i++) {
            Demodulate(I_in[i], Q_in[i], s);
            ConvertSymbolToBits(s, bits_out+j);
            j += bitsPerSymbol;
        }
    }
}


//
void DigitalModem::ScaleSignalAmplitude(
    short * I_in,
    short * Q_in,
    unsigned int N_in,
    unsigned int targetEnergy)
{
    unsigned int i;
    float energy(0.0f), scalingFactor;
    for (i=0; i<N_in; i++) {
        energy += float(I_in[i])*float(I_in[i]);
        energy += float(Q_in[i])*float(Q_in[i]);
    }
    energy /= float(N_in);

    scalingFactor = float(targetEnergy)/sqrtf(energy);

    for (i=0; i<N_in; i++) {
        I_in[i] = short( I_in[i]*scalingFactor );
        Q_in[i] = short( Q_in[i]*scalingFactor );
    }
}


// Converts symbol to bit sequence
void DigitalModem::ConvertSymbolToBits(unsigned short s, char * bitsOut)
{
    for (unsigned int i=0; i<bitsPerSymbol; i++) {
        bitsOut[bitsPerSymbol-i-1] = ( s & 0x01 ) ? BIT1 : BIT0;
        s >>= 1;
    }
}


// Converts bit sequence to symbol
void DigitalModem::ConvertBitsToSymbol(char * bitsIn, unsigned short &s)
{
    s = 0;

    for (unsigned int i=0; i<bitsPerSymbol; i++) {
        s <<= 1;
        s |= (bitsIn[i] > 0) ? 1 : 0;
    }
}


