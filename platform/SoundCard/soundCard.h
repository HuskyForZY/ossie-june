/****************************************************************************

Copyright 2006, 2008 Virginia Polytechnic Institute and State University

This file is part of the OSSIE Sound_out Device.

OSSIE Sound_out Device is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE Sound_out Device is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE Sound_out Device; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


****************************************************************************/

/** \file
    The soundCardPlayback.h file contains declarations for the
    SoundCardPlayback_i classes as well as the data and control ports.
*/

#include <string>
#include <cstdlib>
#include <omnithread.h>
#include <vector>
#include <deque>
#include <stdio.h>

#include <pulse/simple.h>
#include <pulse/error.h>
#include <sndfile.h>

#include "ossie/cf.h"
#include "ossie/PortTypes.h"
#include "ossie/debug.h"

#include <standardinterfaces/realShort_p.h>
#include <standardinterfaces/realShort_u.h>
#include <standardinterfaces/soundControl_p.h>

class SoundCard_i;

class audioOutControl_i: public standardInterfaces_i::audioOutControl_p
{
public:
	audioOutControl_i(SoundCard_i* soundCard,const char* portName, const char* domain);
	void set_sample_rate(::CORBA::ULong rate);
	void set_channels(::CORBA::UShort channels);
	void set_connector(standardInterfaces::audioOutControl::OutType type);
	void set_network_address(const char* address);
	void set_file_name(const char* name);
	void start();
	void stop();
	void mute(::CORBA::Boolean enable);
	void reset();

private:
	SoundCard_i* soundCard;
	audioOutControl_i();
};

class audioInControl_i: public standardInterfaces_i::audioInControl_p
{
public:
	audioInControl_i(SoundCard_i* soundCard,const char* portName, const char* domain);
	void set_sample_rate(::CORBA::ULong rate);
	void set_channels(::CORBA::UShort channels);
	void set_connector(standardInterfaces::audioInControl::InType type);
	void set_network_address(const char* address);
	void set_file_name(const char* name);
	void start();
	void stop();
	void set_frame_size(::CORBA::ULong length);
	void reset();

private:
	SoundCard_i* soundCard;
	audioInControl_i();
};

struct TSample
{
	short left;
	short right;
};

typedef std::vector<TSample> TStereoWave;
typedef std::vector<short> TMonoWave;


/// Main Sound card device definition
class SoundCard_i : public virtual POA_CF::Device

{
	friend class outControl_i;
	friend class inControl_i;
public:
/// Initializing constructor
    SoundCard_i(char *id, char *label, char *profile);

/// Default destructor
    ~SoundCard_i();

// Device methods
    CF::Device::UsageType usageState ()
    throw (CORBA::SystemException);
    CF::Device::AdminType adminState ()
    throw (CORBA::SystemException);
    CF::Device::OperationalType operationalState ()
    throw (CORBA::SystemException);
    CF::AggregateDevice_ptr compositeDevice ()
    throw (CORBA::SystemException);
    void adminState (CF::Device::AdminType _adminType)
    throw (CORBA::SystemException);
    void deallocateCapacity (const CF::Properties & capacities)
    throw (CF::Device::InvalidState, CF::Device::InvalidCapacity,
           CORBA::SystemException);
    CORBA::Boolean allocateCapacity (const CF::Properties & capacities)
    throw (CF::Device::InvalidState, CF::Device::InvalidCapacity,
           CORBA::SystemException);

    char *label () throw (CORBA::SystemException);
    char *softwareProfile () throw (CORBA::SystemException);

// Resource methods
/// Does nothing
    void start()
    throw (CF::Resource::StartError, CORBA::SystemException);

/// Does nothing
    void stop()
    throw (CF::Resource::StopError, CORBA::SystemException);

    char *identifier () throw (CORBA::SystemException);

//// static function for omni thread
    static void play( void * data );
    static void capture( void * data );
    static void ACE( void* data);

// Life Cycle methods
/// Checks if sound card device is not already in use
    void initialize()
    throw (CF::LifeCycle::InitializeError, CORBA::SystemException);
/// Does nothing
    void releaseObject () throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);


