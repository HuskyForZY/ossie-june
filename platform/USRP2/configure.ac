AC_PREREQ([2.61])
AC_INIT([USRP2], [0.8.1])
AC_PREFIX_DEFAULT("/sdr")
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
LT_INIT

AC_PROG_CXX
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

AC_HEADER_SYS_WAIT

AC_FUNC_FORK

AC_LANG_PUSH([C++])

AC_CHECK_LIB([omniORB4], [main], [], [AC_MSG_ERROR([cannot find omniORBi4 library])])

AC_CHECK_LIB([gromnithread], [main], [LIBGROMNITHREAD=true], [AC_CHECK_LIB([omnithread], [main], [LIBGROMNITHREAD=false], [AC_MSG_ERROR([cannot find gromnithread or omnithread libraries])])])
AM_CONDITIONAL([LINK_LIBGROMNITHREAD], [test x$LIBGROMNITHREAD = xtrue])
#AM_CONDITIONAL([LINK_LIBOMNITHREAD], [test x$LIBOMNITHREAD = xtrue])

AC_CHECK_LIB([omniDynamic4], [main], [], [AC_MSG_ERROR([cannot find omniDynamic4 library])])
AC_CHECK_HEADERS([omniORB4/CORBA.h], [], [AC_MSG_ERROR([cannot find omniORB4 header files])])

AX_BOOST_BASE([1.35])
CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
CFLAGS="$CFLAGS $BOOST_CPPFLAGS"

AC_CHECK_LIB([usrp2], [main], [], [AC_MSG_ERROR([cannot find usrp2 libraries])])
AC_CHECK_LIB([usrp], [main], [], [AC_MSG_ERROR([cannot find usrp libraries])])
AC_CHECK_HEADERS([db_base.h], [], [AC_MSG_ERROR([cannot find db_base header file])])
AC_CHECK_HEADERS([usrp2/usrp2.h], [], [AC_MSG_ERROR([cannot find usrp2 header file])])

#AC_CHECK_LIB([standardInterfaces], [main], [], [AC_MSG_ERROR([cannot find standardInterfaces])])
#AC_CHECK_HEADERS([standardinterfaces/complexShort.h], [], [AC_MSG_ERROR([cannot find standardInterfaces header files])])

AC_LANG_POP

export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig"

#PKG_CHECK_MODULES(OSSIE, ossie >= 0.6.0,,exit)
CXXFLAGS="$CXXFLAGS $OSSIE_CFLAGS"
#IDL_FLAGS="$OSSIE_CFLAGS"
#AC_SUBST(IDL_FLAGS)

#LIBS="$LIBS $OSSIE_LIBS"
LIBS="$LIBS"

AC_CONFIG_FILES(Makefile)

AC_OUTPUT
