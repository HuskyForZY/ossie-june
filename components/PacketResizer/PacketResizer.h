/****************************************************************************

Copyright 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE PacketResizer.

OSSIE PacketResizer is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE PacketResizer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE PacketResizer; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/


#ifndef PACKETRESIZER_IMPL_H
#define PACKETRESIZER_IMPL_H

#include <stdlib.h>
#include "ossie/cf.h"
#include "ossie/PortTypes.h"
#include "ossie/Resource_impl.h"
#include "ossie/debug.h"


#include "standardinterfaces/realChar.h"
#include "standardinterfaces/realChar_u.h"
#include "standardinterfaces/realChar_p.h"

/** \brief
 *
 *
 */
class PacketResizer_i : public virtual Resource_impl
{
public:
/// Initializing constructor
    PacketResizer_i(const char *uuid, omni_condition *sem);

/// Destructor
    ~PacketResizer_i(void);

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
    void configure(const CF::Properties&)
    throw (CORBA::SystemException,
           CF::PropertySet::InvalidConfiguration,
           CF::PropertySet::PartialConfiguration);


private:
/// Disallow default constructor
    PacketResizer_i();

/// Disallow copy constructor
    PacketResizer_i(PacketResizer_i&);

/// Main signal processing method
    void ProcessData();

    omni_condition *component_running;  ///< for component shutdown
    omni_thread *processing_thread;     ///< for component writer function

    CORBA::Long simple_0_value;


// list components provides and uses ports
    standardInterfaces_i::realChar_u *dataOut;
    standardInterfaces_i::realChar_p *dataIn;

    unsigned int Packet_size;

};
#endif
