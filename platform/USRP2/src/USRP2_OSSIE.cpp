/****************************************************************************

Copyright 2005,2006 Virginia Polytechnic Institute and State University

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

#include <byteswap.h>
#include <iostream>

#include "ossie/cf.h"
#include "ossie/PortTypes.h"

#include "ossie/ossieSupport.h"
#include "ossie/debug.h"

#include "USRP2_OSSIE.h"

USRP2_i::USRP2_i(char *id, char *label, char *profile) :
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
// Create USRP2 Control ports for TX and RX
    rx_control_port = new USRP2_RX_Control_i(this, "U2_RX_Control", "DomainName1");
    tx_control_port = new USRP2_TX_Control_i(this, "U2_TX_Control", "DomainName1");

// Create the ports for TX data
    tx_data_port = new standardInterfaces_i::complexShort_p("U2_TX_Data", "DomainName1");

// Create the ports for RX Data
    rx_data_1_port = new standardInterfaces_i::complexShort_u("U2_RX_Data_1", "DomainName1");
    rx_data_2_port = new standardInterfaces_i::complexShort_u("U2_RX_Data_2", "DomainName1");
}

void USRP2_i::start() throw (CF::Resource::StartError, CORBA::SystemException)
{
    DEBUG(3, USRP2, "Start USRP2 called")
}

void USRP2_i::stop() throw (CF::Resource::StopError, CORBA::SystemException)
{
    DEBUG(3, USRP2, "Stop USRP2 called")
    (rx_active)?std::cout<<"Active": std::cout<<"NOT Active";
}

CORBA::Object_ptr USRP2_i::getPort(const char* portName)
throw(CF::PortSupplier::UnknownPort, CORBA::SystemException)
{
    DEBUG(3, USRP2, "USRP2 getPort called with : " << portName)

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

void USRP2_i::initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException)
{
    int db_res=0;
    DEBUG(3, USRP2, "USRP2 Initialize called")

    usrp2_ptr = usrp2::usrp2::make("eth0"); ///\NOTE: static function returns an
//instance of usrp2 as a shared pointer
//make(ifc, addr="", rx_bufsize=0)
//rx_bufsize is the length in bytes of the
//kernel networking buffer to allocate
    if (!usrp2_ptr) {
        std::cerr << "Failed to create usrp2" << std::endl;
        throw CF::LifeCycle::InitializeError();
    }


// Check for daughterboards and assign first subdevice
    db_res=usrp2_ptr->rx_daughterboard_id(&rx_db0);
    if ( db_res >= 0) {
        DEBUG(1, USRP2, "RX daughterboard slot 0 : " << usrp_dbid_to_string(rx_db0));
    } else if (db_res  == -1) {
        DEBUG(1, USRP2, "RX daughterboard not installed");
    } else {
        DEBUG(1, USRP2, "Invalid EEPROM on RX daughterboard");
    }

//rx_db0_control = usrp2_ptr->db(0)[0];

    db_res=usrp2_ptr->tx_daughterboard_id(&tx_db0);
    if ( db_res >= 0) {
        DEBUG(1, USRP2, "TX daughterboard slot 0 : " << usrp_dbid_to_string(tx_db0));
    } else if (db_res  == -1) {
        DEBUG(1, USRP2, "TX daughterboard not installed");
    } else {
        DEBUG(1, USRP2, "Invalid EEPROM on TX daughterboard");
    }

//tx_db0_control = usrp2_ptr->db(0)[0];
}

void USRP2_i::configure(const CF::Properties &configProperties)
throw (CORBA::SystemException, CF::PropertySet::InvalidConfiguration,
       CF::PropertySet::PartialConfiguration)
{

}

void USRP2_i::query(CF::Properties &configProperties)
throw (CORBA::SystemException, CF::UnknownProperties)
{

}


void USRP2_i::releaseObject()
throw (CF::LifeCycle::ReleaseError, CORBA::SystemException)
{

    DEBUG(3, USRP2, "USRP2 releaseObject called")
}

void USRP2_i::rx_data_process()
{
    /* std::fstream fp;
     std::cout<<"USRP2::About to open output file!"<<std::endl;
     fp.open("USRP2_data_samples.dat",std::ios::out|std::ios::trunc);
     if (fp.is_open()==false){
         std::cout<< "ERROR opening file!"<<std::endl;
     }*/
///\TODO this assumes USRP2 is sending shorts, fix to match USRP2 config
///\TODO assumes one channel operation
    short* h_buf = new short[USRP2_MIN_RX_SAMPLES*number_of_channels * (complex ? 2 : 1)];
    usrp2::rx_nop_handler::sptr handler = usrp2::rx_nop_handler::sptr(new ossie_cs_handler((std::complex<int16_t>*)h_buf, 0, USRP2_MIN_RX_SAMPLES));//Explicitly define the block size that will be passed from the USRP2 in every call of rx_samples

    PortTypes::ShortSequence I;
    PortTypes::ShortSequence Q;


///\TODO Make sure rx_packet_size is >= than USRP2_MIN_RX_SAMPLES!!!!!
    I.length(rx_packet_size);
    Q.length(rx_packet_size);

    DEBUG(3, USRP2, "RX packet count " << rx_packet_count)

