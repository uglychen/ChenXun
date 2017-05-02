ECIOUS:%.cpp %.c %.C %.cc
	.SUFFIXES:
.SUFFIXES:  .c .o .cpp .ecpp .pc .ec .C .cc .cxx .d 


.cpp.o:
		#${HOME}/TradeInterfaceSDK/upversion-sh $<
		$(CXXCOMPILE) $(EXT_CPPFLAGS) -c -o $@   $<

.cc.o:
		$(CPPCOMPILE) $(EXT_CPPFLAGS) -c -o $@  $<

.cxx.o:
		$(CPPCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR)  $<

.c.o:
		#${HOME}/TradeInterfaceSDK/upversion-sh $<
		$(CCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR) $<

.C.o:
		$(CPPCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR) $

