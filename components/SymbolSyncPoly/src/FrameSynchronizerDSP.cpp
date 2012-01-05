/****************************************************************************

Copyright 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE FrameSynchronizer.

OSSIE FrameSynchronizer is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE FrameSynchronizer is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE FrameSynchronizer; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/


#include <iostream>
#include "FrameSynchronizerDSP.h"
#include <math.h>

// Default constructor
FrameSynchronizerDSP::FrameSynchronizerDSP()
{
    inputBuffer.SetBufferSize( FRAME_SYNC_BUFFER_SIZE );
    inputBuffer.Release();

// Load buffer with zeros
    for (unsigned int i=0; i<FRAME_SYNC_BUFFER_SIZE; i++)
        inputBuffer.Push( 0 );

    operationalMode = EXTRACT_PN_FRAME_SYNC_CODE;
    numControlCodesReceived = 0;

    controlCode = new int[NUM_CONTROL_CODES];

    r_frame_sync = 0;

    UpdatePhaseLoopFilterCoefficients(0.0025f);
    UpdateTimingLoopFilterCoefficients(ALPHA_T_HI, BETA_T_HI);

    e_i = 0;
    e_q = Ac;
    lock_zeta = 0.02;
    header_phase_locked = false;
    header_phase_lock_delay = 64;
    header_phase_lock_delay_counter = 0;

    eomSize = 1024;
    FRAME_SIZE_1=512;            ///< Frame size option 1
    FRAME_SIZE_2=2048;           ///< Frame size option 2
    FRAME_SIZE_3=4096;           ///< Frame size option 3
    FRAME_SIZE_4=8192;           ///< Frame size option 4


#ifdef FS_LOGGING
    input_log.open("framesync_input_log.dat");
#endif
}

// Destructor
FrameSynchronizerDSP::~FrameSynchronizerDSP()
{
    delete [] controlCode;

#ifdef FS_LOGGING
    input_log.close();
#endif
}

// Find frame header using conventional method
bool FrameSynchronizerDSP::FindFrameHeader(
    short * I_in,
    short * Q_in,
    unsigned int input_length,
    unsigned int &num_read,
    short * I_out,
    short * Q_out,
    unsigned int output_length,
    unsigned int &num_written)
{
    unsigned int i(0);
    num_read = 0;
    num_written = 0;
    alpha_c = 0.04f;
//beta_c = 0.0028284f;  // k=8
    beta_c = 0.002;         // k=2

    alpha_s = 0.027f;
//beta_s = 1.024f;
//beta_s = 1.02f;     // k = 8
    beta_s = 1.01f;     // k = 2

    while ( i<input_length ) {
        // Increment sample counter for output enable
        v++;

        PushInputs( I_in, Q_in, i, input_length );

        if ( v >= (signed int) k ) {
            // Enable output
            v -= k;

            // Decimate values; compute filter outputs mf_i, mf_q, dmf_i, dmf_q
            ComputeFilterBankOutputs();

            I_out[num_written] = mf_i;
            Q_out[num_written] = mf_q;
            num_written++;

            // push sample into buffer and correlate with frame sync code
            inputBuffer.Push(mf_i);
            //r = CorrelateSequence( pnFrameSyncCode, 255 );
            // try correlating to only last several elements
            r = CorrelateSequence( pnFrameSyncCode, 255, 127 );

            if ( abs(r) > int( Ac*0.7f ) ) {
                r_frame_sync = abs(r);
                std::cout << "PN FRAME SYNC CODE FOUND!!!" << std::endl;
                std::cout << "  r = " << r_frame_sync << std::endl;

                header_phase_lock_delay_counter = 0;
                // check for phase inversion; if so inject pi radians into NCO
                // [not sure how well this will work with band-limited pulse shapes]
                if ( r<0 ) {
                    std::cout << "  [Frame sync found negative PN code]" << std::endl;
                    nco_control += PI;
                }

                operationalMode = EXTRACT_CONTROL_CODES;
                inputBuffer.Release();
                numControlCodesReceived = 0;

                num_read = i;
                alpha_s = 0.027f;
                //beta_s = 1.024f;  // k=8
                beta_s = 1.018f;    // k=2
                return true;
            }

            // generate timing error
            GenerateTimingError();

            // advance timing loop filter
            AdvanceTimingLoopFilter();

            // compute filter bank index
            ComputeFilterBankIndex();


            // Update lock detection
            e_i = lock_zeta*(float)(abs(mf_i)) + (1-lock_zeta)*e_i;
            e_q = lock_zeta*(float)(abs(mf_q)) + (1-lock_zeta)*e_q;
            header_phase_lock_delay_counter++;
            if ( header_phase_lock_delay_counter >= header_phase_lock_delay ) {
                // Calculate lock detection trigger
                lock_detect = e_i/e_q;
                header_phase_lock_delay_counter = 0;

                if ( lock_detect < 1.25f ) {
                    // Not locked to BPSK header (quadrature signal energy is too large)
                    //   => estimate carrier offset and apply to NCO
                    //printf("estimating offset...\n");
                    //printf("  lock_detect = %f\n", lock_detect);

                    unsigned int N_est = input_length;
                    wcTs = 2*PI*EstimateCarrierOffset(I_in, Q_in, N_est, false);
                    //printf("  estimate: %f\n", wcTs/(2*PI));
                    cv = 0;
                    nco_control = 0;
                    q_c = 0;
                    q_hat_c = 0;
                    buff2_c = 0;
                    q_prime_c = 0;
                    e_i = Ac/2;
                    e_q = Ac/2;
                }
            } else {
                // Calculate phase error

                q_c = GeneratePhaseError( mf_i, mf_q );
                q_c /= Ac;
            }

            AdvancePhaseLoopFilter();

        }
        UpdateNCO();

    }

    num_read = input_length;
    return false;
}


// Extract frame header
bool FrameSynchronizerDSP::ExtractFrameHeader(
    short * I_in,
    short * Q_in,
    unsigned int input_length,
    unsigned int &num_read,
    short * I_out,
    short * Q_out,
    unsigned int output_length,
    unsigned int &num_written)
{
    unsigned int i(0);
    num_read = 0;
    num_written = 0;
//alpha_c = 0.04f;
//beta_c = 0.0028284f;

    while ( i<input_length ) {
        // Increment sample counter for output enable
        v++;

        PushInputs( I_in, Q_in, i, input_length );

        if ( v >= (signed int) k ) {
            // Enable output
            v -= k;

            // Decimate values; compute filter outputs mf_i, mf_q, dmf_i, dmf_q
            ComputeFilterBankOutputs();

            //I_out[num_written] = mf_i;
            //Q_out[num_written] = mf_q;
            //num_written++;

            // Push input into buffer
            inputBuffer.Push( mf_i );

            if ( inputBuffer.GetNumElements() == 31 ) {
                // Correlate input sequence with pnControlCode
                r = CorrelateSequence( pnControlCode, 31, 0 );
                //printf("  code %d, rxy = %d\n", numControlCodesReceived, r);

                // Store result into array
                controlCode[numControlCodesReceived++] = r;

                // Release buffer for next sequence
                inputBuffer.Release();

                if ( numControlCodesReceived == NUM_CONTROL_CODES ) {
                    printf("control codes extracted!\n");
                    numFrameSymbolsReceived = 0;
                    numEOMSymbolsReceived = 0;

                    operationalMode = DECODE_FRAME_HEADER;
                    num_read = i;
                    return true;
                }
            }

            // generate timing error
            GenerateTimingError();

            // advance timing loop filter
            AdvanceTimingLoopFilter();

            // compute filter bank index
            ComputeFilterBankIndex();

            q_c = GeneratePhaseError( mf_i, mf_q );
            q_c /= Ac;

            AdvancePhaseLoopFilter();
        }

        UpdateNCO();

    }

    num_read = input_length;
    return false;
}


bool FrameSynchronizerDSP::DecodeFrameHeader()
{
// switch modulation type
    unsigned int ms = DecodeControlSequence(0,2);
    switch (ms) {
    case (0):
        SetModulationScheme( SigProc::BPSK );
        break;
    case (1):
        SetModulationScheme( SigProc::QPSK );
        break;
    case (2):
        SetModulationScheme( SigProc::PSK8 );
        break;
    case (3):
        SetModulationScheme( SigProc::QAM16 );
        break;
    case (4):
        SetModulationScheme( SigProc::PAM4 );
        break;
    default:
        ;
    }

// specify frame size
    unsigned int fs = DecodeControlSequence(3,4);
    switch ( fs ) {
    case 0:
        frameSize = FRAME_SIZE_1;
        break;
    case 1:
        frameSize = FRAME_SIZE_2;
        break;
    case 2:
        frameSize = FRAME_SIZE_3;
        break;
    case 3:
        frameSize = FRAME_SIZE_4;
        break;
    }

    /**
        // specify frame type
        unsigned int ft = DecodeControlSequence(5,6);
        switch ( ft ) {
        case 0:
            frameType = FRAME_TYPE_DATA;
            eomSize = 1024;
            eomSize=0;
            break;
        case 1:
            std::cout << "FRAME SYNCRONIZER GOT CONTROL FRAME!" << std::endl;
            frameType = FRAME_TYPE_CONTROL;
            SetModulationScheme( SigProc::QPSK );
            frameSize = 512;
            eomSize = 0;
            break;
        default:
            std::cout << "WARNING! Unknown frame type " << ft << std::endl
                      << "  => using FRAME_TYPE_DATA" << std::endl;
            frameType = FRAME_TYPE_DATA;
            eomSize = 1024;
            eomSize=0;
        }
    **/

    std::cout << "FRAME HEADER EXTRACTED (mod: " << ms
              << ", frame size: " << frameSize
              << ", frame type: " << (int) frameType << ") : r="
              << r_frame_sync << std::endl;
    std::cout << "  control : ";
    for (unsigned int ii=0; ii<NUM_CONTROL_CODES; ii++)
        std::cout << controlCode[ii] << " ";
    std::cout << std::endl;

    operationalMode = EXTRACT_FRAME;

    return true;
}



