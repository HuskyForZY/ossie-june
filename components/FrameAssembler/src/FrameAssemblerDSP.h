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


#ifndef FRAMEASSEMBLERDSP_IMPL_H
#define FRAMEASSEMBLERDSP_IMPL_H

#include <stdlib.h>
#include "SigProc.h"
#include "PNCodes.h"

//#define FRAME_SIZE_1 2048   ///< Frame size option 1
//#define FRAME_SIZE_2 4096   ///< Frame size option 2
//#define FRAME_SIZE_3 8192   ///< Frame size option 3
//#define FRAME_SIZE_4 512    ///< Frame size option 4



/** \brief
 *
 *
 */
class FrameAssemblerDSP
{
public:
/// Initializing constructor
    FrameAssemblerDSP();

/// Destructor
    ~FrameAssemblerDSP();

    void SetFrameSize(unsigned int inframesize, unsigned int inframesizeno);

    unsigned int GetFrameSize(unsigned int inframesizeno);

protected:
/// Configure frame for particular modulation scheme
    void ConfigureModulationScheme(SigProc::ModulationScheme _ms);

/// Configure frame for a certain number of output symbols
    void ConfigureFrameType(unsigned int _ft);

/// Configure frame for a certain number of output symbols
    void ConfigureFrameSize(unsigned int _fs);

///
    unsigned int frameSize;

    unsigned int FRAME_SIZE_1,FRAME_SIZE_2,FRAME_SIZE_3,FRAME_SIZE_4;

///
    unsigned int numFrameSymbolsAssembled;

///
    char ** controlBlock;

/// Generates 512-bit \c 10101010 phasing pattern
    void AssemblePhasingPattern(short * I_out, short * Q_out);

/// Generates 512-bit header
    void AssembleHeader(short * I_out, short * Q_out);

/// Modulates input bits
    void AssembleFrame(char * bits_in, short * I_out, short * Q_out);

/// Generates postamble...
/// \todo determine what information should be included here
    void AssemblePostamble();

/// Operational mode
    enum {
        ASSEMBLE_PREAMBLE,
        ASSEMBLE_HEADER,
        ASSEMBLE_FRAME,
        ASSEMBLE_EOM_CODE
    } operationalMode;

/// Type of frame to configure
    enum {
        FRAME_TYPE_DATA=0,
        FRAME_TYPE_CONTROL=1,
        FRAME_TYPE_X,
        FRAME_TYPE_Y
    } frameType;

private:
/// Disallow copy constructor
    FrameAssemblerDSP(FrameAssemblerDSP&);

///
    void WriteSequence(char * bits_in, unsigned int N_in, short * I_out, short * Q_out);

};

#endif

