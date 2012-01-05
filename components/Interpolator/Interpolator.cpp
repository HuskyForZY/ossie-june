/****************************************************************************

Copyright 2006, 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE Interpolator.

OSSIE Interpolator is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE Interpolator is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE Interpolator; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/


#include <iostream>

#include "ossie/debug.h"

#include "Interpolator.h"

Interpolator_i::Interpolator_i(const char *uuid, omni_condition *con) :
        Resource_impl(uuid),
        component_running(con),
        createDynamicFilter(false),
        isConfigured_pulseShape(false),
        isConfigured_k(false),
        isConfigured_m(false),
        isConfigured_beta(false)
{
    previous_length = 0;

    dataIn = new standardInterfaces_i::complexShort_p("inData");
    dataOut = new standardInterfaces_i::complexShort_u("outData");

// Start the run_interpolation thread
    processing_thread = new omni_thread(do_run_interpolation, (void *) this);
    processing_thread->start();

// Interpolation factor
    M = 1;

// Filter
    len_h = 1;
    h = new float[len_h];

    h[0] = 1.0;

    i_filter = new SigProc::fir_filter(h, len_h);
    q_filter = new SigProc::fir_filter(h, len_h);

}


Interpolator_i::~Interpolator_i(void)
{
    delete dataIn;
    delete dataOut;

#ifdef INTERP_LOGGING
    delete in_data;
    delete out_data;
#endif
}

CORBA::Object_ptr Interpolator_i::getPort( const char* portName) throw (CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    DEBUG(4, Interpolator, "Interpolator getPort() called with: " << portName);

    CORBA::Object_var p;

    p = dataIn->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

    p = dataOut->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

    throw CF::PortSupplier::UnknownPort();
}

void Interpolator_i::start() throw (CORBA::SystemException, CF::Resource::StartError)
{
    DEBUG(3, Interpolator, "Interpolator start() called")
}

void Interpolator_i::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    DEBUG(3, Interpolator, "Interpolator stop() called")
}

void Interpolator_i::releaseObject() throw (CORBA::SystemException, CF::LifeCycle::ReleaseError)
{
    DEBUG(3, Interpolator, "In releaseObject.");
    component_running->signal();
    DEBUG(3, Interpolator, "Leaving releaseObject.");
}

void Interpolator_i::initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException)
{
#ifdef INTERP_LOGGING
    out_data = new SigProc::dump_data("out_interp.dat", 0, 100000);
#endif

}

void
Interpolator_i::query (CF::Properties & configProperties)
throw (CORBA::SystemException, CF::UnknownProperties)
{
// for queries of zero length, return all id/value pairs in propertySet
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


void Interpolator_i::configure(const CF::Properties& props) throw (CORBA::SystemException, CF::PropertySet::InvalidConfiguration, CF::PropertySet::PartialConfiguration)
{
    static int init = 0;

    DEBUG(3, Interpolator, "Interpolator configure() called")

    DEBUG(3, Interpolator, "Props length : " << props.length())

    std::cout << "Component - Interpolator" << std::endl;
    if (init == 0) {
        std::cout << "INTERPOLATOR - initial configure call .." << std::endl;
        if ( props.length() <= 0 ) {
            std::cout << "Interpolator: configure called with invalid \
                          props.length() - " << props.length() << std::endl;
            return;
        }
        propertySet.length(props.length());
        for (unsigned int i=0; i < props.length(); i++) {
            std::cout << "Property Id : " << props[i].id << std::endl;
            propertySet[i].id = CORBA::string_dup(props[i].id);
            propertySet[i].value = props[i].value;
        }
        init = 1;
    }
    for (unsigned int i = 0; i < props.length(); i++) {
        DEBUG(3, Interpolator, "Property id : " << props[i].id)

        if (strcmp(props[i].id, "DCE:e5c1d4aa-4b7f-48b7-b3bf-ed90b7653bec") == 0) {
            // InterpFactor property, sets the interpolation factor
            CORBA::UShort D;
            props[i].value >>= D;

            omni_mutex_lock l(accessPrivateData);

            M = D;

            std::cout << "Property id : " << props[i].id << std::endl;
            // Update value of this property in propertySet also
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }


            DEBUG(3, Interpolator, "Interpolation factor set to " << M)
            std::cout << std::endl << "INTERP: k = " << M << std::endl;
            isConfigured_k = true;
        } else if (strcmp(props[i].id, "DCE:9c39de73-54d4-43ad-ab9d-52f5fa526ddf") == 0) {
            // filter property, Filter coefficients
            CORBA::FloatSeq *coeff_ptr;
            props[i].value >>= coeff_ptr;

            omni_mutex_lock l(accessPrivateData);

            len_h = coeff_ptr->length();

            if ( len_h > 1 ) {
                delete []h;
                delete i_filter;
                delete q_filter;

                h = new float[len_h];
                DEBUG(3, Interpolator, "Interpolator filter length = " << len_h)
                for (unsigned int i = 0; i < len_h; i++) {
                    h[i] = (*coeff_ptr)[i];
                    //DEBUG(4, Interpolator, "Coeff[" << i << "] = " << h[i])
                    printf("g(%d) = %f;\n", i+1, h[i]);
                }
                i_filter = new SigProc::fir_filter(h, len_h);
                q_filter = new SigProc::fir_filter(h, len_h);

                std::cout << "Property id : " << props[i].id << std::endl;
                // Update value of this property in propertySet also
                for (unsigned int j=0; j < propertySet.length(); j++ ) {
                    if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                        propertySet[j].value = props[i].value;
                        break;
                    }
                }


            } else {
                // Design filter dynamically
                createDynamicFilter = true;
            }

        } else if (strcmp(props[i].id, "DCE:e65ba870-4c11-11dc-9470-00123f63025f") == 0) {
            // pulse shape
            const char * prop_str;
            props[i].value >>= prop_str;

            // Set appropriate pulse shape
            if ( strcmp(prop_str, "rrcos") == 0 ) {
                pulseShape = "rrcos";
                ///\todo: create function pointer for filter design
            } else {
                // unknown pulse shape
                std::cerr << "ERROR: Interpolator::configure() unknown pulse shape "
                          << prop_str << std::endl;
                throw CF::PropertySet::InvalidConfiguration();
            }
            isConfigured_pulseShape = true;

            std::cout << "Property id : " << props[i].id << std::endl;
            // Update value of this property in propertySet also
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }

            std::cout << std::endl << "INTERP: pulse_shape = " << pulseShape << std::endl;

        } else if (strcmp(props[i].id, "DCE:f6843fdc-4c11-11dc-b0be-00123f63025f") == 0) {
            // m : symbol delay
            CORBA::UShort simple_temp;
            props[i].value >>= simple_temp;
            m = simple_temp;
            isConfigured_m = true;

            std::cout << "Property id : " << props[i].id << std::endl;
            // Update value of this property in propertySet also
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }

            std::cout << std::endl << "INTERP: m = " << m << std::endl;
        } else if (strcmp(props[i].id, "DCE:fed01972-4c11-11dc-8d59-00123f63025f") == 0) {
            // beta : excess bandwidth factor
            CORBA::Float simple_temp;
            props[i].value >>= simple_temp;
            beta = simple_temp;
            isConfigured_beta = true;

            std::cout << "Property id : " << props[i].id << std::endl;
            // Update value of this property in propertySet also
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }

            std::cout << std::endl << "INTERP: beta = " << beta << std::endl;
        } else {
            std::cerr << "Interpolator: Unknown property id " << props[i].id << std::endl;
            throw CF::PropertySet::InvalidConfiguration();
        }

    }

    if ( createDynamicFilter        &&
            isConfigured_pulseShape    &&
            isConfigured_k             &&
            isConfigured_m             &&
            isConfigured_beta ) {
        delete []h;
        delete i_filter;
        delete q_filter;

        ///\todo do not assume always rrcos filter
        std::cout << "Interpolator designing new rrcos filter: "
                  << pulseShape << ", k=" << M << ", m=" << m << ", beta=" << beta
                  << std::endl;

        len_h = 2*M*m + 1;
        h = new float[len_h];
        SigProc::DesignRRCFilter(M, m, beta, h);
        for (unsigned int j=0; j<len_h; j++)
            printf("h(%d) = %f;\n", j+1, h[j]);

        i_filter = new SigProc::fir_filter(h, len_h);
        q_filter = new SigProc::fir_filter(h, len_h);
    }

}

void Interpolator_i::run_interpolation()
{
    DEBUG(3, Interpolator, "Interpolator run_interpolation() thread started")
    PortTypes::ShortSequence *I_in, *Q_in;
    PortTypes::ShortSequence I_out, Q_out;

    unsigned int M_factor(M);
    unsigned int old_M(0);
    unsigned int old_len(0);

    while (1) {

        dataIn->getData(I_in, Q_in);//, metadata);
        {
            omni_mutex_lock l(accessPrivateData);

            M_factor = M;
        }

        // Adjust MetaData tags
        /*        if ( createDynamicFilter )
                    metadata->signal_bandwidth *= (float) ( metadata->sampling_frequency * (1+beta) );
                else
                    metadata->signal_bandwidth *= metadata->sampling_frequency;

                metadata->sampling_frequency *= (float) M;
        */

        unsigned int len = I_in->length();
        unsigned int out_idx(0);

        if (len != old_len || M_factor != old_M) {
            DEBUG(4, Interpolator, "data len or M changed.");
            I_out.length(len * M_factor);
            Q_out.length(len * M_factor);
            old_len = len;
            old_M = M_factor;
            DEBUG(4, Interpolator, "New data len: " << len << " M: " << M);
        }

        for (unsigned int i = 0; i < len; i++) {

            DEBUG(8, Interpolator, "About to filter data.");
            i_filter->do_work(true, (*I_in)[i], I_out[out_idx]);
            q_filter->do_work(true, (*Q_in)[i], Q_out[out_idx]);
            DEBUG(8, Interpolator, "Done filtering data.");

#ifdef INTERP_LOGGING
            out_data->write_data(I_out[out_idx]);
#endif

            out_idx++;

            for (unsigned int j = 0; j < M_factor - 1; j++) {

                i_filter->do_work(true, 0, I_out[out_idx]);
                q_filter->do_work(true, 0, Q_out[out_idx]);

#ifdef INTERP_LOGGING
                out_data->write_data(I_out[out_idx]);
#endif

                out_idx++;
            }
        }
        dataIn->bufferEmptied();
        dataOut->pushPacket(I_out, Q_out);//, *metadata);
    }
}