// Extract frame symbols
bool FrameSynchronizerDSP::ExtractFrameSymbols(
    short * I_in,
    short * Q_in,
    unsigned int input_length,
    unsigned int &num_read,
    short * I_out,
    short * Q_out,
    unsigned int output_length,
    unsigned int &num_written)
{
    return ExtractFrameSymbolsFixedLength(
               I_in, Q_in, input_length, num_read,
               I_out, Q_out, output_length, num_written);
}

// Extract frame symbols
bool FrameSynchronizerDSP::ExtractFrameSymbolsFixedLength(
    short * I_in,
    short * Q_in,
    unsigned int input_length,
    unsigned int &num_read,
    short * I_out,
    short * Q_out,
    unsigned int output_length,
    unsigned int &num_written)
{
    unsigned int i(0);
    num_read = 0;
    num_written = 0;
// reduce bandwidth of carrier loop filter
//alpha_c = 0.002f;
//beta_c =  0.000005f;

    while ( i<input_length ) {
        // Increment sample counter for output enable
        v++;

        PushInputs( I_in, Q_in, i, input_length );

        if ( v >= (signed int) k ) {
            // Enable output
            v -= k;

            // Decimate values; compute filter outputs mf_i, mf_q, dmf_i, dmf_q
            ComputeFilterBankOutputs();

            I_out[num_written] = mf_i;

            Q_out[num_written] = mf_q;//-removed

            num_written++;
            numFrameSymbolsReceived++;

            if ( numFrameSymbolsReceived == frameSize ) {
                // Frame extracted
                std::cout << "FRAME EXTRACTED" << std::endl;

                // switch mode
                SetModulationScheme( SigProc::BPSK );
                if (frameType == FRAME_TYPE_CONTROL) {
                    std::cout << "Control frame: ignoring EOM" << std::endl;
                    operationalMode = EXTRACT_PN_FRAME_SYNC_CODE;
                } else {
                    //std::cout << "Data frame: seeking EOM" << std::endl;
                    //operationalMode = EXTRACT_EOM_CODE;
                    operationalMode = EXTRACT_PN_FRAME_SYNC_CODE;
                }
                num_read = i;
                return true;
            }

            // generate timing error
            GenerateTimingError();

            // advance timing loop filter
            AdvanceTimingLoopFilter();

            // compute filter bank index
            ComputeFilterBankIndex();

            q_c = GeneratePhaseError( mf_i, mf_q );
            q_c /= Ac;

            AdvancePhaseLoopFilter();

        }
        UpdateNCO();

    }

    num_read = input_length;
    return false;
}




