/*
 * soundControl_p.cpp
 *
 *  Created on: Dec 23, 2011
 *      Author: June Kim
 */

#include <iostream>

#include <ossie/ossieSupport.h>

#include <standardinterfaces/soundControl_p.h>

standardInterfaces_i::audioOutControl_p::audioOutControl_p(const char* _name) : portName(_name)
{
    data_servant = new audioOutControl::providesPort(this);
    data_servant_var = data_servant->_this();
}

standardInterfaces_i::audioOutControl_p::audioOutControl_p(const char* _name, const char* _domain) : portName(_name)
{
    ossieSupport::ORB orb;

    data_servant = new audioOutControl::providesPort(this);
    data_servant_var = data_servant->_this();

    std::string objName;
    objName = _domain;
    objName += "/";
    objName += _name;

    orb.bind_object_to_name((CORBA::Object_ptr) data_servant_var, objName.c_str());
}

standardInterfaces_i::audioOutControl_p::~audioOutControl_p()
{

}

CORBA::Object_ptr standardInterfaces_i::audioOutControl_p::getPort(const char* _portName)
{
    if (portName == _portName)
        return CORBA::Object::_duplicate(data_servant_var);
    else
        return CORBA::Object::_nil();
}

audioOutControl::providesPort::providesPort(standardInterfaces_i::audioOutControl_p *_base) :
        base(_base)
{
}

audioOutControl::providesPort::~providesPort()
{
}


standardInterfaces_i::audioInControl_p::audioInControl_p(const char* _name) : portName(_name)
{
    data_servant = new audioInControl::providesPort(this);
    data_servant_var = data_servant->_this();
}

standardInterfaces_i::audioInControl_p::audioInControl_p(const char* _name, const char* _domain) : portName(_name)
{
    ossieSupport::ORB orb;

    data_servant = new audioInControl::providesPort(this);
    data_servant_var = data_servant->_this();

    std::string objName;
    objName = _domain;
    objName += "/";
    objName += _name;

    orb.bind_object_to_name((CORBA::Object_ptr) data_servant_var, objName.c_str());
}

standardInterfaces_i::audioInControl_p::~audioInControl_p()
{

}

CORBA::Object_ptr standardInterfaces_i::audioInControl_p::getPort(const char* _portName)
{
    if (portName == _portName)
        return CORBA::Object::_duplicate(data_servant_var);
    else
        return CORBA::Object::_nil();
}

audioInControl::providesPort::providesPort(standardInterfaces_i::audioInControl_p *_base) :
        base(_base)
{
}

audioInControl::providesPort::~providesPort()
{
}
