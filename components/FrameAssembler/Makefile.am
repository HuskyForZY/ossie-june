ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -Wall -I../../system/ossie/include -I../../system/ossie/include/ossie -I../../system/standardInterfaces/include -I../../system/SigProc

LDADD = ../../system/ossie/framework/libossiecf.la \
	../../system/ossie/parser/libossieparser.la \
	../../system/ossie/idl/cpp/libossieidl.la \
	../../system/standardInterfaces/libstandardInterfaces.la \
	../../system/SigProc/libsigproc.la

bin_PROGRAMS = FrameAssembler
bindir = $(prefix)/dom/bin

# ---------- FrameAssembler ----------

FrameAssembler_SOURCES =                \
    src/FrameAssembler.cpp              \
    src/FrameAssemblerDSP.cpp           \
    src/PNCodes.cpp                     \
    src/main.cpp

ossieName1 = FrameAssembler
xml1dir = $(prefix)/dom/xml/$(ossieName1)
dist_xml1_DATA =                        \
    xml/FrameAssembler.prf.xml          \
    xml/FrameAssembler.scd.xml          \
    xml/FrameAssembler.spd.xml 

EXTRA_DIST = src/FrameAssembler.h \
	     src/FrameAssemblerDSP.h \
	     src/PNCodes.h

# ---------- autotest ----------

#check_PROGRAMS = cxxtest autotest
#autotest_SOURCES = autotest.cpp
#autotest_LDADD =                        \
#    FrameAssemblerDSP.o                 \
#    PNCodes.o

#TESTS = autotest

#cxxtest:
#	cxxtestgen.py --error-printer -o autotest.cpp   \
#	atsrc/FrameAssembler_testsuite.h

# ---------- documentation ---------

# make sure "make docs" doesn't conflict with
# separate docs/ directory
.PHONY: docs

docs :
	./genDocs.py