// Extract EOM symbols
bool FrameSynchronizerDSP::ExtractFrameEOM(
    short * I_in,
    short * Q_in,
    unsigned int input_length,
    unsigned int &num_read,
    short * I_out,
    short * Q_out,
    unsigned int output_length,
    unsigned int &num_written)
{
    unsigned int i(0);
    num_read = 0;
    num_written = 0;
// reduce bandwidth of carrier loop filter
//alpha_c = 0.002f;
//beta_c =  0.000005f;

    while ( i<input_length ) {
        // Increment sample counter for output enable
        v++;

        PushInputs( I_in, Q_in, i, input_length );

        if ( v >= (signed int) k ) {
            // Enable output
            v -= k;

            // Decimate values; compute filter outputs mf_i, mf_q, dmf_i, dmf_q
            ComputeFilterBankOutputs();

            I_out[num_written] = mf_i;
            Q_out[num_written] = mf_q;//-removed

            num_written++;
            numEOMSymbolsReceived++;

            if ( numEOMSymbolsReceived == eomSize ) {
                // Frame extracted
                std::cout << "EOM EXTRACTED" << std::endl;

                // switch mode
                SetModulationScheme( SigProc::BPSK );
                //operationalMode = EXTRACT_EOM_CODE;
                operationalMode = EXTRACT_PN_FRAME_SYNC_CODE;
                num_read = i;
                return true;
            }

            // generate timing error
            GenerateTimingError();

            // advance timing loop filter
            AdvanceTimingLoopFilter();

            // compute filter bank index
            ComputeFilterBankIndex();

            q_c = GeneratePhaseError( mf_i, mf_q );
            q_c /= Ac;

            AdvancePhaseLoopFilter();

        }
        UpdateNCO();

    }

    num_read = input_length;
    return false;
}




