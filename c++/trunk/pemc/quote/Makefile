IDIR =  
LIBS = -Wl,-Bstatic -L/usr/local/lib -lzlog -Wl,-Bdynamic -L/usr/lib -L/usr/local/lib -L/lib64 -lm -pthread
CFLAGS = -DEPOLL -D_GNU_SOURCE -Wstrict-prototypes -Wdeclaration-after-statement -Wall -Wextra -Wno-sign-compare -Wpointer-arith -Wdeclaration-after-statement -Wno-format-zero-length -Wswitch-enum -Wenum-compare -Wunused-parameter -Wstrict-aliasing -Wbad-function-cast -Wcast-align -Wmissing-prototypes -Wmissing-field-initializers -Wno-unused -Wno-unused-parameter -std=gnu99
FILES = quote.c network.c recvBuffer.c list.c crc8.c
quote: quote.c 
	gcc $(CFLAGS) $(PKG_CONFIG) -g -o quote $(FILES) $(LIBS)

