/****************************************************************************

 Copyright 2007 Virginia Polytechnic Institute and State University

 This file is part of the OSSIE Channel.

 OSSIE Channel is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 OSSIE Channel is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with OSSIE Channel; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 ****************************************************************************/

#include <string>
#include <iostream>
#include <ctime>
#include "math.h"
#include "Channel.h"

Channel_i::Channel_i(const char *uuid, omni_condition *condition) :
        Resource_impl(uuid), component_running(condition)
{
    dataIn = new standardInterfaces_i::complexShort_p("data_in");
    dataOut = new standardInterfaces_i::complexShort_u("data_out");

    fading=false;

    noise_sigma=0;
    K_factor=1;
    MaxDopplerRate=0;
    envelope=true;
    srand(time(0));
    Ns=1;

//Create the thread for the writer's processing function
    processing_thread = new omni_thread(Run, (void *) this);

//Start the thread containing the writer's processing function
    processing_thread->start();

}

Channel_i::~Channel_i(void)
{
    delete dataIn;
    delete dataOut;
}

// Static function for omni thread
void Channel_i::Run(void * data)
{
    ((Channel_i *)data)->ProcessData();
}

CORBA::Object_ptr Channel_i::getPort(const char*portName)
throw ( CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    DEBUG(3, Channel, "getPort() invoked with " << portName)

    CORBA::Object_var p;

    p = dataIn->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

    p = dataOut->getPort(portName);

    if (!CORBA::is_nil(p))
        return p._retn();

    /*exception*/
    throw CF::PortSupplier::UnknownPort();
}

void Channel_i::start() throw (CORBA::SystemException,
                               CF::Resource::StartError)
{
    DEBUG	(3 ,Channel,"start()invoked")
}

void Channel_i::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    DEBUG(3, Channel, "stop()invoked")
}
void Channel_i::releaseObject() throw (CORBA::SystemException,
                                       CF::LifeCycle::ReleaseError)
{
    DEBUG (3, Channel,"releaseObject() invoked" )

    component_running->signal();
}

void Channel_i::initialize() throw (CF::LifeCycle::InitializeError,
                                    CORBA::SystemException)
{
    DEBUG(3,Channel , "initialize() invoked")
}

void Channel_i::query(CF::Properties & configProperties)
throw (CORBA::SystemException, CF::UnknownProperties)
{
    if (configProperties.length() == 0) {
        configProperties.length(propertySet.length());
        for (unsigned int i = 0; i < propertySet.length(); i++) {
            configProperties[i].id = CORBA::string_dup(propertySet[i].id);
            configProperties[i].value = propertySet[i].value;
        }

        return;
    } else {
        for (unsigned int i = 0; i < configProperties.length(); i++) {
            for (unsigned int j=0; j < propertySet.length(); j++) {
                if (strcmp(configProperties[i].id, propertySet[j].id) == 0) {
                    configProperties[i].value = propertySet[j].value;
                }
            }
        }
    }
}