// Main signal processing loop
void FrameSynchronizerDSP::SynchronizeAndDecimate(short * I_in,
        short * Q_in,
        unsigned int N_in,
        short * I_out,
        short * Q_out,
        unsigned int & N_out)
{
    if ( !isConfigured ) {
        std::cerr << "ERROR: SymbolSyncPolyDSP::SynchronizeAndDecimate() :" << std::endl
                  << "  => must call ConfigureFilterBank() before invoking this method" << std::endl;
        throw 0;
    }

    unsigned int i(0);
    unsigned int N_out_max(N_out);
    N_out = 0;

    while ( i<N_in ) {
        // Increment sample counter for output enable
        v++;

        PushInputs( I_in, Q_in, i, N_in );

        if ( v >= (signed int) k ) {
            // Enable output
            v -= k;

            // Decimate values; compute filter outputs mf_i, mf_q, dmf_i, dmf_q
            ComputeFilterBankOutputs();
#ifdef FS_LOGGING
            input_log << mf_i << "  " << mf_q << std::endl;
#endif

            //
            switch ( operationalMode ) {
            case EXTRACT_PN_FRAME_SYNC_CODE:
                // correlate to find pnFrameSyncCode
                inputBuffer.Push( mf_i );
                r = CorrelateSequence( pnFrameSyncCode, 255, 0 );

                if ( abs(r) > int( Ac/2 ) ) {
                    r_frame_sync = abs(r);
                    //std::cout << "PN FRAME SYNC CODE FOUND!!!" << std::endl;

                    // check for phase inversion; if so inject pi radians into NCO
                    // [not sure how well this will work with band-limited pulse shapes]
                    if ( r<0 ) {
                        std::cout << "  [Frame sync found negative PN code]" << std::endl;
                        nco_control += PI;
                    }

                    operationalMode = EXTRACT_CONTROL_CODES;
                    inputBuffer.Release();
                    numControlCodesReceived = 0;

                    // reduce bandwidth of loop filters for steady tracking mode
                    UpdatePhaseLoopFilterCoefficients(0.0025f);
                    //UpdateTimingLoopFilterCoefficients(ALPHA_T_LO, BETA_T_LO);

                } else {
                    ///\todo Estimate timing/carrier information if signal is not found
                    /// within a certain amount of time
                }

                break;

            case EXTRACT_CONTROL_CODES:

                // Push input into buffer
                inputBuffer.Push( mf_i );

                if ( inputBuffer.GetNumElements() == 31 ) {
                    // Correlate input sequence with pnControlCode
                    r = CorrelateSequence( pnControlCode, 31, 0 );

                    // Store result into array
                    controlCode[numControlCodesReceived++] = r;

                    // Release buffer for next sequence
                    inputBuffer.Release();

                    if ( numControlCodesReceived == NUM_CONTROL_CODES ) {
                        // switch modulation type
                        unsigned int ms = DecodeControlSequence(0,2);
                        switch (ms) {
                        case (0):
                            SetModulationScheme( SigProc::BPSK );
                            break;
                        case (1):
                            SetModulationScheme( SigProc::QPSK );
                            break;
                        case (2):
                            SetModulationScheme( SigProc::PSK8 );
                            break;
                        case (3):
                            SetModulationScheme( SigProc::QAM16 );
                            break;
                        case (4):
                            SetModulationScheme( SigProc::PAM4 );
                            break;
                        default:
                            ;
                        }

                        // specify frame size
                        unsigned int fs = DecodeControlSequence(3,4);
                        switch ( fs ) {
                        case 0:
                            frameSize = FRAME_SIZE_1;
                            break;
                        case 1:
                            frameSize = FRAME_SIZE_2;
                            break;
                        case 2:
                            frameSize = FRAME_SIZE_3;
                            break;
                        case 3:
                            frameSize = FRAME_SIZE_4;
                            break;
                        }
                        numFrameSymbolsReceived = 0;

                        std::cout << "FRAME HEADER EXTRACTED (mod: " << ms
                                  << ", frame size: " << frameSize << ") : r="
                                  << r_frame_sync << std::endl;
                        std::cout << "  control : ";
                        for (unsigned int ii=0; ii<NUM_CONTROL_CODES; ii++)
                            std::cout << controlCode[ii] << " ";
                        std::cout << std::endl;

                        operationalMode = EXTRACT_FRAME;
                    }
                }
                break;

            case EXTRACT_FRAME:
                // Store result in output buffer
                I_out[N_out] = mf_i;
                Q_out[N_out] = mf_q;//-removed

                // Throw exception if buffer is too small
                if ( N_out == N_out_max ) {
                    std::cerr << "ERROR: FrameSynchronizerDSP::SynchronizeAndDecimate() :"
                              << std::endl << "  => output array is full " << std::endl;
                    throw 0;
                } else {
                    N_out++;
                    numFrameSymbolsReceived++;

                    if ( numFrameSymbolsReceived == frameSize ) {
                        // Frame extracted
                        std::cout << "FRAME EXTRACTED" << std::endl;

                        // switch mode
                        /** Don't go to EOM
                         SetModulationScheme( SigProc::BPSK );
                         operationalMode = EXTRACT_EOM_CODE;
                         **/
                    }
                }
                break;
            case EXTRACT_EOM_CODE:
                ///\todo extract EOM code

                operationalMode = EXTRACT_PN_FRAME_SYNC_CODE;

                // increase bandwidth for acquisition
                //UpdatePhaseLoopFilterCoefficients(0.05);
                //UpdateTimingLoopFilterCoefficients(ALPHA_T_HI, BETA_T_HI);
                break;
            default:
                ;
            }

            GenerateTimingError();

            AdvanceTimingLoopFilter();

            ComputeFilterBankIndex();

            // Calculate phase error

            q_c = GeneratePhaseError( mf_i, mf_q );
            q_c /= Ac;

            AdvancePhaseLoopFilter();

        }

        UpdateNCO();

    } // while

}


