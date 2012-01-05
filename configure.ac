AC_PREREQ([2.61])
AC_INIT([ossie], [0.8.2])
AC_PREFIX_DEFAULT("/sdr")
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CXX

#AM_PATH_PYTHON([2.5])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SUBDIRS([system/ossie \
                   system/standardInterfaces \
                   system/nodebooter \
                   system/customInterfaces \
                   system/c_wavLoader \
                   system/SigProc \
                   platform/domain \
                   platform/dtd \
                   platform/Sound_in \
                   platform/Sound_out \
                   platform/USRP \
                   platform/USRP2 \
                   platform/XilinxFPGA \
                   platform/nodes/default_GPP_node \
                   platform/nodes/default_GPP_sound_node \
                   platform/nodes/default_GPP_USRP_node \
                   platform/nodes/default_GPP_USRP2_node \
                   platform/nodes/default_GPP_USRP_sound_node \
                   platform/nodes/default_ml403_node \
                   platform/GPP \
                   waveforms/ossie_demo \
                   waveforms/ml403_ossie_demo \
                   waveforms/OSSIETalkUSRP \
                   waveforms/OSSIETalkLoopBack \
                   waveforms/Lab5Example \
                   waveforms/pass_data_waveform \
                   components/am_demod \
                   components/amplifier \
                   components/AutomaticGainControl \
                   components/Channel \
                   components/ChannelDemo \
                   components/Conv_Dec \
                   components/Conv_Enc \
                   components/Decimator \
                   components/DigitalModem \
                   components/FrameAssembler \
                   components/Interpolator \
                   components/JPEG_VideoViewer \
                   components/OSSIETalk \
                   components/PacketResizer \
                   components/pass_data \
                   components/rc2007_gui \
                   components/RxDemo \
                   components/SymbolSyncPoly \
                   components/TxDemo \
                   components/USRP_Commander \
                   components/WebCamCapture \
                   components/WFMDemod \
                   components/writeBytestoFile \
                   components/readBytesfromFile \
                   tools])

AC_ARG_WITH([nbdir],
            [AS_HELP_STRING([--with-nodebooter], [install nodeBooter in prefix/bin (default: /usr/local/bin)])],
            [nbdir="$prefix/bin"],
            [nbdir="/usr/local/bin"])

AM_CONDITIONAL([WITHNB], [test "$with_nbdir" = yes])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

