/****************************************************************************

Copyright 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE SymbolSyncPoly.

OSSIE SymbolSyncPoly is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE SymbolSyncPoly is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE SymbolSyncPoly; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/


#include <string>
#include <iostream>
#include "SymbolSyncPoly.h"

#undef SSP_LOGGING

SymbolSyncPoly_i::SymbolSyncPoly_i(const char *uuid, omni_condition *condition) :
        Resource_impl(uuid), component_running(condition)
{
    dataIn_0 = new standardInterfaces_i::complexShort_p("baseband_in");
    dataOut_0 = new standardInterfaces_i::complexShort_u("symbols_out");


//Create the thread for the writer's processing function
    processing_thread = new omni_thread(Run, (void *) this);

//Start the thread containing the writer's processing function
    processing_thread->start();

    isConfigured_pulseShape = false;
    isConfigured_k          = false;
    isConfigured_m          = false;
    isConfigured_beta       = false;
    isConfigured_Npfb       = false;

    UpdateTimingLoopFilterCoefficients(0.002f, 1.00125f);

}

SymbolSyncPoly_i::~SymbolSyncPoly_i(void)
{
    delete dataIn_0;
    delete dataOut_0;
}

// Static function for omni thread
void SymbolSyncPoly_i::Run( void * data )
{
    ((SymbolSyncPoly_i*)data)->ProcessData();
}

CORBA::Object_ptr SymbolSyncPoly_i::getPort( const char* portName ) throw (
    CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    DEBUG(3, SymbolSyncPoly, "getPort() invoked with " << portName)

    CORBA::Object_var p;

    p = dataIn_0->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

    p = dataOut_0->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

    /*exception*/
    throw CF::PortSupplier::UnknownPort();
}

void SymbolSyncPoly_i::start() throw (CORBA::SystemException,
                                      CF::Resource::StartError)
{
    DEBUG(3, SymbolSyncPoly, "start() invoked")
}

void SymbolSyncPoly_i::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    DEBUG(3, SymbolSyncPoly, "stop() invoked")
}

void SymbolSyncPoly_i::releaseObject() throw (CORBA::SystemException,
        CF::LifeCycle::ReleaseError)
{
    DEBUG(3, SymbolSyncPoly, "releaseObject() invoked")

    component_running->signal();
}

void SymbolSyncPoly_i::initialize() throw (CF::LifeCycle::InitializeError,
        CORBA::SystemException)
{
    DEBUG(3, SymbolSyncPoly, "initialize() invoked")
}

void
SymbolSyncPoly_i::query (CF::Properties & configProperties)
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

void SymbolSyncPoly_i::configure(const CF::Properties& props)
throw (CORBA::SystemException,
       CF::PropertySet::InvalidConfiguration,
       CF::PropertySet::PartialConfiguration)
{
    static int init = 0;
    DEBUG(3, SymbolSyncPoly, "configure() invoked")

    std::cout << "Component - SYMBOL SYNC POLY" << std::endl;
    std::cout << "Props length = " << props.length() << std::endl;

    if (init == 0) {
        if ( props.length() <= 0 ) {
            std::cout << "SymbolSyncPoly: configure called with invalid \
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
        init = 1;
    }
    for (unsigned int i = 0; i <props.length(); i++) {
        std::cout << "Property id : " << props[i].id << std::endl;

        if (strcmp(props[i].id, "DCE:0bb52b63-3a66-4aa6-afa9-d0417318c001") == 0) {
            // pulse shape
            const char * prop_str;
            props[i].value >>= prop_str;

            // Set appropriate pulse shape
            if ( strcmp(prop_str, "rrcos") == 0 ) {
                _pulseShape = "rrcos";
            } else {
                // unknown pulse shape
                std::cerr << "ERROR: SymbolSyncPoly::configure() unknown pulse shape "
                          << prop_str << std::endl;
                throw CF::PropertySet::InvalidConfiguration();
            }
            isConfigured_pulseShape = true;
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }

        } else if (strcmp(props[i].id, "DCE:c6f2eba5-e69f-4397-a75d-ae0450a7699b") == 0) {
            // k : samples per symbol
            CORBA::UShort simple_temp;
            props[i].value >>= simple_temp;
            _k = simple_temp;
            isConfigured_k = true;
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }

        } else if (strcmp(props[i].id, "DCE:08f16d2d-f69d-48eb-a558-3e93b2e481d1") == 0) {
            // m : symbol delay
            CORBA::UShort simple_temp;
            props[i].value >>= simple_temp;
            _m = simple_temp;
            isConfigured_m = true;
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }

        } else if (strcmp(props[i].id, "DCE:abe26dd4-ca5d-40ff-8141-093b56ddce16") == 0) {
            // beta : excess bandwidth factor
            CORBA::Float simple_temp;
            props[i].value >>= simple_temp;
            _beta = simple_temp;
            isConfigured_beta = true;
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }

        } else if (strcmp(props[i].id, "DCE:d8f0cbea-a3ba-416f-ada3-16e960938544") == 0) {
            // Npfb : number of filters in bank
            CORBA::UShort simple_temp;
            props[i].value >>= simple_temp;
            _Npfb = simple_temp;
            isConfigured_Npfb = true;
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }

        } else {
            // unknown property
            std::cerr << "ERROR: SymbolSyncPoly::configure() unknown property \""
                      << props[i].id << "\"" << std::endl;
            throw CF::PropertySet::InvalidConfiguration();
        }
    }
