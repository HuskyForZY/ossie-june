ACLOCAL_AMFLAGS = -I m4

AM_CXXFLAGS = -Wall -I../../system/ossie/include

LDADD = ../../system/ossie/framework/libossiecf.la \
	../../system/ossie/parser/libossieparser.la \
	../../system/ossie/idl/cpp/libossieidl.la

ossieName = XilinxFPGA
esseName = XilinxFPGA

devbindir = $(prefix)/dev/bin
devbin_PROGRAMS = XilinxFPGA

xmldir = $(prefix)/dev/xml/$(ossieName)
dist_xml_DATA = XilinxFPGA.scd.xml XilinxFPGA.spd.xml XCV4FX12-FF668-10C.prf.xml \
                EthernetLite.prf.xml UARTLite.prf.xml


XilinxFPGA_SOURCES = main.cpp XilinxFPGA.cpp XilinxFPGA.h