//  FIXME in case it was left running...
    if (!usrp2_ptr->stop_rx_streaming()) {
        DEBUG(1, USRP2, "USRP2 failed to stop streaming ")
    }

    if (!usrp2_ptr->start_rx_streaming()) {
        ;///\TODO: Review streaming receive mode (parameters)
        //start_rx_streaming(uint stream_channel_number, uint items_per_frame)
        DEBUG(3, USRP2, "There was an error with start_rx_streaming");
    }
    unsigned int seq_idx = 0;
    unsigned int h_buf_idx = 0;
    unsigned int previous_samples = 0;

    while (rx_active && ((rx_packet_count == -1) || (rx_packet_count > 0))) {
        //bool overrun = 0;
        //fp<<"Inside the loop"<<std::endl;
        //copy any samples remaining from previous frame
        seq_idx = 0;
        if (previous_samples) {
            //fp<<"USRP2::Inside remainder loop. Copying: "<<previous_samples<<std::endl;
            for (unsigned int i = 0; i<previous_samples; i++, seq_idx++, h_buf_idx+=2) {
                I[seq_idx] = (CORBA::Short) h_buf[h_buf_idx];
                Q[seq_idx] = (CORBA::Short) h_buf[h_buf_idx+1];
            }
            //seq_idx points to the next element in the sequence to receive samples
        }
        //fp<<"USRP2:: the current seq_id is: "<<seq_idx<<std::endl;

        while (!handler->has_errored_p() && !handler->has_finished_p() && seq_idx<rx_packet_size) {
            bool ok = usrp2_ptr->rx_samples(0, handler.get());//gets 371 complex shorts
            if (!ok) {
                DEBUG(1, USRP2, "There was an error when calling rx_samples");
            }
            rx_overruns += usrp2_ptr->rx_overruns();
            rx_missing += usrp2_ptr->rx_missing();
            //fp<<"About to copy buffer into sequence"<<std::endl;
            for (unsigned int i = 0; i < USRP2_MIN_RX_SAMPLES; i++) {
                I[seq_idx] = (CORBA::Short) h_buf[i*2];
                Q[seq_idx] = (CORBA::Short) h_buf[i*2+1];
                //fp<< "seq_idx = "<< seq_idx<< "  "<<I[seq_idx]<<" "<< Q[seq_idx]<<std::endl;
                seq_idx++;
                if (seq_idx == rx_packet_size) {
                    previous_samples = USRP2_MIN_RX_SAMPLES-i-1;
                    h_buf_idx = (i+1)*2;
                    break;
                }
            }
        }

        /*fp<<"-------These are the actual values in the sequence-----"<<std::endl;
        for(unsigned int i = 0; i<rx_packet_size;i++){
            fp<< I[i]<<" "<< Q[i]<<std::endl;
        }*/

        rx_data_1_port->pushPacket(I, Q);

        //fp<<"PushPacket returned"<<std::endl;

        if (rx_packet_count != -1)
            --rx_packet_count;
    }

    DEBUG(3, USRP2, "Exiting rx_data_process thread")
//fp.close();

    usrp2_ptr->stop_rx_streaming();
    rx_active = false;
    rx_thread->exit();
}


// Helper function for tx_data_process
inline static void tx_with_usrp2(
    usrp2::usrp2::sptr usrp2_ptr,
    std::complex<short int>* tx_buf,
    unsigned int tx_buf_len,
    unsigned int &tx_underruns,
    usrp2::tx_metadata &md)
{
//    bool underrun_occured;
//std::cout<<"about to send data to the USRP2"<<std::endl;
    bool rc = usrp2_ptr->tx_16sc((unsigned int)0, tx_buf, tx_buf_len, &md);///\TODO Fix arguments!
//    if (underrun_occured) {
//        ++tx_underruns;
//        std::cerr << "USRP2 TX Underrun occured, total underruns - " << tx_underruns << std::endl;
//    }

    if (rc == false) {
        std::cerr << "USRP2 TX error detected" << std::endl;
    } //else if (rc != (int) tx_buf_len * 2) {
//  std::cerr << "USRP2 TX attempted to write " << tx_buf_len << " bytes, but only "
//            << rc << "bytes actually written." << std::endl;
//}
}

// USRP2 Expects data in little endian format
static inline short convertToLE(short s)
{
#ifdef __powerpc__
    s = bswap_16(s);
#endif

    return s;
}

void USRP2_i::tx_data_process()

{
    PortTypes::ShortSequence *I_in, *Q_in;

    const unsigned int tx_buf_len(USRP2_MAX_TX_SAMPLES);
    usrp2::tx_metadata md;
    md.timestamp = -1;
    md.start_of_burst = 1;
    md.send_now = 1;

    unsigned int tx_buf_idx(0), tx_underruns(0);
    std::complex<short int> tx_buf[tx_buf_len];

    DEBUG(3, USRP2, "Starting tx_data_process thread.")

    tx_active = true; // Cleared to stop TX in USRP2_TX_Control->stop()

    while (tx_active) {
        //std::cout<<"Inside the Tx loop"<<std::endl;
        tx_data_port->getData(I_in, Q_in);
        unsigned int data_length(I_in->length());
        if(data_length > tx_buf_len){
            std::cerr << "USRP2 Tx Buffer Overflow. Throwing Exception"<<std::endl;
            throw std::runtime_error("USRP2 buffer overflow");
        }
        //std::cout<<"There is data to transmit"<<std::endl;

        for (unsigned int i = 0; i < data_length; ++i) {
            tx_buf[i] = std::complex<short int> (convertToLE((*I_in)[i]),
                                                 convertToLE((*Q_in)[i]));
        }
        tx_with_usrp2(usrp2_ptr, tx_buf, data_length, tx_underruns, md);
        tx_data_port->bufferEmptied();
    }

    DEBUG(3, USRP2, "Exiting tx_data_process thread.")
//usrp2_ptr->stop(); ///\TODO: Do  we need to stop??
    tx_thread->exit();

}
