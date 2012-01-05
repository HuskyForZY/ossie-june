/****************************************************************************

Copyright 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE SymbolSyncPoly.

OSSIE SymbolSyncPoly is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE SymbolSyncPoly is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE SymbolSyncPoly; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/


#include <iostream>
#include "SymbolSyncPolyDSP.h"
#include <cstring>
#include <math.h>

// Default constructor
SymbolSyncPolyDSP::SymbolSyncPolyDSP()
{
// Initialize pointers
    MF  = NULL;
    dMF = NULL;

// Initially assume the next input is shifted
    lc = SHIFT;

    mf_i =  0;
    dmf_i = 0;
    mf_q =  0;
    dmf_q = 0;

// initialize loop control variables
    Ac = 10e3f;
    alpha_s = 0.05f;
    beta_s = 1.025;
    q_s = 0.0f;
    q_hat_s = 0.0f;

// initialize filter bank variables
    b = 0;
    b_soft = 0.0f;
    Npfb = 0;
    k = 0;
    v = 0;

    isConfigured = false;
}

// Destructor
SymbolSyncPolyDSP::~SymbolSyncPolyDSP()
{
    delete MF;
    delete dMF;
}

// Configure filter bank
void SymbolSyncPolyDSP::ConfigureFilterBank(char * type,
        unsigned int _k,
        unsigned int _m,
        float _beta,
        unsigned int _Npfb)
{
    if ( strcmp(type, "rrcos")==0 ) {
        // square root raised-cosine
        MF =  new SigProc::FIRPolyphaseFilterBank("rrcos", _k, _m, _beta, _Npfb);
        dMF = new SigProc::FIRPolyphaseFilterBank("drrcos", _k, _m, _beta, _Npfb);
    } else {
        std::cerr << "ERROR: SymbolSyncPolyDSP::CreateFilterBank() :" << std::endl
                  << "  => unknown/unsupported filter type " << type << std::endl;
        throw 0;
    }

    k = _k;
    Npfb = (int) _Npfb;

    input_i.SetBufferSize( MF->GetFilterLength() );
    input_q.SetBufferSize( MF->GetFilterLength() );

    isConfigured = true;
}

// Estimate timing information
float SymbolSyncPolyDSP::EstimateTimingOffset(
    short *I_in,
    short *Q_in,
    unsigned int N_in)
{
    if (!isConfigured) {
        std::cerr << "ERROR: SymbolSyncPolyDSP::EstimateTimingOffset() :" << std::endl
                  << "  => must call ConfigureFilterBank() before invoking this method" << std::endl;
        throw 0;
    }

// 1. allocate memory to error signal s-curve array
    unsigned int N(k*Npfb);
    float *s_curve = new float[N];
    for (unsigned int i=0; i<N; i++)
        s_curve[i] = 0.0f;
    short s_mf_i(0), s_dmf_i(0);
    short s_mf_q(0), s_dmf_q(0);
    unsigned int s_index;
    float s_err;

// 2. calculate error signal
    unsigned int h_len = MF->GetFilterLength();
    short * buf_i = new short[h_len];   // non-disruptive in-phase buffer
    short * buf_q = new short[h_len];   // non-disruptive quadrature buffer
    for (unsigned int i=0; i<N_in; i++) {
        // push value into the in-phase buffer
        memmove(buf_i, &buf_i[1], sizeof(short)*(h_len-1));
        buf_i[h_len-1] = I_in[i];

        // push value into the quadrature buffer
        memmove(buf_q, &buf_q[1], sizeof(short)*(h_len-1));
        buf_q[h_len-1] = Q_in[i];

        // wait until buffer is full
        if (i<h_len)
            continue;

        for (unsigned int j=0; int(j)<Npfb; j++) {
            // Compute filter bank ouptuts
            MF->ComputeOutput(s_mf_i, Npfb-j-1, buf_i);
            dMF->ComputeOutput(s_dmf_i, Npfb-j-1, buf_i);

            MF->ComputeOutput(s_mf_q, Npfb-j-1, buf_q);
            dMF->ComputeOutput(s_dmf_q, Npfb-j-1, buf_q);

            // error signal
            s_err = -( float(s_mf_i)*float(s_dmf_i) +
                       float(s_mf_q)*float(s_dmf_q) );

            // s-curve is averaged over several symbols
            s_index = (i%k)*Npfb + j;
            s_curve[s_index] += s_err;
        }
    }
    delete [] buf_i;
    delete [] buf_q;
//for (unsigned int i=0; i<N; i++)
//    printf("s(%d) = %d;\n", i+1, (int) s_curve[i]);

// 3. fit to sinusoid by taking single point DFT
    float rxy_i(0.0f);  // in-phase vector correlation
    float rxy_q(0.0f);  // quadrature vector correlation
    float d_phi;        // partial index phase
    float theta;        // vector angle
    float n_hat;        // sample phase estimate
    for (unsigned int i=0; i<N; i++) {
        d_phi = 2*PI*float(i)/float(N);
        rxy_i += s_curve[i] * sin( d_phi );
        rxy_q -= s_curve[i] * cos( d_phi );
    }
//printf("rxy_i = %f\n", rxy_i);
//printf("rxy_q = %f\n", rxy_q);
    theta = atan2(rxy_q,rxy_i);
    if (theta<0)
        theta += PI;
    else if (theta>0)
        theta -= PI;
    else;
//printf("theta = %f\n", theta);

    n_hat = theta*float(N)/(2*PI);
//printf("n_hat = %f\n", n_hat);
    float dT_hat = n_hat/(k*Npfb);
//printf("dT_hat = %f\n", dT_hat);
    dT_hat -= (dT_hat > 0.5) ? 1 : 0;
//printf("dT_hat = %f\n", dT_hat);

// 4. Extrapolate sample offset and polyphase filter
//    bank index estimate from s-curve phase
    v = (unsigned int) floor(n_hat/Npfb);
    b_soft = n_hat - v*Npfb;
    b = (unsigned int) floor(b_soft);

// clean up memory
    delete [] s_curve;

    return dT_hat;
}

// Compute (quantize) filter bank index from soft loop filter output
void SymbolSyncPolyDSP::ComputeFilterBankIndex()
{
    b = (int) round(b_soft);
    if ( b < 0 ) {
        // underflow (skip to next sample)
        while ( b < 0 ) {
            b += Npfb;
            b_soft += float(Npfb);
        }
        lc = SKIP;
    } else if ( b >= Npfb ) {
        // overflow (stuff previous sample)
        while ( b >= Npfb ) {
            b -= Npfb;
            b_soft -= float(Npfb);
        }
        lc = STUFF;
    } else {
        // Normal operation (shift)
        lc = SHIFT;
    }

}

void SymbolSyncPolyDSP::PushInputs(
    short * I_in,
    short * Q_in,
    unsigned int &i,
    unsigned int N_in )
{
// Load samples into filter banks as necessary
    switch ( lc ) {
    case SHIFT:
        // Shift sample into state registers, normal operation
        PushInput( I_in[i], Q_in[i] );
        i++;
        break;
    case SKIP:
        // 'Skip' input sample by incrementing index by two

        PushInput( I_in[i], Q_in[i] );
        i++;
        if ( i < N_in ) {
            PushInput( I_in[i], Q_in[i] );
            i++;
        } else {
            // not enough samples in input buffer: save state and
            // wait for more
            /// \todo configure
            std::cout << "SymbolSyncPolyDSP::PushInputs(): waiting for more data..."
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

// Main signal processing loop
void SymbolSyncPolyDSP::SynchronizeAndDecimate(short * I_in,
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
                std::cerr << "ERROR: SymbolSyncPolyDSP::SynchronizeAndDecimate() :" << std::endl
                          << "  => output array is full " << std::endl;
                throw 0;
            } else {
                N_out++;
            }

            // Timing error from mf_i*dmf_i and mf_q*dmf_q
            GenerateTimingError();

            // Filter timing error signal
            AdvanceTimingLoopFilter();

            // Compute filter bank index and control flow from timing error
            ComputeFilterBankIndex();
        }

    } // while

}



