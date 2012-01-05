/****************************************************************************

Copyright 2005,2006 Virginia Polytechnic Institute and State University

This file is part of the OSSIE USRP Device.

OSSIE USRP Device is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE USRP Device is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE USRP Device; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


****************************************************************************/

#include <byteswap.h>
#include <iostream>
//#include <fstream>
#include "ossie/cf.h"
#include "ossie/PortTypes.h"

#include "ossie/ossieSupport.h"
#include "ossie/debug.h"

#include "USRP.h"

USRP_i::USRP_i(char *id, char *label, char *profile) :
        Device_impl(id, label, profile),
        set_rx_packet_count(-1),
        rx_packet_count(1024),
        rx_packet_size(1024),
        rx_data_size(2),
        number_of_channels(1),
        complex(true),
        rx_overruns(0),
        rx_active(false),
        tx_active(false)
{
// Create USRP Control ports for TX and RX
    rx_control_port = new USRP_RX_Control_i(this, "RX_Control", "DomainName1");
    tx_control_port = new USRP_TX_Control_i(this, "TX_Control", "DomainName1");

// Create the ports for TX data
    tx_data_port = new standardInterfaces_i::complexShort_p("TX_Data", "DomainName1");

// Create the ports for RX Data
    rx_data_1_port = new standardInterfaces_i::complexShort_u("RX_Data_1", "DomainName1");
    rx_data_2_port = new standardInterfaces_i::complexShort_u("RX_Data_2", "DomainName1");
}

void USRP_i::start() throw (CF::Resource::StartError, CORBA::SystemException)
{
    DEBUG(3, USRP, "Start USRP called")
}

void USRP_i::stop() throw (CF::Resource::StopError, CORBA::SystemException)
{
    DEBUG(3, USRP, "Stop USRP called")
}

CORBA::Object_ptr USRP_i::getPort(const char* portName)
throw(CF::PortSupplier::UnknownPort, CORBA::SystemException)
{
    DEBUG(3, USRP, "USRP getPort called with : " << portName)

    CORBA::Object_var p;

    if (!CORBA::is_nil(p = rx_control_port->getPort(portName)))
        return p._retn();
    else if (!CORBA::is_nil(p = rx_data_1_port->getPort(portName)))
        return p._retn();
    else if (!CORBA::is_nil(p = rx_data_1_port->getPort(portName))) {
        return p._retn();
    } else if (!CORBA::is_nil(p = tx_control_port->getPort(portName))) {
        return p._retn();
    } else if (!CORBA::is_nil(p = tx_data_port->getPort(portName))) {
        return p._retn();
    }
    std::cerr << "Couldn't find port " << portName << "Throwing exception" << std::endl;
    throw CF::PortSupplier::UnknownPort();
}

void USRP_i::initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException)
{
    DEBUG(3, USRP, "USRP Initialize called")

    usrp_rx = usrp_standard_rx::make(0, 256);

    if (!usrp_rx) {
        std::cerr << "Failed to create usrp rx" << std::endl;
        throw CF::LifeCycle::InitializeError();
    }

    usrp_tx = usrp_standard_tx::make(0,512);

    if (!usrp_tx) {
        std::cerr << "Failed to create usrp tx" << std::endl;
        throw CF::LifeCycle::InitializeError();
    }

// Check for daughterboards and assign first subdevice
    rx_db0 = usrp_rx->daughterboard_id(0);
    rx_db1 = usrp_rx->daughterboard_id(1);
    //rx_db0_control = usrp_rx->db(0)[0];
    rx_db0_control = usrp_rx->selected_subdev(0);
  unsigned int mux = usrp_rx->determine_rx_mux_value(0);
  usrp_rx->set_mux(mux);

  float gain_min = rx_db0_control->gain_min();
  float gain_max = rx_db0_control->gain_max();
  std::cout<<"the gain range in subdev is: "<<gain_min<<" - "<< gain_max<< std::endl;
    rx_db1_control = usrp_rx->selected_subdev(1);

    tx_db0 = usrp_tx->daughterboard_id(0);
    tx_db1 = usrp_tx->daughterboard_id(1);
    tx_db0_control = usrp_tx->db(0)[0];
    tx_db1_control = usrp_tx->db(1)[0];

    DEBUG(1, USRP, "RX daughterboard slot 0 : " << usrp_dbid_to_string(rx_db0))
    DEBUG(1, USRP, "RX daughterboard slot 1 : " << usrp_dbid_to_string(rx_db1))
    DEBUG(1, USRP, "TX daughterboard slot 0 : " << usrp_dbid_to_string(tx_db0))
    DEBUG(1, USRP, "TX daughterboard slot 1 : " << usrp_dbid_to_string(tx_db1))

}

