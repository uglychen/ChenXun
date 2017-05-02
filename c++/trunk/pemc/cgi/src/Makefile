

# Project name.
WSNAME = cgi

# Where to find activemq code.
MQ_DIR = ../../../tools/activemq
MQFLAGS = -I$(MQ_DIR) -I/usr/local/ActiveMQ-CPP/include/activemq-cpp-3.9.4 -I/usr/local/apr/include/apr-1 -L/usr/local/ActiveMQ-CPP/lib -lactivemq-cpp

CC = g++ -g -Wall -lpthread -luuid
CPPFLAGS =  -DWITH_NONAMESPACES 

all : cgi_server_multi.cpp
	$(CC) $(MQFLAGS) -o cgi_server_multi cgi_server_multi.cpp soapC.cpp soapServer.cpp accept_thread.cpp proc_thread.cpp soap_buffer.cpp $(MQ_DIR)/producer/sync_producer.cpp -lgsoap++

clean:
	rm -f *.o *.xml *.a *.wsdl *.nsmap soapH.h $(WSNAME)Stub.* $(WSNAME)_server $(WSNAME)_test ns.xsd 
