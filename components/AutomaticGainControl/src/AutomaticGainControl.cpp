/****************************************************************************

Copyright 2006 Virginia Polytechnic Institute and State University

This file is part of the OSSIE AutomaticGainControl.

OSSIE AutomaticGainControl is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE AutomaticGainControl is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE AutomaticGainControl; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/


#include <string>
#include <iostream>
#include "AutomaticGainControl.h"

AutomaticGainControl_i::AutomaticGainControl_i(const char *uuid, omni_condition *condition) : Resource_impl(uuid), component_running(condition)
{
    dataOut_0 = new standardInterfaces_i::complexShort_u("data_out");
    dataIn_0 = new standardInterfaces_i::complexShort_p("data_in");

//Create the thread for the writer's processing function
    processing_thread = new omni_thread(run, (void *) this);

//Start the thread containing the writer's processing function
    processing_thread->start();
}

AutomaticGainControl_i::~AutomaticGainControl_i(void)
{
    delete dataOut_0;
    delete dataIn_0;
}

void AutomaticGainControl_i::run(void * data)
{
    ((AutomaticGainControl_i*) data)->run_loop();
}

CORBA::Object_ptr AutomaticGainControl_i::getPort( const char* portName ) throw (CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    DEBUG(3, AutomaticGainControl, "getPort() invoked with : " << portName)
    CORBA::Object_var p;

    p = dataOut_0->getPort(portName);
    if (!CORBA::is_nil(p))
        return p._retn();

    p = dataIn_0->getPort(portName);
    if (!CORBA::is_nil(p))
        return p._retn();
    /*exception*/
    throw CF::PortSupplier::UnknownPort();
}

void AutomaticGainControl_i::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    DEBUG(3, AutomaticGainControl, "start invoked()")
}

void AutomaticGainControl_i::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    DEBUG(3, AutomaticGainControl, "stop invoked()")
}

void AutomaticGainControl_i::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{
    DEBUG(3, AutomaticGainControl, "releaseObject invoked()")

    component_running->signal();
}

void AutomaticGainControl_i::initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException)
{
    DEBUG(3, AutomaticGainControl, "initialize invoked()")
}

void AutomaticGainControl_i::query(CF::Properties & configProperties)
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