// Once all properties have been configured, create the filter bank
    if ( isConfigured_pulseShape    &&
            isConfigured_k             &&
            isConfigured_m             &&
            isConfigured_beta          &&
            isConfigured_Npfb ) {
        DEBUG(2, SymbolSyncPoly, "configuring filter bank for " <<
              _pulseShape << ", k=" << _k << ", m=" << _m << ", beta=" << _beta << ", Npfb=" << _Npfb)
        ConfigureFilterBank(_pulseShape, _k, _m, _beta, _Npfb);
    }
}

void SymbolSyncPoly_i::ProcessData()
{
    DEBUG(3, SymbolSyncPoly, "ProcessData() invoked")

    PortTypes::ShortSequence I_out_0, Q_out_0;

    PortTypes::ShortSequence *I_in_0(NULL), *Q_in_0(NULL);
    unsigned int I_in_0_length, Q_in_0_length, N_in(0), N_out(0);
    short * I_in(NULL);
    short * Q_in(NULL);
    short * I_out(NULL);
    short * Q_out(NULL);

#ifdef SSP_LOGGING
    std::ofstream input_log;
    input_log.open("ssp_input_log.dat");

    std::ofstream output_log;
    output_log.open("ssp_output_log.dat");
#endif

    while ( true ) {
        DEBUG(7, SymbolSyncPoly, "iterating loop...")

        // Get data from port
        dataIn_0->getData(I_in_0, Q_in_0);

        DEBUG(7, SymbolSyncPoly, "Got " << I_in_0->length() << " samples")



        // Read input data length
        I_in_0_length = I_in_0->length();
        Q_in_0_length = Q_in_0->length();

        if ( I_in_0_length != Q_in_0_length ) {
            // Not sure how to handle multiple-length inputs
            std::cerr << "ERROR: SymbolSyncPoly_i::ProcessData()" << std::endl
                      << "  => Not sure how to handle multiple-length inputs" << std::endl;
            throw 0;
        } else {
            N_in = I_in_0_length;
        }

        /// \todo Initialize I_in and Q_in from CORBA sequence instead
        /// of copying values in loop
        DEBUG(7, SymbolSyncPoly, "Copying data from CORBA sequence...")
        /// \todo This is incredibly inefficient; need to manage memory better
        I_in = new short[N_in];
        Q_in = new short[N_in];

        I_out = new short[N_in];
        Q_out = new short[N_in];
        N_out = N_in;

        for (unsigned int ii=0; ii<N_in; ii++) {
            I_in[ii] = (*I_in_0)[ii];
            Q_in[ii] = (*Q_in_0)[ii];
#ifdef SSP_LOGGING
            input_log      << (*I_in_0)[ii] << "  " << (*Q_in_0)[ii] << std::endl;
#endif

            I_out[ii] = 0;
            Q_out[ii] = 0;
        }

        // Main signal processing algorithm
        DEBUG(7, SymbolSyncPoly, "Attempting to synchronize and decimate...")
        SynchronizeAndDecimate(
            I_in, Q_in, N_in,
            I_out, Q_out, N_out);

        DEBUG(7, SymbolSyncPoly, "Setting output length...")
        I_out_0.length(N_out);
        Q_out_0.length(N_out);

        /// \todo Initialize I_out_0 and Q_out_0 from array instead
        /// of copying values in loop
        DEBUG(7, SymbolSyncPoly, "Copying data (N=" << N_out << ") to CORBA sequence...")
        for (unsigned int ii=0; ii<N_out; ii++) {
            I_out_0[ii] = I_out[ii];
            Q_out_0[ii] = Q_out[ii];
#ifdef SSP_LOGGING
            output_log      << I_out_0[ii] << "  " << Q_out_0[ii] << std::endl;
#endif
        }

        DEBUG(7, SymbolSyncPoly, "Emptying port buffer...")
        dataIn_0->bufferEmptied();

        DEBUG(7, SymbolSyncPoly, "calling pushPacket()...")
        dataOut_0->pushPacket(I_out_0, Q_out_0);

        /// \todo This is incredibly inefficient; need to manage memory better
        DEBUG(7, SymbolSyncPoly, "Deleting temp buffers...")
        delete [] I_in;
        delete [] Q_in;

        delete [] I_out;
        delete [] Q_out;
    }

#ifdef SSP_LOGGING
    input_log.close();
    output_log.close();
#endif


}


