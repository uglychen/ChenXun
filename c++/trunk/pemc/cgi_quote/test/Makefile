

CC = g++ -g -Wall -lpthread
CPPFLAGS =  -DWITH_NONAMESPACES 

all : cgi_client_multi.cpp
	$(CC) $(MQFLAGS) -o quote_cgi_client_multi cgi_client_multi.cpp soapC.cpp soapClient.cpp  -I/usr/local/gSOAP/include   -L/usr/local/gSOAP/lib -lgsoap++

clean:
	rm -f *.o *.xml *.a *.wsdl *.nsmap soapH.h cgi_client_multi ns.xsd soapC.cpp soapClient.cpp soapClientLib.cpp soapStub.h
