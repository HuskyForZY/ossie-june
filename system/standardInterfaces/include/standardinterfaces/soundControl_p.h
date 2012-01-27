/*
 * soundControl_p.h
 *
 *  Created on: Dec 23, 2011
 *      Author: June Kim
 */

#ifndef SOUNDCONTROL_P_H_
#define SOUNDCONTROL_P_H_

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
	class providesPort;
}

namespace standardInterfaces_i
{

	class audioOutControl_p
	{
		friend class audioOutControl::providesPort;

	public:
		audioOutControl_p(const char* portName);
		audioOutControl_p(const char* portName, const char* domain);
		virtual ~audioOutControl_p() = 0;

		CORBA::Object_ptr getPort(const char* portName);

	    virtual void set_sample_rate(::CORBA::ULong rate) = 0;
	    virtual void set_channels(::CORBA::UShort channels) = 0;
	    virtual void set_connector(standardInterfaces::audioOutControl::OutType type) = 0;
	    virtual void set_network_address(const char* address) = 0;
	    virtual void set_file_name(const char* name) = 0;
	    virtual void start() = 0;
	    virtual void stop() = 0;
	    virtual void mute(::CORBA::Boolean enable) = 0;
	    virtual void reset() = 0;




	private:
		audioOutControl_p();
		audioOutControl_p(const audioOutControl_p &);

		std::string portName;

	// Provides port
		audioOutControl::providesPort *data_servant;
		standardInterfaces::audioOutControl_var data_servant_var;


	};

}

namespace audioOutControl
{

	class providesPort : public virtual POA_standardInterfaces::audioOutControl
	{
	public:
		providesPort(standardInterfaces_i::audioOutControl_p *_base);
		~providesPort();

		void set_sample_rate(::CORBA::ULong rate){
			base->set_sample_rate(rate);
		}
		void set_channels(::CORBA::UShort channels){
			base->set_channels(channels);
		}
		void set_connector(standardInterfaces::audioOutControl::OutType type){
			base->set_connector(type);
		}
	    	void set_network_address(const char* address){
			base->set_network_address(address);
		}
	    	void set_file_name(const char* name){
			base->set_file_name(name);
		}
	    	void start(){
	    		base->start();
	    	}
	    	void stop(){
	    		base->stop();
	    	}
	    	void mute(::CORBA::Boolean enable){
	    		base->mute(enable);
	    	}

	    	void reset(){
	    		base->reset();
	    	}


	private:
		standardInterfaces_i::audioOutControl_p *base;
	};

}

namespace audioInControl
{
	class providesPort;
}

namespace standardInterfaces_i
{

	class audioInControl_p
	{
		friend class audioInControl::providesPort;

	public:
		audioInControl_p(const char* portName);
		audioInControl_p(const char* portName, const char* domain);
		virtual ~audioInControl_p() = 0;

		CORBA::Object_ptr getPort(const char* portName);

	    virtual void set_sample_rate(::CORBA::ULong rate) = 0;
	    virtual void set_channels(::CORBA::UShort channels) = 0;
	    virtual void set_connector(standardInterfaces::audioInControl::InType type) = 0;
	    virtual void set_network_address(const char* address) = 0;
	    virtual void set_file_name(const char* name) = 0;
	    virtual void start() = 0;
	    virtual void stop() = 0;
	    virtual void set_frame_size(::CORBA::ULong length) = 0;
	    virtual void reset() = 0;



	private:
		audioInControl_p();
		audioInControl_p(const audioInControl_p &);

		std::string portName;

	// Provides port
		audioInControl::providesPort *data_servant;
		standardInterfaces::audioInControl_var data_servant_var;


	};

}

namespace audioInControl
{

	class providesPort : public virtual POA_standardInterfaces::audioInControl
	{
	public:
		providesPort(standardInterfaces_i::audioInControl_p *_base);
		~providesPort();

		void set_sample_rate(::CORBA::ULong rate){
			base->set_sample_rate(rate);
		}
		void set_channels(::CORBA::UShort channels){
			base->set_channels(channels);
		}
		void set_connector(standardInterfaces::audioInControl::InType type){
			base->set_connector(type);
		}
	    	void set_network_address(const char* address){
			base->set_network_address(address);
		}
	    	void set_file_name(const char* name){
			base->set_file_name(name);
		}
	    void start(){
	    	base->start();
	    }
	    void stop(){
	    	base->stop();
	    }
	    void set_frame_size(::CORBA::ULong length){
	    	base->set_frame_size(length);
	    }
	    void reset(){
	    	base->reset();
	    }


	private:
		standardInterfaces_i::audioInControl_p *base;
	};

}

#endif /* SOUNDCONTROL_P_H_ */
