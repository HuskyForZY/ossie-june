AM_CXXFLAGS = -Wall 

AM_CXXFLAGS = -Wall -I../../system/ossie/include -I../../system/ossie/include/ossie -I../../system/standardInterfaces/include

LDADD = ../../system/ossie/framework/libossiecf.la \
	../../system/ossie/parser/libossieparser.la \
	../../system/ossie/idl/cpp/libossieidl.la \
	../../system/standardInterfaces/libstandardInterfaces.la

bin_PROGRAMS = readBytesfromFile

readBytesfromFile_SOURCES = readBytesfromFile.cpp readBytesfromFile.h main.cpp

ossieName = readBytesfromFile
xmldir = $(prefix)/dom/xml/$(ossieName)
bindir = $(prefix)/dom/bin
dist_xml_DATA = readBytesfromFile.prf.xml readBytesfromFile.scd.xml \
readBytesfromFile.spd.xml 

