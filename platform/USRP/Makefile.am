ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -Wall -I../../system/ossie/include -I../../system/ossie/include/ossie -I../../system/standardInterfaces/include
LDADD = ../../system/ossie/framework/libossiecf.la \
       ../../system/ossie/parser/libossieparser.la \
       ../../system/ossie/idl/cpp/libossieidl.la \
       ../../system/standardInterfaces/libstandardInterfaces.la

devbindir = $(prefix)/dev/bin
devbin_PROGRAMS = USRP

USRP_SOURCES =                  \
    src/main.cpp                \
    src/USRP.cpp                \
    src/port_impl.cpp           \
    src/USRP.h 

ossieName = USRP
xmldir = $(prefix)/dev/xml/$(ossieName)

dist_xml_DATA =                 \
    xml/USRP.scd.xml            \
    xml/USRP.spd.xml


# ---------- documentation ---------

# make sure "make docs" doesn't conflict with
# separate docs/ directory
.PHONY: docs documentation

docs :
	./genDocs.py


