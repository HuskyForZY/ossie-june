/****************************************************************************

Copyright 2005, 2006, 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE USRP2 Device.

OSSIE USRP2 Device is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE USRP2 Device is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE USRP2 Device; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


****************************************************************************/

#include <iostream>
#include <omniORB4/CORBA.h>
#include "ossie/debug.h"
#include "USRP2_OSSIE.h"

USRP2_TX_Control_i::USRP2_TX_Control_i(USRP2_i *_usrp2, const char* _name, const char* _domain) : standardInterfaces_i::TX_Control_p(_name, _domain), usrp2(_usrp2)
{
    DEBUG(3, USRP2, "TX Control port constructor called")
}

void USRP2_TX_Control_i::set_number_of_channels(CORBA::ULong nchan)
{
    DEBUG(3, USRP2, "Setting number of TX channels to " << nchan<< " is not supported");

///\TODO: Define if we need this interface. Number of channels is defined at transmit or receive

//omni_mutex_lock l(usrp2->tx_control_access);

//usrp2->usrp2_ptr->set_nchannels(nchan);
}

void USRP2_TX_Control_i::get_number_of_channels(CORBA::ULong &num)
{
///\TODO: Define if we need this interface. Number of channels is defined at transmit or receive
    DEBUG(3, USRP2, "Getting number of TX channels is not supported");
//omni_mutex_lock l(usrp2->tx_control_access);

//num = usrp2->usrp2_ptr->nchannels();
}

void USRP2_TX_Control_i::get_gain_range(CORBA::ULong channel, CORBA::Float &gmin, CORBA::Float &gmax, CORBA::Float &gstep)
{
    if (channel == 0) {
        if (usrp2->tx_db0_control) {
            gmin = usrp2->tx_db0_control->gain_min();
            gmax = usrp2->tx_db0_control->gain_max();
            gstep = usrp2->tx_db0_control->gain_db_per_step();
        }
    } else {
        return; ///\todo throw bad channel exception
    }
}

void USRP2_TX_Control_i::set_gain(CORBA::ULong channel, CORBA::Float gain)
{
    omni_mutex_lock l(usrp2->tx_control_access);

    if (channel == 0) {
        if (usrp2->tx_db0_control) {
            usrp2->tx_db0_control->set_gain(gain);
        }
    } else {
        return; ///\todo throw bad channel exception
    }
}

void USRP2_TX_Control_i::get_gain(CORBA::ULong channel, CORBA::Float &gain)
{
    DEBUG(1, USRP2, "get_gain is not supported for the time being")
//omni_mutex_lock l(usrp2->tx_control_access);

//gain = usrp2->usrp2_ptr->pga(channel);
}

void USRP2_TX_Control_i::get_frequency_range(CORBA::ULong channel, CORBA::Float &fmin, CORBA::Float &fmax, CORBA::Float &fstep)
{
    if (channel == 0) {
        if (usrp2->tx_db0_control) {
            fmin = usrp2->tx_db0_control->freq_min();
            fmax = usrp2->tx_db0_control->freq_max();
            fstep = 0;
        }
    } else {
        return; ///\todo throw bad channel exception
    }
}

void USRP2_TX_Control_i::set_frequency(CORBA::ULong channel, CORBA::Float f)
{
    DEBUG(3, USRP2, "In TX Control set frequency channel: " << channel << ", frequency: " << f);

    db_base_sptr db;
    usrp2::tune_result result;

    omni_mutex_lock l(usrp2->tx_control_access);

    if (channel == 0) {
        db = usrp2->tx_db0_control;
    } else {
        DEBUG(3, USRP2, "USRP2 TX set_frequency (invalid channel)");
        return;
    }

    if (!usrp2->usrp2_ptr->set_tx_center_freq( f, &result)) {
        DEBUG(3, USRP2, "USRP2 TX set_frequency tune failed");
        return; // throw exception
    }

    DEBUG(3, USRP2, "USRP2 TX tune_result:" <<
          " baseband_freq="      << result.baseband_freq <<
          " dxc_freq="           << result.dxc_freq      <<
          " residual_freq="      << result.residual_freq <<
          " inverted="           << result.spectrum_inverted);
}

void USRP2_TX_Control_i::get_frequency(CORBA::ULong channel, CORBA::Float &f)
{
    DEBUG(1, USRP2, "USRP2 TX get_frequency not supported for the time being");
//omni_mutex_lock l(usrp2->tx_control_access);

//f = usrp2->usrp2_ptr->tx_freq(channel);
}

void USRP2_TX_Control_i::start(CORBA::ULong channel)
{
    omni_mutex_lock l(usrp2->tx_control_access);

    if (channel == 0) {
        if (usrp2->tx_db0_control)
            usrp2->tx_db0_control->set_enable(true);
    } else {
        return; /// \todo throw bad channel exception
    }

    if (!usrp2->tx_active) {
        //usrp2->usrp2_ptr->start();///\FIXME: this is a huge fixme. Fix it!!
        usrp2->tx_thread = new omni_thread(USRP2_i::do_tx_data_process, ((void *)usrp2));
        usrp2->tx_thread->start();
    }
}

