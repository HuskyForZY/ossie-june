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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include "writeBytestoFile.h"

#define OUTPUT_FILE "/sdr/output.byte"

writeBytestoFile_i::writeBytestoFile_i(const char *uuid, omni_condition *condition) :
    Resource_impl(uuid), component_running(condition)
{
    dataIn_0 = new standardInterfaces_i::realChar_p("inputBits");
    start();
}

writeBytestoFile_i::~writeBytestoFile_i(void)
{
    delete dataIn_0;
}

// Static function for omni thread
void writeBytestoFile_i::Run( void * data )
{
    ((writeBytestoFile_i*)data)->ProcessData();
}

CORBA::Object_ptr writeBytestoFile_i::getPort( const char* portName ) throw (
    CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    DEBUG(3, writeBytestoFile, "getPort() invoked with " << portName)

    CORBA::Object_var p;

    p = dataIn_0->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

    /*exception*/
    throw CF::PortSupplier::UnknownPort();
}

void writeBytestoFile_i::start() throw (CORBA::SystemException,
    CF::Resource::StartError)
{
    DEBUG(3, writeBytestoFile, "start() invoked")
	omni_mutex_lock  l(processing_mutex);
	if( false == thread_started )
	{
		thread_started = true;
		// Create the thread for the writer's processing function
		processing_thread = new omni_thread(Run, (void *) this);

		// Start the thread containing the writer's processing function
		processing_thread->start();

		/// open destination file if not opened
		if(!destinationFileOpen){
			destinationFD = open(OUTPUT_FILE, O_RDWR|O_APPEND, S_IRWXU|S_IROTH);
			destinationFileOpen = true;
		}
	}
}

void writeBytestoFile_i::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    DEBUG(3, writeBytestoFile, "stop() invoked")
	omni_mutex_lock l(processing_mutex);
	thread_started = false;
	/// close destination file if not opened
	if(destinationFileOpen){
		close(destinationFD);
		destinationFileOpen = false;
	}
}

void writeBytestoFile_i::releaseObject() throw (CORBA::SystemException,
    CF::LifeCycle::ReleaseError)
{
    DEBUG(3, writeBytestoFile, "releaseObject() invoked")

    component_running->signal();
}

void writeBytestoFile_i::initialize() throw (CF::LifeCycle::InitializeError,
    CORBA::SystemException)
{
    DEBUG(3, writeBytestoFile, "initialize() invoked")
	destinationFileOpen = false;
}

void writeBytestoFile_i::query( CF::Properties & configProperties ) throw (CORBA::SystemException, CF::UnknownProperties)
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

void writeBytestoFile_i::configure(const CF::Properties& props)
throw (CORBA::SystemException,
    CF::PropertySet::InvalidConfiguration,
    CF::PropertySet::PartialConfiguration)
{
    DEBUG(3, writeBytestoFile, "configure() invoked")

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

    /// open destination file if not opened
    if(!destinationFileOpen){
    	destinationFD = open(OUTPUT_FILE, O_CREAT|O_RDWR, S_IRWXU|S_IROTH);
    	destinationFileOpen = true;
    }

}

void writeBytestoFile_i::ProcessData()
{
    DEBUG(3, writeBytestoFile, "ProcessData() invoked")

    PortTypes::CharSequence *I_in_0(NULL);
    CORBA::UShort I_in_0_length;

    while(continue_processing()){
    	std::cout << "In writebytestofile" << std::endl;
    	dataIn_0->getData(I_in_0);

        I_in_0_length = I_in_0->length();

        unsigned char value = 0;
        char actualByte;
        int j = 7;
        /// takes 8 byte from input, rebuilds one byte and writes it in the output file
        for(unsigned int i = 0; i < I_in_0_length; ++i,--j){
        	actualByte = (*I_in_0)[i];
        	value |= (actualByte << (j));
        	if(j == 0){
        		write(destinationFD, &value, sizeof(char));		///< writes byte on output file
        		value = 0;
        		j = 8;
        	}
        }
        dataIn_0->bufferEmptied();
    }

}

bool writeBytestoFile_i::continue_processing()
{
	omni_mutex_lock l(processing_mutex);
	return thread_started;
}


