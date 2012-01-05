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
#include "Sound_Commander.h"

Sound_Commander_i::Sound_Commander_i(const char *uuid, omni_condition *condition) :
    Resource_impl(uuid), component_running(condition)
{
	soundOutControl = new standardInterfaces_i::audioOutControl_u("soundOutControl");
	soundInControl = new standardInterfaces_i::audioInControl_u("soundInControl");
//    start();
}

Sound_Commander_i::~Sound_Commander_i(void)
{
    delete soundOutControl;
    delete soundInControl;
}

// Static function for omni thread
void Sound_Commander_i::Run( void * data )
{
    ((Sound_Commander_i*)data)->ProcessData();
}

CORBA::Object_ptr Sound_Commander_i::getPort( const char* portName ) throw (
    CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    DEBUG(3, Sound_Commander, "getPort() invoked with " << portName)

    CORBA::Object_var p;

    p = soundOutControl->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

    p = soundInControl->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

    /*exception*/
    throw CF::PortSupplier::UnknownPort();
}

void Sound_Commander_i::start() throw (CORBA::SystemException,
    CF::Resource::StartError)
{
    DEBUG(3, Sound_Commander, "start() invoked")

	/*
	omni_mutex_lock  l(processing_mutex);
	if( false == thread_started )
	{
		thread_started = true;
		// Create the thread for the writer's processing function
		processing_thread = new omni_thread(Run, (void *) this);

		// Start the thread containing the writer's processing function
		processing_thread->start();
	}
	*/
	soundOutControl->start();
    soundInControl->start();
}

void Sound_Commander_i::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    //DEBUG(3, Sound_Commander, "stop() invoked")
	//omni_mutex_lock l(processing_mutex);
	//thread_started = false;
	soundOutControl->stop();
    soundInControl->stop();
}

void Sound_Commander_i::releaseObject() throw (CORBA::SystemException,
    CF::LifeCycle::ReleaseError)
{
    DEBUG(3, Sound_Commander, "releaseObject() invoked")

    component_running->signal();
}

void Sound_Commander_i::initialize() throw (CF::LifeCycle::InitializeError,
    CORBA::SystemException)
{
    DEBUG(3, Sound_Commander, "initialize() invoked")
}

void Sound_Commander_i::query( CF::Properties & configProperties ) throw (CORBA::SystemException, CF::UnknownProperties)
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

void Sound_Commander_i::configure(const CF::Properties& props)
throw (CORBA::SystemException,
    CF::PropertySet::InvalidConfiguration,
    CF::PropertySet::PartialConfiguration)
{
    DEBUG(3, Sound_Commander, "configure() invoked")

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

        if (strcmp(props[i].id, "DCE:d1e1764c-2f72-11e1-9da8-000c29c0b7da") == 0)
        {
            CORBA::ULong simple_temp;
            props[i].value >>= simple_temp;
            simple_0_value = simple_temp;
            soundOutControl->set_sample_rate(simple_0_value);
            for( int k = 0; k < propertySet.length(); k++ ) {
                if( strcmp(propertySet[k].id, props[i].id) == 0 ) {
                    propertySet[k].value = props[i].value;
                    break;
                }
            }
        }

        if (strcmp(props[i].id, "DCE:fad06162-2f72-11e1-b943-000c29c0b7da") == 0)
        {
            CORBA::UShort simple_temp;
            props[i].value >>= simple_temp;
            simple_1_value = simple_temp;
            soundOutControl->set_channels(simple_1_value);
            for( int k = 0; k < propertySet.length(); k++ ) {
                if( strcmp(propertySet[k].id, props[i].id) == 0 ) {
                    propertySet[k].value = props[i].value;
                    break;
                }
            }
        }

        if (strcmp(props[i].id, "DCE:88c75d18-3126-11e1-8542-000c29c0b7da") == 0)
        {
            CORBA::UShort simple_temp;
            props[i].value >>= simple_temp;
            simple_2_value = simple_temp;
            soundOutControl->set_connector((standardInterfaces::audioOutControl::OutType)simple_2_value);
            for( int k = 0; k < propertySet.length(); k++ ) {
                if( strcmp(propertySet[k].id, props[i].id) == 0 ) {
                    propertySet[k].value = props[i].value;
                    break;
                }
            }
        }

        if (strcmp(props[i].id, "DCE:ec607c06-3126-11e1-b64b-000c29c0b7da") == 0)
        {
            CORBA::Boolean simple_temp;
            props[i].value >>= simple_temp;
            simple_3_value = simple_temp;
            soundOutControl->mute(simple_3_value);
            for( int k = 0; k < propertySet.length(); k++ ) {
                if( strcmp(propertySet[k].id, props[i].id) == 0 ) {
                    propertySet[k].value = props[i].value;
                    break;
                }
            }
        }

        if (strcmp(props[i].id, "DCE:fc2787ec-3126-11e1-af56-000c29c0b7da") == 0)
        {
            CORBA::ULong simple_temp;
            props[i].value >>= simple_temp;
            simple_4_value = simple_temp;
            soundInControl->set_sample_rate(simple_4_value);
            for( int k = 0; k < propertySet.length(); k++ ) {
                if( strcmp(propertySet[k].id, props[i].id) == 0 ) {
                    propertySet[k].value = props[i].value;
                    break;
                }
            }
        }

        if (strcmp(props[i].id, "DCE:0c354192-3127-11e1-bd4d-000c29c0b7da") == 0)
        {
            CORBA::UShort simple_temp;
            props[i].value >>= simple_temp;
            simple_5_value = simple_temp;
            soundInControl->set_channels(simple_5_value);
            for( int k = 0; k < propertySet.length(); k++ ) {
                if( strcmp(propertySet[k].id, props[i].id) == 0 ) {
                    propertySet[k].value = props[i].value;
                    break;
                }
            }
        }

        if (strcmp(props[i].id, "DCE:1959b75e-3127-11e1-b345-000c29c0b7da") == 0)
        {
            CORBA::UShort simple_temp;
            props[i].value >>= simple_temp;
            simple_6_value = simple_temp;
            soundInControl->set_connector((standardInterfaces::audioInControl::InType)simple_6_value);
            for( int k = 0; k < propertySet.length(); k++ ) {
                if( strcmp(propertySet[k].id, props[i].id) == 0 ) {
                    propertySet[k].value = props[i].value;
                    break;
                }
            }
        }

        if (strcmp(props[i].id, "DCE:2736085a-3127-11e1-8b7e-000c29c0b7da") == 0)
        {
        	DEBUG(3, Sound_Commander, "capture_start property")
            CORBA::Boolean simple_temp;
            props[i].value >>= simple_temp;
            simple_7_value = simple_temp;
            if(simple_7_value){
            	soundInControl->start();
            	DEBUG(3, Sound_Commander, "capture_start = true property")
            }
            else{
            	soundInControl->stop();
            	DEBUG(3, Sound_Commander, "capture_start = false property")
            }
            for( int k = 0; k < propertySet.length(); k++ ) {
                if( strcmp(propertySet[k].id, props[i].id) == 0 ) {
                    propertySet[k].value = props[i].value;
                    break;
                }
            }
        }

    }
}

void Sound_Commander_i::ProcessData()
{
    DEBUG(3, Sound_Commander, "ProcessData() invoked")



    while(continue_processing())
    {
        /*insert code here to do work*/






    }
}

bool Sound_Commander_i::continue_processing()
{
	omni_mutex_lock l(processing_mutex);
	return thread_started;
}


