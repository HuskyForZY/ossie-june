ACLOCAL_AMFLAGS = -I m4

SUBDIRS = system/ossie \
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
	  components/RxDemo \
	  components/SymbolSyncPoly \
	  components/TxDemo \
	  components/USRP_Commander \
	  components/WebCamCapture \
	  components/WFMDemod \
          components/readBytesfromFile \
          components/writeBytestoFile \
	  tools

if WITHNB
 nbdir="$(prefix)/bin"
else
 nbdir="/usr/local/bin"
endif
dist-hook:
	-rm -rf `find $(distdir) -name .svn`

install-data-hook:
	-cp -rf $(prefix)/lib/* $(libdir)
	-rm -rf $(prefix)/lib
	-cp -rf $(prefix)/dom/xml/dtd/ $(prefix)/dev/xml/
	-mv $(prefix)/bin/nodeBooter $(nbdir)
	-mv $(prefix)/bin/c_wavLoader $(nbdir)
	-chown -R $(SUDO_USER).$(SUDO_USER) $(prefix)

uninstall-hook:
	-rm -rf $(prefix)/*
