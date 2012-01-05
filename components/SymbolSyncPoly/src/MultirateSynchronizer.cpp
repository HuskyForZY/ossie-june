/****************************************************************************

Copyright 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE MultirateSynchronizer.

OSSIE MultirateSynchronizer is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE MultirateSynchronizer is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE MultirateSynchronizer; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/


#include <string>
#include <iostream>
#include "MultirateSynchronizer.h"

MultirateSynchronizer_i::MultirateSynchronizer_i(const char *uuid, omni_condition *condition) :
        Resource_impl(uuid), component_running(condition)
{
    dataIn_0 = new standardInterfaces_i::complexShort_p("IF_in");
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
    isConfigured_mod_scheme = false;

    UpdateTimingLoopFilterCoefficients(ALPHA_T_HI, BETA_T_HI);

}

MultirateSynchronizer_i::~MultirateSynchronizer_i(void)
{
    delete dataIn_0;
    delete dataOut_0;
}

// Static function for omni thread
void MultirateSynchronizer_i::Run( void * data )
{
    ((MultirateSynchronizer_i*)data)->ProcessData();
}

CORBA::Object_ptr MultirateSynchronizer_i::getPort( const char* portName ) throw (
    CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    DEBUG(3, MultirateSynchronizer, "getPort() invoked with " << portName)

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

void MultirateSynchronizer_i::start() throw (CORBA::SystemException,
        CF::Resource::StartError)
{
    DEBUG(3, MultirateSynchronizer, "start() invoked")
}

void MultirateSynchronizer_i::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    DEBUG(3, MultirateSynchronizer, "stop() invoked")
}

void MultirateSynchronizer_i::releaseObject() throw (CORBA::SystemException,
        CF::LifeCycle::ReleaseError)
{
    DEBUG(3, MultirateSynchronizer, "releaseObject() invoked")

    component_running->signal();
}

void MultirateSynchronizer_i::initialize() throw (CF::LifeCycle::InitializeError,
        CORBA::SystemException)
{
    DEBUG(3, MultirateSynchronizer, "initialize() invoked")
}

void
MultirateSynchronizer_i::query (CF::Properties & configProperties)
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

void MultirateSynchronizer_i::configure(const CF::Properties& props)
throw (CORBA::SystemException,
       CF::PropertySet::InvalidConfiguration,
       CF::PropertySet::PartialConfiguration)
{
    static int init=0;
    DEBUG(3, MultirateSynchronizer, "configure() invoked")

    std::cout << "Component - MULTIRATE SYNCHRONIZER" << std::endl;
    std::cout << "Props length = " << props.length() << std::endl;

    if (init == 0) {
        if ( props.length() <= 0 ) {
            std::cout << "Multirate Synchronizer: configure called with invalid \
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

        if (strcmp(props[i].id, "DCE:c104305d-27ee-44d8-ab29-8672e5ccb205") == 0) {
            // pulse shape
            const char * prop_str;
            props[i].value >>= prop_str;

            // Set appropriate pulse shape
            if ( strcmp(prop_str, "rrcos") == 0 ) {
                _pulseShape = "rrcos";
                for (unsigned int j=0; j < propertySet.length(); j++ ) {
                    if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                        propertySet[j].value = props[i].value;
                        break;
                    }
                }

            } else {
                // unknown pulse shape
                std::cerr << "ERROR: MultirateSynchronizer::configure() unknown pulse shape "
                          << prop_str << std::endl;
                throw CF::PropertySet::InvalidConfiguration();
            }
            isConfigured_pulseShape = true;

        } else if (strcmp(props[i].id, "DCE:ba19e325-46d0-48ec-9620-cedb38e3d822") == 0) {
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
        } else if (strcmp(props[i].id, "DCE:93c150e6-8bbd-4976-98a7-372254fd739f") == 0) {
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
        } else if (strcmp(props[i].id, "DCE:73c150e6-8bbd-4976-98a7-372254fd739f") == 0) {
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
        } else if (strcmp(props[i].id, "DCE:6dbe74a9-279d-4c70-9eed-47e27c98857f") == 0) {
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
        } else if (strcmp(props[i].id, "DCE:512cb885-789b-42cd-a5bb-c8ac53752269") == 0) {
            // mod_scheme
            const char * prop_str;
            props[i].value >>= prop_str;

            // Set appropriate modulation function
            if ( strcmp(prop_str, "bpsk") == 0 ) {
                SetModulationScheme(SigProc::BPSK);
            } else if ( strcmp(prop_str, "qpsk") == 0 ) {
                SetModulationScheme(SigProc::QPSK);
            } else if ( strcmp(prop_str, "8psk") == 0 ) {
                SetModulationScheme(SigProc::PSK8);
            } else if ( strcmp(prop_str, "16qam") == 0 ) {
                SetModulationScheme(SigProc::QAM16);
            } else if ( strcmp(prop_str, "4pam") == 0 ) {
                SetModulationScheme(SigProc::PAM4);
            } else {
                // unknown property
                std::cerr << "ERROR: MultirateSynchronizer::configure() unknown mod. scheme \""
                          << prop_str << "\"" << std::endl;
                throw CF::PropertySet::InvalidConfiguration();
            }
            isConfigured_mod_scheme = true;
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
        } else {
            // unknown property
            std::cerr << "ERROR: MultirateSynchronizer::configure() unknown property \""
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
        DEBUG(1, MultirateSynchronizer, "configuring filter bank for " <<
              _pulseShape << ", k=" << _k << ", m=" << _m << ", beta=" << _beta << ", Npfb=" << _Npfb)
        ConfigureFilterBank(_pulseShape, _k, _m, _beta, _Npfb);
    }
}


void MultirateSynchronizer_i::ProcessData()
{
    DEBUG(3, MultirateSynchronizer, "ProcessData() invoked")

    PortTypes::ShortSequence I_out_0, Q_out_0;

    PortTypes::ShortSequence *I_in_0(NULL), *Q_in_0(NULL);
    unsigned int I_in_0_length, Q_in_0_length, N_in(0), N_out(0);
    short * I_in(NULL);
    short * Q_in(NULL);
    short * I_out(NULL);
    short * Q_out(NULL);

    while ( true ) {
        DEBUG(7, MultirateSynchronizer, "iterating loop...")

        // Get data from port
        dataIn_0->getData(I_in_0, Q_in_0);

        DEBUG(7, MultirateSynchronizer, "Got " << I_in_0->length() << " samples")

        // Read input data length
        I_in_0_length = I_in_0->length();
        Q_in_0_length = Q_in_0->length();

        if ( I_in_0_length != Q_in_0_length ) {
            // Not sure how to handle multiple-length inputs
            std::cerr << "ERROR: MultirateSynchronizer_i::ProcessData()" << std::endl
                      << "  => Not sure how to handle multiple-length inputs" << std::endl;
            throw 0;
        } else {
            N_in = I_in_0_length;
        }

        /// \todo Initialize I_in and Q_in from CORBA sequence instead
        /// of copying values in loop
        DEBUG(7, MultirateSynchronizer, "Copying data from CORBA sequence...")
        /// \todo This is incredibly inefficient; need to manage memory better
        I_in = new short[N_in];
        Q_in = new short[N_in];

        I_out = new short[N_in];
        Q_out = new short[N_in];
        N_out = N_in;

        for (unsigned int ii=0; ii<N_in; ii++) {
            I_in[ii] = (*I_in_0)[ii];
            Q_in[ii] = (*Q_in_0)[ii];

            I_out[ii] = 0;
            Q_out[ii] = 0;
        }

        // Main signal processing algorithm
        DEBUG(7, MultirateSynchronizer, "Attempting to synchronize and decimate...")
        SynchronizeAndDecimate(
            I_in, Q_in, N_in,
            I_out, Q_out, N_out);

        DEBUG(7, MultirateSynchronizer, "Setting output length...")
        I_out_0.length(N_out);
        Q_out_0.length(N_out);

        /// \todo Initialize I_out_0 and Q_out_0 from array instead
        /// of copying values in loop
        DEBUG(7, MultirateSynchronizer, "Copying data to CORBA sequence...")
        for (unsigned int ii=0; ii<N_out; ii++) {
            I_out_0[ii] = I_out[ii];
            Q_out_0[ii] = Q_out[ii];
        }

        DEBUG(7, MultirateSynchronizer, "Emptying port buffer...")
        dataIn_0->bufferEmptied();

        DEBUG(7, MultirateSynchronizer, "calling pushPacket()...")
        dataOut_0->pushPacket(I_out_0, Q_out_0);

        /// \todo This is incredibly inefficient; need to manage memory better
        DEBUG(7, MultirateSynchronizer, "Deleting temp buffers...")
        delete [] I_in;
        delete [] Q_in;

        delete [] I_out;
        delete [] Q_out;
    }

}


