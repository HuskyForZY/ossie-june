ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -Wall -I../../system/ossie/include -I../../system/ossie/include/ossie -I../../system/standardInterfaces/include -I../../system/SigProc

LDADD = ../../system/ossie/framework/libossiecf.la \
	../../system/ossie/parser/libossieparser.la \
	../../system/ossie/idl/cpp/libossieidl.la \
	../../system/standardInterfaces/libstandardInterfaces.la \
	../../system/SigProc/libsigproc.la

bin_PROGRAMS = AutomaticGainControl

AutomaticGainControl_SOURCES =          \
    src/AutomaticGainControl.cpp        \
    src/AutomaticGainControl.h          \
    src/main.cpp

ossieName = AutomaticGainControl
xmldir = $(prefix)/dom/xml/$(ossieName)
bindir = $(prefix)/dom/bin

dist_xml_DATA =                         \
    xml/AutomaticGainControl.prf.xml    \
    xml/AutomaticGainControl.scd.xml    \
    xml/AutomaticGainControl.spd.xml

# ---------- documentation ---------

# make sure "make docs" doesn't conflict with
# separate docs/ directory
.PHONY: docs documentation

docs :
	./genDocs.py


