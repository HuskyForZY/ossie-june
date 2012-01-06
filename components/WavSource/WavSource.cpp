/****************************************************************************
Copyright 2007 Virginia Polytechnic Institute and State University
This file is part of the OSSIE __COMP_NAME__.
OSSIE __COMP_NAME__ is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
"(at your option) any later version
OSSIE __COMP_NAME__ is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with OSSIE __COMP_NAME__; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****************************************************************************/

#include <string>
#include <iostream>
#include <stdio.h>
#include "WavSource.h"



WavSource_i::WavSource_i(const char *uuid, omni_condition *condition) :
    Resource_impl(uuid), component_running(condition)
{
	ossieDebugLevel = 100;
    dataOut_0 = new standardInterfaces_i::realShort_u("sndOut");
   // start();
}

WavSource_i::~WavSource_i(void)
{
    delete dataOut_0;
}

// Static function for omni thread
void WavSource_i::Run( void * data )
{
    ((WavSource_i*)data)->ProcessData();
}

CORBA::Object_ptr WavSource_i::getPort( const char* portName ) throw (
    CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    DEBUG(3, WavSource, "getPort() invoked with " << portName)

    CORBA::Object_var p;

    p = dataOut_0->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

    /*exception*/
    throw CF::PortSupplier::UnknownPort();
}

void WavSource_i::start() throw (CORBA::SystemException,
    CF::Resource::StartError)
{
    DEBUG(3, WavSource, "start() invoked")
	omni_mutex_lock  l(processing_mutex);
	if( false == thread_started )
	{
		thread_started = true;
		// Create the thread for the writer's processing function
		processing_thread = new omni_thread(Run, (void *) this);

		// Start the thread containing the writer's processing function
		processing_thread->start();
	}
}

void WavSource_i::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    DEBUG(3, WavSource, "stop() invoked")
	omni_mutex_lock l(processing_mutex);
	thread_started = false;
}

void WavSource_i::releaseObject() throw (CORBA::SystemException,
    CF::LifeCycle::ReleaseError)
{
    DEBUG(3, WavSource, "releaseObject() invoked")

    component_running->signal();
}

void WavSource_i::initialize() throw (CF::LifeCycle::InitializeError,
    CORBA::SystemException)
{
    DEBUG(3, WavSource, "initialize() invoked")
}

void WavSource_i::query( CF::Properties & configProperties ) throw (CORBA::SystemException, CF::UnknownProperties)
{
	if( configProperties.length() == 0 )
	{
		configProperties.length( propertySet.length() );
		for( int i = 0; i < propertySet.length(); i++ )
		{
			configProperties[i].id = CORBA::string_dup( propertySet[i].id );
			configProperties[i].value = propertySet[i].value;
		}
		return;
	} else {
		for( int i = 0; i < configProperties.length(); i++ ) {
			for( int j = 0; j < propertySet.length(); j++ ) {
				if( strcmp(configProperties[i].id, propertySet[j].id) == 0 ) {
					configProperties[i].value = propertySet[j].value;
				}
			}
		}
	} // end if-else
}

void WavSource_i::configure(const CF::Properties& props)
throw (CORBA::SystemException,
    CF::PropertySet::InvalidConfiguration,
    CF::PropertySet::PartialConfiguration)
{
    DEBUG(3, WavSource, "configure() invoked")

    static int init = 0;
    if( init == 0 ) {
        if( props.length() == 0 ) {
            std::cout << "configure called with invalid props.length - " << props.length() << std::endl;
            return;
        }
        propertySet.length(props.length());
        for( int j=0; j < props.length(); j++ ) {
            propertySet[j].id = CORBA::string_dup(props[j].id);
            propertySet[j].value = props[j].value;
        }
        init = 1;
    }

    std::cout << "props length : " << props.length() << std::endl;

    for ( int i = 0; i <props.length(); i++)
    {
        std::cout << "Property id : " << props[i].id << std::endl;

    }
}

void WavSource_i::ProcessData()
{
	const int LENGTH = 512;
    DEBUG(3, WavSource, "ProcessData() invoked");
    PortTypes::ShortSequence I_out_0;
    FILE* fp = fopen("/home/june/workspace/WavSource/HighSky.wav","rb");
    short* buffer = new short[LENGTH*2];
    int N;
    while(continue_processing())
    {
        I_out_0.length(LENGTH); //must define length of output
        N = fread(buffer,4,LENGTH,fp);
        for(int i=0;i<N;i++){
        	I_out_0[i] = buffer[2*i];
        }
        if(feof(fp)){
        	std::cout<<"endoffile";
        	break;
        }
        dataOut_0->pushPacket(I_out_0);
        DEBUG(3, WavSource, "sending packet")

    }
    DEBUG(3, WavSource, "End of ProcessData")
	fclose(fp);
}

bool WavSource_i::continue_processing()
{
	omni_mutex_lock l(processing_mutex);
	return thread_started;
}


