/*******************************************************************************
 Copyright 2009 Virginia Polytechnic Institute and State University

 This file is part of the OSSIE Parser.

 OSSIE Parser is free software; you can redistribute it and/or modify
 it under the terms of the Lesser GNU General Public License as published by
 the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.

 OSSIE Parser is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 Lesser GNU General Public License for more details.

 You should have received a copy of the Lesser GNU General Public License
 along with OSSIE Parser; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 Even though all code is original, the architecture of the OSSIE Parser is based
 on the architecture of the CRCs SCA Reference Implementation (SCARI)
 see: http://www.crc.ca/en/html/rmsc/home/sdr/projects/scari
*********************************************************************************/

#ifndef PROPERTYFILE_H
#define PROPERTYFILE_H

#include "ossie/ossieparser.h"
#include "ossie/debug.h"

#include <string>
#include <vector>

#include "tinyxml.h"

class OSSIEPARSER_API PropertyFile
{
public:
    PropertyFile(TiXmlElement *elem);
    ~PropertyFile();
    std::string getType();
    std::string getLocalFile();

protected:
    void parseElement(TiXmlElement *elem);
    void parseAttributes(TiXmlElement *elem);
    void parseChildElements(TiXmlElement *elem);

private:
    PropertyFile(); // no default constructor
    PropertyFile(const PropertyFile& aPropertyFile); // no copying

    std::string type;
    std::string localfile;

};

#endif