void Channel_i::configure(const CF::Properties& props)
throw (CORBA::SystemException,
       CF::PropertySet::InvalidConfiguration,
       CF::PropertySet::PartialConfiguration)
{
    static int init = 0;
    DEBUG(3, Channel, "configure() invoked")

    if (init == 0) {
        if ( props.length() <= 0 ) {
            std::cout << "Channel: configure called with invalid props.length() - " << props.length() << std::endl;
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
    for (unsigned int i=0; i < props.length(); i++) {
        if (strcmp(props[i].id, "DCE:1d91b55a-2fcb-4d2d-ad7b-1ee58c32cad8") == 0) {
            //The number of samples per symbol
            CORBA::Short simple_temp;
            props[i].value >>= simple_temp;
            Ns = simple_temp;
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
        } else if (strcmp(props[i].id, "DCE:eddc66ce-8a82-11dd-ae05-0016769e497b") == 0) {
            //The power of the AWGN noise
            CORBA::Long simple_temp;
            props[i].value >>= simple_temp;
            noise_sigma = sqrt(simple_temp/2.0);
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
        } else if (strcmp(props[i].id, "DCE:3bbc0656-8a83-11dd-bcd3-0016769e497b") == 0) {
            //Specifies the fading type. Valid values are None and Ricean
            const char * prop_str;
            props[i].value >>= prop_str;
            if ( strcmp(prop_str, "None") == 0 ) {
                fading=false;
            } else if ( strcmp(prop_str, "Ricean") == 0 ) {
                fading=true;
            } else {
                std::cerr << "ERROR: Channel::configure() unknown fading type" << std::endl;
                throw CF::PropertySet::InvalidConfiguration();
            };
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
        } else if (strcmp(props[i].id, "DCE:4ba6cb86-29fb-40c8-a90c-7e69b7e59d85") == 0) {
            const char * prop_str;
            props[i].value >>= prop_str;
            if ( strcmp(prop_str, "True") == 0 ) {
                envelope=true;
            } else if ( strcmp(prop_str, "False") == 0 ) {
                envelope=false;
            } else {
                std::cerr << "ERROR: Channel::configure() unknown envelope type" << std::endl;
                throw CF::PropertySet::InvalidConfiguration();
            };
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
        } else if (strcmp(props[i].id, "DCE:76abee66-8a83-11dd-b96d-0016769e497b") == 0) {
            // K_factor
            CORBA::Double simple_temp;
            props[i].value >>= simple_temp;
            K_factor=simple_temp;
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
        } else if (strcmp(props[i].id, "DCE:9cc6196c-8a85-11dd-b572-0016769e497b") == 0) {
            // The maximum doppler rate in terms of the sample rate
            CORBA::Double simple_temp;
            props[i].value >>= simple_temp;
            MaxDopplerRate=simple_temp;
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
        }
    }
    std::cout<<"noise_sigma:"<<noise_sigma<<"fading:"<<fading<<"envelope"<<envelope<<"K_factor:"<<K_factor<<"MaxDopplerRate"<<MaxDopplerRate<<std::endl;

}

void Channel_i::ProcessData()
{
    DEBUG(3, Channel, "ProcessData() invoked")

    PortTypes::ShortSequence I_out, Q_out;

    PortTypes::ShortSequence *I_in(NULL), *Q_in(NULL);
    CORBA::UShort Data_length,NumberOfMultipath;

    NumberOfMultipath=50;

    double DopplerFreq[NumberOfMultipath];
    double phis[NumberOfMultipath];

    double totalphi,pI,pQ;

    short Iin,Qin;

    const double PI = 4.0*atan(1.0);
    double PathPower=sqrt(NumberOfMultipath);
    long packetno=0;

    while (1) {
        dataIn->getData(I_in, Q_in);

        Data_length = I_in->length();
        Ns=1;

        I_out.length(Data_length); //must define length of output
        Q_out.length(Data_length); //must define length of output

        /*insert code here to do work*/

        //phi=randf();
        if (packetno==0) {
            for (unsigned int j=0; j<NumberOfMultipath; j++) {
                DopplerFreq[j]=MaxDopplerRate*randf();
                phis[j]=randf();
            }
        }

        for (unsigned int i=0; i<Data_length; i++) {

            if ((i % Ns)==0) {
                if (fading) {
                    pI=0;
                    pQ=0;

                    for (unsigned int j=0; j<NumberOfMultipath; j++) {

                        totalphi=2*PI*( (DopplerFreq[j]*(i+packetno*Data_length))+phis[j]);
                        pI+=cos(totalphi);
                        pQ+=sin(totalphi);

                    }

                    pI/=PathPower;
                    pQ/=PathPower;

                    if (envelope) {
                        pI=sqrt(pI*pI+pQ*pQ);
                        pQ=0;

                    }

                    pI=1/sqrt(K_factor+1)*pI +sqrt(K_factor/(1+K_factor));
                    pQ=1/sqrt(K_factor+1)*pQ +sqrt(K_factor/(1+K_factor));

                } else {
                    pI=1;
                    pQ=0;
                };
            };

            //Complex multiplication
            //(x + yi)(u + vi) = (xu – yv) + (xv + yu)i.
            //(Iin +Qin)(pI+pQ)=(IinpI-QinPq)+(IinpQ+QinpI)

            //Multiply and add AWGN Noise
            //std::cout<<fading<<envelope<<"pI"<<pI<<",pQ"<<pQ<<" totalphi"<<totalphi<<std::endl;
            Iin=(*I_in)[i];
            Qin=(*Q_in)[i];
            I_out[i]=(CORBA::Short)(Iin*pI-Qin*pQ)+(CORBA::Short)(noise_sigma*randn()/sqrt(float(Ns)));
            Q_out[i]=(CORBA::Short)(Iin*pQ+Qin*pI)+(CORBA::Short)(noise_sigma*randn()/sqrt(float(Ns)));
        }

        packetno++;
        dataIn->bufferEmptied();
        dataOut->pushPacket(I_out, Q_out);
    }
}

float Channel_i::randn()
//Generates a Gaussian distributed number with zero mean and unit standard deviation
// Adapted from http://www.taygeta.com/random/gaussian.html
// Algorithm bny Dr. Everett (Skip) Carter, Jr
{
    float x1,x2,w;
    do {
        x1=2.0*randf()-1.0;
        x2=2.0*randf()-1.0;
        w=x1*x1+x2*x2;

    } while (w>=1.0);

    w=sqrt((-2.0*log(w))/w);
    return x1*w;
//y2=x2*w;

}

float Channel_i::randf()
// Uniformly distributed number from 0 to 1
{
    return rand()/(float)RAND_MAX;
}

