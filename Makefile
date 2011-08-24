all: 
	gcc -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -fPIC -I/usr/include/python2.7 -c nbio.c -o build/nbio.o
	gcc -pthread -shared -Wl,-O1 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions build/nbio.o -o nbio.so -lpthread /lib/libNBioBSP.so
clean: 
	rm build/nbio.o
	rm nbio.so
