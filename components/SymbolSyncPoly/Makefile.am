ACLOCAL_AMFLAGS = -I m4
AM_CXXFLAGS = -Wall -I../../system/ossie/include -I../../system/ossie/include/ossie -I../../system/standardInterfaces/include -I../../system/SigProc


bin_PROGRAMS =                          \
    SymbolSyncPoly                      \
    MultirateSynchronizer               \
    FrameSynchronizer

bindir = $(prefix)/dom/bin

# ---------- SymbolSyncPoly ----------

SymbolSyncPoly_SOURCES =                \
    src/SymbolSyncPoly.cpp              \
    src/SymbolSyncPoly.h                \
    src/SymbolSyncPolyDSP.h             \
    src/SymbolSyncPolyDSP.cpp           \
    src/main_SymbolSyncPoly.cpp

SymbolSyncPoly_LDADD = ../../system/ossie/framework/libossiecf.la \
		       ../../system/ossie/parser/libossieparser.la \
		       ../../system/ossie/idl/cpp/libossieidl.la \
		       ../../system/standardInterfaces/libstandardInterfaces.la \
		       ../../system/SigProc/libsigproc.la

ossieName1 = SymbolSyncPoly
xml1dir = $(prefix)/dom/xml/$(ossieName1)
dist_xml1_DATA =                        \
    xml/SymbolSyncPoly.prf.xml          \
    xml/SymbolSyncPoly.scd.xml          \
    xml/SymbolSyncPoly.spd.xml


# ---------- MultirateSynchronizer ----------

MultirateSynchronizer_SOURCES =         \
    src/MultirateSynchronizer.cpp       \
    src/MultirateSynchronizer.h         \
    src/MultirateSynchronizerDSP.h      \
    src/MultirateSynchronizerDSP.cpp    \
    src/main_MultirateSynchronizer.cpp

MultirateSynchronizer_LDADD =           \
    SymbolSyncPolyDSP.o \
    ../../system/ossie/framework/libossiecf.la \
    ../../system/ossie/parser/libossieparser.la \
    ../../system/ossie/idl/cpp/libossieidl.la \
    ../../system/standardInterfaces/libstandardInterfaces.la \
    ../../system/SigProc/libsigproc.la

ossieName2 = MultirateSynchronizer
xml2dir = $(prefix)/dom/xml/$(ossieName2)
dist_xml2_DATA =                        \
    xml/MultirateSynchronizer.prf.xml   \
    xml/MultirateSynchronizer.scd.xml   \
    xml/MultirateSynchronizer.spd.xml


# ---------- FrameSynchronizer ----------

FrameSynchronizer_SOURCES =             \
    src/FrameSynchronizer.cpp           \
    src/FrameSynchronizer.h             \
    src/FrameSynchronizerDSP.h          \
    src/FrameSynchronizerDSP.cpp        \
    src/PNCodes.h                       \
    src/PNCodes.cpp                     \
    src/main_FrameSynchronizer.cpp

FrameSynchronizer_LDADD =               \
    SymbolSyncPolyDSP.o                 \
    MultirateSynchronizerDSP.o \
    ../../system/ossie/framework/libossiecf.la \
    ../../system/ossie/parser/libossieparser.la \
    ../../system/ossie/idl/cpp/libossieidl.la \
    ../../system/standardInterfaces/libstandardInterfaces.la \
    ../../system/SigProc/libsigproc.la

ossieName3 = FrameSynchronizer
xml3dir = $(prefix)/dom/xml/$(ossieName3)
dist_xml3_DATA =                        \
    xml/FrameSynchronizer.prf.xml       \
    xml/FrameSynchronizer.scd.xml       \
    xml/FrameSynchronizer.spd.xml


# ---------- autotest ----------

#check_PROGRAMS = cxxtest autotest
#autotest_SOURCES = autotest.cpp
#autotest_LDADD =                        \
#    SymbolSyncPolyDSP.o                 \
#    MultirateSynchronizerDSP.o          \
#    FrameSynchronizerDSP.o              \
#    PNCodes.o
#TESTS = autotest

#cxxtest:
#	cxxtestgen.py --error-printer -o autotest.cpp       \
#	autotest_sources/SymbolSyncPoly_testsuite.h         \
#	autotest_sources/MultirateSynchronizer_testsuite.h  \
#	autotest_sources/EstimateCarrierOffset_testsuite.h  \
#	autotest_sources/EstimateTimingOffset_testsuite.h   \
#	autotest_sources/FrameSynchronizer_testsuite.h

# ---------- documentation ---------

docs :
	./genDocs.py


