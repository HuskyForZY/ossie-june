ACLOCAL_AMFLAGS =
       
AM_CXXFLAGS = -Wall -I../../system/ossie/include -I../../system/ossie/include/ossie -I../../system/standardInterfaces/include \
	-lpulse -lpulse-simple -lACE

LDADD = ../../system/ossie/framework/libossiecf.la \
	../../system/ossie/parser/libossieparser.la \
	../../system/ossie/idl/cpp/libossieidl.la \
	../../system/standardInterfaces/libstandardInterfaces.la

bin_PROGRAMS = soundCard
soundCard_SOURCES =         \
    main.cpp                        \
    soundCard.h             \
    soundCard.cpp           \
    port_impl.cpp

BUILT_SOURCES =                     


    

ossieName = soundCard
xmldir = $(prefix)/dev/xml/$(ossieName)
bindir = $(prefix)/dev/bin

dist_xml_DATA =                     \
    soundCard.scd.xml       \
    soundCard.spd.xml       \
    soundCard.prf.xml


# ---------- documentation ---------

docs :
	doxygen Doxyfile