// Property Set methods
/// Sets the following properties (not currently read from .prf.xml)
///   - number of channels (default is 2 for stereo)
///   - sampling rate (default is 16kHz)
///   - periodsize
///   - buffer size
    void configure(const CF::Properties &props)
    throw (CORBA::SystemException,
           CF::PropertySet::InvalidConfiguration,
           CF::PropertySet::PartialConfiguration);

    void query (CF::Properties & configProperties)
    throw (CF::UnknownProperties, CORBA::SystemException);

// Port Supplier interfaces
    CORBA::Object* getPort (const char *) throw (CF::PortSupplier::UnknownPort,
            CORBA::SystemException);

// TestableObject Interfaces
    void runTest (CORBA::ULong TestID, CF::Properties & testValues)
    throw (CF::UnknownProperties, CF::TestableObject::UnknownTest,
           CORBA::SystemException);

	void set_play_rate(unsigned long rate);
	void set_play_channels(unsigned short number);
	void set_play_connector(standardInterfaces::audioOutControl::OutType type);
	void set_play_network_address(const char* address);
	void set_play_file_name(const char* name);
	void start_play();
	void stop_play();
	void mute_play(bool enable);
	void reset_play();


	void set_capture_rate(unsigned long rate);
	void set_capture_channels(unsigned short number);
	void set_capture_connector(standardInterfaces::audioInControl::InType type);
	void set_capture_network_address(const char* address);
	void set_capture_file_name(const char* name);
	void start_capture();
	void stop_capture();
	void set_capture_frame_size(unsigned long length);
	void reset_capture();


private:
/// Disallowing default constructor
    SoundCard_i();

/// Disallowing copy constructor
    SoundCard_i(SoundCard_i&);

    CF::Device::AdminType dev_adminState;
    CF::Device::UsageType dev_usageState;
    CF::Device::OperationalType dev_operationalState;

    std::string dev_id;
    std::string dev_label;
    std::string dev_profile;

    standardInterfaces_i::realShort_p *sound_out;
    standardInterfaces_i::realShort_u *sound_in;
    audioOutControl_i* sound_out_control;
    audioInControl_i* sound_in_conrtol;


    unsigned int buffer_size;


    bool refresh_play_profile;
    bool refresh_capture_profile;

    standardInterfaces::audioOutControl::OutType OutputType;
    standardInterfaces::audioInControl::InType InputType;
    omni_mutex OutputType_mutex;
    omni_mutex InputType_mutex;

    int error;

/// Main processing playback loop
    omni_thread* play_thread;
    omni_thread* capture_thread;

    omni_mutex playing_mutex;
    omni_mutex capturing_mutex;



    bool play_started;
    bool play_muted;
    bool capture_started;

    pa_sample_spec playback_profile;
    pa_sample_spec capture_profile;

    omni_mutex playback_profile_mutex;
    omni_mutex capture_profile_mutex;


    pa_simple *paPlayHandle;
    pa_simple *paCaptureHandle;

    unsigned long capture_frame_length;

    SNDFILE *oWav_file, *iWav_file;
    std::string oWav_name,iWav_name;
    std::string oAddress, iAddress;
    SF_INFO oWav_profile, iWav_profile;





    void play_sound();
    void capture_sound();
    void handle_sock_event();


    void capture_open(standardInterfaces::audioInControl::InType media);
    void pa_capture_open();
    void net_capture_open(const char* address);
    void file_capture_open(const char* name);

    void capture_close(standardInterfaces::audioInControl::InType media);
    void pa_capture_close();
    void net_capture_close();
    void file_capture_close();

    void play_open(standardInterfaces::audioOutControl::OutType media);
    void pa_play_open();
    void file_play_open(const char* name);
    void net_play_open(const char* address);

    void play_close(standardInterfaces::audioOutControl::OutType media);
    void pa_play_close();
    void net_play_close();
    void file_play_close();

    int pa_play_write(const void* buffer,unsigned int length);
    int net_play_write(void* buffer,unsigned int length);
    int file_play_write(void* buffer,unsigned int length);
    int play_write(void* buffer,unsigned int length);

    int pa_capture_read(void* buffer,unsigned int length);
    int net_capture_read(void* buffer,unsigned int length);
    int file_capture_read(void* buffer,unsigned int length);
    int capture_read(void* buffer,unsigned int length);


    //void wav_open(char* finename);
    //void wav_read(char* buff,unsigned int length);
    //void wav_write(char* buff,unsigned int length);
    //void wav_close();

    bool continue_playing();
    bool continue_capturing();

};



