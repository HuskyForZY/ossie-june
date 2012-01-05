ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -Wall -I../../system/ossie/include -I../../system/ossie/include/ossie -I../../system/standardInterfaces/include -I../../system/SigProc

LDADD = ../../system/ossie/framework/libossiecf.la \
	../../system/ossie/parser/libossieparser.la \
	../../system/ossie/idl/cpp/libossieidl.la \
	../../system/standardInterfaces/libstandardInterfaces.la \
	../../system/SigProc/libsigproc.la

ossieName = Interpolator
dombindir = $(prefix)/dom/bin
dombin_PROGRAMS = Interpolator

xmldir = $(prefix)/dom/xml/$(ossieName)
dist_xml_DATA = Interpolator.prf.xml Interpolator.scd.xml \
Interpolator.spd.xml 

Interpolator_SOURCES = Interpolator.cpp Interpolator.h main.cpp

# --- documentation ---

docs :
	doxygen Doxyfile


