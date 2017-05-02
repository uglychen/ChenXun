

CC = g++ -g -Wall -lpthread -DWITH_OPENSSL
CPPFLAGS =  -DWITH_NONAMESPACES 

all : cgi_client_multi.cpp
	$(CC) $(MQFLAGS) -o cgi_client_multi cgi_client_multi.cpp soapC.cpp soapClient.cpp -lgsoapssl++  -lssl -lcrypto

#-lgsoap++ 
clean:
	rm -f *.o *.xml *.a *.wsdl *.nsmap soapH.h cgi_client_multi ns.xsd soapC.cpp soapClient.cpp soapClientLib.cpp soapStub.h
