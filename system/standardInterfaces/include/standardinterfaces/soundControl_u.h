/*
 * soundControl_u.h
 *
 *  Created on: Dec 23, 2011
 *      Author: June Kim
 */

#ifndef SOUNDCONTROL_U_H_
#define SOUNDCONTROL_U_H_

#include <string>
#include <vector>

#ifdef HAVE_OMNIORB4
#include "omniORB4/CORBA.h"
#endif

#include <omnithread.h>

#include <ossie/PortTypes.h>
#include <ossie/cf.h>

#include <standardinterfaces/soundControl.h>

// Forward declaration
namespace audioOutControl
{

	class usesPort;

}


namespace standardInterfaces_i
{

	class audioOutControl_u{
		friend class audioOutControl::usesPort;

	public:
		audioOutControl_u(const char* portName);
		virtual ~audioOutControl_u();

		CORBA::Object_ptr getPort(const char* portName);

	    void set_sample_rate(::CORBA::ULong rate);
	    void set_channels(::CORBA::UShort channels);
	    void set_connector(standardInterfaces::audioOutControl::OutType type);
	    void set_network_port(unsigned short port);
	    void set_file_name(const char* name);
	    void start();
	    void stop();
	    void mute(::CORBA::Boolean enable);

	private:
		audioOutControl_u();
		audioOutControl_u(const audioOutControl_u &);

		std::string portName;

	// Uses port
		audioOutControl::usesPort *data_servant;
		CF::Port_var data_servant_var;
		standardInterfaces::audioOutControl_var dest_port;
		std::string connectionID;
		omni_mutex port_mutex;


	};

}


namespace audioOutControl
{

	class usesPort : public virtual POA_CF::Port
	{
	public:
		usesPort(standardInterfaces_i::audioOutControl_u *_base);
		~usesPort();

		void connectPort(CORBA::Object_ptr connection, const char* connectionID);
		void disconnectPort(const char* connectionID);

	private:
		standardInterfaces_i::audioOutControl_u *base;
	};

}

namespace audioInControl
{

	class usesPort;

}


namespace standardInterfaces_i
{

	class audioInControl_u{
		friend class audioInControl::usesPort;

	public:
		audioInControl_u(const char* portName);
		virtual ~audioInControl_u();

		CORBA::Object_ptr getPort(const char* portName);

	    void set_sample_rate(::CORBA::ULong rate);
	    void set_channels(::CORBA::UShort channels);
	    void set_connector(standardInterfaces::audioInControl::InType type);
	    void set_network_port(unsigned short port);
	    void set_file_name(const char* name);
	    void start();
	    void stop();
	    void set_frame_size(::CORBA::ULong length);




	private:
		audioInControl_u();
		audioInControl_u(const audioInControl_u &);

		std::string portName;

	// Uses port
		audioInControl::usesPort *data_servant;
		CF::Port_var data_servant_var;
		standardInterfaces::audioInControl_var dest_port;
		std::string connectionID;
		omni_mutex port_mutex;


	};

}

namespace audioInControl
{

	class usesPort : public virtual POA_CF::Port
	{
	public:
		usesPort(standardInterfaces_i::audioInControl_u *_base);
		~usesPort();

		void connectPort(CORBA::Object_ptr connection, const char* connectionID);
		void disconnectPort(const char* connectionID);

	private:
		standardInterfaces_i::audioInControl_u *base;
	};

}

#endif /* SOUNDCONTROL_U_H_ */
