ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -Wall -I../../system/ossie/include -I../../system/ossie/include/ossie -I../../system/standardInterfaces/include

LDADD = ../../system/ossie/framework/libossiecf.la \
	../../system/ossie/parser/libossieparser.la \
	../../system/ossie/idl/cpp/libossieidl.la \
	../../system/standardInterfaces/libstandardInterfaces.la

bin_PROGRAMS = JPEG_VideoViewer
bindir = $(prefix)/dom/bin

JPEG_VideoViewer_SOURCES = JPEG_VideoViewer.cpp JPEG_VideoViewer.h main.cpp utils.cpp utils.h

ossieName = JPEG_VideoViewer
xmldir = $(prefix)/dom/xml/$(ossieName)
dist_xml_DATA = JPEG_VideoViewer.prf.xml JPEG_VideoViewer.scd.xml \
JPEG_VideoViewer.spd.xml 

# NOTE: This is not a good way to distribute source files, but I can't find a better way
EXTRA_DIST = jconfig.h \
	     jdatatype.h \
	     SDL_audioin.h \
	     SDL_audioin.c \
	     SDL_audioin_internal.h
