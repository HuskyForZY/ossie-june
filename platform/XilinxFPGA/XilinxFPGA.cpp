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

#include "XilinxFPGA.h"

XilinxFPGA_i::XilinxFPGA_i (char *id, char* label, char* profile) : Device_impl(id, label, profile)
{
    DEBUG(3, XilinxFPGA, "in constructor.");

    initialConfiguration = true;
}

void XilinxFPGA_i:: configure (const CF::Properties & configProperties) throw (CF::PropertySet::PartialConfiguration, CF::PropertySet::InvalidConfiguration, CORBA::SystemException)
{
    DEBUG(4, XilinxFPGA, "in configure");

    if (initialConfiguration) {
        initialConfiguration = false;

        originalCap.length (configProperties.length ());

        for (unsigned int i = 0; i < configProperties.length (); i++) {
            originalCap[i].id = CORBA::string_dup (configProperties[i].id);
            originalCap[i].value = configProperties[i].value;
        }
    }

    PropertySet_impl::configure(configProperties);

    DEBUG(7, XilinxFPGA, "leaving configure");
}

CORBA::Boolean XilinxFPGA_i::allocateCapacity (const CF::Properties & capacities)
throw (CORBA::SystemException, CF::Device::InvalidCapacity, CF::Device::InvalidState)
{
    DEBUG(7, XilinxFPGA, "in allocateCapacity");

    CF::Properties currentCapacities;

    bool extraCap = false;	/// Flag to check remaining extra capacity to allocate
    bool foundProperty;		/// Flag to indicate if the requested property was found

    if (capacities.length() == 0) {
        // Nothing to do, return
        DEBUG(7, Device, "no capacities to configure.");
        return true;
    }

// Verify that the device is in a valid state
    if (!isUnlocked () || isDisabled ()) {
        std::cout << "Cannot allocate capacity: System is either LOCKED, SHUTTING DOWN, or DISABLED." << std::endl;
        throw (CF::Device::InvalidState("Cannot allocate capacity. System is either LOCKED, SHUTTING DOWN or DISABLED."));
    }
    if (!isBusy ()) {
        // The try is just a formality in this case
        try {
            // Get all properties currently in device
            query (currentCapacities);
        } catch (CF::UnknownProperties) {
        }

        /* Look in propertySet for the properties requested */
        for (unsigned i = 0; i < capacities.length (); i++) {
            foundProperty = false;

            for (unsigned j = 0; j < currentCapacities.length (); j++) {
                DEBUG(9, Device, "Comparing IDs: " << capacities[i].id << ", " << currentCapacities[j].id );
                if (strcmp (capacities[i].id, currentCapacities[j].id) == 0) {
                    // Verify that both values have the same type
                    if (!ORB_WRAP::isValidType (currentCapacities[j].value, capacities[i].value)) {
                        std::cout << "Cannot allocate capacity: Incorrect data type." << std::endl;
                        throw (CF::Device::InvalidCapacity("Cannot allocate capacity. Incorrect Data Type.", capacities));
                    } else {
                        // Check for sufficient capacity and allocate it
                        if (!allocate (currentCapacities[j].value, capacities[i].value)) {
                            std::cout << "Cannnot allocate capacity: Insufficient capacity." << std::endl;
                            throw (CF::Device::InvalidCapacity("Cannot allocate capacity. Insufficient capacity.", capacities));
                        }
                        CORBA::Short capValue;
                        currentCapacities[j].value >>= capValue;
                        DEBUG(7, Device, "Device Capacity ID: " << currentCapacities[j].id << ", New Capacity: " << capValue);
                    }

                    foundProperty = true;     // Report that the requested property was found
                    break;
                }
            }

            if (!foundProperty) {
                std::cout << "Cannot allocate capacity: Invalid property ID: " << capacities[i].id << "\n";
                throw (CF::Device::InvalidCapacity("Cannot allocate capacity. Invalid property ID", capacities));
            }
        }

        // Check for remaining capacity.
        for (unsigned i = 0; i < currentCapacities.length (); i++) {
            if (compareAnyToZero (currentCapacities[i].value) == POSITIVE) {
                extraCap = true;

                // No need to keep going. if after allocation there is any capacity available, the device is ACTIVE.
                break;
            }
        }

        // Store new capacities, here is when the allocation takes place
        configure (currentCapacities);

        /* Update usage state */
        if (!extraCap) {
            setUsageState (CF::Device::BUSY);
        } else {
            setUsageState (CF::Device::ACTIVE);   /* Assumes it allocated something. Not considering zero allocations */
        }

        return true;
    } else {
        /* Not sure */
        std::cout << "Cannot allocate capacity: System is BUSY" << std::endl;
        throw (CF::Device::InvalidCapacity ("Cannot allocate capacity. System is BUSY.", capacities));
    }
}


