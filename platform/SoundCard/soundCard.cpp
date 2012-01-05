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
    The soundCardPlayback.cpp file contains definitions for the
    SoundCardPlayback_i class implementation.
*/

#include <iostream>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "ossie/cf.h"
#include "ossie/portability.h"
#include <ossie/ossieSupport.h>

#include "soundCard.h"



// Initializing constructor
SoundCard_i::SoundCard_i(char *id, char *label, char *profile)
{
	ossieSupport::ORB orb;
    DEBUG(3, SoundCard, "constructor invoked")


    dev_id = id;
    dev_label = label;
    dev_profile = profile;


// initialize variables
    sound_out = new standardInterfaces_i::realShort_p("soundOut","DomainName1");
    sound_in =  new standardInterfaces_i::realShort_u("soundIn","DomainName1");
    sound_out_control = new audioOutControl_i(this,"soundOutControl","DomainName1");
    sound_in_conrtol = new audioInControl_i(this,"soundInControl","DomainName1");


// Start the play_sound thread

    capture_thread = new omni_thread(capture, (void *) this);


    dev_usageState = CF::Device::IDLE;
    dev_operationalState = CF::Device::ENABLED;
    dev_adminState = CF::Device::UNLOCKED;

    OutputType = standardInterfaces::audioOutControl::otSpeaker;

    playback_profile.channels = 1;
    playback_profile.rate = 44100;
    playback_profile.format = PA_SAMPLE_S16LE;

    oWav_profile.samplerate = 44100;
    oWav_profile.format = SF_FORMAT_WAV|SF_FORMAT_PCM_16;
    oWav_profile.channels = 1;

    oWav_name = "/home/june/out.wav";
    iWav_name = "/home/june/in.wav";

    capture_profile.channels = 1;
    capture_profile.rate = 44100;
    capture_profile.format = PA_SAMPLE_S16LE;

    iWav_profile.samplerate = 0;
    iWav_profile.format = 0;
    iWav_profile.channels = 0;


    capture_frame_length = 512;



    pa_simple *paHandle = NULL;


    set_play_channels(1);
    refresh_play_profile = false;
    refresh_capture_profile = false;



}

// Default destructor
SoundCard_i::~SoundCard_i()
{

}

// start data processing thread
void SoundCard_i::play( void * data )
{
	DEBUG(3, SoundCard, "playing thread is running")
    ((SoundCard_i*) data)->play_sound();
}

void SoundCard_i::capture( void * data )
{
	DEBUG(3, SoundCard, "capturing thread is running")
    ((SoundCard_i*) data)->capture_sound();
}

// Device methods
CF::Device::UsageType SoundCard_i::usageState() throw (CORBA::SystemException)
{
    return dev_usageState;
}

CF::Device::AdminType SoundCard_i::adminState() throw (CORBA::SystemException)
{
    return dev_adminState;
}

CF::Device::OperationalType SoundCard_i::operationalState() throw (CORBA::SystemException)
{
    return dev_operationalState;
}

CF::AggregateDevice_ptr SoundCard_i::compositeDevice() throw (CORBA::SystemException)
{
    return NULL;
}

void SoundCard_i::adminState (CF::Device::AdminType _adminType)
throw (CORBA::SystemException)
{
    dev_adminState = _adminType;
}

CORBA::Boolean SoundCard_i::allocateCapacity (const CF::
        Properties & capacities)
throw (CORBA::SystemException, CF::Device::InvalidCapacity,
       CF::Device::InvalidState)
{

    return true;
}

void SoundCard_i::deallocateCapacity (const CF::Properties & capacities)
throw (CORBA::SystemException, CF::Device::InvalidCapacity,
       CF::Device::InvalidState)
{

}

char *SoundCard_i::label ()
throw (CORBA::SystemException)
{
    return CORBA::string_dup(dev_label.c_str());
}


char *SoundCard_i::softwareProfile ()
throw (CORBA::SystemException)
{
    return CORBA::string_dup(dev_profile.c_str());
}

// Resource methods
void SoundCard_i::start() throw (CF::Resource::StartError, CORBA::SystemException)

{
    DEBUG(3, SoundCard, "start() invoked")
}

void SoundCard_i::stop() throw (CF::Resource::StopError, CORBA::SystemException)

{
    DEBUG(3, SoundCard, "stop() invoked")
}

char *SoundCard_i::identifier () throw (CORBA::SystemException)
{
    return CORBA::string_dup(dev_id.c_str());
}


// PortSupplier methods
CORBA::Object_ptr SoundCard_i::getPort(const char* portName) throw(CF::PortSupplier::UnknownPort, CORBA::SystemException)

