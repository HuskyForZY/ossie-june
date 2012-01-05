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


#ifndef SYMBOLSYNCPOLYDSP_IMPL_H
#define SYMBOLSYNCPOLYDSP_IMPL_H

#include <stdlib.h>
#include "SigProc.h"

#ifndef PI
#define PI       3.14159265358979f
#endif

#define ALPHA_T_LO 0.0002
#define ALPHA_T_HI 0.002
#define BETA_T_LO  1.000125
#define BETA_T_HI  1.00125

/// Signal flow control for polyphase filter bank synchronizer
enum flow_control {
    SHIFT = 0,  ///< normal operation
    SKIP  = 1,  ///< filter bank underflow
    STUFF = 2   ///< filter bank overflow
};

/** \brief Digital signal processing for the symbol synchronizer component
 *  implemented with the polyphase filter bank
 *
 *
 */
class SymbolSyncPolyDSP
{
public:
/// Initializing constructor
    SymbolSyncPolyDSP();

/// Destructor
    ~SymbolSyncPolyDSP();

/// Configure filter bank
///
/// \param[in] type  Type of filter prototype
/// \param[in] _k    Samples per symbol
/// \param[in] _m    Symbol delay
/// \param[in] _beta Excess bandwidth factor
/// \param[in] _Npfb Number of filters in the polyphase filter bank
    void ConfigureFilterBank(char * type,
                             unsigned int _k,
                             unsigned int _m,
                             float _beta,
                             unsigned int _Npfb);

///
///\todo change to : void UpdateTimingLoopFilterCoefficients(float _BT);
    void UpdateTimingLoopFilterCoefficients(float _alpha_s, float _beta_s) {
        alpha_s = _alpha_s;
        beta_s = _beta_s;
    }

/// Estimate timing information
/// \todo implement this method!!!
    float EstimateTimingOffset(short * I_in,
                               short * Q_in,
                               unsigned int N_in);

/// \brief Main signal processing loop
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

protected:

/// \brief Calculate error from MF and dMF
///
/// The error signal is the maximum likelihood estimate of the phase offset, viz.
/// \f[ \hat{e}(nT) = \dot{h}(nT) h(nT) \f]
    void GenerateTimingError() {
        q_s = -( float(mf_i)*float(dmf_i) + float(mf_q)*float(dmf_q) )/2.0f;
        q_s /= Ac*Ac;
    }

/// Advance IIR loop filter
/// \todo use better loop filter
    void AdvanceTimingLoopFilter() {
        q_hat_s = beta_s*q_hat_s - alpha_s*(q_hat_s-q_s);
        b_soft = b_soft - q_hat_s*float(Npfb);
    }

/// Compute (quantize) filter bank index from soft loop filter output
    void ComputeFilterBankIndex();

/// Pushes inputs determined by flow control as necessary
    void PushInputs( short * I_in, short * Q_in, unsigned int &i, unsigned int N_in );

/// Pushes inputs determined by flow control as necessary
    void PushInput( short I_in, short Q_in ) {
        input_i.Push( I_in );
        input_q.Push( Q_in );
    }

///
    void ComputeFilterBankOutputs() {
        // Retrieve pointer to external buffers
        short * i_ptr = input_i.GetHeadPtr();
        short * q_ptr = input_q.GetHeadPtr();

        // Filter in-phase signal with MF and dMF with filter bank b
        MF->ComputeOutput(mf_i, b, i_ptr);
        dMF->ComputeOutput(dmf_i, b, i_ptr);

        // Filter quadrature signal with MF and dMF with filter bank b
        MF->ComputeOutput(mf_q, b, q_ptr);
        dMF->ComputeOutput(dmf_q, b, q_ptr);
    }

    SigProc::FIRPolyphaseFilterBank * MF;   ///< matched filter bank
    SigProc::FIRPolyphaseFilterBank * dMF;  ///< derivative matched filter bank

    SigProc::CircularBuffer <short> input_i;///< in-phase signal buffer
    SigProc::CircularBuffer <short> input_q;///< quadrature signal buffer

    short mf_i;         ///< In-phase matched filter output
    short dmf_i;        ///< In-phase derivative matched filter output
    short mf_q;         ///< Quadrature matched filter output
    short dmf_q;        ///< Quadrature derivative matched filter output

    float Ac;           ///< Approximate signal amplitude
    float alpha_s;      ///< loop filter coefficient, \f$\alpha_s\f$
    float beta_s;       ///< loop filter coefficient, \f$\beta_s\f$
    float q_s;          ///< Error signal
    float q_hat_s;      ///< Filtered error signal

    float b_soft;       ///< soft filter bank index
    int b;              ///< hard filter bank index
    int Npfb;           ///< number of filters in bank

    unsigned int k;     ///< samples per symbol
    signed int v;       ///< sample counter for output sample enable

    flow_control lc;    ///< loop flow control state

    bool isConfigured;  ///< flag set when ConfigureFilterBank() is invoked properly

private:
/// Disallow copy constructor
    SymbolSyncPolyDSP(SymbolSyncPolyDSP&);

};

#endif

