/****************************************************************************

Copyright 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE FrameSynchronizer.

OSSIE FrameSynchronizer is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE FrameSynchronizer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE FrameSynchronizer; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/


#ifndef FRAMESYNCHRONIZERDSP_IMPL_H
#define FRAMESYNCHRONIZERDSP_IMPL_H

#include <stdlib.h>
#include "MultirateSynchronizer.h"
#include "PNCodes.h"

#define FRAME_SYNC_BUFFER_SIZE 255  ///< size of frame sync buffer
#define NUM_CONTROL_CODES 5         ///< number of control codes

//#define FRAME_SIZE_1 2048           ///< Frame size option 1
//#define FRAME_SIZE_2 4096           ///< Frame size option 2
//#define FRAME_SIZE_3 8192           ///< Frame size option 3
//#define FRAME_SIZE_4 512            ///< Frame size option 4

#define FRAME_SYNCHRONIZER_DEFINE_API(X)    \
bool X(                                     \
    short * I_in,                           \
    short * Q_in,                           \
    unsigned int input_length,              \
    unsigned int &num_read,                 \
    short * I_out,                          \
    short * Q_out,                          \
    unsigned int output_length,             \
    unsigned int &num_written);


#undef FS_LOGGING

/** \brief Digital signal processing for the symbol synchronizer component
 *  implemented with the polyphase filter bank
 *
 *
 */
class FrameSynchronizerDSP : public MultirateSynchronizerDSP
{
public:
/// Initializing constructor
    FrameSynchronizerDSP();

/// Destructor
    ~FrameSynchronizerDSP();

// Finds the frame header P/N sync code
    FRAME_SYNCHRONIZER_DEFINE_API(FindFrameHeader)

/// \brief Extracts the frame header data
    FRAME_SYNCHRONIZER_DEFINE_API(ExtractFrameHeader)

/// \brief Decodes the frame header information
    bool DecodeFrameHeader();

/// \brief Extracts the frame header data
    FRAME_SYNCHRONIZER_DEFINE_API(ExtractFrameSymbols)

    FRAME_SYNCHRONIZER_DEFINE_API(ExtractFrameSymbolsFixedLength)

/// \brief Extracts the frame header data
    FRAME_SYNCHRONIZER_DEFINE_API(ExtractFrameEOM)

/// \brief Main signal processing loop (overridden method)
///
/// This method performs simultaneous synchronization and decimation.  It
/// assumes that the calling platform takes care of memory management.
/// N_out initially stores the number of memory elements allocated to
/// I_out and Q_out, and assumes that this number is sufficiently large
/// to run the algorithm.  The method returns N_out as the actual number
/// of elements stored in I_out and Q_out.  This is approximately
/// \f$ N_{out} \approx N_{in}/k \f$
///
/// \param[in]      I_in  In-phase input signal
/// \param[in]      Q_in  Quadrature input signal
/// \param[in]      N_in  Number of input samples
/// \param[out]     I_out In-phase output signal
/// \param[out]     Q_out Quadrature output signal
/// \param[in,out]  N_out Number of (maximum) output samples
    void SynchronizeAndDecimate(short * I_in,
                                short * Q_in,
                                unsigned int N_in,
                                short * I_out,
                                short * Q_out,
                                unsigned int & N_out);

    void SetFrameSize(unsigned int inframesize, unsigned int inframesizeno);

    unsigned int GetFrameSize(unsigned int inframesizeno);

protected:
/// Operational mode
    enum {
        EXTRACT_PN_FRAME_SYNC_CODE,
        EXTRACT_CONTROL_CODES,
        DECODE_FRAME_HEADER,
        EXTRACT_FRAME,
        EXTRACT_EOM_CODE
    } operationalMode;

///
    unsigned int DecodeControlSequence( unsigned int _i1, unsigned int _i2 );

///
    int * controlCode;

/// Circular input buffer
    SigProc::CircularBuffer <short> inputBuffer;

/// Correlate input sequence with PN code
    int CorrelateSequence( char * _pncode, unsigned int _N, unsigned int offset );

/// correlator output
    int r;

    unsigned int numControlCodesReceived;

    unsigned int numFrameSymbolsReceived;

    unsigned int frameSize;

    unsigned int FRAME_SIZE_1,FRAME_SIZE_2,FRAME_SIZE_3,FRAME_SIZE_4;

/// Type of frame
    enum {
        FRAME_TYPE_DATA=0,
        FRAME_TYPE_CONTROL=1,
        FRAME_TYPE_X,
        FRAME_TYPE_Y
    } frameType;

    unsigned int numEOMSymbolsReceived;

    unsigned int eomSize;

    unsigned int r_frame_sync;

// Phase lock detect variables
    float e_i;
    float e_q;
    float lock_zeta;
    float lock_detect;
    bool header_phase_locked;
    unsigned int header_phase_lock_delay;
    unsigned int header_phase_lock_delay_counter;

private:
/// Disallow copy constructor
    FrameSynchronizerDSP(FrameSynchronizerDSP&);

#ifdef FS_LOGGING
    std::ofstream input_log;
#endif

};

#endif

