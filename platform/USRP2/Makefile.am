ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -Wall -I../../system/ossie/include -I../../system/ossie/include/ossie -I../../system/standardInterfaces/include

LDADD = ../../system/ossie/framework/libossiecf.la \
        ../../system/ossie/parser/libossieparser.la \
        ../../system/ossie/idl/cpp/libossieidl.la \
        ../../system/standardInterfaces/libstandardInterfaces.la

if LINK_LIBGROMNITHREAD
  LIBS+=-lgromnithread
else
  LIBS+=-lomnithread
endif

devbindir = $(prefix)/dev/bin
devbin_PROGRAMS = USRP2

USRP2_SOURCES =                  \
    src/main.cpp                \
    src/USRP2_OSSIE.cpp                \
    src/port_impl.cpp           \
    src/USRP2_OSSIE.h 

ossieName = USRP2
xmldir = $(prefix)/dev/xml/$(ossieName)

dist_xml_DATA =                 \
    xml/USRP2.scd.xml            \
    xml/USRP2.spd.xml


# ---------- documentation ---------

# make sure "make docs" doesn't conflict with
# separate docs/ directory
.PHONY: docs documentation

docs :
	./genDocs.py


