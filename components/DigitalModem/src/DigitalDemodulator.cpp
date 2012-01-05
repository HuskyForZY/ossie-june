/****************************************************************************

Copyright 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE DigitalDemodulator.

OSSIE DigitalDemodulator is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE DigitalDemodulator is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE DigitalDemodulator; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/


#include <string>
#include <iostream>
#include "DigitalDemodulator.h"

DigitalDemodulator_i::DigitalDemodulator_i(const char *uuid, omni_condition *condition) :
        Resource_impl(uuid), component_running(condition)
{
    dataIn_0 = new standardInterfaces_i::complexShort_p("SymbolsIn");
    dataOut_0 = new standardInterfaces_i::realChar_u("DataBitsOut");

//Create the thread for the writer's processing function
    processing_thread = new omni_thread(Run, (void *) this);

//Start the thread containing the writer's processing function
    processing_thread->start();

}

DigitalDemodulator_i::~DigitalDemodulator_i(void)
{
    delete dataIn_0;
    delete dataOut_0;
}

// Static function for omni thread
void DigitalDemodulator_i::Run( void * data )
{
    ((DigitalDemodulator_i*)data)->ProcessData();
}

CORBA::Object_ptr DigitalDemodulator_i::getPort( const char* portName ) throw (
    CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    DEBUG(3, DigitalDemodulator, "getPort() invoked with " << portName)

    CORBA::Object_var p;

    p = dataIn_0->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

    p = dataOut_0->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

// Requested port not recognized
    throw CF::PortSupplier::UnknownPort();
}

void DigitalDemodulator_i::start() throw (CORBA::SystemException,
        CF::Resource::StartError)
{
    DEBUG(3, DigitalDemodulator, "start() invoked")
}

void DigitalDemodulator_i::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    DEBUG(3, DigitalDemodulator, "stop() invoked")
}

void DigitalDemodulator_i::releaseObject() throw (CORBA::SystemException,
        CF::LifeCycle::ReleaseError)
{
    DEBUG(3, DigitalDemodulator, "releaseObject() invoked")

    component_running->signal();
}

void DigitalDemodulator_i::initialize() throw (CF::LifeCycle::InitializeError,
        CORBA::SystemException)
{
    DEBUG(3, DigitalDemodulator, "initialize() invoked")
}

void
DigitalDemodulator_i::query (CF::Properties & configProperties)
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

void DigitalDemodulator_i::configure(const CF::Properties& props)
throw (CORBA::SystemException,
       CF::PropertySet::InvalidConfiguration,
       CF::PropertySet::PartialConfiguration)
{
    static int init = 0;
    DEBUG(3, DigitalDemodulator, "configure() invoked")

    DEBUG(3, DigitalDemodulator, "props length : " << props.length());
    if (init == 0) {
        if ( props.length() <= 0 ) {
            std::cout << "DigitalDemodulator: configure called with invalid \
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
        DEBUG(3, DigitalDemodulator, "Property id : " << props[i].id);

        if (strcmp(props[i].id, "DCE:de4641d3-5c25-47d4-8d0b-89a069ac0248") == 0) {
            // Demodulation scheme
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
                std::cerr << "ERROR: DigitalDemodulator::configure() unknown mod. scheme "
                          << prop_str << std::endl;
                throw CF::PropertySet::InvalidConfiguration();
            }
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
            ///\todo catch exception thrown by DigitalDemodulatorDSP::ConfigureModulationScheme()

        } else {
            // unknown property
            std::cerr << "ERROR: DigitalDemodulator::configure() unknown property" << std::endl;
            throw CF::PropertySet::InvalidConfiguration();
        }
    }
}

void DigitalDemodulator_i::ProcessData()
{
    DEBUG(3, DigitalDemodulator, "ProcessData() invoked")

    PortTypes::CharSequence bits_out;

    PortTypes::ShortSequence *I_in(NULL), *Q_in(NULL);

    unsigned int N_in(0), N_out(0);

    unsigned int i;
    char * b(NULL);
    short * I(NULL), * Q(NULL);

    while ( true ) {
        // Get data from port

        dataIn_0->getData(I_in, Q_in);

        if ( ossieDebugLevel > 5 ) {
            std::cout << "DigitalDemodulator-metadata got data" << std::endl;

        }

        // Read input data length
        N_in = I_in->length();
        N_out = N_in * bitsPerSymbol;

        // Configure output data length
        bits_out.length(N_out);

        // Copy data
        ///\todo Copying data is incredibly inefficient; need to initialize
        /// CORBA array on buffer and vise versa
        b = new char[N_out];
        I = new short[N_in];
        Q = new short[N_in];

        for (i=0; i<N_in; i++) {
            I[i] = (*I_in)[i];
            Q[i] = (*Q_in)[i];
        }

        // Signal processing goes here
        DemodulateSequence(I, Q, N_in, b);

        for (i=0; i<N_out; i++) {
            bits_out[i] = b[i];
        }

        dataIn_0->bufferEmptied();
        dataOut_0->pushPacket(bits_out);

        delete [] b;
        delete [] I;
        delete [] Q;
    }

}