// Correlate input sequence with PN code
int FrameSynchronizerDSP::CorrelateSequence( char * _pncode, unsigned int _N, unsigned int offset )
{
    int rxy(0);

    for (unsigned int i=offset; i<_N; i++)
        rxy += ( _pncode[i] ) ? inputBuffer[i] : -inputBuffer[i];

// Scale by length
    rxy /= int(_N-offset);

    return rxy;
}

unsigned int FrameSynchronizerDSP::DecodeControlSequence(
    unsigned int _i1,
    unsigned int _i2 )
{
    if ( _i1 > _i2 ) {
        std::cerr << "ERROR: FrameSyncrhonizerDSP:::DecodeControlSequence "
                  << "_i1 > _i2 (" << _i1 << ">" << _i2 << ")" << std::endl;
        throw 0;
    }

    unsigned int val(0), tmp, len(_i2 - _i1);
    for (unsigned int i=0; i<=len; i++) {
        tmp = ( controlCode[i+_i1] > 0 ) ? 1 : 0;
        tmp <<= len-i;
        val += tmp;
    }
    return val;
}

void FrameSynchronizerDSP::SetFrameSize(unsigned int inframesize, unsigned int inframesizeno)
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


unsigned int FrameSynchronizerDSP::GetFrameSize(unsigned int inframesizeno)
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
