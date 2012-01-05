ACLOCAL_AMFLAGS =

%SK.cpp %.h :
	@IDL@ @IDL_FLAGS@ -bcxx -Wbh=.h -Wbs=SK.cpp -Wbkeep_inc_path soundControl.idl
       
AM_CXXFLAGS = -Wall -I../../system/ossie/include -I../../system/ossie/include/ossie -I../../system/standardInterfaces/include

LDADD = ../../system/ossie/framework/libossiecf.la \
	../../system/ossie/parser/libossieparser.la \
	../../system/ossie/idl/cpp/libossieidl.la \
	../../system/standardInterfaces/libstandardInterfaces.la

bin_PROGRAMS = soundCardPlayback
soundCardPlayback_SOURCES =         \
    main.cpp                        \
    soundCardPlayback.h             \
    soundCardPlayback.cpp           \
    port_impl.cpp

BUILT_SOURCES =                     \
   soundControlSK.cpp               

CLEANFILES =                        \
    soundControlSK.cpp              \
    soundControl.h                  

nodist_soundCardPlayback_SOURCES =  \
    soundControlSK.cpp              

ossieName = soundCardPlayback
xmldir = $(prefix)/dev/xml/$(ossieName)
bindir = $(prefix)/dev/bin

dist_xml_DATA =                     \
    soundCardPlayback.scd.xml       \
    soundCardPlayback.spd.xml       \
    soundCardPlayback.prf.xml

EXTRA_DIST = soundControl.idl

# ---------- documentation ---------

docs :
	doxygen Doxyfile