void AutomaticGainControl_i::configure(const CF::Properties& props) throw (CORBA::SystemException, CF::PropertySet::InvalidConfiguration, CF::PropertySet::PartialConfiguration)
{
    static int init = 0;

    CORBA::Float simple_temp;

    DEBUG(3, AutomaticGainControl, "configure() invoked")

    DEBUG(3, AutomaticGainControl, "props length : " << props.length())

    if (init == 0) {
        if ( props.length() <= 0 ) {
            std::cout << "AutomaticGainControl: configure called with invalid props.length() - " << props.length() << std::endl;
            return;
        }

        propertySet.length(props.length());
        for (unsigned int i=0; i < props.length(); i++) {
            propertySet[i].id = CORBA::string_dup(props[i].id);
            propertySet[i].value = props[i].value;
        }
        init++;
    }

    for (unsigned int i=0; i < props.length(); i++) {
        std::cout << "Property Id : " << props[i].id << std::endl;
        if (strcmp(props[i].id, "DCE:aaf97fa0-d184-4d88-9954-3a1334c73d6d") == 0) {
            // energy_lo
            props[i].value >>= simple_temp;
            omni_mutex_lock oml(accessPrivateData);
            energy_lo = simple_temp;
            // Update value of this property in propertySet also
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
            DEBUG(3, AutomaticGainControl, "prop (energy_lo): " << simple_temp)

        } else if (strcmp(props[i].id, "DCE:346e17c9-6678-483a-bffb-1909c64bddc0") == 0) {
            // energy_hi
            props[i].value >>= simple_temp;
            omni_mutex_lock oml(accessPrivateData);
            energy_hi = simple_temp;
            // Update value of this property in propertySet also
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
            DEBUG(3, AutomaticGainControl, "prop (energy_hi): " << simple_temp)

        } else if (strcmp(props[i].id, "DCE:4608b943-4fe2-49df-91fb-afa287b609d4") == 0) {
            // k_attack
            props[i].value >>= simple_temp;
            omni_mutex_lock oml(accessPrivateData);
            k_attack = simple_temp;
            // Update value of this property in propertySet also
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
            DEBUG(3, AutomaticGainControl, "prop (k_attack): " << simple_temp)

        } else if (strcmp(props[i].id, "DCE:491ec3de-ed45-48af-a6fc-ca2d6465e136") == 0) {
            // k_release
            props[i].value >>= simple_temp;
            omni_mutex_lock oml(accessPrivateData);
            k_release = simple_temp;
            // Update value of this property in propertySet also
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
            DEBUG(3, AutomaticGainControl, "prop (k_release): " << simple_temp)

        } else if (strcmp(props[i].id, "DCE:312f63fe-709a-4217-933b-c584c8d6a9bb") == 0) {
            // g_min
            props[i].value >>= simple_temp;
            omni_mutex_lock oml(accessPrivateData);
            g_min = simple_temp;
            // Update value of this property in propertySet also
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
            DEBUG(3, AutomaticGainControl, "prop (g_min): " << simple_temp)

        } else if (strcmp(props[i].id, "DCE:8357ee0d-2417-46d9-8475-2e5778d797e4") == 0) {
            // g_max
            props[i].value >>= simple_temp;
            omni_mutex_lock oml(accessPrivateData);
            g_max = simple_temp;
            // Update value of this property in propertySet also
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
            DEBUG(3, AutomaticGainControl, "prop (g_max): " << simple_temp)

        } else if (strcmp(props[i].id, "DCE:b9b72ec8-d0bd-4060-b356-dcc6b0809e65") == 0) {
            // rssi_pass
            props[i].value >>= simple_temp;
            omni_mutex_lock oml(accessPrivateData);
            rssi_pass = simple_temp;
            // Update value of this property in propertySet also
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
            DEBUG(3, AutomaticGainControl, "prop (rssi_pass): " << simple_temp)

        } else {
            // unknown property
            std::cerr << "ERROR: SymbolSyncPoly::configure() unknown property \""
                      << props[i].id << "\"" << std::endl;
            throw CF::PropertySet::InvalidConfiguration();
        }
    }
}


void AutomaticGainControl_i::run_loop()
{
    DEBUG(3, AutomaticGainControl, "run_loop() thread started")

    PortTypes::ShortSequence I_out, Q_out;

    PortTypes::ShortSequence One_out;

    PortTypes::ShortSequence *I_in(NULL), *Q_in(NULL);
//CORBA::UShort I_in_length, Q_in_length;

    unsigned int i, N;
    short I, Q;

    SigProc::AutomaticGainControl * agc;

    agc = new SigProc::AutomaticGainControl();
    agc->SetValues(
        energy_lo,
        energy_hi,
        k_attack,
        k_release,
        g_min,
        g_max
    );

    while ( true ) {
        /// get input data
        dataIn_0->getData(I_in, Q_in);

        // overwrite agc values with properties
        // lock mutex for asynchronous configure() invocation
        {
            omni_mutex_lock oml(accessPrivateData);
            agc->SetValues(
                energy_lo,
                energy_hi,
                k_attack,
                k_release,
                g_min,
                g_max
            );
        }

        N = I_in->length();

        I_out.length(N); // define output length
        Q_out.length(N); // define output length
        One_out.length(N);

        /// apply gain to input signal
        for (i=0; i<N; i++) {
            I = (*I_in)[i];
            Q = (*Q_in)[i];
            agc->ApplyGain(I, Q);
            I_out[i] = I;
            Q_out[i] = Q;
            One_out[i] = 1;
        }

        float gain(0.0f), energy(0.0f);
        agc->GetStatus(gain, energy);
        DEBUG(3, AutomaticGainControl, "gain: " << gain << ", energy: " << energy)

        // empty input buffer
        dataIn_0->bufferEmptied();

        // Push data if energy exceeds RSSI threshold
        if ( (energy/gain) > rssi_pass) {
            dataOut_0->pushPacket(I_out, Q_out);
            rssi_pass_packet_counter = 4;
        } else if ( rssi_pass_packet_counter > 0 ) {
            dataOut_0->pushPacket(I_out, Q_out);
            rssi_pass_packet_counter--;
        } else {
            // energy level too low; push zeroes
            dataOut_0->pushPacket(One_out, One_out);
        }

    }
}


