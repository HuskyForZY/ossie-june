ACLOCAL_AMFLAGS =
AM_CXXFLAGS = -Wall -I../../system/ossie/include -I../../system/ossie/include/ossie -I../../system/standardInterfaces/include -I../../system/SigProc

LDADD = ../../system/ossie/framework/libossiecf.la \
	../../system/ossie/parser/libossieparser.la \
	../../system/ossie/idl/cpp/libossieidl.la \
	../../system/standardInterfaces/libstandardInterfaces.la \
	../../system/SigProc/libsigproc.la

bin_PROGRAMS = Conv_Dec

Conv_Dec_SOURCES = Conv_Dec.cpp Conv_Dec.h main.cpp

ossieName = Conv_Dec
xmldir = $(prefix)/dom/xml/$(ossieName)
bindir = $(prefix)/dom/bin
dist_xml_DATA = Conv_Dec.prf.xml Conv_Dec.scd.xml \
Conv_Dec.spd.xml 

