AM_CXXFLAGS = -Wall 

AM_CXXFLAGS = -Wall -I../../system/ossie/include -I../../system/ossie/include/ossie -I../../system/standardInterfaces/include

LDADD = ../../system/ossie/framework/libossiecf.la \
	../../system/ossie/parser/libossieparser.la \
	../../system/ossie/idl/cpp/libossieidl.la \
	../../system/standardInterfaces/libstandardInterfaces.la

bin_PROGRAMS = writeBytestoFile

writeBytestoFile_SOURCES = writeBytestoFile.cpp writeBytestoFile.h main.cpp

ossieName = writeBytestoFile
xmldir = $(prefix)/dom/xml/$(ossieName)
bindir = $(prefix)/dom/bin
dist_xml_DATA = writeBytestoFile.prf.xml writeBytestoFile.scd.xml \
writeBytestoFile.spd.xml 

