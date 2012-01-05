/****************************************************************************

Copyright 2006 Virginia Polytechnic Institute and State University

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


#ifndef INTERPOLATOR_IMPL_H
#define INTERPOLATOR_IMPL_H

#include <stdlib.h>

#include "ossie/cf.h"

#include "standardinterfaces/complexShort_u.h"
#include "standardinterfaces/complexShort_p.h"

#include "ossie/Resource_impl.h"
#include "SigProc.h"

#undef INTERP_LOGGING

class Interpolator_i;

class Interpolator_i : public Resource_impl
{

public:
    Interpolator_i(const char *uuid, omni_condition *con);
    ~Interpolator_i(void);

    static void do_run_interpolation(void *data) {
        ((Interpolator_i *)data)->run_interpolation();
    };

    void start() throw (CF::Resource::StartError, CORBA::SystemException);

    void stop() throw (CF::Resource::StopError, CORBA::SystemException);

    CORBA::Object_ptr getPort( const char* _id ) throw (CF::PortSupplier::UnknownPort, CORBA::SystemException);

    void releaseObject() throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);

    void initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException);

    void query(CF::Properties &configProperties)
    throw (CF::UnknownProperties, CORBA::SystemException);

    void configure(const CF::Properties&)
    throw (CORBA::SystemException, CF::PropertySet::InvalidConfiguration, CF::PropertySet::PartialConfiguration);


private:
    Interpolator_i();
    Interpolator_i(const Interpolator_i &);

    void run_interpolation();

    omni_condition *component_running;
    omni_thread *processing_thread;
    omni_mutex accessPrivateData;

// For interpolation operation
    float *h;
    unsigned int len_h;

// Interpolation factor
    unsigned int M;

    unsigned int sample_count;
    unsigned int previous_length;

    SigProc::fir_filter *i_filter, *q_filter;

    short f2s(float r);

// For data in port
    standardInterfaces_i::complexShort_p *dataIn;

// For data out port
    standardInterfaces_i::complexShort_u *dataOut;

//standardInterfaces::MetaData *metadata;

// For debugging
#ifdef INTERP_LOGGING
    SigProc::dump_data *in_data;
    SigProc::dump_data *out_data;
#endif

// configuration variables
    char * pulseShape;  ///< pulse shape (rrcos, etc.)
    unsigned int m;     ///< symbol delay
    float beta;         ///< excess bandwidth factor

// configuration flags
    bool createDynamicFilter;
    bool isConfigured_pulseShape;
    bool isConfigured_k;
    bool isConfigured_m;
    bool isConfigured_beta;


};
#endif
