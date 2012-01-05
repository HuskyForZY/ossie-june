/****************************************************************************

Copyright 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE FrameAssembler.

OSSIE FrameAssembler is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE FrameAssembler is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE FrameAssembler; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/


#include <iostream>
#include "FrameAssemblerDSP.h"
#include <math.h>

using namespace SigProc;

// Default constructor
FrameAssemblerDSP::FrameAssemblerDSP()
{
// Initialize control block pointers
    controlBlock = new char*[8];

// Default control block codes to be all positive
    for (unsigned int i=0; i<8; i++)
        controlBlock[i] = pnControlCode;

    numFrameSymbolsAssembled = 0;

    FRAME_SIZE_1=512;            ///< Frame size option 1
    FRAME_SIZE_2=2048;           ///< Frame size option 2
    FRAME_SIZE_3=4096;           ///< Frame size option 3
    FRAME_SIZE_4=8192;           ///< Frame size option 4

    ConfigureModulationScheme( BPSK );
    ConfigureFrameSize(1);
    ConfigureFrameType( FRAME_TYPE_DATA );
}

// Destructor
FrameAssemblerDSP::~FrameAssemblerDSP()
{
    delete [] controlBlock;
}

void FrameAssemblerDSP::ConfigureModulationScheme(ModulationScheme _ms)
{
// Set appropriate modulation function
    switch (_ms) {
    case BPSK:
        controlBlock[0] = pnControlCodeInv; // 0
        controlBlock[1] = pnControlCodeInv; // 0
        controlBlock[2] = pnControlCodeInv; // 0
        break;
    case QPSK:
        controlBlock[0] = pnControlCodeInv; // 0
        controlBlock[1] = pnControlCodeInv; // 0
        controlBlock[2] = pnControlCode;    // 1
        break;
    case PSK8:
        controlBlock[0] = pnControlCodeInv; // 0
        controlBlock[1] = pnControlCode;    // 1
        controlBlock[2] = pnControlCodeInv; // 0
        break;
    case QAM16:
        controlBlock[0] = pnControlCodeInv; // 0
        controlBlock[1] = pnControlCode;    // 1
        controlBlock[2] = pnControlCode;    // 1
        break;
    case PAM4:
        controlBlock[0] = pnControlCode;    // 1
        controlBlock[1] = pnControlCodeInv; // 0
        controlBlock[2] = pnControlCodeInv; // 0
        break;
    default:
        // unknown property
        std::cerr << "ERROR: FrameAssemblerDSP::ConfigureModulationScheme(): "
                  << "unsupported mod. scheme " << _ms << std::endl;
        throw 0;
    }
}


void FrameAssemblerDSP::ConfigureFrameType(unsigned int _ft)
{
    switch (_ft) {
    case FRAME_TYPE_DATA:
        controlBlock[5] = pnControlCodeInv; // 0
        controlBlock[6] = pnControlCodeInv; // 0
        frameType = FRAME_TYPE_DATA;
        break;
    case FRAME_TYPE_CONTROL:
        controlBlock[5] = pnControlCodeInv; // 0
        controlBlock[6] = pnControlCode;    // 1
        frameType = FRAME_TYPE_CONTROL;
        ConfigureFrameSize(1);
        break;
    default:
        // unsupported frame size
        std::cout << "WARNING: FrameAssemblerDSP::ConfigureFrameType(): "
                  << "unsupported frame type (" << _ft << ")" << std::endl;
        std::cout << "  => using FRAME_TYPE_DATA instead" << std::endl;
        ConfigureFrameType(FRAME_TYPE_DATA);
        return;
    }

}

void FrameAssemblerDSP::ConfigureFrameSize(unsigned int _fs)
{
    switch (_fs) {
    case 1:
        frameSize = FRAME_SIZE_1;
        controlBlock[3] = pnControlCodeInv;
        controlBlock[4] = pnControlCodeInv;
        break;
    case 2:
        frameSize = FRAME_SIZE_2;
        controlBlock[3] = pnControlCodeInv;
        controlBlock[4] = pnControlCode;
        break;
    case 3:
        frameSize = FRAME_SIZE_3;
        controlBlock[3] = pnControlCode;
        controlBlock[4] = pnControlCodeInv;
        break;
    case 4:
        frameSize = FRAME_SIZE_4;
        controlBlock[3] = pnControlCode;
        controlBlock[4] = pnControlCode;
        break;
    default:
        // unsupported frame size
        std::cout << "WARNING: FrameAssemblerDSP::ConfigureFrameSize(): "
                  << "unsupported frame size (" << _fs << ")" << std::endl;
        std::cout << "  => using " << FRAME_SIZE_1 << " instead" << std::endl;
        ConfigureFrameSize(1);
    }

//std::cout << "FrameAssemblerDSP::ConfigureFrameSize(): "<<frameSize<<std::endl;
}


// Generates 10101010 phasing pattern
void FrameAssemblerDSP::AssemblePhasingPattern(
    short * I_out,
    short * Q_out)
{
    bool bitFlag(true);

    for (unsigned int i=0; i<512; i++) {

        if ( bitFlag ) {
            I_out[i] =  BPSK_LEVEL;
            Q_out[i] =  0;
        } else {
            I_out[i] = -BPSK_LEVEL;
            Q_out[i] =  0;
        }

        bitFlag = !bitFlag;
    }
}

// Generates header
void FrameAssemblerDSP::AssembleHeader(
    short * I_out,
    short * Q_out)
{
// Write tail end of phasing pattern
    char phasingTail[9] = {1, 0, 1, 0, 1, 0, 1, 0, 1};
    WriteSequence(phasingTail, 9, I_out, Q_out);
    I_out += 9;
    Q_out += 9;

// Write P/N Sync code
    WriteSequence(pnFrameSyncCode, 255, I_out, Q_out);
    I_out += 255;
    Q_out += 255;

// Write control blocks
    for (unsigned int i=0; i<5; i++) {
        WriteSequence(controlBlock[i], 31, I_out, Q_out);
        I_out += 31;
        Q_out += 31;
    }

}

// Generates postamble...
void FrameAssemblerDSP::AssemblePostamble()
{
}

//
void FrameAssemblerDSP::WriteSequence(
    char * bits_in,
    unsigned int N_in,
    short * I_out,
    short * Q_out)
{
//
    for (unsigned int i=0; i<N_in; i++) {
        I_out[i] = ( bits_in[i] ) ? BPSK_LEVEL : -BPSK_LEVEL;
        Q_out[i] = 0;
    }
}

void FrameAssemblerDSP::SetFrameSize(unsigned int inframesize, unsigned int inframesizeno)
{
    switch ( inframesizeno ) {
    case 0:
        std::cerr << "ERROR: FrameSynchronizerDSP::SetFrameSize:0 frame size option not allowed to be set"<<std::endl;
        throw 0;
        break;
    case 1:
        FRAME_SIZE_1=inframesize;
        break;
    case 2:
        FRAME_SIZE_2=inframesize;
        break;
    case 3:
        FRAME_SIZE_3=inframesize;
        break;
    case 4:
        FRAME_SIZE_4=inframesize;
        break;
    default:
        std::cerr << "ERROR: FrameSynchronizerDSP::SetFrameSize:Invalid frame size option"<<std::endl;
        throw 0;
    }

}


unsigned int FrameAssemblerDSP::GetFrameSize(unsigned int inframesizeno)
{
    switch ( inframesizeno ) {
    case 0:
        return frameSize;
        break;
    case 1:
        return FRAME_SIZE_1;
        break;
    case 2:
        return FRAME_SIZE_2;
        break;
    case 3:
        return FRAME_SIZE_3;
        break;
    case 4:
        return FRAME_SIZE_4;
        break;
    default:
        std::cerr << "ERROR: FrameSynchronizerDSP::GetFrameSize:Invalid frame size option"<<std::endl;
        throw 0;
    }

}

