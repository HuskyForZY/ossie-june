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


#ifndef MULTIRATESYNCHRONIZERDSP_IMPL_H
#define MULTIRATESYNCHRONIZERDSP_IMPL_H

#include <stdlib.h>
#include "SymbolSyncPolyDSP.h"

#define ALPHA_C_LO 0.0066667
#define ALPHA_C_HI 0.066667
#define BETA_C_LO  7.8567e-06
#define BETA_C_HI  0.0023570

#define QAM16_PHASE_ERROR_THRESHOLD_1 7236
#define QAM16_PHASE_ERROR_THRESHOLD_2 11708


/** \brief Digital signal processing for the synchronizer component
 *  implemented with the polyphase filter bank
 *
 *
 */
class MultirateSynchronizerDSP : public SymbolSyncPolyDSP
{
public:
/// Initializing constructor
    MultirateSynchronizerDSP();

/// Destructor
    ~MultirateSynchronizerDSP();

/// Set modulation scheme for phase error generator
    void SetModulationScheme(SigProc::ModulationScheme _ms);

/// \brief Update phase loop filter coefficients on bandwdith-time product
///
/// \f[ \beta_c =  \frac{2 BT_c}{\xi + 1/(4\xi)} \f]
/// \f[ \alpha_c = 2 \xi \beta_c \f]
///
/// For some unexplained reason \f$ \beta_c \f$ needs to be scaled by a
/// factor \f$ g\left( BT_c \right) \f$
///
/// <table>
/// <tr><td> \f$BT_c\f$ </td><td>  \f$g\f$ </td></tr>
/// <tr><td> 0.00025    </td><td>  1500    </td></tr>
/// <tr><td> 0.00100    </td><td>  500     </td></tr>
/// <tr><td> 0.00250    </td><td>  200     </td></tr>
/// <tr><td> 0.00500    </td><td>  125     </td></tr>
/// <tr><td> 0.00750    </td><td>  75      </td></tr>
/// <tr><td> 0.01000    </td><td>  50      </td></tr>
/// <tr><td> 0.02500    </td><td>  18      </td></tr>
/// <tr><td> 0.05000    </td><td>  9       </td></tr>
/// </table>
///
/// \f[ \log_2(BT_c) = m \log_2(g) + b \f]
/// \f[ m = -0.976096479585969 \f]
/// \f[ b = -0.844443644088639 \f]
    void UpdatePhaseLoopFilterCoefficients(float _BT_c) {
        BT_c = _BT_c;
        float xi(0.707106781186547f); // damping factor, xi = 1/sqrt(2)
        beta_c = 2*BT_c/(xi+1/(4*xi));
        alpha_c = 2*xi*beta_c;

        // Emperical relationship between critical damping factor and BT_c
        float g = exp2f(-0.976096479585969f*log2f(BT_c)-0.844443644088639f);
        beta_c /= g;
    }

    float EstimateCarrierOffset(
        short * I_in,
        short * Q_in,
        unsigned int input_length,
        bool use_matched_filter);

/// Overriding method...
    void SynchronizeAndDecimate(short * I_in,
                                short * Q_in,
                                unsigned int N_in,
                                short * I_out,
                                short * Q_out,
                                unsigned int & N_out);

protected:
///
    SigProc::ModulationScheme mod_scheme;

///
    float (*GeneratePhaseError) (short &I, short &Q);

/// Pushes inputs determined by flow control as necessary (overloaded function)
    void PushInputs( short * I_in, short * Q_in, unsigned int &i, unsigned int N_in );

///
    void AdvancePhaseLoopFilter() {
        q_prime_c = q_c*beta_c + buff2_c;
        q_hat_c = alpha_c*q_c + q_prime_c;
        buff2_c = q_prime_c;
    }

/// Apply numerically controlled oscillator
    void ApplyNCO( short I_in, short Q_in, short &I_out, short &Q_out ) {
        I_out = short( float(I_in) * nco_i + float(Q_in) * nco_q );
        Q_out = short( float(Q_in) * nco_i - float(I_in) * nco_q );
    }

/// Update numerically controlled oscillator
    void UpdateNCO();

// Carrier phase loop filter
    float BT_c;         ///< Bandwidth-time factor of loop filter
    float alpha_c;      ///< Phase loop filter coefficient, \f$\alpha_c\f$
    float beta_c;       ///< Phase loop filter coefficient, \f$\beta_c\f$
    float q_c;          ///< Phase error signal
    float q_hat_c;      ///< Filtered phase error signal
    float q_prime_c;    ///< Phase loop filter buffer 1
    float buff2_c;      ///< Phase loop filter buffer 2

// Numerically-controlled oscillator
    float nco_control;  ///< NCO control phase, \f$\int{cv}\f$
    float cv;           ///< NCO control voltage
    float wcTs;         ///< Initial frequency offset estimate
    float nco_i;        ///< NCO in-phase output
    float nco_q;        ///< NCO quadrature output

private:
/// Disallow copy constructor
    MultirateSynchronizerDSP(MultirateSynchronizerDSP&);

};

/// Phase error generator for BPSK modulation
float GeneratePhaseErrorBPSK(short &I, short &Q);

/// Phase error generator for QPSK modulation
float GeneratePhaseErrorQPSK(short &I, short &Q);

/// Phase error generator for QAM4 modulation
float GeneratePhaseErrorQAM4(short &I, short &Q);

/// Phase error generator for 8PSK modulation
float GeneratePhaseError8PSK(short &I, short &Q);

/// Phase error generator for 16QAM modulation
float GeneratePhaseError16QAM(short &I, short &Q);

/// Phase error generator for 4-PAM modulation
float GeneratePhaseError4PAM(short &I, short &Q);

#endif

