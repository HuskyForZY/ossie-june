ACLOCAL_AMFLAGS =
AM_CXXFLAGS = -Wall -I../../system/ossie/include -I../../system/ossie/include/ossie -I../../system/standardInterfaces/include

LDADD = ../../system/ossie/framework/libossiecf.la \
	../../system/ossie/parser/libossieparser.la \
	../../system/ossie/idl/cpp/libossieidl.la \
	../../system/standardInterfaces/libstandardInterfaces.la

bin_PROGRAMS = USRP_Commander

# ---------- USRP_Commander ----------

USRP_Commander_SOURCES =                \
    src/USRP_Commander.cpp              \
    src/USRP_Commander.h                \
    src/main.cpp

ossieName = USRP_Commander
xmldir = $(prefix)/dom/xml/$(ossieName)
bindir = $(prefix)/dom/bin

dist_xml_DATA =                         \
    xml/USRP_Commander.prf.xml          \
    xml/USRP_Commander.scd.xml          \
    xml/USRP_Commander.spd.xml 

# ---------- documentation ---------

# make sure "make docs" doesn't conflict with
# separate docs/ directory
.PHONY: docs documentation

docs :
	./genDocs.py

