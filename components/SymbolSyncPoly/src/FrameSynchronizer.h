/****************************************************************************

Copyright 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE FrameSynchronizer.

OSSIE FrameSynchronizer is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE FrameSynchronizer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE FrameSynchronizer; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/


#ifndef FRAMESYNCHRONIZER_IMPL_H
#define FRAMESYNCHRONIZER_IMPL_H

#include <stdlib.h>
#include "ossie/cf.h"
#include "ossie/PortTypes.h"
#include "ossie/Resource_impl.h"
#include "ossie/debug.h"

#include "FrameSynchronizerDSP.h"

#include "standardinterfaces/complexShort_u.h"
#include "standardinterfaces/complexShort_p.h"

/** \brief
 *
 *
 */
class FrameSynchronizer_i : public virtual Resource_impl, public FrameSynchronizerDSP
{
public:
/// Initializing constructor
    FrameSynchronizer_i(const char *uuid, omni_condition *sem);

/// Destructor
    ~FrameSynchronizer_i(void);

/// Static function for omni thread
    static void Run( void * data );

///
    void start() throw (CF::Resource::StartError, CORBA::SystemException);

///
    void stop() throw (CF::Resource::StopError, CORBA::SystemException);

///
    CORBA::Object_ptr getPort( const char* portName )
    throw (CF::PortSupplier::UnknownPort, CORBA::SystemException);

///
    void releaseObject() throw (CF::LifeCycle::ReleaseError, CORBA::SystemException);

///
    void initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException);

    void query(CF::Properties &configProperties)
    throw (CF::UnknownProperties, CORBA::SystemException);

/// Configures properties read from .prf.xml
///   - \ref prop_pulse_shape
///   - \ref prop_k
///   - \ref prop_m
///   - \ref prop_beta
///   - \ref prop_Npfb
    void configure(const CF::Properties&)
    throw (CORBA::SystemException,
           CF::PropertySet::InvalidConfiguration,
           CF::PropertySet::PartialConfiguration);


private:
/// Disallow default constructor
    FrameSynchronizer_i();

/// Disallow copy constructor
    FrameSynchronizer_i(FrameSynchronizer_i&);

/// Main signal processing method
    void ProcessData();

    omni_condition *component_running;  ///< for component shutdown
    omni_thread *processing_thread;     ///< for component writer function

// Configuration variables
    char * _pulseShape;
    unsigned int _k;
    unsigned int _m;
    float _beta;
    unsigned int _Npfb;
    unsigned int buffer_size;

// list components provides and uses ports
    standardInterfaces_i::complexShort_p *dataIn_0;
    standardInterfaces_i::complexShort_u *dataOut_0;

// configuration flags
    bool isConfigured_pulseShape;
    bool isConfigured_k;
    bool isConfigured_m;
    bool isConfigured_beta;
    bool isConfigured_Npfb;

};
#endif

