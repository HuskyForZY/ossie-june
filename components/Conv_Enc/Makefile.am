ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -Wall -I../../system/ossie/include -I../../system/ossie/include/ossie -I../../system/standardInterfaces/include -I../../system/SigProc

LDADD = ../../system/ossie/framework/libossiecf.la \
	../../system/ossie/parser/libossieparser.la \
	../../system/ossie/idl/cpp/libossieidl.la \
	../../system/standardInterfaces/libstandardInterfaces.la \
	../../system/SigProc/libsigproc.la

bin_PROGRAMS = Conv_Enc

Conv_Enc_SOURCES = Conv_Enc.cpp Conv_Enc.h main.cpp

ossieName = Conv_Enc
xmldir = $(prefix)/dom/xml/$(ossieName)
bindir = $(prefix)/dom/bin
dist_xml_DATA = Conv_Enc.prf.xml Conv_Enc.scd.xml \
Conv_Enc.spd.xml 

