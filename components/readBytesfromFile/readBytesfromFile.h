/****************************************************************************
Copyright 2007 Virginia Polytechnic Institute and State University
This file is part of the OSSIE __COMP_NAME__.
OSSIE __COMP_NAME__ is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
"(at your option) any later version
OSSIE __COMP_NAME__ is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with OSSIE __COMP_NAME__; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****************************************************************************/

#ifndef READBYTESFROMFILE_IMPL_H
#define READBYTESFROMFILE_IMPL_H

#include <stdlib.h>
#include "ossie/cf.h"
#include "ossie/PortTypes.h"
#include "ossie/Resource_impl.h"
#include "ossie/debug.h"


#include "standardinterfaces/realChar.h"
#include "standardinterfaces/realChar_u.h"

/** \brief This module reads bytes from a file and sends them on the output port as a flow of bits mapped on bytes
 *
 *	\author Flavio Crisciani <flavio.crisciani@intecs.it>
 *  \author Eva Pierattelli <eva.pierattelli@intecs.it>
 *
 */
class readBytesfromFile_i : public virtual Resource_impl
{
  public:
    /// Initializing constructor
    readBytesfromFile_i(const char *uuid, omni_condition *sem);

    /// Destructor
    ~readBytesfromFile_i(void);

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

	/// Query properties from .prf.xml
	void query(CF::Properties&)
		throw (CORBA::SystemException,
				CF::UnknownProperties);

    /// Configures properties read from .prf.xml
    void configure(const CF::Properties&)
        throw (CORBA::SystemException,
            CF::PropertySet::InvalidConfiguration,
            CF::PropertySet::PartialConfiguration);


  private:
    /// Disallow default constructor
    readBytesfromFile_i();

    /// Disallow copy constructor
    readBytesfromFile_i(readBytesfromFile_i&);

    /// Main signal processing method
    void ProcessData();
	bool continue_processing();
	volatile bool thread_started;
	omni_mutex processing_mutex;
   
    omni_condition *component_running;  ///< for component shutdown
    omni_thread *processing_thread;     ///< for component writer function
    	
        CORBA::ULong simple_0_value;

	/// File descriptor of the input file
	int inputFD;
	bool inputFileOpen;

    
    // list components provides and uses ports
        standardInterfaces_i::realChar_u *dataOut_0;
    
};
#endif
