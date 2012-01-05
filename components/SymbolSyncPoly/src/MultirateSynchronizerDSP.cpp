/****************************************************************************

Copyright 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE MultirateSynchronizer.

OSSIE MultirateSynchronizer is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE MultirateSynchronizer is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE MultirateSynchronizer; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/


#include <iostream>
#include "MultirateSynchronizerDSP.h"
#include <math.h>

// Default constructor
MultirateSynchronizerDSP::MultirateSynchronizerDSP()
{
    SetModulationScheme( SigProc::BPSK );

// Initially set to slow tracking
    UpdatePhaseLoopFilterCoefficients(0.0025f);

    q_c = 0.0f;
    q_hat_c = 0.0f;
    buff2_c = 0.0f;
    q_prime_c = 0.0f;

    nco_control = 0.0f;
    cv = 0.0f;
    wcTs = 0.0f;
    nco_i = 1.0f;
    nco_q = 0.0f;

}

// Destructor
MultirateSynchronizerDSP::~MultirateSynchronizerDSP()
{
}

// Set modulation scheme for phase error generator
void MultirateSynchronizerDSP::SetModulationScheme( SigProc::ModulationScheme _ms )
{
    switch ( _ms ) {
    case SigProc::BPSK:
        GeneratePhaseError = &GeneratePhaseErrorBPSK;
        break;
    case SigProc::QPSK:
        GeneratePhaseError = &GeneratePhaseErrorQPSK;
        break;
    case SigProc::QAM4:
        GeneratePhaseError = &GeneratePhaseErrorQAM4;
        break;
    case SigProc::PSK8:
        GeneratePhaseError = &GeneratePhaseError8PSK;
        break;
    case SigProc::QAM16:
        GeneratePhaseError = &GeneratePhaseError16QAM;
        break;
    case SigProc::PAM4:
        GeneratePhaseError = &GeneratePhaseErrorBPSK;
        break;
    default:
        std::cerr << "ERROR: MultirateSynchronizerDSP::SetModulationScheme(): "
                  << "unknown mod. scheme " << _ms << std::endl;
        std::cerr << "  => Using BPSK instead" << std::endl;
        SetModulationScheme( SigProc::BPSK );
        //throw 0;
    }

    mod_scheme = _ms;
}

float MultirateSynchronizerDSP::EstimateCarrierOffset(
    short * I_in,
    short * Q_in,
    unsigned int input_length,
    bool use_matched_filter)
{
    if (use_matched_filter) {
        // create new filter object
        printf("WARNING: MultirateSynchronizerDSP::EstimateCarrierOffset():\n");
        printf("    => does not work yet with matched filter\n");
        use_matched_filter = false;
    }

    float sum_i(0.0f);
    float sum_q(0.0f);
    float _a, _b, _c, _d;
    float nu_hat;

// loop through input
    for (unsigned int i=0; i<input_length-1; i++) {
        // filter input?
        if (use_matched_filter) {
            // push values; compute output
        } else {
            // x[n]
            _a = (float) I_in[i+1];
            _b = (float) Q_in[i+1];

            // conj(x[n-1])
            _c = (float) I_in[i];
            _d = (float) -Q_in[i];
        }
        sum_i += _a*_c - _b*_d;
        sum_q += _a*_d + _b*_c;
    }

    nu_hat = atan2(sum_q, sum_i) / (2*PI);
    return nu_hat;
}


// Main signal processing loop
void MultirateSynchronizerDSP::SynchronizeAndDecimate(short * I_in,
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

        if ( v >= k ) {
            // Enable output
            v -= k;

            // Decimate values; compute filter outputs mf_i, mf_q, dmf_i, dmf_q
            ComputeFilterBankOutputs();

            // Store result in output buffer
            I_out[N_out] = mf_i;
            Q_out[N_out] = mf_q;

            // Throw exception if buffer is too small
            if ( N_out == N_out_max ) {
                std::cerr << "ERROR: MultirateSynchronizerDSP::SynchronizeAndDecimate() :"
                          << std::endl << "  => output array is full " << std::endl;
                throw 0;
            } else {
                N_out++;
            }

            GenerateTimingError();

            AdvanceTimingLoopFilter();

            ComputeFilterBankIndex();

            // Calculate phase error

            q_c = GeneratePhaseError( mf_i, mf_q );
            q_c /= Ac;
            //std::cout << "q_c = " << q_c << std::endl;

            AdvancePhaseLoopFilter();

            UpdateNCO();

        }

    } // while
}

void MultirateSynchronizerDSP::PushInputs(
    short * I_in,
    short * Q_in,
    unsigned int &i,
    unsigned int N_in )
{
    short I(0), Q(0);

// Load samples into filter banks as necessary
    switch ( lc ) {
    case SHIFT:
        // Shift sample into state registers, normal operation

        ApplyNCO(I_in[i], Q_in[i], I, Q);
        PushInput( I, Q );
        i++;
        break;
    case SKIP:
        // 'Skip' input sample by incrementing index by two

        ApplyNCO(I_in[i], Q_in[i], I, Q);
        PushInput( I, Q );
        i++;
        if ( i < N_in ) {
            ApplyNCO(I_in[i], Q_in[i], I, Q);
            PushInput( I, Q );
            i++;
        } else {
            // not enough samples in input buffer: save state and
            // wait for more
            /// \todo configure
            std::cout << "MultirateSynchronizerDSP::PushInputs(): waiting for more data..."
                      << std::endl;
            return;
        }

        // return loop control to normal 'shift' state
        lc = SHIFT;
        break;
    case STUFF:
        // 'Stuff' (repeat) input sample by not incrementing index
        // This effectively does not update filter states

        // return loop control to normal 'shift' state
        lc = SHIFT;
        break;
    default:
        ;
    }
}


void MultirateSynchronizerDSP::UpdateNCO()
{
    cv = wcTs + q_hat_c/k;

    cv = ( cv >  PI/2 ) ?  PI/2 : cv;
    cv = ( cv < -PI/2 ) ? -PI/2 : cv;

    nco_control += cv;

    if ( nco_control > 2*PI )
        nco_control -= 2*PI;
    else if ( nco_control < -2*PI )
        nco_control += 2*PI;
    else;

// Update NCO values
    nco_i = cosf(nco_control);
    nco_q = sinf(nco_control);
}

// Phase error generator for BPSK modulation
float GeneratePhaseErrorBPSK(
    short &I, short &Q)
{
    float e;
    e = ( I > 0 ) ? (float) Q : (float) -Q;
    return e;
}

// Phase error generator for QPSK modulation
float GeneratePhaseErrorQPSK(
    short &I, short &Q)
{
///\todo test GeneratePhaseErrorQPSK()
// Rotate by 45 degrees and return error QAM4 error

    short I_tmp = short(  float(I*0.707106781f) - float(Q*0.707106781f)  );
    short Q_tmp = short(  float(I*0.707106781f) + float(Q*0.707106781f)  );

    return GeneratePhaseErrorQAM4(I_tmp, Q_tmp);
}

// Phase error generator for QAM4 modulation
float GeneratePhaseErrorQAM4(
    short &I, short &Q)
{
///\todo test GeneratePhaseErrorQAM4()
    float e;
    if ( I>=0 && Q>=0 ) {
        // Quadrant 1
        e = float(Q-I);
    } else if ( I>=0 && Q<=0 ) {
        // Quadrant 2
        e = float(I+Q);
    } else if ( I<=0 && Q<=0 ) {
        // Quadrant 3
        e = float(I-Q);
    } else {
        // Quadrant 4
        e = float(I+Q);
    }

    return e;
}


// Phase error generator for 8PSK modulation
float GeneratePhaseError8PSK(
    short &I, short &Q)
{
    float phi = atan2(Q, I);
    float theta(0.0f);

// De-rotate signal
    while ( phi > PI/8 ) {
        phi -= PI/4;
        theta -= PI/4;
    }

    while ( phi < -PI/8 ) {
        phi += PI/4;
        theta += PI/4;
    }

    return I*sinf(theta) + Q*cosf(theta);
}


// Phase error generator for 16-QAM modulation
float GeneratePhaseError16QAM(
    short &I, short &Q)
{
    short signalMagnitude;

    float I_f = float(I);
    float Q_f = float(Q);
    signalMagnitude = short( sqrtf( I_f*I_f + Q_f*Q_f ) );

    if ( signalMagnitude > QAM16_PHASE_ERROR_THRESHOLD_2 ||
            signalMagnitude < QAM16_PHASE_ERROR_THRESHOLD_1 ) {
        // 4-QAM
        return GeneratePhaseErrorQAM4(I, Q);
    } else {
        // 8PSK
        // rotate pi/8
        short I_tmp, Q_tmp;
        float c = cos(PI/8);
        float s = sin(PI/8);
        I_tmp = (short) ( I*c - Q*s );
        Q_tmp = (short) ( I*s + Q*c );
        return GeneratePhaseError8PSK(I_tmp, Q_tmp);
    }

}

// Phase error generator for 4-PAM
float GeneratePhaseError4PAM(
    short &I, short &Q)
{
    return GeneratePhaseErrorBPSK(I, Q);
}

