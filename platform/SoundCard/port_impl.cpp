/****************************************************************************

Copyright 2006 Virginia Polytechnic Institute and State University

This file is part of the OSSIE Sound_out Device.

OSSIE Sound_out Device is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE Sound_out Device is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE Sound_out Device; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


****************************************************************************/

/** \file
    The port_impl.h file contains definitions for the port implementations.
*/


#include <iostream>
#include <omnithread.h>

#include <unistd.h>

#include <ossie/debug.h>

#include "soundCard.h"



audioOutControl_i::audioOutControl_i(SoundCard_i* Card,const char* portName, const char* domain):
	standardInterfaces_i::audioOutControl_p(portName, domain),soundCard(Card)
{
	DEBUG(3, SoundCard, "name of audioOutControl was registerd");
}

void audioOutControl_i::set_sample_rate(::CORBA::ULong rate)
{
	DEBUG(3, audioOutControl_i, "set_sample_rate invoked")
	soundCard->set_play_rate(rate);
}
void audioOutControl_i::set_channels(::CORBA::UShort channels)
{
	soundCard->set_play_channels(channels);
}
void audioOutControl_i::set_connector(standardInterfaces::audioOutControl::OutType type)
{
	soundCard->set_play_connector(type);
}

void audioOutControl_i::set_network_address(const char* address)
{
	soundCard->set_play_network_address(address);
}
void audioOutControl_i::set_file_name(const char* name)
{
	soundCard->set_play_file_name(name);
}

void audioOutControl_i::start()
{
	soundCard->start_play();
}
void audioOutControl_i::stop()
{
	soundCard->stop_play();
}
void audioOutControl_i::mute(::CORBA::Boolean enable)
{
	soundCard->mute_play(enable);
}



audioInControl_i::audioInControl_i(SoundCard_i* Card,const char* portName, const char* domain):
	standardInterfaces_i::audioInControl_p(portName, domain),soundCard(Card)
{
	DEBUG(3, SoundCard, "name of audioInControl was registerd");
}

void audioInControl_i::set_sample_rate(::CORBA::ULong rate)
{
	DEBUG(3, audioOutControl_i, "set_sample_rate invoked")
	soundCard->set_capture_rate(rate);
}
void audioInControl_i::set_channels(::CORBA::UShort channels)
{
	soundCard->set_capture_channels(channels);
}
void audioInControl_i::set_connector(standardInterfaces::audioInControl::InType type)
{
	soundCard->set_capture_connector(type);
}

void audioInControl_i::set_network_address(const char* address)
{
	soundCard->set_capture_network_address(address);
}
void audioInControl_i::set_file_name(const char* name)
{
	soundCard->set_capture_file_name(name);
}

void audioInControl_i::start()
{
	soundCard->start_capture();
}
void audioInControl_i::stop()
{
	soundCard->stop_capture();
}

void audioInControl_i::set_frame_size(::CORBA::ULong length)
{
	soundCard->set_capture_frame_size(length);
}