void USRP_i::configure(const CF::Properties &configProperties)
throw (CORBA::SystemException, CF::PropertySet::InvalidConfiguration,
       CF::PropertySet::PartialConfiguration)
{

}

void USRP_i::query(CF::Properties &configProperties)
throw (CORBA::SystemException, CF::UnknownProperties)
{

}


void USRP_i::releaseObject()
throw (CF::LifeCycle::ReleaseError, CORBA::SystemException)
{

    DEBUG(3, USRP, "USRP releaseObject called")
}

void USRP_i::rx_data_process()
{
    short *rx_buffer;
//    std::ofstream file;
//    file.open("usrp_sample_from_ossie.dat", std::ios::trunc | std::ios::binary);

///\todo this assumes USRP is sending shorts, fix to match USRP config
///\todo assumes one channel operation
    unsigned int buf_size = rx_packet_size * number_of_channels * (complex ? 2 : 1);

    rx_buffer = new short[buf_size*2];

    PortTypes::ShortSequence I;
    PortTypes::ShortSequence Q;

    I.length(rx_packet_size);
    Q.length(rx_packet_size);

    DEBUG(3, USRP, "RX packet count " << rx_packet_count)

    usrp_rx->start();

    while (rx_active && ((rx_packet_count == -1) || (rx_packet_count > 0))) {
        bool overrun = 0;
        usrp_rx->read(rx_buffer, buf_size * 2, &overrun);
        if (overrun) {
            std::cerr << "RX Overrun occurred" << std::endl;
            ++rx_overruns;
        }

        for (unsigned int i = 0; i < rx_packet_size * 2; i += 2) {
            if (i < 0)
                DEBUG(5, USRP, "I - " << rx_buffer[i] << "  Q -  " << rx_buffer[i+1])
            I[i/2] = (CORBA::Short) rx_buffer[i];
            Q[i/2] = (CORBA::Short) rx_buffer[i+1];
//            file.write((char*)rx_buffer,buf_size*2);
        }
        rx_data_1_port->pushPacket(I, Q);

        if (rx_packet_count != -1)
            --rx_packet_count;
    }
//    file.close();
    DEBUG(3, USRP, "Exiting rx_data_process thread")

    usrp_rx->stop();
    rx_active = false;
    rx_thread->exit();
}


// Helper function for tx_data_process
inline static void tx_with_usrp(
    usrp_standard_tx_sptr usrp_tx,
    short *tx_buf,
    unsigned int tx_buf_len,
    unsigned int &tx_underruns)
{
    bool underrun_occured;

    int rc = usrp_tx->write(tx_buf, tx_buf_len * 2, &underrun_occured);
    if (underrun_occured) {
        ++tx_underruns;
        std::cerr << "USRP TX Underrun occured, total underruns - " << tx_underruns << std::endl;
    }

    if (rc < 0) {
        std::cerr << "USRP TX error detected" << std::endl;
    } else if (rc != (int) tx_buf_len * 2) {
        std::cerr << "USRP TX attempted to write " << tx_buf_len << " bytes, but only "
                  << rc << "bytes actually written." << std::endl;
    }
}

// USRP Expects data in little endian format
static inline short convertToLE(short s)
{
#ifdef __powerpc__
    s = bswap_16(s);
#endif

    return s;
}

void USRP_i::tx_data_process()

{
    PortTypes::ShortSequence *I_in, *Q_in;

    const unsigned int tx_buf_len(512*1); // Must be a multiple of 512

    unsigned int tx_buf_idx(0), tx_underruns(0);
    short tx_buf[tx_buf_len];

    DEBUG(3, USRP, "Starting tx_data_process thread.")

    tx_active = true; // Cleared to stop TX in USRP_TX_Control->stop()

    while (tx_active) {

        tx_data_port->getData(I_in, Q_in);
        unsigned int data_length(I_in->length());

        for (unsigned int i = 0; i < data_length; ++i) {
            tx_buf[tx_buf_idx] = convertToLE((*I_in)[i]);
            ++tx_buf_idx;
            tx_buf[tx_buf_idx] = convertToLE((*Q_in)[i]);
            ++tx_buf_idx;

            if (tx_buf_idx == tx_buf_len) {
                tx_with_usrp(usrp_tx, tx_buf, tx_buf_len, tx_underruns);
                tx_buf_idx = 0;
            }
        }
        tx_data_port->bufferEmptied();
    }

    DEBUG(3, USRP, "Exiting tx_data_process thread.")
    usrp_tx->stop();
    tx_thread->exit();

}