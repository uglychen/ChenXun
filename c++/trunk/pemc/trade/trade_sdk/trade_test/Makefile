# TradeInterfaceSDKTest/Makefile.
#
# # Copyright (C) 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002,
# # 2003, 2004  Free Software Foundation, Inc.
# # This Makefile.in is free software; the Free Software Foundation
# # gives unlimited permission to copy and/or distribute it,
# # with or without modifications, as long as this notice is preserved.
#
# # This program is distributed in the hope that it will be useful,
# # but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# # even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# # PARTICULAR PURPOSE

CXX = g++
DEFAULT_INCLUDES = -I. 
CPPFLAGS =  -g -O2 -Wall -fPIC -fpic -m64
CXXFLAGS = -g -O2
CXXCOMPILE = $(CXX) $(DEFAULT_INCLUDES) $(INCLUDES) $(CPPFLAGS) $(CXXFLAGS)
SHELL = /bin/bash
LIB_CXXFLAGS=@LIB_CXXFLAGS@
LIB_LDFLAGS=-shared -fPIC -fpi

###################################################################################
#write your own rules here                                                        #
##################################################################################

PROJECT_BIN =  TradeInterfaceTest

PROJECT_OBJ = TradeClient.o TradeInterfaceSDKTest.o 
	     
		 PROJECT_SRC=$(PROJECT_OBJ:.o=.cpp)

EXT_CPPFLAGS = -I../include

EXT_LDFLAGS = -L../lib/ -lTradeInterfaceSDK -lpthread -lssl -lcrypto

all:$(PROJECT_BIN)

$(PROJECT_BIN):$(PROJECT_OBJ)
		$(CXXCOMPILE) $(EXT_CPPFLAGS) -o $@ $(PROJECT_OBJ) $(EXT_LDFLAGS) 

depend:$(PROJECT_SRC)
		makedepend  $(DEFAULT_INCLUDES) $(INCLUDES) $(EXT_CPPFLAGS) $(PROJECT_SRC) 2>/dev/null

check:
		clang --analyze -x c++ ${PROJECT_SRC} ${EXT_CPPFLAGS}
			
install:$(PROJECT_BIN)
		cp -f $(PROJECT_BIN) ./$(PROJECT_BIN)
clean:
		rm -f *.o 
			rm -f $(PROJECT_BIN)

include rules.mk