void XilinxFPGA_i::deallocateCapacity (const CF::Properties & capacities)
throw (CORBA::SystemException, CF::Device::InvalidCapacity, CF::Device::InvalidState)
{
    CF::Properties currentCapacities;

    bool totalCap = true;                         /* Flag to check remaining extra capacity to allocate */
    bool foundProperty;                           /* Flag to indicate if the requested property was found */
    AnyComparisonType compResult;

    /* Verify that the device is in a valid state */
    if (isLocked () || isDisabled ()) {
        throw (CF::Device::InvalidState("Cannot deallocate capacity. System is either LOCKED or DISABLED."));
        return;
    }

    /* Now verify that there is capacity currently being used */
    if (!isIdle ()) {
        query (currentCapacities);

        /* Look in propertySet for the properties requested */
        for (unsigned i = 0; i < capacities.length (); i++) {
            foundProperty = false;

            for (unsigned j = 0; j < currentCapacities.length (); j++) {
                if (strcmp (capacities[i].id, currentCapacities[j].id) == 0) {

                    // Verify that both values have the same type
                    if (!ORB_WRAP::isValidType (currentCapacities[j].value, capacities[i].value)) {
                        throw (CF::Device::InvalidCapacity("Cannot deallocate capacity. Incorrect Data Type.", capacities));
                    } else {
                        deallocate (currentCapacities[j].value, capacities[i].value);
                    }

                    foundProperty = true;     /* Report that the requested property was found */
                    break;
                }
            }

            if (!foundProperty) {
                throw (CF::Device::InvalidCapacity("Cannot deallocate capacity. Invalid property ID",	capacities));
            }
        }

        // Check for exceeding dealLocations and back-to-total capacity
        for (unsigned i = 0; i < currentCapacities.length (); i++) {
            for (unsigned j = 0; j < originalCap.length (); j++) {
                if (strcmp (currentCapacities[i].id, originalCap[j].id) == 0) {
                    compResult = compareAnys (currentCapacities[i].value, originalCap[j].value);

                    if (compResult == FIRST_BIGGER) {
                        throw (CF::Device::InvalidCapacity("Cannot deallocate capacity. New capacity would exceed original bound.", capacities));
                    } else if (compResult == SECOND_BIGGER) {
                        totalCap = false;
                        break;
                    }
                }
            }
        }

        /* Write new capacities */
        configure (currentCapacities);

        /* Update usage state */
        if (!totalCap) {
            setUsageState (CF::Device::ACTIVE);
        } else {
            setUsageState (CF::Device::IDLE);     /* Assumes it allocated something. Not considering zero allocations */
        }

        return;
    } else {
        /* Not sure */
        throw (CF::Device::InvalidCapacity ("Cannot deallocate capacity. System is IDLE.", capacities));
        return;
    }
}

bool XilinxFPGA_i::allocate (CORBA::Any & deviceCapacity, const CORBA::Any & resourceRequest)
{
    CORBA::TypeCode_var tc1 = deviceCapacity.type ();
    CORBA::TypeCode_var tc2 = resourceRequest.type ();

    switch (tc1->kind ()) {
    case CORBA::tk_ulong: {
        CORBA::ULong devCapac, rscReq;
        deviceCapacity >>= devCapac;
        resourceRequest >>= rscReq;

        if (rscReq <= devCapac) {
            devCapac -= rscReq;
            deviceCapacity <<= devCapac;
            return true;
        } else {
            return false;
        }
    }

    case CORBA::tk_long: {
        CORBA::Long devCapac, rscReq;
        deviceCapacity >>= devCapac;
        resourceRequest >>= rscReq;

        if (rscReq <= devCapac) {
            devCapac -= rscReq;
            deviceCapacity <<= devCapac;
            return true;
        } else {
            return false;
        }
    }

    case CORBA::tk_short: {
        CORBA::Short devCapac, rscReq;
        deviceCapacity >>= devCapac;
        resourceRequest >>= rscReq;

        if (rscReq <= devCapac) {
            devCapac -= rscReq;
            deviceCapacity <<= devCapac;
            return true;
        } else {
            return false;
        }
    }

    default:
        return false;
    }

//Should never reach this point
    return false;
}


void XilinxFPGA_i::deallocate (CORBA::Any & deviceCapacity, const CORBA::Any & resourceRequest)
{
    CORBA::TypeCode_var tc1 = deviceCapacity.type ();
    CORBA::TypeCode_var tc2 = resourceRequest.type ();

    switch (tc1->kind ()) {
    case CORBA::tk_ulong: {
        CORBA::ULong devCapac, rscReq;
        deviceCapacity >>= devCapac;
        resourceRequest >>= rscReq;
        devCapac += rscReq;
        deviceCapacity <<= devCapac;
        break;
    }

    case CORBA::tk_long: {
        CORBA::Long devCapac, rscReq;
        deviceCapacity >>= devCapac;
        resourceRequest >>= rscReq;
        devCapac += rscReq;
        deviceCapacity <<= devCapac;
        break;
    }

    case CORBA::tk_short: {
        CORBA::Short devCapac, rscReq;
        deviceCapacity >>= devCapac;
        resourceRequest >>= rscReq;
        devCapac += rscReq;
        deviceCapacity <<= devCapac;
        break;
    }

    default:
        break;
    }

    return;
}