void USRP2_TX_Control_i::stop(CORBA::ULong channel)
{
    omni_mutex_lock l(usrp2->tx_control_access);

    if (channel == 0) {
        if (usrp2->tx_db0_control)
            usrp2->tx_db0_control->set_enable(false);
    }

    usrp2->tx_active = false;

// Previously never called
//usrp2->usrp2_ptr->stop();
}

void USRP2_TX_Control_i::set_values(const CF::Properties &values)
{
    DEBUG(3, USRP2, "USRP2 TX setting " << values.length() << " values, value[0].id " << values[0].id)

    for (unsigned int i =0; i < values.length(); ++i) {
        if (strcmp(values[i].id, "SET_MUX") == 0 ) {
            CORBA::ULong mux;
            values[i].value >>= mux;
            DEBUG(1, USRP2, "Request to set transmit mux to  " << mux<<" Not Supported")
            //omni_mutex_lock l(usrp2->tx_control_access);
            //usrp2->usrp2_ptr->set_mux(mux);
        } else if (strcmp(values[i].id, "SET_AUTO_TR_1") == 0) {
            CORBA::ULong atx;
            values[i].value >>= atx;
            DEBUG(3, USRP2, "Set Auto TX/RX for side 1 to " << atx)
            omni_mutex_lock l(usrp2->tx_control_access);
            if (usrp2->tx_db0_control && usrp2->rx_db0_control) {
                if (atx) {
                    usrp2->tx_db0_control->set_auto_tr(true);
                    usrp2->rx_db0_control->set_auto_tr(true);
                } else {
                    usrp2->tx_db0_control->set_auto_tr(false);
                    usrp2->rx_db0_control->set_auto_tr(false);
                }
            }
        }
    }
}

void USRP2_TX_Control_i::set_interpolation_rate(CORBA::ULong channel, CORBA::ULong I)
{
// USRP2 interpolation rate applies to all channels, ignore channel
    omni_mutex_lock l(usrp2->tx_control_access);

    usrp2->usrp2_ptr->set_tx_interp(I);
}

void USRP2_TX_Control_i::get_interpolation_range(CORBA::ULong channel, CORBA::ULong &imin, CORBA::ULong &imax, CORBA::ULong &istep)
{
    imin = 4;
    imax = 512;
    istep = 4;
}

USRP2_RX_Control_i::USRP2_RX_Control_i(USRP2_i *_usrp2, const char* _name, const char* _domain) : standardInterfaces_i::RX_Control_p(_name, _domain), usrp2(_usrp2)
{
    DEBUG(3, USRP2, "RX Control port constructor called")
}

void USRP2_RX_Control_i::set_number_of_channels(CORBA::ULong nchan)
{
    DEBUG(1, USRP2, "Request to set number of channels to " << nchan<<" not supported")

//omni_mutex_lock l(usrp2->rx_control_access);

//usrp2->usrp2_ptr->set_nchannels(nchan);
}

void USRP2_RX_Control_i::get_number_of_channels(CORBA::ULong &num)
{
    DEBUG(1, USRP2, "Request to get number of channels not supported")
//omni_mutex_lock l(usrp2->rx_control_access);

//num = usrp2->usrp2_ptr->nchannels();
}

void USRP2_RX_Control_i::get_gain_range(CORBA::ULong channel, CORBA::Float &gmin, CORBA::Float &gmax, CORBA::Float &gstep)
{
    if (channel == 0) {
        if (usrp2->rx_db0_control) {
            gmin = usrp2->rx_db0_control->gain_min();
            gmax = usrp2->rx_db0_control->gain_max();;
            gstep = usrp2->rx_db0_control->gain_db_per_step();
        }
    } else {
        return; ///\todo throw bad channel exception
    }
}

void USRP2_RX_Control_i::set_gain(CORBA::ULong channel, CORBA::Float gain)
{
    DEBUG(1, USRP2, "set gain to "<<gain);

    omni_mutex_lock l(usrp2->rx_control_access);

    if (channel == 0) {
        if (usrp2->rx_db0_control)
            usrp2->rx_db0_control->set_gain(gain);
    } else {
        DEBUG(1, USRP2, "Attempt to set gain on non-existent db.");
        return; ///\todo throw bad channel exception
    }
}

void USRP2_RX_Control_i::get_gain(CORBA::ULong channel, CORBA::Float &gain)
{
    DEBUG(1, USRP2, "Get_gain function not supported");
    /*omni_mutex_lock l(usrp2->rx_control_access);

    int temp_gain, temp_aux;
    temp_gain = (int) usrp2->usrp2_ptr->pga(channel);
    temp_aux = usrp2->usrp2_ptr->read_aux_adc(channel, 0);

    // for now adding temp_gain and temp_aux.....PHELPS with fix it
    gain = temp_gain + temp_aux;*/
}

