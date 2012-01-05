ACLOCAL_AMFLAGS = 
AM_CXXFLAGS = -Wall -I../../system/ossie/include -I../../system/ossie/include/ossie -I../../system/standardInterfaces/include

LDADD = ../../system/ossie/framework/libossiecf.la \
	../../system/ossie/parser/libossieparser.la \
	../../system/ossie/idl/cpp/libossieidl.la \
	../../system/standardInterfaces/libstandardInterfaces.la

bin_PROGRAMS = soundCardCapture

soundCardCapture_SOURCES =          \
    main.cpp                        \
    soundCardCapture.cpp            \
    soundCardCapture.h

ossieName = soundCardCapture
xmldir = $(prefix)/dev/xml/$(ossieName)
bindir = $(prefix)/dev/bin

dist_xml_DATA =                     \
    soundCardCapture.scd.xml        \
    soundCardCapture.spd.xml        \
    soundCardCapture.prf.xml


# ---------- documentation ---------

docs :
	doxygen Doxyfile


