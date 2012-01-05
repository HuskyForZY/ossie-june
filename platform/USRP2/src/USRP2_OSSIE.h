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

#include <vector>

#include <omnithread.h>
#include <string.h> //Just for debug
#include <stdexcept> //Just for debug
#include <fstream>

#include "usrp2/usrp2.h"
#include <usrp2/rx_nop_handler.h>
#include <usrp2/copiers.h>

#include "db_base.h"
#include "usrp_dbid.h"
#include "usrp_prims.h"

#include "ossie/cf.h"
#include "ossie/PortTypes.h"

#include "standardinterfaces/complexShort_u.h"
#include "standardinterfaces/complexShort_p.h"
#include "standardinterfaces/Radio_Control_p.h"

#include "ossie/Device_impl.h"

///\FIXME: note from gr-usrp2/src/usrp2_base.h
//BIG ASS FIXME: get from lower layer MTU calculation
#define USRP2_MIN_RX_SAMPLES 371
#define USRP2_MAX_TX_SAMPLES 4096 //maximum number of complex short samples that can be transmitted per packet

// Definitions for provides ports
class USRP2_i;

class USRP2_RX_Control_i : public standardInterfaces_i::RX_Control_p,
        boost::noncopyable
{
public:
    USRP2_RX_Control_i(USRP2_i *_usrp2, const char* _name, const char* _domain);

    void set_number_of_channels(CORBA::ULong num);
    void get_number_of_channels(CORBA::ULong &num);
    void get_gain_range(CORBA::ULong channel, CORBA::Float &gmin,
                        CORBA::Float &gmax, CORBA::Float &gstep);
    void set_gain(CORBA::ULong channel, CORBA::Float gain);
    void get_gain(CORBA::ULong channel, CORBA::Float &gain);
    void get_frequency_range(CORBA::ULong channel, CORBA::Float &fmin,
                             CORBA::Float &fmax, CORBA::Float &fstep);
    void set_frequency(CORBA::ULong channel, CORBA::Float f);
    void get_frequency(CORBA::ULong channel, CORBA::Float &f);

    void start(CORBA::ULong channel);
    void stop(CORBA::ULong channel);

    void set_values(const CF::Properties &values);
    void set_decimation_rate(CORBA::ULong channel, CORBA::ULong M);
    void get_decimation_range(CORBA::ULong channel, CORBA::ULong &dmin,
                              CORBA::ULong &dmax, CORBA::ULong &dstep);
    void set_data_packet_size(CORBA::ULong channel, CORBA::ULong N);

private:
    USRP2_RX_Control_i();  // No default constructor
    USRP2_i *usrp2;
};

class USRP2_TX_Control_i : public standardInterfaces_i::TX_Control_p,
        boost::noncopyable
{
public:
    USRP2_TX_Control_i(USRP2_i *_usrp2, const char* _name, const char* _domain);

    void set_number_of_channels(CORBA::ULong num);
    void get_number_of_channels(CORBA::ULong &num);
    void get_gain_range(CORBA::ULong channel, CORBA::Float &gmin,
                        CORBA::Float &gmax, CORBA::Float &gstep);
    void set_gain(CORBA::ULong channel, CORBA::Float gain);
    void get_gain(CORBA::ULong channel, CORBA::Float &gain);
    void get_frequency_range(CORBA::ULong channel, CORBA::Float &fmin,
                             CORBA::Float &fmax, CORBA::Float &fstep);
    void set_frequency(CORBA::ULong channel, CORBA::Float f);
    void get_frequency(CORBA::ULong channel, CORBA::Float &f);

    void start(CORBA::ULong channel);
    void stop(CORBA::ULong channel);

    void set_values(const CF::Properties &values);
    void set_interpolation_rate(CORBA::ULong channel, CORBA::ULong I);
    void get_interpolation_range(CORBA::ULong channel, CORBA::ULong &imin,
                                 CORBA::ULong &imax, CORBA::ULong &istep);

private:
    USRP2_TX_Control_i();  // No default constructor
    USRP2_i *usrp2;
};

class TX_data_i : public POA_standardInterfaces::complexShort
{
public:
    TX_data_i(USRP2_i *_usrp2);
    ~TX_data_i();

    void pushPacket(const PortTypes::ShortSequence &I,
                    const PortTypes::ShortSequence &Q);

private:
    USRP2_i *usrp2;

    short *tx_buf;
    unsigned int tx_buf_idx;
    int tx_buf_len;
    unsigned int tx_underruns;
};