{
    DEBUG(3, SoundCard, "getPort() invoked with : " << portName)

	CORBA::Object_var p;

	p = sound_out->getPort(portName);
	if (!CORBA::is_nil(p))
		return p._retn();

	p = sound_in->getPort(portName);
	if (!CORBA::is_nil(p))
		return p._retn();

	/*exception*/
	throw CF::PortSupplier::UnknownPort();
}

// Life Cycle methods
void SoundCard_i::initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException)

{
    DEBUG(3, SoundCard, "initialize() invoked")
	start_play();
}

void SoundCard_i::releaseObject() throw (CF::LifeCycle::ReleaseError, CORBA::SystemException)

{
    DEBUG(3, SoundCard, "releaseObject invoked")
	pa_play_close();
    pa_capture_close();
}

// Property Set methods
void SoundCard_i::configure(const CF::Properties &props) throw (CORBA::SystemException, CF::PropertySet::InvalidConfiguration, CF::PropertySet::PartialConfiguration)
{
    DEBUG(3, SoundCard, "configure invoked. Number of props = " << props.length() );

}

void SoundCard_i::query (CF::Properties & configProperties)
throw (CORBA::SystemException, CF::UnknownProperties)
{

}

// TestableObject interfaces

void SoundCard_i::runTest (CORBA::ULong _number, CF::Properties & _props)
throw (CORBA::SystemException, CF::TestableObject::UnknownTest,
       CF::UnknownProperties)
{

}

void SoundCard_i::set_play_channels(unsigned short number)
{
	omni_mutex_lock l(playback_profile_mutex);
	playback_profile.channels = number;
	oWav_profile.channels = number;
	refresh_play_profile = true;

}

void SoundCard_i::set_play_connector(standardInterfaces::audioOutControl::OutType type)
{
	omni_mutex_lock l(OutputType_mutex);
	DEBUG(3, SoundCard, "set_play_connector invoked")
    OutputType = type;
    switch(OutputType){
    case standardInterfaces::audioOutControl::otFile:
    	DEBUG(3, SoundCard, "Output Media = Wave File");
        oWav_file = sf_open(oWav_name.c_str(),SFM_WRITE,&oWav_profile);
    	break;
    case standardInterfaces::audioOutControl::otNet:
    	DEBUG(3, SoundCard, "Output Media = Network");
    	break;
    default:
    	DEBUG(3, SoundCard, "Output Media = Speaker");
    	break;
    }
    //refresh_play_profile = true;
}


void SoundCard_i::set_play_rate(::CORBA::ULong rate)
{
	DEBUG(3, SoundCard, "set_out_sample_rate invoked")
	omni_mutex_lock l(playback_profile_mutex);
	playback_profile.rate = rate;
	oWav_profile.samplerate = rate;

	refresh_play_profile = true;

}

void SoundCard_i::start_play()
{
	omni_mutex_lock l(playing_mutex);
	if(play_started == false){
		play_started = true;
		play_thread = new omni_thread(play, (void *) this);
		play_thread->start();
	}

}
void SoundCard_i::stop_play()
{
	omni_mutex_lock l(playing_mutex);
	play_started = false;
}
void SoundCard_i::mute_play(bool enable)
{

}

void SoundCard_i::play_sound()
{

    PortTypes::ShortSequence *S_in(NULL);
    CORBA::UShort S_in_length;
    TStereoWave sWave;
    TMonoWave mWave;
    pa_play_open();



    while(continue_playing()) {

    	sound_out->getData(S_in);

    	playback_profile_mutex.lock();
    	if(refresh_play_profile){
    		pa_play_close();
    		pa_play_open();
    		refresh_play_profile = false;
    		DEBUG(3, SoundCard, "paHandle refreshed")
    	}
    	playback_profile_mutex.unlock();

        S_in_length = S_in->length();
        if(playback_profile.channels == 1){
			if(mWave.size() != S_in_length)
				mWave.resize(S_in_length,0);
        	for(int i=0;i<S_in_length;i++){
        		mWave[i] = (*S_in)[i];
        	}
        	DEBUG(3, SoundCard, "playing mono sound")
        	pa_play_write(&mWave[0], mWave.size()*sizeof(TMonoWave::value_type));



        }
        else{
        	if(sWave.size() != S_in_length/2)
        		sWave.resize(S_in_length/2,TSample());
        	for(int i=0;i<S_in_length/2;i++){
        		sWave[i].left = (*S_in)[2*i];
        		sWave[i].right = (*S_in)[2*i+1];
        	}

        	DEBUG(3, SoundCard, "playing stereo sound")
        	pa_play_write(&sWave[0], sWave.size()*sizeof(TStereoWave::value_type));


        }

    	sound_out->bufferEmptied();

    }

    pa_play_close();

}

void SoundCard_i::set_capture_channels(unsigned short number)
{
	omni_mutex_lock l(capture_profile_mutex);
	capture_profile.channels = number;
	iWav_profile.channels = number;
	refresh_capture_profile = true;

}

