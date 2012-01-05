/****************************************************************************

Copyright 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE Conv_Enc.

OSSIE Conv_Enc is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE Conv_Enc is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE Conv_Enc; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/


#include <string>
#include <iostream>
#include "Conv_Enc.h"

Conv_Enc_i::Conv_Enc_i(const char *uuid, omni_condition *condition) :
        Resource_impl(uuid), component_running(condition)
{
    dataOut_0 = new standardInterfaces_i::realChar_u("encoded_bits");
    dataIn_0 = new standardInterfaces_i::realChar_p("bits_to_enc_in");

    inputGeneratorPolynomialsLength = 2;
    inputGeneratorPolynomials = new unsigned int[inputGeneratorPolynomialsLength];
    inputGeneratorPolynomials[0]=0;
    inputGeneratorPolynomials[1]=0;
    encoder=new SigProc::fec_conv_encoder();
    configured=false;
//Create the thread for the writer's processing function
    processing_thread = new omni_thread(Run, (void *) this);

//Start the thread containing the writer's processing function
    processing_thread->start();

}

Conv_Enc_i::~Conv_Enc_i(void)
{
    delete dataOut_0;
    delete dataIn_0;
    delete []inputGeneratorPolynomials;
    delete trellisTables;
    delete encoder;
}

// Static function for omni thread
void Conv_Enc_i::Run( void * data )
{
    ((Conv_Enc_i*)data)->ProcessData();
}

CORBA::Object_ptr Conv_Enc_i::getPort( const char* portName ) throw (
    CORBA::SystemException, CF::PortSupplier::UnknownPort)
{
    DEBUG(3, Conv_Enc, "getPort() invoked with " << portName)

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

void Conv_Enc_i::start() throw (CORBA::SystemException,
                                CF::Resource::StartError)
{
    DEBUG(3, Conv_Enc, "start() invoked")
}

void Conv_Enc_i::stop() throw (CORBA::SystemException, CF::Resource::StopError)
{
    DEBUG(3, Conv_Enc, "stop() invoked")
}

void Conv_Enc_i::releaseObject() throw (CORBA::SystemException,
                                        CF::LifeCycle::ReleaseError)
{
    DEBUG(3, Conv_Enc, "releaseObject() invoked")

    component_running->signal();
}

void Conv_Enc_i::initialize() throw (CF::LifeCycle::InitializeError,
                                     CORBA::SystemException)
{
    DEBUG(3, Conv_Enc, "initialize() invoked")
}

void
Conv_Enc_i::query (CF::Properties & configProperties)
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

void Conv_Enc_i::configure(const CF::Properties& props)
throw (CORBA::SystemException,
       CF::PropertySet::InvalidConfiguration,
       CF::PropertySet::PartialConfiguration)
{
    static int init = 0;
    DEBUG(3, Conv_Enc, "configure() invoked")

    if (init == 0) {
        if ( props.length() <= 0 ) {
            std::cout << "Conv_Enc: configure called with invalid props.length() - " << props.length() << std::endl;
            return;
        }

        propertySet.length(props.length());
        for (unsigned int i=0; i < props.length(); i++) {
            propertySet[i].id = CORBA::string_dup(props[i].id);
            propertySet[i].value = props[i].value;
        }
        init = 1;
    }
    for (unsigned int i = 0; i <props.length(); i++) {
        if (strcmp(props[i].id, "DCE:345df262-1611-11dc-a219-0016769e497b") == 0) {
            CORBA::Short simple_temp;
            props[i].value >>= simple_temp;
            rate_index = simple_temp;
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
        }
        if (strcmp(props[i].id, "DCE:7be7e584-1611-11dc-b945-0016769e497b") == 0) {
            CORBA::Short simple_temp;
            props[i].value >>= simple_temp;
            mode = simple_temp;
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
        }
        if (strcmp(props[i].id, "DCE:d2ee004a-18ee-11dc-8925-0016769e497b") == 0) {
            CORBA::Short simple_temp;
            props[i].value >>= simple_temp;
            k = simple_temp;
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
        }
        if (strcmp(props[i].id, "DCE:04aaa5ac-18ef-11dc-83ea-0016769e497b") == 0) {
            CORBA::Short simple_temp;
            props[i].value >>= simple_temp;
            K = simple_temp;
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
        }
        if (strcmp(props[i].id, "DCE:2d17e716-18ef-11dc-bf5c-0016769e497b") == 0) {
            CORBA::Short simple_temp;
            props[i].value >>= simple_temp;
            n = simple_temp;
            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
        }
        if (strcmp(props[i].id, "DCE:4ef1b3b0-18f1-11dc-99b1-0016769e497b") == 0) {
            CORBA::ShortSeq *simplesequence;
            props[i].value >>= simplesequence;
            inputGeneratorPolynomialsLength = simplesequence->length();
            std::cout << "inputGeneratorPolynomials has length : " <<
                      inputGeneratorPolynomialsLength << std::endl;

            delete []inputGeneratorPolynomials;

            DEBUG(4, Conv_Enc, "does delete fails?")
            inputGeneratorPolynomials = new unsigned int [inputGeneratorPolynomialsLength];

            DEBUG(4, Conv_Enc, "the new?")

            for (unsigned int j = 0; j < inputGeneratorPolynomialsLength; j++) {
                DEBUG(3, Conv_Enc, "get the input from the simplesequence")
                inputGeneratorPolynomials[j] = (*simplesequence)[j];
            }

            for (unsigned int j=0; j < propertySet.length(); j++ ) {
                if ( strcmp(propertySet[j].id, props[i].id) == 0 ) {
                    propertySet[j].value = props[i].value;
                    break;
                }
            }
        }
    }

    DEBUG(3, Conv_Enc, "Configure is done, generate the trellis etc")
    switch (rate_index) {
        unsigned int genPoly[10];
    case 0:
        trellisTables=new SigProc::trellisTable(inputGeneratorPolynomials,
                                                (short unsigned int)k,
                                                (short unsigned int)n,
                                                (short unsigned int)K);

        DEBUG(3, Conv_Dec, "custom trellis table generated")
        break;
    case 1:
        //Rate 1, no trellis needed;
        DEBUG(3, Conv_Dec, "Pass through mode")
        break;
    case 2:
        //4/5
        genPoly[0]=237;
        genPoly[1]=274;
        genPoly[2]=156;
        genPoly[3]=255;
        genPoly[4]=337;
        trellisTables=new SigProc::trellisTable(genPoly,
                                                (short unsigned int)4,
                                                (short unsigned int)5,
                                                (short unsigned int)2);
        DEBUG(3, Conv_Dec, "4/5 trellis table generated")
        break;
    case 3:
        //2/3
        genPoly[0]=236;
        genPoly[1]=155;
        genPoly[2]=337;
        trellisTables=new SigProc::trellisTable(genPoly,
                                                (short unsigned int)2,
                                                (short unsigned int)3,
                                                (short unsigned int)4);
        DEBUG(3, Conv_Dec, "2/3 trellis table generated")
        break;
    case 4:
        //1/2
        genPoly[0]=133;
        genPoly[1]=171;

        trellisTables=new SigProc::trellisTable(genPoly,
                                                (short unsigned int)1,
                                                (short unsigned int)2,
                                                (short unsigned int)7);
        DEBUG(3, Conv_Dec, "1/2 trellis table generated")
        break;
    case 5:
        //1/3
        genPoly[0]=133;
        genPoly[1]=145;
        genPoly[2]=175;
        trellisTables=new SigProc::trellisTable(genPoly,
                                                (short unsigned int)1,
                                                (short unsigned int)3,
                                                (short unsigned int)7);
        DEBUG(3, Conv_Dec, "1/3 trellis table generated")
        break;
    case 6:
        //1/4
        genPoly[0]=135;
        genPoly[1]=135;
        genPoly[2]=147;
        genPoly[3]=163;
        trellisTables=new SigProc::trellisTable(genPoly,
                                                (short unsigned int)1,
                                                (short unsigned int)4,
                                                (short unsigned int)7);
        DEBUG(3, Conv_Dec, "1/4 trellis table generated")
        break;
    case 7:
        //1/5
        genPoly[0]=175;
        genPoly[1]=131;
        genPoly[2]=135;
        genPoly[3]=135;
        genPoly[4]=147;

        trellisTables=new SigProc::trellisTable(genPoly,
                                                (short unsigned int)1,
                                                (short unsigned int)5,
                                                (short unsigned int)7);
        DEBUG(3, Conv_Dec, "1/5 trellis table generated")
        break;
    case 8:
        //1/6
        genPoly[0]=173;
        genPoly[1]=151;
        genPoly[2]=135;
        genPoly[3]=135;
        genPoly[4]=163;
        genPoly[5]=137;

        trellisTables=new SigProc::trellisTable(genPoly,
                                                (short unsigned int)1,
                                                (short unsigned int)6,
                                                (short unsigned int)7);
        DEBUG(3, Conv_Dec, "1/6 trellis table generated")
        break;
    case 9:
        //1/7
        genPoly[0]=165;
        genPoly[1]=145;
        genPoly[2]=173;
        genPoly[3]=135;
        genPoly[4]=135;
        genPoly[5]=147;
        genPoly[6]=137;

        trellisTables=new SigProc::trellisTable(genPoly,
                                                (short unsigned int)1,
                                                (short unsigned int)7,
                                                (short unsigned int)7);
        DEBUG(3, Conv_Dec, "1/7 trellis table generated")
        break;
    case 10:
        //1/8
        genPoly[0]=153;
        genPoly[1]=111;
        genPoly[2]=165;
        genPoly[3]=173;
        genPoly[4]=135;
        genPoly[5]=135;
        genPoly[6]=147;
        genPoly[7]=137;
        trellisTables=new SigProc::trellisTable(genPoly,
                                                (short unsigned int)1,
                                                (short unsigned int)8,
                                                (short unsigned int)7);
        DEBUG(3, Conv_Dec, "1/8 trellis table generated")
        break;
    default:
        //Unknown rate
        throw 0;
    };

    if (rate_index!=1)  {
        encoder->SetTrellisTable(trellisTables);

        DEBUG(4, Conv_Enc, "the trellis was sent to the encoder")
    }
    configured=true;

}

void Conv_Enc_i::ProcessData()
{
    DEBUG(3, Conv_Enc, "ProcessData() invoked")

    PortTypes::CharSequence I_out_0;


    PortTypes::CharSequence *I_in_0(NULL);
    CORBA::UShort I_in_0_length;
    unsigned short int data2Enc[20],encData[20],noOfSymbols=0;
    signed short int tmp;
    unsigned short int numberOfBits=0, numberOfOutBits=0;
    SigProc::trellisTable *theTrellisTable;



    while (1) {

        dataIn_0->getData(I_in_0);


        if (!configured) throw 0;

        I_in_0_length = I_in_0->length();

        numberOfBits=I_in_0_length;

        if (rate_index==1) {
            I_out_0.length(I_in_0_length);
            for (unsigned int i=0; i<I_in_0_length; i++) {
                I_out_0[i]=(*I_in_0)[i];
            };
            DEBUG(4, Conv_Enc, "Pass through mode, no encoding will done")
        } else {

            DEBUG(4, Conv_Enc, numberOfBits<<" bits received")
            theTrellisTable=trellisTables;

            if (numberOfBits % theTrellisTable->k>0) {
                std::cout<<"ERROR Conv_Enc::ProcessData number of input bits don't match the selected rate"<<theTrellisTable->k <<"/"<<theTrellisTable->n<<"\n";
                throw 0;
            }

            noOfSymbols=numberOfBits/theTrellisTable->k;

            ///Adjust the output lengh according to the encoding mode
            switch (mode) {
            case 0:
                ///The encoder starts from zero state
                numberOfOutBits=noOfSymbols*theTrellisTable->n;
                break;
            case 1:
                ///The encoder starts and ends at the zero state
                numberOfOutBits=(noOfSymbols+theTrellisTable->K)*theTrellisTable->n;
                break;
            default:
                std::cout<<"ERROR Conv_Enc::ProcessData Unknown mode "<<mode<<"\n";
                throw 0;
            };
            I_out_0.length(numberOfOutBits);
            DEBUG(6, Conv_Enc, " Output length set to:"<<numberOfOutBits<<" mode:"<< mode <<" rate:"<<theTrellisTable->k <<"/"<<theTrellisTable->n)
            ///Set the encoder state to 0
            encoder->ResetState();
            DEBUG(6, Conv_Enc, " Encoder state was reset")
            /*insert code here to do work*/
            for (unsigned int i=0; i<noOfSymbols; i++) {
                for (unsigned int j=0; j<theTrellisTable->k; j++) {
                    data2Enc[j]=(*I_in_0)[i*theTrellisTable->k +j];
                }

                DEBUG(11, Conv_Enc, i+1<<" symbol to be encoded")

                encoder->Encode(data2Enc,encData);

                for (int j=0; j<theTrellisTable->n; j++) {
                    tmp=(short int )encData[j];
                    I_out_0[i*theTrellisTable->n+j]=tmp;
                }
                DEBUG(11, Conv_Enc, i+1<<" symbol encoded")
            }

            ///If mode=1 add additional symbols to make the last state of the encoder zero
            if (mode==1) {

                for (unsigned int j=0; j<theTrellisTable->k; j++) {
                    data2Enc[j]=0;
                }

                for (unsigned int i=noOfSymbols; i<noOfSymbols+theTrellisTable->K; i++) {

                    encoder->Encode(data2Enc,encData);

                    for (int j=0; j<theTrellisTable->n; j++) {
                        tmp=(short int )encData[j];
                        I_out_0[i*theTrellisTable->n+j]=tmp;
                    }
                    DEBUG(11, Conv_Enc, i+1<<" (zero) symbol encoded")
                }

            };

        }//else

        DEBUG(6, Conv_Enc, "Trying to push packet")
        dataOut_0->pushPacket(I_out_0);

        DEBUG(6, Conv_Enc, "Now trying to empty the buffer")
        dataIn_0->bufferEmptied();

        DEBUG(6, Conv_Enc, "buffer emptied")

        if (rate_index!=1) {
            DEBUG(4, Conv_Enc, numberOfOutBits<<" Bits sent to the next component")

        } else {
            DEBUG(4, Conv_Enc, numberOfBits<<" Bits sent to the next component")
        }
    }
}


