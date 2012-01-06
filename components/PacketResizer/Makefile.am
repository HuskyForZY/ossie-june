ACLOCAL_AMFLAGS = 
AM_CXXFLAGS = -Wall -I../../system/ossie/include -I../../system/ossie/include/ossie -I../../system/standardInterfaces/include

LDADD = ../../system/ossie/framework/libossiecf.la \
	../../system/ossie/parser/libossieparser.la \
	../../system/ossie/idl/cpp/libossieidl.la \
	../../system/standardInterfaces/libstandardInterfaces.la

bin_PROGRAMS = PacketResizer

PacketResizer_SOURCES = PacketResizer.cpp PacketResizer.h main.cpp

ossieName = PacketResizer
xmldir = $(prefix)/dom/xml/$(ossieName)
bindir = $(prefix)/dom/bin
dist_xml_DATA = PacketResizer.prf.xml PacketResizer.scd.xml \
PacketResizer.spd.xml 

