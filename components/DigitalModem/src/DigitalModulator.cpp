/****************************************************************************

Copyright 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE DigitalModulator.

OSSIE DigitalModulator is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE DigitalModulator is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE DigitalModulator; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/


#include <string>
#include <iostream>
#include "DigitalModulator.h"

DigitalModulator_i::DigitalModulator_i(const char *uuid, omni_condition *condition) :
        Resource_impl(uuid), component_running(condition)
{
    bitsIn = new standardInterfaces_i::realChar_p("bitsIn");
    symbolsOut = new standardInterfaces_i::complexShort_u("symbolsOut");

//Create the thread for the writer's processing function
    processing_thread = new omni_thread(Run, (void *) this);

//Start the thread containing the writer's processing function
    processing_thread->start();

}

DigitalModulator_i::~DigitalModulator_i(void)
{
    delete bitsIn;
    delete symbolsOut;
}

// Static function for omni thread
void DigitalModulator_i::Run( void * data )
{
    ((DigitalModulator_i*)data)->ProcessData();
}

CORBA::Object_ptr DigitalModulator_i::getPort( const char* portName ) throw (
    CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    DEBUG(3, DigitalModulator, "getPort() invoked with " << portName)

    CORBA::Object_var p;

    p = symbolsOut->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

    p = bitsIn->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

// Requested port not recognized
    throw CF::PortSupplier::UnknownPort();
}

void DigitalModulator_i::start() throw (CORBA::SystemException,
                                        CF::Resource::StartError)
{
    DEBUG(3, DigitalModulator, "start() invoked")
}

void DigitalModulator_i::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    DEBUG(3, DigitalModulator, "stop() invoked")
}

void DigitalModulator_i::releaseObject() throw (CORBA::SystemException,
        CF::LifeCycle::ReleaseError)
{
    DEBUG(3, DigitalModulator, "releaseObject() invoked")

    component_running->signal();
}

void DigitalModulator_i::initialize() throw (CF::LifeCycle::InitializeError,
        CORBA::SystemException)
{
    DEBUG(3, DigitalModulator, "initialize() invoked")
}

void
DigitalModulator_i::query (CF::Properties & configProperties)
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


void DigitalModulator_i::configure(const CF::Properties& props)
throw (CORBA::SystemException,
       CF::PropertySet::InvalidConfiguration,
       CF::PropertySet::PartialConfiguration)
{
    static int init=0;
    DEBUG(3, DigitalModulator, "configure() invoked")

    DEBUG(3, DigitalModulator, "props length : " << props.length());

    if (init == 0) {
        if ( props.length() <= 0 ) {
            std::cout << "ChannelDemo: configure called with invalid \
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
        DEBUG(3, DigitalModulator, "Property id : " << props[i].id);

        if (strcmp(props[i].id, "DCE:cc5a494c-4c04-11dc-92c1-00123f63025f") == 0) {
            // Modulation scheme
            const char * prop_str;
            props[i].value >>= prop_str;

            // Set appropriate modulation function
            if ( strcmp(prop_str, "BPSK") == 0 ) {
                SetModulationScheme( SigProc::BPSK );
            } else if ( strcmp(prop_str, "QPSK") == 0 ) {
                SetModulationScheme( SigProc::QPSK );
            } else if ( strcmp(prop_str, "8PSK") == 0 ) {
                SetModulationScheme( SigProc::PSK8 );
            } else if ( strcmp(prop_str, "16QAM") == 0 ) {
                SetModulationScheme( SigProc::QAM16 );
            } else if ( strcmp(prop_str, "4PAM") == 0 ) {
                SetModulationScheme( SigProc::PAM4 );
            } else {
                // unknown property
                std::cerr << "ERROR: DigitalModulator::configure() unknown mod. scheme "
                          << prop_str << std::endl;
                throw CF::PropertySet::InvalidConfiguration();
            }
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
            ///\todo catch exception thrown by DigitalModulatorDSP::ConfigureModulationScheme()

        } else {
            // unknown property
            std::cerr << "ERROR: DigitalModulator::configure() unknown property" << std::endl;
            throw CF::PropertySet::InvalidConfiguration();
        }
    }
}


void DigitalModulator_i::ProcessData()
{
    DEBUG(3, DigitalModulator, "ProcessData() invoked")

    PortTypes::CharSequence *bits_in;

    PortTypes::ShortSequence I_out, Q_out;
    unsigned int N_in(0), N_out(0);

    unsigned int i;
    char * b(NULL);
    short * I(NULL), * Q(NULL);

    while ( true ) {
        // Get data from port
        //bitsIn->getData(bits_in, metadata);
        bitsIn->getData(bits_in);


        if ( ossieDebugLevel > 5 ) {
            std::cout << "DigitalModulator got data" << std::endl;

        }



        // Read input data length
        N_in = bits_in->length();
        N_out = N_in / bitsPerSymbol;
        N_out += (N_in % bitsPerSymbol == 0) ? 0 : 1;
        ///\todo: check that N_out is proper length

        // Configure output data length
        I_out.length(N_out);
        Q_out.length(N_out);

        // Copy data
        ///\todo Copying data is incredibly inefficient; need to initialize
        /// CORBA array on buffer and vise versa
        b = new char[N_in];
        I = new short[N_out];
        Q = new short[N_out];

        for (i=0; i<N_in; i++)
            b[i] = (*bits_in)[i];

        // Signal processing goes here
        ModulateSequence(b, N_in, I, Q);

        for (i=0; i<N_out; i++) {
            I_out[i] = I[i];
            Q_out[i] = Q[i];
        }

        bitsIn->bufferEmptied();

        symbolsOut->pushPacket(I_out, Q_out);

        delete [] b;
        delete [] I;
        delete [] Q;
    }

}


