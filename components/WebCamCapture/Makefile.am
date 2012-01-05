ACLOCAL_AMFLAGS = -I m4

AM_CXXFLAGS = -Wall -I../../system/ossie/include -I../../system/ossie/include/ossie -I../../system/standardInterfaces/include

LDADD = ../../system/ossie/framework/libossiecf.la \
	../../system/ossie/parser/libossieparser.la \
	../../system/ossie/idl/cpp/libossieidl.la \
	../../system/standardInterfaces/libstandardInterfaces.la

bin_PROGRAMS = WebCamCapture
bindir = $(prefix)/dom/bin

WebCamCapture_SOURCES = WebCamCapture.cpp WebCamCapture.h utils.cpp utils.h main.cpp

ossieName = WebCamCapture
xmldir = $(prefix)/dom/xml/$(ossieName)
dist_xml_DATA = WebCamCapture.prf.xml WebCamCapture.scd.xml \
WebCamCapture.spd.xml 

EXTRA_DIST = jconfig.h \
	     jdatatype.h \
	     SDL_audioin.c \
	     SDL_audioin.h \
	     SDL_audioin_internal.h \
	     spcaframe.h \
	     spcav4l.c \
	     spcav4l.h
