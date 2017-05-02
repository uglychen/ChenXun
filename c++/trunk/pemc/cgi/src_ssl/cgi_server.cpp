#include <string>
#include "soapH.h"
#include "./gsoap/soapService.h"
#include "soap.nsmap"
#include <producer/producer.h>
#include <consumer/consumer.h>


int Service::request(xsd__string req, xsd__string *result)
{
	printf("recv request = %s \n", req.c_str());

	ActiveMqProducer* instance = ActiveMqProducer::getInstance();
	std::string brokerURI = "failover://(tcp://101.37.33.121:61616)";    //  101.37.33.121   127.0.0.1
	instance->initProducer(brokerURI);
	std::string destURI = "cgi_pemc_mid_trade_req";
	instance->sendMessage(destURI, req);
	//printf("send over.\n");	

	*result = "test";
    return SOAP_OK;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("usage: %s /n", argv[0]);
        return -1;
    }

	activemq::library::ActiveMQCPP::initializeLibrary();

	Service request_service;
    if (request_service.run(atoi(argv[1])))
    {
        printf( "add service run failed\n" );
        return -1;
    }

	activemq::library::ActiveMQCPP::shutdownLibrary();
    return 0;
}
