/****************************************************************************

Copyright 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE FrameSynchronizer.

OSSIE FrameSynchronizer is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE FrameSynchronizer is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE FrameSynchronizer; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/


#include <string>
#include <iostream>
#include <fstream>
#include "FrameSynchronizer.h"

#undef LOGGING

FrameSynchronizer_i::FrameSynchronizer_i(const char *uuid, omni_condition *condition) :
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
}

FrameSynchronizer_i::~FrameSynchronizer_i(void)
{
    delete dataIn_0;
    delete dataOut_0;
}

// Static function for omni thread
void FrameSynchronizer_i::Run( void * data )
{
    ((FrameSynchronizer_i*)data)->ProcessData();
}

CORBA::Object_ptr FrameSynchronizer_i::getPort( const char* portName ) throw (
    CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    DEBUG(3, FrameSynchronizer, "getPort() invoked with " << portName)

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

void FrameSynchronizer_i::start() throw (CORBA::SystemException,
        CF::Resource::StartError)
{
    DEBUG(3, FrameSynchronizer, "start() invoked")
}

void FrameSynchronizer_i::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    DEBUG(3, FrameSynchronizer, "stop() invoked")
}

void FrameSynchronizer_i::releaseObject() throw (CORBA::SystemException,
        CF::LifeCycle::ReleaseError)
{
    DEBUG(3, FrameSynchronizer, "releaseObject() invoked")

    component_running->signal();
}

void FrameSynchronizer_i::initialize() throw (CF::LifeCycle::InitializeError,
        CORBA::SystemException)
{
    DEBUG(3, FrameSynchronizer, "initialize() invoked")
}

void
FrameSynchronizer_i::query (CF::Properties & configProperties)
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

void FrameSynchronizer_i::configure(const CF::Properties& props)
throw (CORBA::SystemException,
       CF::PropertySet::InvalidConfiguration,
       CF::PropertySet::PartialConfiguration)
{
    static int init = 0;
    DEBUG(3, FrameSynchronizer, "configure() invoked")

    std::cout << "Component - FRAME SYNCHRONIZER" << std::endl;
    std::cout << "Props length = " << props.length() << std::endl;

    if (init == 0) {
        if ( props.length() <= 0 ) {
            std::cout << "FrameSynchronizer: configure called with invalid \
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

        if (strcmp(props[i].id, "DCE:18576cfb-849b-4856-8b53-b3ab215df53c") == 0) {
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
                std::cerr << "ERROR: FrameSynchronizer::configure() unknown pulse shape "
                          << prop_str << std::endl;
                throw CF::PropertySet::InvalidConfiguration();
            }
            isConfigured_pulseShape = true;

        } else if (strcmp(props[i].id, "DCE:1bc8ae91-fa37-4905-aeeb-dff58870b76e") == 0) {
            // k : samples per symbol
            CORBA::UShort simple_temp;
            props[i].value >>= simple_temp;
            _k = simple_temp;
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
            isConfigured_k = true;
        } else if (strcmp(props[i].id, "DCE:a70a36df-a580-4e1e-90d6-05a1b713edd4") == 0) {
            // m : symbol delay
            CORBA::UShort simple_temp;
            props[i].value >>= simple_temp;
            _m = simple_temp;
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
            isConfigured_m = true;
        } else if (strcmp(props[i].id, "DCE:a233bee5-9696-4b37-b01e-3a910f77afe6") == 0) {
            // beta : excess bandwidth factor
            CORBA::Float simple_temp;
            props[i].value >>= simple_temp;
            _beta = simple_temp;
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
            isConfigured_beta = true;
        } else if (strcmp(props[i].id, "DCE:8b80d4ae-b108-4e6c-8755-dc10d8f46f3b") == 0) {
            // Npfb : number of filters in bank
            CORBA::UShort simple_temp;
            props[i].value >>= simple_temp;
            _Npfb = simple_temp;
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
            isConfigured_Npfb = true;
        } else if (strcmp(props[i].id, "DCE:d11bdc8d-1069-4f3a-9541-ecfcb67a1875") == 0) {
            CORBA::UShort simple_temp;
            props[i].value >>= simple_temp;
            SetFrameSize(simple_temp,1);
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
        } else if (strcmp(props[i].id, "DCE:0af7e693-4387-46d1-9d04-fdb1e2b83675") == 0) {
            CORBA::UShort simple_temp;
            props[i].value >>= simple_temp;
            SetFrameSize(simple_temp,2);
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
        } else if (strcmp(props[i].id, "DCE:12b10d48-a8b2-4e96-bf5a-e7fa96000326") == 0) {
            CORBA::UShort simple_temp;
            props[i].value >>= simple_temp;
            SetFrameSize(simple_temp,3);
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
        } else if (strcmp(props[i].id, "DCE:d04d7b28-bfc9-4304-9a0c-9308ecc07812") == 0) {
            CORBA::UShort simple_temp;
            props[i].value >>= simple_temp;
            SetFrameSize(simple_temp,4);
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
        } else if (strcmp(props[i].id, "DCE:841e7c53-9e6a-4efa-9f99-090bf1d33dd5") == 0) {
            CORBA::UShort simple_temp;
            props[i].value >>= simple_temp;
            buffer_size=simple_temp;
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
        } else {
            // unknown property
            std::cerr << "ERROR: FrameSynchronizer::configure() unknown property \""
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
        DEBUG(5, FrameSynchronizer, "configuring filter bank for " <<
              _pulseShape << ", k=" << _k << ", m=" << _m << ", beta=" << _beta << ", Npfb=" << _Npfb)
        ConfigureFilterBank(_pulseShape, _k, _m, _beta, _Npfb);
    }
}

void FrameSynchronizer_i::ProcessData()
{
    DEBUG(3, FrameSynchronizer, "ProcessData() invoked")

#ifdef LOGGING
    std::ofstream input_log;
    input_log.open("fs_input_log.dat");

    std::ofstream output_log;
    output_log.open("fs_output_log.dat");
#endif



    PortTypes::ShortSequence I_out_0, Q_out_0;

    PortTypes::ShortSequence *I_in_0(NULL), *Q_in_0(NULL);
    unsigned int I_in_0_length, Q_in_0_length, N_in(0), N_out(0);
    unsigned int buf_len(1024);
    short * I_in = new short[buf_len];
    short * Q_in = new short[buf_len];
    short * I_out = new short[buf_len];
    short * Q_out = new short[buf_len];

    bool extracted;
    unsigned int nr;        // num samples read
    unsigned int nw;        // num samples written
    unsigned int nr_total;  // total samples read
    unsigned int nw_total;  // total samples written
    unsigned int nout;      // total samples at buffer

    float rssi_threshold_dB(3.0f);
    unsigned int rssi_pass_num(3);
    unsigned int rssi_counter(0);


    nout=0;

    while ( true ) {
        DEBUG(7, FrameSynchronizer, "iterating loop...")

        // Get data from port

        dataIn_0->getData(I_in_0, Q_in_0);



        DEBUG(7, FrameSynchronizer, "Got " << I_in_0->length() << " samples")



        // Read input data length
        I_in_0_length = I_in_0->length();
        Q_in_0_length = Q_in_0->length();

        if ( I_in_0_length != Q_in_0_length ) {
            // Not sure how to handle multiple-length inputs
            std::cerr << "ERROR: FrameSynchronizer_i::ProcessData()" << std::endl
                      << "  => Not sure how to handle multiple-length inputs" << std::endl;
            throw 0;
        } else {
            N_in = I_in_0_length;
        }

        /// \todo Initialize I_in and Q_in from CORBA sequence instead
        /// of copying values in loop
        DEBUG(7, FrameSynchronizer, "Copying data from CORBA sequence...")
        /// \todo This is incredibly inefficient; need to manage memory better
        if (N_in > buf_len) {
            delete [] I_in;
            delete [] Q_in;
            delete [] I_out;
            delete [] Q_out;
            buf_len = N_in;

            I_in = new short[buf_len];
            Q_in = new short[buf_len];
            I_out = new short[buf_len];
            Q_out = new short[buf_len];
            N_out = N_in;
        }

        // copy data to input buffer
        for (unsigned int ii=0; ii<N_in; ii++) {
            I_in[ii] = (*I_in_0)[ii];
            Q_in[ii] = (*Q_in_0)[ii];

            I_out[ii] = 0;
            Q_out[ii] = 0;
        }

#ifdef LOGGING
        for (unsigned int ii=0; ii<I_in_0_length; ii++)
            input_log << I_in[ii] << "  " << Q_in[ii] << std::endl;
#endif

        nr=0;           // num samples read
        nw=0;           // num samples written
        nr_total=0;     // total samples read
        nw_total=0;     // total samples written

        while ( nr_total < N_in ) {
            switch (operationalMode) {
            case EXTRACT_PN_FRAME_SYNC_CODE:
                DEBUG(5, FrameSynchronizer, "trying to extract frame sync code...");
                extracted = FindFrameHeader(
                                &I_in[nr_total],  &Q_in[nr_total],  N_in-nr_total,  nr,
                                &I_out[nw_total], &Q_out[nw_total], N_out-nw_total, nw);
                nr_total += nr;
                //nw_total += nw; // uncomment to allow preamble data to be pushed
                nw_total=0;

                if (!extracted)
                    break;

            case EXTRACT_CONTROL_CODES:
                DEBUG(5, FrameSynchornizer, "trying to extract frame header...");
                extracted = ExtractFrameHeader(
                                &I_in[nr_total],  &Q_in[nr_total],  N_in-nr_total,  nr,
                                &I_out[nw_total], &Q_out[nw_total], N_out-nw_total, nw);
                nr_total += nr;
                //nw_total += nw; // uncomment to allow control codes to be pushed
                nout=0;
                nw_total=0;

                if (!extracted)
                    break;

            case DECODE_FRAME_HEADER:
                DEBUG(5, FrameSynchronizer, "trying to decode frame header...");
                DecodeFrameHeader();



            case EXTRACT_FRAME:
                DEBUG(5, FrameSynchronizer, "trying to extract frame symbols...");
                extracted = ExtractFrameSymbols(
                                &I_in[nr_total],  &Q_in[nr_total],  N_in-nr_total,  nr,
                                &I_out[nw_total], &Q_out[nw_total], N_out-nw_total, nw);
                nr_total += nr;
                nw_total += nw;

                break;
            case EXTRACT_EOM_CODE:
                // set to BPSK

                DEBUG(5, FrameSynchronizer, "trying to extract EOM code...");
                extracted = ExtractFrameEOM(
                                &I_in[nr_total],  &Q_in[nr_total],  N_in-nr_total,  nr,
                                &I_out[nw_total], &Q_out[nw_total], N_out-nw_total, nw);
                nr_total += nr;
                //nw_total += nw;

                break;
            default:
                std::cerr << "ERROR! FrameSynchronizer: unknown operational mode!" << std::endl;
                throw 0;
            }

            //usleep(2000000);
            // push data to output buffer

            if ( nw_total == 0 )
                continue;

            if (buffer_size>0) {

                I_out_0.length(buffer_size);
                Q_out_0.length(buffer_size);
                for (unsigned int ii=0; ii<nw_total; ii++) {
                    I_out_0[nout] = I_out[ii];
                    Q_out_0[nout] = Q_out[ii];
                    nout++;
                    if (nout==buffer_size) {
                        dataOut_0->pushPacket(I_out_0, Q_out_0);
                        nout=0;
                    };
                }

            } else {
                DEBUG(7, FrameSynchronizer, "Setting output length (" << nw_total << ")...")
                I_out_0.length(nw_total);
                Q_out_0.length(nw_total);

                /// \todo Initialize I_out_0 and Q_out_0 from array instead
                /// of copying values in loop
                DEBUG(7, FrameSynchronizer, "Copying data to CORBA sequence...")
                for (unsigned int ii=0; ii<nw_total; ii++) {
                    I_out_0[ii] = I_out[ii];
                    Q_out_0[ii] = Q_out[ii];
                }

                DEBUG(5, FrameSynchronizer, "pushing " << I_out_0.length() << " samples")
                dataOut_0->pushPacket(I_out_0, Q_out_0);
            }

        }

        // release input buffer
        dataIn_0->bufferEmptied();


    }

/// \todo This is incredibly inefficient; need to manage memory better
    DEBUG(7, FrameSynchronizer, "Deleting temp buffers...")
    delete [] I_in;
    delete [] Q_in;

    delete [] I_out;
    delete [] Q_out;

#ifdef LOGGING
    input_log.close();
    output_log.close();
#endif

}


