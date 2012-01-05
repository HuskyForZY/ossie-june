/****************************************************************************

Copyright 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE PacketResizer.

OSSIE PacketResizer is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE PacketResizer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE PacketResizer; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/


#include <string>
#include <iostream>
#include "PacketResizer.h"

PacketResizer_i::PacketResizer_i(const char *uuid, omni_condition *condition) :
        Resource_impl(uuid), component_running(condition)
{
    dataOut = new standardInterfaces_i::realChar_u("packet_out");
    dataIn = new standardInterfaces_i::realChar_p("packet_in");

//Create the thread for the writer's processing function
    processing_thread = new omni_thread(Run, (void *) this);

//Start the thread containing the writer's processing function
    processing_thread->start();

}

PacketResizer_i::~PacketResizer_i(void)
{
    delete dataOut;
    delete dataIn;
}

// Static function for omni thread
void PacketResizer_i::Run( void * data )
{
    ((PacketResizer_i*)data)->ProcessData();
}

CORBA::Object_ptr PacketResizer_i::getPort( const char* portName ) throw (
    CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    DEBUG(3, PacketResizer, "getPort() invoked with " << portName)

    CORBA::Object_var p;

    p = dataOut->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

    p = dataIn->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

    /*exception*/
    throw CF::PortSupplier::UnknownPort();
}

void PacketResizer_i::start() throw (CORBA::SystemException,
                                     CF::Resource::StartError)
{
    DEBUG(3, PacketResizer, "start() invoked")
}

void PacketResizer_i::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    DEBUG(3, PacketResizer, "stop() invoked")
}

void PacketResizer_i::releaseObject() throw (CORBA::SystemException,
        CF::LifeCycle::ReleaseError)
{
    DEBUG(3, PacketResizer, "releaseObject() invoked")

    component_running->signal();
}

void PacketResizer_i::initialize() throw (CF::LifeCycle::InitializeError,
        CORBA::SystemException)
{
    DEBUG(3, PacketResizer, "initialize() invoked")
}

void
PacketResizer_i::query (CF::Properties & configProperties)
throw (CORBA::SystemException, CF::UnknownProperties)
{
    if (configProperties.length () == 0) {
        configProperties.length (propertySet.length ());
        for (unsigned int i = 0; i < propertySet.length (); i++) {
            configProperties[i].id = CORBA::string_dup (propertySet[i].id);
            configProperties[i].value = propertySet[i].value;
        }

        return ;
    } else {
        for (unsigned int i = 0; i < configProperties.length(); i++) {
            for (unsigned int j=0; j < propertySet.length(); j++) {
                if ( strcmp(configProperties[i].id, propertySet[j].id) == 0 ) {
                    configProperties[i].value = propertySet[j].value;
                }
            }
        }
    }
}

void PacketResizer_i::configure(const CF::Properties& props)
throw (CORBA::SystemException,
       CF::PropertySet::InvalidConfiguration,
       CF::PropertySet::PartialConfiguration)
{
    static int init=0;
    DEBUG(3, PacketResizer, "configure() invoked")

    std::cout << "Component - PACKET RESIZER" << std::endl;
    std::cout << "props length : " << props.length() << std::endl;
    if (init == 0) {
        if ( props.length() <= 0 ) {
            std::cout << "PacketResizer: configure called with invalid \
                          props.length() - " << props.length() << std::endl;
            return;
        }
        std::cout << "initial configure call .." << std::endl;
        propertySet.length(props.length());
        for (unsigned int i=0; i < props.length(); i++) {
            std::cout << "Property Id : " << props[i].id << std::endl;
            propertySet[i].id = CORBA::string_dup(props[i].id);
            propertySet[i].value = props[i].value;
        }
        init++;
    }
    for (unsigned int i = 0; i <props.length(); i++) {
        std::cout << "Property id : " << props[i].id << std::endl;

        if (strcmp(props[i].id, "DCE:863201ca-0505-11de-b7e0-0000e80014f9") == 0) {
            //Packet_size
            CORBA::ULong simple_temp;
            props[i].value >>= simple_temp;
            Packet_size=simple_temp;
            std::cout <<"Packet_size="<< Packet_size<<std::endl;
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
        }
        /**else {
            // unknown property
            std::cerr << "ERROR: PacketResizer::configure() unknown property "<< props[i].id<< std::endl;
            throw CF::PropertySet::InvalidConfiguration();
        };**/
    }
}

void PacketResizer_i::ProcessData()
{
    DEBUG(3, PacketResizer, "ProcessData() invoked")
    PortTypes::CharSequence outdata;


    PortTypes::CharSequence *indata(NULL);
    CORBA::UShort indata_length;
    unsigned int dataoutN=0;
    unsigned int i=0;

    while (1) {
        DEBUG(4, PacketResizer, "Waiting for data")
        dataIn->getData(indata);

        indata_length = indata->length();

        DEBUG(4, PacketResizer, "In packet size="<<indata_length<<"received")
        DEBUG(4, PacketResizer, "DataoutN="<<dataoutN)


        outdata.length(Packet_size); //must define length of out

        for (i=0; i<indata_length; i++) {
            outdata[dataoutN]= (char)(*indata)[i];
            dataoutN++;

            if (dataoutN==Packet_size) {
                DEBUG(5, PacketResizer, "DataoutN="<<dataoutN<<" Packetsize="<<Packet_size<<" i="<<i)
                dataoutN=0;
                dataOut->pushPacket(outdata);
                DEBUG(5, PacketResizer, "Datapushed out DataoutN="<<dataoutN)
            }

        }

        dataIn->bufferEmptied();
        DEBUG(4, PacketResizer, "Input buffer emptied")

    }
}


