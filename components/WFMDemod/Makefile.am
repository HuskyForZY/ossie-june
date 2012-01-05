ACLOCAL_AMFLAGS = -I m4

AM_CXXFLAGS = -Wall -I../../system/ossie/include -I../../system/ossie/include/ossie -I../../system/standardInterfaces/include

LDADD = ../../system/ossie/framework/libossiecf.la \
	../../system/ossie/parser/libossieparser.la \
	../../system/ossie/idl/cpp/libossieidl.la \
	../../system/standardInterfaces/libstandardInterfaces.la

bin_PROGRAMS = WFMDemod

WFMDemod_SOURCES = WFMDemod.cpp WFMDemod.h main.cpp

ossieName = WFMDemod
xmldir = $(prefix)/dom/xml/$(ossieName)
bindir = $(prefix)/dom/bin
dist_xml_DATA = WFMDemod.prf.xml WFMDemod.scd.xml \
WFMDemod.spd.xml 