void USRP2_RX_Control_i::get_frequency_range(CORBA::ULong channel, CORBA::Float &fmin, CORBA::Float &fmax, CORBA::Float &fstep)
{
    if (channel == 0) {
        if (usrp2->rx_db0_control) {
            fmin = usrp2->rx_db0_control->freq_min();
            fmax = usrp2->rx_db0_control->freq_max();
            fstep = 0;
        }
    } else {
        return; ///\todo throw bad channel exception
    }
}

void USRP2_RX_Control_i::set_frequency(CORBA::ULong channel, CORBA::Float f)
{
    DEBUG(3, USRP2, "In RX Control set frequency channel: " << channel << ", frequency: " << f);

    db_base_sptr db;
    usrp2::tune_result result;

    omni_mutex_lock l(usrp2->rx_control_access);

    if (channel == 0) {
        if (usrp2->rx_db0_control)
            db = usrp2->rx_db0_control;
    } else {
        std::cerr << "Bad channel specified" << std::endl;
        return; ///\todo throw bad channel exception
    }

    if (!usrp2->usrp2_ptr->set_rx_center_freq( f, &result)) {
        DEBUG(3, USRP2, "USRP2 RX set_frequency failed");
        return; // throw exception
    }

    DEBUG(3, USRP2, "USRP2 RX tune_result:" <<
          " baseband_freq="      << result.baseband_freq <<
          " dxc_freq="           << result.dxc_freq      <<
          " residual_freq="      << result.residual_freq <<
          " inverted="           << result.spectrum_inverted);
}

void USRP2_RX_Control_i::get_frequency(CORBA::ULong channel, CORBA::Float &f)
{
    DEBUG(1, USRP2, "USRP2 RX get_frequency not supported for the time being");
//omni_mutex_lock l(usrp2->rx_control_access);

//f = usrp2->usrp2_ptr->rx_freq(channel);
}

void USRP2_RX_Control_i::start(CORBA::ULong channel)
{
    omni_mutex_lock l(usrp2->rx_control_access);

    usrp2->rx_packet_count = usrp2->set_rx_packet_count;
    usrp2->rx_active = true;

// Set up RX thread
    usrp2->rx_thread = new omni_thread(USRP2_i::do_rx_data_process, ((void *)usrp2));
    usrp2->rx_thread->start();
}

void USRP2_RX_Control_i::stop(CORBA::ULong channel)
{
    omni_mutex_lock l(usrp2->rx_control_access);
    DEBUG(0, USRP2, "USRP_RX_Control stop Called")

    usrp2->rx_active = 0;//false;
    usrp2->stop();
}

void USRP2_RX_Control_i::set_values(const CF::Properties &values)
{
    DEBUG(3, USRP2, "USRP2 RX setting " << values.length() << " values, value[0].id " << values[0].id)

    for (unsigned int i =0; i < values.length(); ++i) {
        if (strcmp(values[i].id, "SET_NUM_RX_PACKETS") == 0) {
            CORBA::ULong num_packets;
            values[i].value >>= num_packets;
            DEBUG(3, USRP2, "Number of packets to RX = " << num_packets)
            usrp2->set_rx_packet_count = num_packets;
        } else if (strcmp(values[i].id, "SET_MUX") == 0 ) {
            CORBA::ULong mux;
            values[i].value >>= mux;
            DEBUG(1, USRP2, "Request to setting receive mux to  " << mux<<" not supported")
            //omni_mutex_lock l(usrp2->rx_control_access);
            //usrp2->usrp2_ptr->set_mux(mux);
        } else if (strcmp(values[i].id, "SET_RX_ANT_1") == 0 ) {
            CORBA::ULong ant;
            values[i].value >>= ant;
            DEBUG(3, USRP2, "Setting receive antenna to  " << ant)
            omni_mutex_lock l(usrp2->rx_control_access);
            if (usrp2->rx_db0_control)
                usrp2->rx_db0_control->select_rx_antenna(ant);
        }
    }
}

void USRP2_RX_Control_i::set_decimation_rate(CORBA::ULong channel, CORBA::ULong D)
{
    omni_mutex_lock l(usrp2->rx_control_access);

    usrp2->usrp2_ptr->set_rx_decim(D);
}

void USRP2_RX_Control_i::get_decimation_range(CORBA::ULong channel, CORBA::ULong &dmin, CORBA::ULong &dmax, CORBA::ULong &dstep)
{
    dmin = 4;
    dmax = 256;
    dstep = 2;
}

void USRP2_RX_Control_i::set_data_packet_size(CORBA::ULong channel, CORBA::ULong N)
{
    omni_mutex_lock l(usrp2->rx_control_access);

    usrp2->rx_packet_size = N;
}
