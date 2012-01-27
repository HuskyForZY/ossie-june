/*
 * soundControl_u.cpp
 *
 *  Created on: Dec 23, 2011
 *      Author: june
 */

#include <iostream>

#include <ossie/debug.h>

#include <standardinterfaces/soundControl_u.h>

standardInterfaces_i::audioOutControl_u::audioOutControl_u(const char* _name) : portName(_name)
{
    data_servant = new audioOutControl::usesPort(this);
    data_servant_var = data_servant->_this();
}


standardInterfaces_i::audioOutControl_u::~audioOutControl_u()
{

}

CORBA::Object_ptr standardInterfaces_i::audioOutControl_u::getPort(const char* _portName)
{
    if (portName == _portName)
        return CORBA::Object::_duplicate(data_servant_var);
    else
        return CORBA::Object::_nil();
}

void standardInterfaces_i::audioOutControl_u::set_sample_rate(::CORBA::ULong rate)
{
	DEBUG(3, audioOutControl_u, "set_sample_rate invoked")
    omni_mutex_lock l(port_mutex);
    if (!CORBA::is_nil(dest_port))
        dest_port->set_sample_rate(rate);
}
void standardInterfaces_i::audioOutControl_u::set_channels(::CORBA::UShort channels)
{
    omni_mutex_lock l(port_mutex);
    if (!CORBA::is_nil(dest_port))
        dest_port->set_channels(channels);
}
void standardInterfaces_i::audioOutControl_u::set_connector(standardInterfaces::audioOutControl::OutType type)
{
    omni_mutex_lock l(port_mutex);
    if (!CORBA::is_nil(dest_port))
        dest_port->set_connector(type);
}

void standardInterfaces_i::audioOutControl_u::set_network_address(const char* address)
{
    omni_mutex_lock l(port_mutex);
    if (!CORBA::is_nil(dest_port))
        dest_port->set_network_address(address);
}
void standardInterfaces_i::audioOutControl_u::set_file_name(const char* name)
{
    omni_mutex_lock l(port_mutex);
    if (!CORBA::is_nil(dest_port))
        dest_port->set_file_name(name);
}

void standardInterfaces_i::audioOutControl_u::start()
{
    omni_mutex_lock l(port_mutex);
    if (!CORBA::is_nil(dest_port))
        dest_port->start();
}
void standardInterfaces_i::audioOutControl_u::stop()
{
    omni_mutex_lock l(port_mutex);
    if (!CORBA::is_nil(dest_port))
        dest_port->stop();
}
void standardInterfaces_i::audioOutControl_u::mute(::CORBA::Boolean enable)
{
    omni_mutex_lock l(port_mutex);
    if (!CORBA::is_nil(dest_port))
        dest_port->mute(enable);
}

void standardInterfaces_i::audioOutControl_u::reset()
{
    omni_mutex_lock l(port_mutex);
    if (!CORBA::is_nil(dest_port))
        dest_port->reset();
}

audioOutControl::usesPort::usesPort(standardInterfaces_i::audioOutControl_u *_base) :
        base(_base)
{
}

audioOutControl::usesPort::~usesPort()
{
}

void audioOutControl::usesPort::connectPort(CORBA::Object_ptr connection, const char* _connectionID)
{
    omni_mutex_lock l(base->port_mutex);

    base->dest_port = standardInterfaces::audioOutControl::_narrow(connection);
    if (CORBA::is_nil(base->dest_port)) {
        std::cout <<"The port is not audioOutControl" << std::endl;
        return;
    }

    base->connectionID = _connectionID;
}

void audioOutControl::usesPort::disconnectPort(const char* _connectionID)
{
    omni_mutex_lock l(base->port_mutex);
    if (base->connectionID == _connectionID) {
    	base->dest_port->reset();
        base->dest_port = standardInterfaces::audioOutControl::_nil();
        base->connectionID.clear();
        return;
    }
    DEBUG(2, StandardInterfaces, "Attempted to disconnect non-existent port: " << _connectionID);


}

standardInterfaces_i::audioInControl_u::audioInControl_u(const char* _name) : portName(_name)
{
    data_servant = new audioInControl::usesPort(this);
    data_servant_var = data_servant->_this();
}


standardInterfaces_i::audioInControl_u::~audioInControl_u()
{

}

CORBA::Object_ptr standardInterfaces_i::audioInControl_u::getPort(const char* _portName)
{
    if (portName == _portName)
        return CORBA::Object::_duplicate(data_servant_var);
    else
        return CORBA::Object::_nil();
}

void standardInterfaces_i::audioInControl_u::set_sample_rate(::CORBA::ULong rate)
{
    omni_mutex_lock l(port_mutex);
    if (!CORBA::is_nil(dest_port))
        dest_port->set_sample_rate(rate);
}
void standardInterfaces_i::audioInControl_u::set_channels(::CORBA::UShort channels)
{
    omni_mutex_lock l(port_mutex);
    if (!CORBA::is_nil(dest_port))
        dest_port->set_channels(channels);
}
void standardInterfaces_i::audioInControl_u::set_connector(standardInterfaces::audioInControl::InType type)
{
    omni_mutex_lock l(port_mutex);
    if (!CORBA::is_nil(dest_port))
        dest_port->set_connector(type);
}

void standardInterfaces_i::audioInControl_u::set_network_address(const char* address)
{
    omni_mutex_lock l(port_mutex);
    if (!CORBA::is_nil(dest_port))
        dest_port->set_network_address(address);
}
void standardInterfaces_i::audioInControl_u::set_file_name(const char* name)
{
    omni_mutex_lock l(port_mutex);
    if (!CORBA::is_nil(dest_port))
        dest_port->set_file_name(name);
}
void standardInterfaces_i::audioInControl_u::start()
{
    omni_mutex_lock l(port_mutex);
    if (!CORBA::is_nil(dest_port))
        dest_port->start();
}
void standardInterfaces_i::audioInControl_u::stop()
{
    omni_mutex_lock l(port_mutex);
    if (!CORBA::is_nil(dest_port))
        dest_port->stop();
}

void standardInterfaces_i::audioInControl_u::set_frame_size(::CORBA::ULong length)
{
    omni_mutex_lock l(port_mutex);
    if (!CORBA::is_nil(dest_port))
        dest_port->set_frame_size(length);
}
void standardInterfaces_i::audioInControl_u::reset()
{
    omni_mutex_lock l(port_mutex);
    if (!CORBA::is_nil(dest_port))
        dest_port->reset();
}

audioInControl::usesPort::usesPort(standardInterfaces_i::audioInControl_u *_base) :
        base(_base)
{
}

audioInControl::usesPort::~usesPort()
{
}

void audioInControl::usesPort::connectPort(CORBA::Object_ptr connection, const char* _connectionID)
{
    omni_mutex_lock l(base->port_mutex);

    base->dest_port = standardInterfaces::audioInControl::_narrow(connection);
    if (CORBA::is_nil(base->dest_port)) {
        std::cout <<"The port is not audioInControl" << std::endl;
        return;
    }

    base->connectionID = _connectionID;

}

void audioInControl::usesPort::disconnectPort(const char* _connectionID)
{
    omni_mutex_lock l(base->port_mutex);
    if (base->connectionID == _connectionID) {
    	base->dest_port->reset();
        base->dest_port = standardInterfaces::audioInControl::_nil();
        base->connectionID.clear();
        return;
    }

    DEBUG(2, StandardInterfaces, "Attempt to disconnect non-existent connection: " << _connectionID);


}