/*
///\FIXME: This is a blunt copy of gr-usrp2/src/ossie_cs_handler()
class ossie_cs_handler : public usrp2::rx_nop_handler
{
  std::complex<int16_t> *d_dest;
  int count; //just for debug

  // Private constructor
  ossie_cs_handler(uint64_t max_samples, uint64_t max_quantum, std::complex<int16_t> *dest)
    : rx_nop_handler(max_samples, max_quantum), d_dest(dest), count(0) {}

  public:
  // Shared pointer to one of these
  typedef boost::shared_ptr<ossie_cs_handler> sptr;

  // Factory function to return a shared pointer to a new instance
  static sptr make(uint64_t max_samples, uint64_t max_quantum, std::complex<int16_t> *dest)
  {
      return sptr(new ossie_cs_handler(max_samples, max_quantum, dest));
  }

  // Invoked by USRP2 API when samples are available
  bool operator()(const uint32_t *items, size_t nitems, const usrp2::rx_metadata *metadata)
  {
std::cout<<"Counter = "<< count++<< " nitems = "<< nitems<<std::endl;
      // Copy/reformat/endian swap USRP2 data to destination buffer
      usrp2::copy_u2_16sc_to_host_16sc(nitems, items, d_dest);
      d_dest += nitems;
      // FIXME: do something with metadata

      // Call parent to determine if there is room to be called again
      return rx_nop_handler::operator()(items, nitems, metadata);
  }
  ~ossie_cs_handler(){};
};*/


class ossie_cs_handler : public usrp2::rx_nop_handler
{
    std::complex<int16_t>* host_items;

public:

    ossie_cs_handler(std::complex<int16_t>* items_ptr,  uint64_t max_samples, uint64_t max_quantum=0)
            : usrp2::rx_nop_handler(max_samples,max_quantum), host_items(items_ptr) {}

    ~ossie_cs_handler() {};

    bool
    operator()(const uint32_t *items, size_t nitems, const usrp2::rx_metadata *metadata) {
        //std::cout<<"OSSIE_Handler:: # items : "<<nitems<<std::endl;
        bool ok = rx_nop_handler::operator()(items, nitems, metadata);

        //size_t host_nitems = nitems;

        usrp2::copy_u2_16sc_to_host_16sc(nitems, items, host_items);

        return ok;
    }
};


class USRP2_i : public virtual Device_impl, boost::noncopyable
{
    friend class USRP2_RX_Control_i;
    friend class USRP2_TX_Control_i;

public:
    USRP2_i(char *id, char *label, char *profile);

    static void do_rx_data_process(void *u) {
        ((USRP2_i *)u)->rx_data_process();
    };
    static void do_tx_data_process(void *u) {
        ((USRP2_i *)u)->tx_data_process();
    };

// Methods from the SCA definition
    void start()
    throw (CF::Resource::StartError, CORBA::SystemException);
    void stop()
    throw (CF::Resource::StopError, CORBA::SystemException);
    CORBA::Object_ptr getPort(const char* portName)
    throw(CF::PortSupplier::UnknownPort, CORBA::SystemException);
    void initialize()
    throw (CF::LifeCycle::InitializeError, CORBA::SystemException);
    void configure(const CF::Properties &configProperties)
    throw (CORBA::SystemException, CF::PropertySet::InvalidConfiguration,
           CF::PropertySet::PartialConfiguration);
    void query(CF::Properties &configProperties)
    throw (CORBA::SystemException, CF::UnknownProperties);
    void releaseObject()
    throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);

    omni_mutex rx_run;

// These mutex's protect port and usrp2 config operations
    omni_mutex rx_control_access;
    omni_mutex tx_control_access;

private:
    USRP2_i();  // No default constructor

// Port objects
    USRP2_RX_Control_i* rx_control_port;
    USRP2_TX_Control_i* tx_control_port;

    standardInterfaces_i::complexShort_u* rx_data_1_port;
    standardInterfaces_i::complexShort_u* rx_data_2_port;

    standardInterfaces_i::complexShort_p* tx_data_port;

// usrp2 variables
    usrp2::usrp2::sptr usrp2_ptr;
//    ossie_cs_handler rx_short_buffer;

// Daughterboard data
//
// A vector of subdevices is associated for each daughterboard with libusrp2.
// Only a single subdevice (the first) is supported at this time.
    int rx_db0;
    int tx_db0;

    db_base_sptr rx_db0_control;
    db_base_sptr tx_db0_control;

    omni_thread *rx_thread;
    omni_thread *tx_thread;

    void rx_data_process();
    void tx_data_process();

    long set_rx_packet_count;
    long rx_packet_count;           ///< Number of packets to collect from USRP2, -1 is forever
    unsigned int rx_packet_size;    ///< Number of samples to send to clients
    unsigned int rx_data_size;      ///< Size of words coming from USRP2
    unsigned int number_of_channels;
    bool complex;                   ///< True for complex data from USRP2
    unsigned int rx_overruns;
    unsigned int rx_missing;

    volatile bool rx_active;
    volatile bool tx_active;
};