void SoundCard_i::set_capture_connector(standardInterfaces::audioInControl::InType type)
{
	omni_mutex_lock l(InputType_mutex);
	DEBUG(3, SoundCard, "set_capture_connector invoked")
    InputType = type;
    switch(InputType){
    case standardInterfaces::audioInControl::itFile:
    	DEBUG(3, SoundCard, "Input Media = Wave File");
        iWav_file = sf_open(iWav_name.c_str(),SFM_READ,&iWav_profile);
    	break;
    case standardInterfaces::audioInControl::itNet:
    	DEBUG(3, SoundCard, "Input Media = Network");
    	break;
    default:
    	DEBUG(3, SoundCard, "Input Media = Microphone");
    	break;
    }

}


void SoundCard_i::set_capture_rate(::CORBA::ULong rate)
{
	omni_mutex_lock l(capture_profile_mutex);
	capture_profile.rate = rate;
	iWav_profile.samplerate = rate;
	refresh_capture_profile = true;
}

void SoundCard_i::start_capture()
{
	omni_mutex_lock l(capturing_mutex);
	if(capture_started == false){
		capture_started = true;
		capture_thread = new omni_thread(capture, (void *) this);
		capture_thread->start();
	}
}

void SoundCard_i::stop_capture()
{
	omni_mutex_lock l(capturing_mutex);
	capture_started = false;
}

void SoundCard_i::set_capture_frame_length(unsigned long length)
{
	capture_frame_length = length;
}

void SoundCard_i::capture_sound()
{
    TStereoWave sWave(capture_frame_length);
    TMonoWave mWave(capture_frame_length);
    PortTypes::ShortSequence I_out;
    pa_capture_open();

	while(continue_capturing()){

		capture_profile_mutex.lock();
		if(refresh_capture_profile){
			pa_capture_close();
			pa_capture_open();
			refresh_capture_profile = false;
			DEBUG(3, SoundCard, "paHandle refreshed")
		}
		capture_profile_mutex.unlock();

		if(capture_profile.channels == 1){
			I_out.length(512);
			DEBUG(3, SoundCard, "capturing mono sound")
		}
		else{
			I_out.length(capture_frame_length);
			DEBUG(3, SoundCard, "capturing mono sound")
		}

		pa_capture_read(&(I_out[0]),I_out.length()*2);

		sound_in->pushPacket(I_out);

	}
	pa_capture_close();
}

void SoundCard_i::pa_play_open()
{

	if (!(paPlayHandle = pa_simple_new(NULL,"soundCard", PA_STREAM_PLAYBACK, NULL, "playback", &playback_profile, NULL, NULL, &error))) {
		fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
	}

}
void SoundCard_i::pa_capture_open()
{
    if (!(paCaptureHandle = pa_simple_new(NULL,"soundCard", PA_STREAM_RECORD, NULL, "record", &capture_profile, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
    }
}
void SoundCard_i::pa_play_close()
{
	if(paPlayHandle != NULL)
		pa_simple_free(paPlayHandle);
	if(oWav_file != NULL)
		sf_close(oWav_file);

}

void SoundCard_i::pa_capture_close()
{
	pa_simple_free(paCaptureHandle);
}

int SoundCard_i::pa_play_write(const void* buffer,unsigned int length)
{
	omni_mutex_lock l(OutputType_mutex);
	unsigned int r = 0;
    switch(OutputType){
    case standardInterfaces::audioOutControl::otFile:
        r = sf_write_short(oWav_file,(short*)buffer,length/2);
        if(r < 1)
        	std::cout<<"SouncCard: Wav write error - "<<sf_strerror(oWav_file)<<std::endl;
    	break;

    case standardInterfaces::audioOutControl::otNet:
    	break;

    default:
        if (pa_simple_write(paPlayHandle, buffer, length, &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
        }
    	break;
    }

}
int SoundCard_i::pa_capture_read(void* buffer,unsigned int length)
{
	omni_mutex_lock l(InputType_mutex);
	unsigned int r = 0;
    switch(InputType){
    case standardInterfaces::audioInControl::itFile:
        r = sf_read_short(iWav_file,(short*)buffer,length/2);
        if(r == 0)
        	r = sf_seek(iWav_file, 0,SEEK_SET);
        if(r < 0)
        	std::cout<<"SouncCard: Wav write error - "<<sf_strerror(iWav_file)<<std::endl;
    	break;

    case standardInterfaces::audioInControl::itNet:
    	break;

    default:
        if (pa_simple_read(paCaptureHandle, buffer, length, &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
        }
    	break;
    }


}

bool SoundCard_i::continue_playing()
{
	omni_mutex_lock l(playing_mutex);
	return play_started;
}

bool SoundCard_i::continue_capturing()
{
	omni_mutex_lock l(capturing_mutex);
	return capture_started;
}

