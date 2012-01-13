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

#include <ace/Acceptor.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Reactor.h>
#include <ace/Thread.h>
#include <ace/Condition_T.h>
#include <ace/Signal.h>

#include "ossie/cf.h"
#include "ossie/portability.h"
#include <ossie/ossieSupport.h>

#include <pulse/timeval.h>

#include "soundCard.h"
#include "NetHandler.h"

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Acceptor <Logging_Handler, ACE_SOCK_ACCEPTOR>;
template class ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Acceptor <Logging_Handler, ACE_SOCK_ACCEPTOR>
#pragma instantiate ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */

ACE_Reactor *g_reactor;
Logging_Handler *NetworkHandler = NULL;

omni_condition *NetOnAccepted;
omni_condition *NetOnReceied;
ACE_Thread_Condition<ACE_Thread_Mutex>* ACond;

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


    port_num = ACE_DEFAULT_SERVER_PORT;

// Start the play_sound thread
    capture_frame_length = 512;



    dev_usageState = CF::Device::IDLE;
    dev_operationalState = CF::Device::ENABLED;
    dev_adminState = CF::Device::UNLOCKED;

    OutputType = standardInterfaces::audioOutControl::otSpeaker;
    paPlayHandle = NULL;

    InputType = standardInterfaces::audioInControl::itMicrophone;
    paCaptureHandle = NULL;


    playback_profile.channels = 1;
    playback_profile.rate = 44100;
    playback_profile.format = PA_SAMPLE_S16LE;

    oWav_profile.samplerate = 44100;
    oWav_profile.format = SF_FORMAT_WAV|SF_FORMAT_PCM_16;
    oWav_profile.channels = 1;

    oWav_name = "/sdr/out.wav";
    iWav_name = "/sdr/in.wav";

    capture_profile.channels = 1;
    capture_profile.rate = 44100;
    capture_profile.format = PA_SAMPLE_S16LE;

    iWav_profile.samplerate = 0;
    iWav_profile.format = 0;
    iWav_profile.channels = 0;







    pa_simple *paHandle = NULL;


    set_play_channels(1);
    refresh_play_profile = false;
    refresh_capture_profile = false;

    capture_open(InputType);
    play_open(OutputType);

    ACE_thread = new omni_thread(ACE, (void *) this);
    ACE_thread->start();

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

