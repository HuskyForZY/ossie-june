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
	DEBUG(3, Sound_Commander, "constructor() invoked")
	audioOutControl = new standardInterfaces_i::audioOutControl_u("soundOutControl");
	audioInControl = new standardInterfaces_i::audioInControl_u("soundInControl");

}

Sound_Commander_i::~Sound_Commander_i(void)
{
    delete audioOutControl;
    delete audioInControl;
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

    p = audioOutControl->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

    p = audioInControl->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

    /*exception*/
    throw CF::PortSupplier::UnknownPort();
}

void Sound_Commander_i::start() throw (CORBA::SystemException,
    CF::Resource::StartError)
{
    DEBUG(3, Sound_Commander, "start() invoked")
	audioInControl->start();
}

void Sound_Commander_i::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    DEBUG(3, Sound_Commander, "stop() invoked")
	audioInControl->stop();

}

void Sound_Commander_i::releaseObject() throw (CORBA::SystemException,
    CF::LifeCycle::ReleaseError)
{
    DEBUG(3, Sound_Commander, "releaseObject() invoked")

}

void Sound_Commander_i::initialize() throw (CF::LifeCycle::InitializeError,
    CORBA::SystemException)
{
    DEBUG(3, Sound_Commander, "initialize() invoked")
}

void Sound_Commander_i::query( CF::Properties & configProperties ) throw (CORBA::SystemException, CF::UnknownProperties)
{
	DEBUG(3, Sound_Commander, "query() invoked")
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
            std::cout << "Sound_Commander_i configure called with invalid props.length - " << props.length() << std::endl;
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

        if (strcmp(props[i].id, "DCE:dce70c7c-3d8d-11e1-a802-000c29c0b7da") == 0)
        {
            CORBA::Long simple_temp;
            props[i].value >>= simple_temp;
            play_sample_rate = simple_temp;
            audioOutControl->set_sample_rate(play_sample_rate);
            for( int k = 0; k < propertySet.length(); k++ ) {
                if( strcmp(propertySet[k].id, props[i].id) == 0 ) {
                    propertySet[k].value = props[i].value;
                    break;
                }
            }
        }

        if (strcmp(props[i].id, "DCE:e9fa6e5e-3d8d-11e1-89f9-000c29c0b7da") == 0)
        {
            CORBA::Short simple_temp;
            props[i].value >>= simple_temp;
            play_channels = simple_temp;
            audioOutControl->set_channels(play_channels);
            for( int k = 0; k < propertySet.length(); k++ ) {
                if( strcmp(propertySet[k].id, props[i].id) == 0 ) {
                    propertySet[k].value = props[i].value;
                    break;
                }
            }
        }

        if (strcmp(props[i].id, "DCE:f9e77262-3d8d-11e1-82c5-000c29c0b7da") == 0)
        {
            const char* simple_temp;
            props[i].value >>= CORBA::Any::to_string(simple_temp,0);
            play_destination = simple_temp;
            if(play_destination.substr(play_destination.length()-3,3) == "wav"){
            	audioOutControl->set_file_name(play_destination.c_str());
            	audioOutControl->set_connector(standardInterfaces::audioOutControl::otFile);

            }
            else{
            	audioOutControl->set_network_address(play_destination.c_str());
            	audioOutControl->set_connector(standardInterfaces::audioOutControl::otNet);
            }

            for( int k = 0; k < propertySet.length(); k++ ) {
                if( strcmp(propertySet[k].id, props[i].id) == 0 ) {
                    propertySet[k].value = props[i].value;
                    break;
                }
            }
        }

        if (strcmp(props[i].id, "DCE:1556893e-3d8e-11e1-ac15-000c29c0b7da") == 0)
        {
            CORBA::Boolean simple_temp;
            props[i].value >>= simple_temp;
            play_mute = simple_temp;
            audioOutControl->mute(play_mute);
            for( int k = 0; k < propertySet.length(); k++ ) {
                if( strcmp(propertySet[k].id, props[i].id) == 0 ) {
                    propertySet[k].value = props[i].value;
                    break;
                }
            }
        }

        if (strcmp(props[i].id, "DCE:8ecb6d7e-3d8f-11e1-8cc1-000c29c0b7da") == 0)
        {
            CORBA::Long simple_temp;
            props[i].value >>= simple_temp;
            capture_sample_rate = simple_temp;
            audioInControl->set_sample_rate(capture_sample_rate);
            for( int k = 0; k < propertySet.length(); k++ ) {
                if( strcmp(propertySet[k].id, props[i].id) == 0 ) {
                    propertySet[k].value = props[i].value;
                    break;
                }
            }
        }

        if (strcmp(props[i].id, "DCE:99257a26-3d8f-11e1-92cc-000c29c0b7da") == 0)
        {
            CORBA::Short simple_temp;
            props[i].value >>= simple_temp;
            capture_channels = simple_temp;
            audioInControl->set_channels(capture_channels);
            for( int k = 0; k < propertySet.length(); k++ ) {
                if( strcmp(propertySet[k].id, props[i].id) == 0 ) {
                    propertySet[k].value = props[i].value;
                    break;
                }
            }
        }

        if (strcmp(props[i].id, "DCE:07a8ba48-3d96-11e1-a462-000c29c0b7da") == 0)
        {
            const char* simple_temp;
            props[i].value >>= CORBA::Any::to_string(simple_temp,0);
            capture_source = simple_temp;
            if(capture_source.substr(capture_source.length()-3,3) == "wav"){
            	std::cout<<"file"<<std::endl;
            	audioInControl->set_file_name(capture_source.c_str());
            	audioInControl->set_connector(standardInterfaces::audioInControl::itFile);
            }
            else{
            	std::cout<<"net"<<std::endl;
            	audioInControl->set_network_address(capture_source.c_str());
            	audioInControl->set_connector(standardInterfaces::audioInControl::itNet);
            }
            for( int k = 0; k < propertySet.length(); k++ ) {
                if( strcmp(propertySet[k].id, props[i].id) == 0 ) {
                    propertySet[k].value = props[i].value;
                    break;
                }
            }
        }

        if (strcmp(props[i].id, "DCE:1b757606-3d96-11e1-95fc-000c29c0b7da") == 0)
        {
            CORBA::Boolean simple_temp;
            props[i].value >>= simple_temp;
            capture_start = simple_temp;
            if(capture_start)
            	audioInControl->start();
            else
            	audioInControl->stop();
            for( int k = 0; k < propertySet.length(); k++ ) {
                if( strcmp(propertySet[k].id, props[i].id) == 0 ) {
                    propertySet[k].value = props[i].value;
                    break;
                }
            }
        }

        if (strcmp(props[i].id, "DCE:24c0655e-3d96-11e1-94bd-000c29c0b7da") == 0)
        {
            CORBA::Long simple_temp;
            props[i].value >>= simple_temp;
            capture_size = simple_temp;
            audioInControl->set_frame_size(capture_size);
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


