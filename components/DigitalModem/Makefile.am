ACLOCAL_AMFLAGS =
AM_CXXFLAGS = -Wall -I../../system/ossie/include -I../../system/ossie/include/ossie -I../../system/standardInterfaces/include -I../../system/SigProc

bin_PROGRAMS =                      \
    DigitalDemodulator              \
    DigitalModulator

bindir = $(prefix)/dom/bin

# ---------- DigitalModulator ----------

DigitalModulator_SOURCES =          \
    src/DigitalModem.h              \
    src/DigitalModem.cpp            \
    src/DigitalModulator.cpp        \
    src/DigitalModulator.h          \
    src/main_mod.cpp

DigitalModulator_LDADD = ../../system/ossie/framework/libossiecf.la \
			 ../../system/ossie/parser/libossieparser.la \
			 ../../system/ossie/idl/cpp/libossieidl.la \
			 ../../system/standardInterfaces/libstandardInterfaces.la \
			 ../../system/SigProc/libsigproc.la

ossieName1 = DigitalModulator
xml1dir = $(prefix)/dom/xml/$(ossieName1)

dist_xml1_DATA =                    \
    xml/DigitalModulator.prf.xml    \
    xml/DigitalModulator.scd.xml    \
    xml/DigitalModulator.spd.xml 


# ---------- DigitalDemodulator ----------

DigitalDemodulator_SOURCES =        \
    src/DigitalDemodulator.h     	\
    src/DigitalDemodulator.cpp   	\
    src/main_demod.cpp

DigitalDemodulator_LDADD =          \
    DigitalModem.o \
    ../../system/ossie/framework/libossiecf.la \
    ../../system/ossie/parser/libossieparser.la \
    ../../system/ossie/idl/cpp/libossieidl.la \
    ../../system/standardInterfaces/libstandardInterfaces.la \
    ../../system/SigProc/libsigproc.la

ossieName2 = DigitalDemodulator
xml2dir = $(prefix)/dom/xml/$(ossieName2)

dist_xml2_DATA =                    \
    xml/DigitalDemodulator.prf.xml  \
    xml/DigitalDemodulator.scd.xml  \
    xml/DigitalDemodulator.spd.xml 

# ---------- autotest ----------

#check_PROGRAMS = cxxtest autotest
#autotest_SOURCES = autotest.cpp
#autotest_LDADD =                    \
#    DigitalModem.o
#TESTS = autotest

#cxxtest:
#	cxxtestgen.py --error-printer -o autotest.cpp   \
#	atsrc/DigitalModulator_testsuite.h              \
#	atsrc/DigitalDemodulator_testsuite.h

# ---------- documentation ---------

docs :
	./genDocs.py