void SoundCard_i::ACE( void * data )
{
	DEBUG(3, SoundCard, "ACE thread is running")
    ((SoundCard_i*) data)->handle_sock_event();
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
	play_close(OutputType);
    capture_close(InputType);
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

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------


void SoundCard_i::set_play_channels(unsigned short number)
{
	omni_mutex_lock l(playback_profile_mutex);
	playback_profile.channels = number;
	oWav_profile.channels = number;
	refresh_play_profile = true;

}

void SoundCard_i::set_play_connector(standardInterfaces::audioOutControl::OutType type)
{
	if(OutputType != type){
		omni_mutex_lock l(OutputType_mutex);
		play_close(OutputType);
		play_open(type);
		OutputType = type;

	}
}


void SoundCard_i::set_play_rate(::CORBA::ULong rate)
{
	DEBUG(3, SoundCard, "set_out_sample_rate invoked")
	omni_mutex_lock l(playback_profile_mutex);
	playback_profile.rate = rate;
	oWav_profile.samplerate = rate;

	refresh_play_profile = true;

}
void SoundCard_i::set_play_network_port(unsigned short port)
{
	DEBUG(3, SoundCard, "set_play_network_port invoked")
	port_num = port;

}
void SoundCard_i::set_play_file_name(const char* name)
{
	DEBUG(3, SoundCard, "set_play_file_name invoked")
	oWav_name = name;

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

bool SoundCard_i::continue_playing()
{
	omni_mutex_lock l(playing_mutex);
	return play_started;
}

void SoundCard_i::play_open(standardInterfaces::audioOutControl::OutType media)
{
    switch(media){
    case standardInterfaces::audioOutControl::otFile:
    	DEBUG(3, SoundCard, "Output Media = Wave File");
    	file_play_open(oWav_name.c_str());
    	break;
    case standardInterfaces::audioOutControl::otNet:
    	DEBUG(3, SoundCard, "Output Media = Network");
    	net_play_open(port_num);
    	break;
    default:
    	DEBUG(3, SoundCard, "Output Media = Speaker");
    	pa_play_open();
    	break;
    }
}




void SoundCard_i::net_play_open(unsigned short port)
{
	/*
	if(peer_acceptor.open (ACE_INET_Addr (port),g_reactor) == -1)
		printf("SoundCard: Net error - %s\n",ACE_OS::strerror(ACE_OS::last_error ()) );
	DEBUG(3, SoundCard, "starting up audio streaming daemon");
	*/

}

void SoundCard_i::net_play_close()
{
	/*
	if(peer_acceptor.close() == -1)
		printf("SoundCard: Net error - %s\n",ACE_OS::strerror(ACE_OS::last_error ()) );
		*/
}

int SoundCard_i::net_play_write(void* buffer,unsigned int length)
{
	size_t transfered = 0;
	ACE_Time_Value t(0,500000);
//	g_reactor->handle_events();
	if(NetworkHandler != NULL){
		NetworkHandler->peer().send_n(buffer,length,&t,&transfered);
	}
	else{
		DEBUG(3, SoundCard, "No available client");
	}
	return transfered;
}

void SoundCard_i::pa_play_open()
{
    int latency_msec = 1;
	pa_buffer_attr buffer_attr;
    buffer_attr.fragsize = buffer_attr.tlength = pa_usec_to_bytes(latency_msec * PA_USEC_PER_MSEC, &playback_profile);

	if (!(paPlayHandle = pa_simple_new(NULL,"soundCard", PA_STREAM_PLAYBACK, NULL, "playback", &playback_profile, NULL, &buffer_attr, &error))) {
		fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
	}



}

int SoundCard_i::pa_play_write(const void* buffer,unsigned int length)
{
	int err,r;
	r = pa_simple_write(paPlayHandle, buffer, length, &err);
    if ( r < 0) {
        fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
        return r;
    }
    else
    	return length;


}

void SoundCard_i::pa_play_close()
{
	if(paPlayHandle != NULL){
		pa_simple_free(paPlayHandle);
		paPlayHandle = NULL;
	}
}

void SoundCard_i::play_close(standardInterfaces::audioOutControl::OutType media)
{
    switch(media){
    case standardInterfaces::audioOutControl::otFile:
        file_play_close();
    	break;
    case standardInterfaces::audioOutControl::otNet:
    	net_play_close();
    	break;
    default:
    	pa_play_close();
    	break;
    }
}

void SoundCard_i::file_play_open(const char* name)
{
    oWav_file = sf_open(name,SFM_WRITE,&oWav_profile);
    DEBUG(3, SoundCard, "File Name: "<<name);
    if(oWav_file == NULL)
	std::cout<<"output wave file open error"<<std::endl;
}


void SoundCard_i::file_play_close()
{
	if(oWav_file != NULL)
		sf_close(oWav_file);
}

int SoundCard_i::file_play_write(void* buffer,unsigned int length)
{
	int r = sf_write_short(oWav_file,(short*)buffer,length/2);
	if(r == 0)
		sf_perror(oWav_file); 
    return r;
}

int SoundCard_i::play_write(void* buffer,unsigned int length)
{
	omni_mutex_lock l(OutputType_mutex);
	unsigned int r = 0;
    switch(OutputType){
    case standardInterfaces::audioOutControl::otFile:
    	r = file_play_write(buffer,length);
    	break;

    case standardInterfaces::audioOutControl::otNet:
    	r = net_play_write(buffer,length);
    	break;

    default:
    	r = pa_play_write(buffer,length);
    	break;
    }
    return r;
}

void SoundCard_i::play_sound()
{

    PortTypes::ShortSequence *S_in(NULL);
    CORBA::UShort S_in_length;
    TStereoWave sWave;
    TMonoWave mWave;
 //   g_reactor->owner(ACE_OS::thr_self());

    while(continue_playing()) {

    	sound_out->getData(S_in);

    	playback_profile_mutex.lock();
    	if(refresh_play_profile){
    		play_close(OutputType);
    		play_open(OutputType);
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

        	if(play_write(&mWave[0], mWave.size()*sizeof(TMonoWave::value_type)) > 0)
        		DEBUG(3, SoundCard, "playing mono sound");

        }
        else{
        	if(sWave.size() != S_in_length/2)
        		sWave.resize(S_in_length/2,TSample());
        	for(int i=0;i<S_in_length/2;i++){
        		sWave[i].left = (*S_in)[2*i];
        		sWave[i].right = (*S_in)[2*i+1];
        	}

        	if(play_write(&sWave[0], sWave.size()*sizeof(TStereoWave::value_type)) > 0)
        		DEBUG(3, SoundCard, "playing stereo sound");

        }

    	sound_out->bufferEmptied();

    }

}


//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void SoundCard_i::set_capture_channels(unsigned short number)
{
	omni_mutex_lock l(capture_profile_mutex);
	capture_profile.channels = number;
	iWav_profile.channels = number;
	refresh_capture_profile = true;

}

void SoundCard_i::set_capture_connector(standardInterfaces::audioInControl::InType type)
{
	if(InputType != type){
		omni_mutex_lock l(InputType_mutex);
		capture_close(InputType);
		capture_open(type);
		InputType = type;

	}

}


void SoundCard_i::set_capture_rate(::CORBA::ULong rate)
{
	omni_mutex_lock l(capture_profile_mutex);
	capture_profile.rate = rate;
	iWav_profile.samplerate = rate;
	refresh_capture_profile = true;
}

void SoundCard_i::set_capture_network_port(unsigned short port)
{
	DEBUG(3, SoundCard, "set_capture_network_port invoked")
	port_num = port;

}
void SoundCard_i::set_capture_file_name(const char* name)
{
	DEBUG(3, SoundCard, "set_capture_file_name invoked")
	iWav_name = name;

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

void SoundCard_i::set_capture_frame_size(unsigned long length)
{
	capture_frame_length = length;
}




void SoundCard_i::capture_open(standardInterfaces::audioInControl::InType media)
{

    switch(media){
    case standardInterfaces::audioInControl::itFile:
    	std::cout<<"Input Media = Wave File"<<std::endl;
        file_capture_open(iWav_name.c_str());
    	break;
    case standardInterfaces::audioInControl::itNet:
    	std::cout<<"Input Media = Network"<<std::endl;
    	net_capture_open(port_num);
    	break;
    default:
    	std::cout<<"Input Media = Microphone"<<std::endl;
    	pa_capture_open();
    	break;
    }
}

void SoundCard_i::pa_capture_open()
{
    /*paCaptureHandle = pa_simple_new(NULL,"soundCard", PA_STREAM_RECORD, NULL, "record", &capture_profile, NULL, NULL, &error);
    if(paCaptureHandle == NULL)
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
    */
}

void SoundCard_i::net_capture_open(unsigned short port)
{
	/*
	if(peer_acceptor.open (ACE_INET_Addr (port),g_reactor) == -1)
		printf("SoundCard: Net error - %s\n",ACE_OS::strerror(ACE_OS::last_error ()) );
	DEBUG(3, SoundCard, "starting up audio streaming daemon");
	*/
}

void SoundCard_i::file_capture_open(const char* name)
{
	char buff[128];
    iWav_profile.format = 0;
    iWav_profile.samplerate = 22050;
    iWav_file = sf_open(name,SFM_READ,&iWav_profile);
    sprintf(buff,"Capture Media Info: sample rate=%d, format=0x%x",iWav_profile.samplerate,iWav_profile.format);
    DEBUG(3, SoundCard,buff);
}

int SoundCard_i::capture_read(void* buffer,unsigned int length)
{
	omni_mutex_lock l(InputType_mutex);
	size_t r=0;
    switch(InputType){
    case standardInterfaces::audioInControl::itFile:
    	r = file_capture_read(buffer,length);
    	break;
    case standardInterfaces::audioInControl::itNet:
    	r = net_capture_read(buffer,length);
    	break;
    default:
    	r = pa_capture_read(buffer,length);
    	break;
    }
    return r;

}


int SoundCard_i::pa_capture_read(void* buffer,unsigned int length)
{
	size_t r = 0;
    int latency_msec = 1;
	pa_buffer_attr buffer_attr;
    buffer_attr.fragsize = buffer_attr.tlength = pa_usec_to_bytes(latency_msec * PA_USEC_PER_MSEC, &capture_profile);
	if(paCaptureHandle == NULL)
		paCaptureHandle = pa_simple_new(NULL,"soundCard", PA_STREAM_RECORD, NULL, "record", &capture_profile, NULL, &buffer_attr, &error);

    if(paCaptureHandle == NULL)
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));

	if(paCaptureHandle != NULL)
		r = pa_simple_read(paCaptureHandle, buffer, length, &error);
	else{
		DEBUG(3, SoundCard, "paCaptureHandle is not yet opened\n");
		return -1;
	}

	if (r < 0)
		fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
	else
		r = length;

	return r;
}

int SoundCard_i::file_capture_read(void* buffer,unsigned int length)
{
	sf_command(iWav_file,SFC_SET_SCALE_FLOAT_INT_READ,NULL,SF_TRUE);
    size_t r = sf_read_short(iWav_file,(short*)buffer,length/2);
    if(r == 0)
    	r = sf_seek(iWav_file, 0,SEEK_SET);
    if(r < 0)
    	std::cout<<"SouncCard: Wav write error - "<<sf_strerror(iWav_file)<<std::endl;
    return r;
}


int SoundCard_i::net_capture_read(void* buffer,unsigned int length)
{
	while(1){
		if(NetworkHandler != NULL){
			if(NetworkHandler->sock_queue.size() > length)
				break;
			NetOnReceied->wait();
		}
		else{
			NetOnAccepted->wait();
		}

	}
	NetworkHandler->sock_mutex.lock();
	std::copy(NetworkHandler->sock_queue.begin(),NetworkHandler->sock_queue.begin()+length, (char*)buffer);
	NetworkHandler->sock_queue.erase(NetworkHandler->sock_queue.begin(),NetworkHandler->sock_queue.begin()+length);
	NetworkHandler->sock_mutex.unlock();
	return length;
}

void SoundCard_i::capture_close(standardInterfaces::audioInControl::InType media)
{

    switch(media){
    case standardInterfaces::audioInControl::itFile:
        file_capture_close();
    	break;
    case standardInterfaces::audioInControl::itNet:
    	net_capture_close();
    	break;
    default:
    	pa_capture_close();
    	break;
    }
}

void SoundCard_i::pa_capture_close()
{
	if(paCaptureHandle != NULL){
		pa_simple_free(paCaptureHandle);
		paCaptureHandle = NULL;
	}

}

void SoundCard_i::file_capture_close()
{

}

void SoundCard_i::net_capture_close()
{
	/*
	if(peer_acceptor.close() == -1)
		printf("SoundCard: Net error - %s\n",ACE_OS::strerror(ACE_OS::last_error ()) );
	*/
}

bool SoundCard_i::continue_capturing()
{
	omni_mutex_lock l(capturing_mutex);
	return capture_started;
}

void SoundCard_i::capture_sound()
{
	//g_reactor->owner(ACE_OS::thr_self());

    TStereoWave sWave(capture_frame_length);
    TMonoWave mWave(capture_frame_length);
    PortTypes::ShortSequence I_out;

	while(continue_capturing()){

		//capture_profile_mutex.lock();
		if(refresh_capture_profile){
			capture_close(InputType);
			capture_open(InputType);
			refresh_capture_profile = false;
			DEBUG(3, SoundCard, "paHandle refreshed")
		}
		//capture_profile_mutex.unlock();

		if(capture_profile.channels == 1){
			I_out.length(capture_frame_length);

		}
		else{
			I_out.length(capture_frame_length*2);

		}

		if(capture_read(&(I_out[0]),I_out.length()*2) > 0){
			DEBUG(3, SoundCard, "Sound Captured")
			sound_in->pushPacket(I_out);
		}

	}

}

void SoundCard_i::handle_sock_event()
{

    omni_mutex OnAccepted,OnReceived;
	NetOnAccepted = new omni_condition(&OnAccepted);
	NetOnReceied = new omni_condition(&OnReceived);

	DEBUG(3, SoundCard, "ACE running")

	ACE_NEW(g_reactor, ACE_Reactor);

	if(peer_acceptor.open (ACE_INET_Addr (port_num),g_reactor) == -1)
		printf("SoundCard: Net error - %s\n",ACE_OS::strerror(ACE_OS::last_error ()) );
	DEBUG(3, SoundCard, "starting up audio streaming daemon");

	while(1){
		//ACE_DEBUG ((LM_DEBUG,"(%P|%t) waiting event\n"));
		g_reactor->handle_events();
	}
	delete NetOnAccepted;
	delete NetOnReceied;
}




