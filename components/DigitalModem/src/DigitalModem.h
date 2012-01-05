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


#ifndef __DIGITALMODEM_IMPL_H__
#define __DIGITALMODEM_IMPL_H__

#include <stdlib.h>
#include "SigProc.h"

#define BIT0 0
#define BIT1 1

#define TARGET_SIGNAL_ENERGY 10000

/** \brief
 *
 *
 */
class DigitalModem
{
public:
/// Initializing constructor
    DigitalModem();

/// Destructor
    ~DigitalModem();

/// Set modulation scheme
    void SetModulationScheme(SigProc::ModulationScheme _ms);

/// Modulates sequence of bits
    void ModulateSequence(
        char * bits_in,
        unsigned int N_in,
        short * I_out,
        short * Q_out);

/// Demodulates sequence of symbols
    void DemodulateSequence(
        short * I_in,
        short * Q_in,
        unsigned int N_in,
        char * bits_out);

protected:
/// Demodulation function pointer
    void (*Demodulate) (
        short I_in,
        short Q_in,
        short & symbolOut);

/// Modulation function pointer
    void (*Modulate) (
        short symbolIn,
        short &I_out,
        short &Q_out);

///
///\todo check amplitude correction method
    void ScaleSignalAmplitude(
        short * I_in,
        short * Q_in,
        unsigned int N_in,
        unsigned int targetEnergy);

/// Converts bit sequence to symbol
    void ConvertBitsToSymbol(char * bitsIn, unsigned short &s);

/// Converts symbol to bit sequence
    void ConvertSymbolToBits(unsigned short s, char * bitsOut);

    unsigned int bitsPerSymbol;     ///< Number of bits represented by each symbol
    short state_i;                  ///< In-phase state for differential mode
    short state_q;                  ///< Quadrature state for differential mode
    SigProc::ModulationScheme mod_scheme;    ///< Current (de)modulation scheme
    bool differentialMode;          ///< Differential PSK mode flag

private:
/// Disallow copy constructor
    DigitalModem(DigitalModem&);

};

#endif

