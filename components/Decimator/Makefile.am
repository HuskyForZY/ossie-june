ACLOCAL_AMFLAGS =
AM_CXXFLAGS = -Wall -O3 -g -I../../system/ossie/include -I../../system/ossie/include/ossie -I../../system/standardInterfaces/include -I../../system/SigProc

LDADD = ../../system/ossie/framework/libossiecf.la \
	../../system/ossie/parser/libossieparser.la \
	../../system/ossie/idl/cpp/libossieidl.la \
	../../system/standardInterfaces/libstandardInterfaces.la \
	../../system/SigProc/libsigproc.la

bin_PROGRAMS = Decimator
bindir = $(prefix)/dom/bin

Decimator_SOURCES = main.cpp Decimator.cpp Decimator.h

ossieName = Decimator
xmldir = $(prefix)/dom/xml/$(ossieName)
dist_xml_DATA = Decimator.scd.xml Decimator.spd.xml Decimator.prf.xml

