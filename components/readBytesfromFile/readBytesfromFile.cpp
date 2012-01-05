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
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include "readBytesfromFile.h"

#define INPUT_FILE "/sdr/input"

readBytesfromFile_i::readBytesfromFile_i(const char *uuid, omni_condition *condition) :
    Resource_impl(uuid), component_running(condition)
{
    dataOut_0 = new standardInterfaces_i::realChar_u("outputBits");
    start();
}

readBytesfromFile_i::~readBytesfromFile_i(void)
{
    delete dataOut_0;
}

// Static function for omni thread
void readBytesfromFile_i::Run( void * data )
{
    ((readBytesfromFile_i*)data)->ProcessData();
}

CORBA::Object_ptr readBytesfromFile_i::getPort( const char* portName ) throw (
    CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    DEBUG(3, readBytesfromFile, "getPort() invoked with " << portName)

    CORBA::Object_var p;

    p = dataOut_0->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

    /*exception*/
    throw CF::PortSupplier::UnknownPort();
}

void readBytesfromFile_i::start() throw (CORBA::SystemException,
    CF::Resource::StartError)
{
    DEBUG(3, readBytesfromFile, "start() invoked")
	omni_mutex_lock  l(processing_mutex);
	if( false == thread_started )
	{
		thread_started = true;
		// Create the thread for the writer's processing function
		processing_thread = new omni_thread(Run, (void *) this);

		// Start the thread containing the writer's processing function
		processing_thread->start();

		/// open destination file if not opened
		if(!inputFileOpen){
			inputFD = open(INPUT_FILE, O_RDONLY);
			if(inputFD < 0){
				perror("\n Error during opening input");
				abort();
			}
			inputFileOpen = true;
		}
	}
}

void readBytesfromFile_i::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    DEBUG(3, readBytesfromFile, "stop() invoked")
	omni_mutex_lock l(processing_mutex);
	thread_started = false;

	/// close destination file if not opened
	if(inputFileOpen){
		close(inputFD);
		inputFileOpen = false;
	}
}

void readBytesfromFile_i::releaseObject() throw (CORBA::SystemException,
    CF::LifeCycle::ReleaseError)
{
    DEBUG(3, readBytesfromFile, "releaseObject() invoked")

    component_running->signal();
}

void readBytesfromFile_i::initialize() throw (CF::LifeCycle::InitializeError,
    CORBA::SystemException)
{
    DEBUG(3, readBytesfromFile, "initialize() invoked")

	inputFileOpen = false;
}

void readBytesfromFile_i::query( CF::Properties & configProperties ) throw (CORBA::SystemException, CF::UnknownProperties)
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

void readBytesfromFile_i::configure(const CF::Properties& props)
throw (CORBA::SystemException,
    CF::PropertySet::InvalidConfiguration,
    CF::PropertySet::PartialConfiguration)
{
    DEBUG(3, readBytesfromFile, "configure() invoked")

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

        if (strcmp(props[i].id, "DCE:eb12dbd0-078f-11e0-a6e8-0024bec489f8") == 0)
        {
            CORBA::ULong simple_temp;
            props[i].value >>= simple_temp;
            simple_0_value = simple_temp;
            for( int k = 0; k < propertySet.length(); k++ ) {
                if( strcmp(propertySet[k].id, props[i].id) == 0 ) {
                    propertySet[k].value = props[i].value;
                    break;
                }
            }
        }

    }

    /// open destination file if not opened
    if(!inputFileOpen){
    	inputFD = open(INPUT_FILE, O_RDONLY);
    	if(inputFD < 0){
    		perror("\n Error during opening input");
    		abort();
    	}
    	inputFileOpen = true;
    }
}

void readBytesfromFile_i::ProcessData()
{
    DEBUG(3, readBytesfromFile, "ProcessData() invoked")

	PortTypes::CharSequence I_out_0;

    unsigned char byte;

    while(continue_processing())
    {
    	// output length is set from the configuration mask
    	std::cout << "In readbytesfromfile" << std::endl;
        I_out_0.length(simple_0_value);

        for(int i = 0; i < simple_0_value;){
        	int nbyte = read(inputFD, &byte, sizeof(char));						///< read one byte
        	if (nbyte == 0){													///< EOF reached
        		lseek(inputFD,0,SEEK_SET);   									///< set the file selector to the beginning
        		read(inputFD, &byte, sizeof(char));
        	}
        	for(int j=7; j >= 0; --j){											///< transform each byte in a sequence of 8 byte
        		if((byte & (1 << j)) > 0)
        			I_out_0[i] = 1;
        		else
        			I_out_0[i] = 0;
        		i++;
        	}
        }

        dataOut_0->pushPacket(I_out_0);
    }
}

bool readBytesfromFile_i::continue_processing()
{
	omni_mutex_lock l(processing_mutex);
	return thread_started;
}


