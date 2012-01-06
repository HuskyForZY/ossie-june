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
#include "WavSinc.h"

WavSinc_i::WavSinc_i(const char *uuid, omni_condition *condition) :
    Resource_impl(uuid), component_running(condition)
{

    dataIn_0 = new standardInterfaces_i::realShort_p("sndIn");
    start();
}

WavSinc_i::~WavSinc_i(void)
{
    delete dataIn_0;
}

// Static function for omni thread
void WavSinc_i::Run( void * data )
{
    ((WavSinc_i*)data)->ProcessData();
}

CORBA::Object_ptr WavSinc_i::getPort( const char* portName ) throw (
    CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    DEBUG(3, WavSinc, "getPort() invoked with " << portName)

    CORBA::Object_var p;

    p = dataIn_0->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

    /*exception*/
    throw CF::PortSupplier::UnknownPort();
}

void WavSinc_i::start() throw (CORBA::SystemException,
    CF::Resource::StartError)
{
    DEBUG(3, WavSinc, "start() invoked")
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

void WavSinc_i::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    DEBUG(3, WavSinc, "stop() invoked")
	omni_mutex_lock l(processing_mutex);
	thread_started = false;
}

void WavSinc_i::releaseObject() throw (CORBA::SystemException,
    CF::LifeCycle::ReleaseError)
{
    DEBUG(3, WavSinc, "releaseObject() invoked")

    component_running->signal();
}

void WavSinc_i::initialize() throw (CF::LifeCycle::InitializeError,
    CORBA::SystemException)
{
    DEBUG(3, WavSinc, "initialize() invoked")
}

void WavSinc_i::query( CF::Properties & configProperties ) throw (CORBA::SystemException, CF::UnknownProperties)
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

void WavSinc_i::configure(const CF::Properties& props)
throw (CORBA::SystemException,
    CF::PropertySet::InvalidConfiguration,
    CF::PropertySet::PartialConfiguration)
{
    DEBUG(3, WavSinc, "configure() invoked")

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

void WavSinc_i::ProcessData()
{
    DEBUG(3, WavSinc, "ProcessData() invoked")



    PortTypes::ShortSequence *I_in_0(NULL);
    CORBA::UShort I_in_0_length;

    while(continue_processing())
    {
        dataIn_0->getData(I_in_0);

        I_in_0_length = I_in_0->length();

        /*insert code here to do work*/
        DEBUG(3, WavSinc, "Sound Drained")






        //dataIn_0->bufferEmptied();
    }
}

bool WavSinc_i::continue_processing()
{
	omni_mutex_lock l(processing_mutex);
	return thread_started;
}


