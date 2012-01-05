/****************************************************************************

Copyright 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE FrameAssembler.

OSSIE FrameAssembler is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE FrameAssembler is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE FrameAssembler; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/


#include <string>
#include <iostream>
#include "FrameAssembler.h"

FrameAssembler_i::FrameAssembler_i(const char *uuid, omni_condition *condition) :
        Resource_impl(uuid), component_running(condition)
{
    dataIn_0 = new standardInterfaces_i::complexShort_p("SymbolsIn");
    dataOut_0 = new standardInterfaces_i::complexShort_u("FrameSymbolsOut");

    frame_size_option=1;

//Create the thread for the writer's processing function
    processing_thread = new omni_thread(Run, (void *) this);

//Start the thread containing the writer's processing function
    processing_thread->start();

}

FrameAssembler_i::~FrameAssembler_i(void)
{
    delete dataIn_0;
    delete dataOut_0;
}

// Static function for omni thread
void FrameAssembler_i::Run( void * data )
{
    ((FrameAssembler_i*)data)->ProcessData();
}

CORBA::Object_ptr FrameAssembler_i::getPort( const char* portName ) throw (
    CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    DEBUG(3, FrameAssembler, "getPort() invoked with " << portName)

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

void FrameAssembler_i::start() throw (CORBA::SystemException,
                                      CF::Resource::StartError)
{
    DEBUG(3, FrameAssembler, "start() invoked")
}

void FrameAssembler_i::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    DEBUG(3, FrameAssembler, "stop() invoked")
}

void FrameAssembler_i::releaseObject() throw (CORBA::SystemException,
        CF::LifeCycle::ReleaseError)
{
    DEBUG(3, FrameAssembler, "releaseObject() invoked")

    component_running->signal();
}

void FrameAssembler_i::initialize() throw (CF::LifeCycle::InitializeError,
        CORBA::SystemException)
{
    DEBUG(3, FrameAssembler, "initialize() invoked")
}

void
FrameAssembler_i::query (CF::Properties & configProperties)
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

void FrameAssembler_i::configure(const CF::Properties& props)
throw (CORBA::SystemException,
       CF::PropertySet::InvalidConfiguration,
       CF::PropertySet::PartialConfiguration)
{
    static int init = 0;
    DEBUG(3, FrameAssembler, "configure() invoked")

    if (init == 0) {
        if ( props.length() <= 0 ) {
            std::cout << "FrameAssembler: configure called with invalid \
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

        if (strcmp(props[i].id, "DCE:a15e70e4-96e4-4cc8-b1f0-06cb301f6c2e") == 0) {
            // Modulation type
            const char * prop_str;
            props[i].value >>= prop_str;

            std::cout << "Modulation scheme = " << prop_str << std::endl;

            // Set appropriate modulation function
            if ( strcmp(prop_str, "BPSK") == 0 ) {
                ConfigureModulationScheme(SigProc::BPSK);
            } else if ( strcmp(prop_str, "QPSK") == 0 ) {
                ConfigureModulationScheme(SigProc::QPSK);
            } else if ( strcmp(prop_str, "8PSK") == 0 ) {
                ConfigureModulationScheme(SigProc::PSK8);
            } else if ( strcmp(prop_str, "16QAM") == 0 ) {
                ConfigureModulationScheme(SigProc::QAM16);
            } else if ( strcmp(prop_str, "4PAM") == 0 ) {
                ConfigureModulationScheme(SigProc::PAM4);
            } else {
                // unknown property
                std::cerr << "ERROR: FrameAssembler::configure() unknown mod. scheme "
                          << prop_str << std::endl;
                throw CF::PropertySet::InvalidConfiguration();
            }
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[i].value = props[i].value;
                    break;
                }
            }
            ///\todo catch exception thrown by FrameAssemblerDSP::ConfigureModulationScheme()

        } else if (strcmp(props[i].id, "DCE:96b798ba-5412-4bbe-bf0b-8ca665d25b83") == 0) {
            // Frame size
            CORBA::UShort simple_temp;
            props[i].value >>= simple_temp;
            frame_size_option=simple_temp;
            ConfigureFrameSize(frame_size_option);
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
            DEBUG(3, FrameAssembler, "Frame Size Option "<<frame_size_option<<" read and configured")
        } else if (strcmp(props[i].id, "DCE:cdac2d50-214c-4c47-8a6d-d9ec453b3b3c") == 0) {
            CORBA::UShort simple_temp;
            props[i].value >>= simple_temp;
            SetFrameSize(simple_temp,1);
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
        } else if (strcmp(props[i].id, "DCE:a6caa288-08d2-4f0e-b0f0-f34fa513ce3d") == 0) {
            CORBA::UShort simple_temp;
            props[i].value >>= simple_temp;
            SetFrameSize(simple_temp,2);
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
        } else if (strcmp(props[i].id, "DCE:bcf3ff9b-9f9d-4498-b29a-78f813678271") == 0) {
            CORBA::UShort simple_temp;
            props[i].value >>= simple_temp;
            SetFrameSize(simple_temp,3);
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
        } else if (strcmp(props[i].id, "DCE:34311a06-2055-4490-a949-478786cad4d4") == 0) {
            CORBA::UShort simple_temp;
            props[i].value >>= simple_temp;
            SetFrameSize(simple_temp,4);
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }

        } else {
            // unknown property
            std::cerr << "ERROR: FrameAssembler::configure() unknown property" << std::endl;
            throw CF::PropertySet::InvalidConfiguration();
        }
    }
//Update frame size
    ConfigureFrameSize(frame_size_option);
    DEBUG(3, FrameAssembler, "Frame Size Option "<<frame_size_option<<" configured")

}



void FrameAssembler_i::ProcessData()
{
    DEBUG(3, FrameAssembler, "ProcessData() invoked")

    PortTypes::ShortSequence I_out_header, Q_out_header;
    PortTypes::ShortSequence I_out_data, Q_out_data;
    PortTypes::ShortSequence I_out_0, Q_out_0;

    PortTypes::ShortSequence *I_in_0(NULL), *Q_in_0(NULL);
    unsigned int N_in(0);
    unsigned int j(0);
    unsigned int blockSize(512);
    unsigned int ramp_length=128;
    unsigned int header_length=419;

// Prepare preamble phasing pattern, ramp up/down
    short * I_phasing_pattern = new short[512];
    short * Q_phasing_pattern = new short[512];
    PortTypes::ShortSequence I_out_phasing_pattern, Q_out_phasing_pattern;
    I_out_phasing_pattern.length(ramp_length);
    Q_out_phasing_pattern.length(ramp_length);
    PortTypes::ShortSequence I_out_ramp_up, Q_out_ramp_up;
    I_out_ramp_up.length(ramp_length);
    Q_out_ramp_up.length(ramp_length);
    PortTypes::ShortSequence I_out_ramp_down, Q_out_ramp_down;
    I_out_ramp_down.length(ramp_length);
    Q_out_ramp_down.length(ramp_length);

    AssemblePhasingPattern(I_phasing_pattern, Q_phasing_pattern);
    for (unsigned int i=0; i<ramp_length; i++) {
        I_out_phasing_pattern[i] = I_phasing_pattern[i];
        Q_out_phasing_pattern[i] = Q_phasing_pattern[i];

        I_out_ramp_up[i] = (short) (I_phasing_pattern[i] * (0.5 - 0.5*cos( PI*i/ramp_length )));
        Q_out_ramp_up[i] = (short) (Q_phasing_pattern[i] * (0.5 - 0.5*cos( PI*i/ramp_length )));

        I_out_ramp_down[i] = 1 - I_out_ramp_up[i];
        Q_out_ramp_down[i] = 1 - Q_out_ramp_up[i];
    }

// control
    PortTypes::ShortSequence I_out_control, Q_out_control;
    I_out_control.length(1024);
    Q_out_control.length(1024);
    for (unsigned int i=0; i<256; i++) {
        I_out_control[i] = (i%2 == 0) ? -BPSK_LEVEL : BPSK_LEVEL;
        Q_out_control[i] = 0;
    }

    delete [] I_phasing_pattern;
    delete [] Q_phasing_pattern;


    short * I_header = new short[header_length];
    short * Q_header = new short[header_length];

    while ( true ) {

        numFrameSymbolsAssembled = 0;

        while ( numFrameSymbolsAssembled < (frameSize-1) ) {
            // Get data from port
            dataIn_0->getData(I_in_0, Q_in_0);
            // Read input data length
            N_in = I_in_0->length();
            DEBUG(1, FrameAssembler, "got " << N_in << " samples");


            /* HV

                            for (unsigned int i=0; i<N_in; i++) {
                                I_out_control[i] = (*I_in_0)[i];
                                Q_out_control[i] = (*Q_in_0)[i];
                            }
                            dataIn_0->bufferEmptied();
                            continue;
                        }
            */

            if ( numFrameSymbolsAssembled==0 ) {
                // first packet
                //ConfigureModulationScheme(SigProc::QPSK);


                //ConfigureFrameType(FRAME_TYPE_DATA);

                ConfigureFrameSize(frame_size_option);


                I_out_data.length(frameSize);
                Q_out_data.length(frameSize);
            }

            DEBUG(1, FrameAssembler, "Assembling " << N_in << " symbols (max: " << frameSize);

            for (unsigned int i=0; i<N_in; i++) {
                //
                I_out_data[numFrameSymbolsAssembled] = (*I_in_0)[i];
                Q_out_data[numFrameSymbolsAssembled] = (*Q_in_0)[i];

                numFrameSymbolsAssembled++;
            }

            // empty input buffer; release semaphore
            dataIn_0->bufferEmptied();

        }

        // ASSEMBLE_PREAMBLE
        DEBUG(4, FrameAssembler, "Assembling preamble")


        // push ramp-up preamble phasing pattern
        DEBUG(7, FrameAssembler, "pushing ramp up phasing " << I_out_ramp_up.length() << " frame samples")
        dataOut_0->pushPacket(I_out_ramp_up, Q_out_ramp_up);

        // push preamble
        unsigned int phasing_pattern_length;

        phasing_pattern_length = 32;

        for (unsigned int i=0; i<phasing_pattern_length; i++) {
            DEBUG(7, FrameAssembler, "pushing phasing " << I_out_phasing_pattern.length() << " frame samples")
            dataOut_0->pushPacket(I_out_phasing_pattern, Q_out_phasing_pattern);
        }

        // ASSEMBLE_HEADER
        DEBUG(4, FrameAssembler, "Assembling header")
        // copy header
        I_out_header.length(header_length);
        Q_out_header.length(header_length);
        AssembleHeader( I_header, Q_header );
        for (unsigned int i=0; i<header_length; i++) {
            I_out_header[i] = I_header[i];
            Q_out_header[i] = Q_header[i];
        }

        // push data
        DEBUG(7, FrameAssembler, "pushing header " << I_out_header.length() << " frame samples")

        dataOut_0->pushPacket(I_out_header, Q_out_header);


        // ASSEMBLE_FRAME
        // break output into smaller pieces

        I_out_0.length(blockSize);
        Q_out_0.length(blockSize);

        j = 0;
        for ( unsigned int i=0; i<frameSize; i++ ) {
            I_out_0[j] = I_out_data[i];
            Q_out_0[j] = Q_out_data[i];
            j++;

            if ( j==blockSize || i==(frameSize-1) ) {
                I_out_0.length(j);
                Q_out_0.length(j);
                DEBUG(7, FrameAssembler, "pushing frame " << I_out_0.length() << " frame samples")
                dataOut_0->pushPacket(I_out_0, Q_out_0);
                j=0;
            }
        }

        // ASSEMBLE_EOM_CODE:
        // push phasing pattern
        ///\todo push control packet (BPSK)
        if (frameType != FRAME_TYPE_CONTROL) {
            // No need to push additional control information as internal frame data
            // are already control
            DEBUG(7, FrameAssembler, "pushing control " << I_out_control.length() << " frame samples")

            //no EOM      dataOut_0->pushPacket(I_out_control, Q_out_control);

        }

        // push ramp-down phasing pattern
        DEBUG(7, FrameAssembler, "pushing ramp down phasing " << I_out_ramp_up.length() << " frame samples")

        dataOut_0->pushPacket(I_out_ramp_down, Q_out_ramp_down);

    }

    delete [] I_header;
    delete [] Q_header;
}


