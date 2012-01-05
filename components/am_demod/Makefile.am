ACLOCAL_AMFLAGS =
AM_CXXFLAGS = -Wall -I../../system/ossie/include -I../../system/ossie/include/ossie -I../../system/standardInterfaces/include

LDADD = ../../system/ossie/framework/libossiecf.la \
	../../system/ossie/parser/libossieparser.la \
	../../system/ossie/idl/cpp/libossieidl.la \
	../../system/standardInterfaces/libstandardInterfaces.la

bin_PROGRAMS = am_demod

am_demod_SOURCES = am_demod.cpp am_demod.h main.cpp

ossieName = am_demod
bindir = $(prefix)/dom/bin/
xmldir = $(prefix)/dom/xml/$(ossieName)
dist_xml_DATA = am_demod.prf.xml am_demod.scd.xml \
am_demod.spd.xml 



