/****************************************************************************

Copyright 2009 Virginia Polytechnic Institute and State University

This file is part of the OSSIE XilinxFPGA.

OSSIE XilinxFPGA is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE XilinxFPGA is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE XilinxFPGA; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


****************************************************************************/

#ifndef XILINXFPGA_H
#define XILINXFPGA_H

#include "ossie/cf.h"
#include "ossie/orb_wrap.h"
#include "ossie/Device_impl.h"

class XilinxFPGA_i : public virtual Device_impl

{
public:
    XilinxFPGA_i(char *id, char* label, char* profile);
    void configure (const CF::Properties & configProperties)
    throw (CF::PropertySet::PartialConfiguration, CF::PropertySet::InvalidConfiguration,
           CORBA::SystemException);
    void deallocateCapacity (const CF::Properties & capacities)
    throw (CF::Device::InvalidState, CF::Device::InvalidCapacity, CORBA::SystemException);
    CORBA::Boolean allocateCapacity (const CF::Properties & capacities)
    throw (CF::Device::InvalidState, CF::Device::InvalidCapacity, CORBA::SystemException);




private:
    XilinxFPGA_i();
    XilinxFPGA_i(const XilinxFPGA_i &);

    void deallocate (CORBA::Any & deviceCapacity, const CORBA::Any & resourceRequest);
    bool allocate (CORBA::Any & deviceCapacity, const CORBA::Any & resourceRequest);

    CF::Properties originalCap; // internal list of properties

};

#endif
