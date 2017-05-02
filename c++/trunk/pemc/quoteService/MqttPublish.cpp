#include "MqttPublish.h"
#include "Product_def.h"

using namespace std;
MqttPublish::MqttPublish(const string& address, const string& clientId) :
	client(address, clientId),
	msgId(0)
{
}

MqttPublish::~MqttPublish()
{
}


void MqttPublish::setAddress(const string& address)
{

}


void MqttPublish::setClientId(const string& clientId)
{

}

void MqttPublish::init()
{
	client.set_callback(cb);
	std::cout << client.get_client_id() << endl;
	try 
	{
		conntok = client.connect();
		std::cout<< "Waiting for the connection..." << std::endl;
		conntok->wait_for_completion();
		std::cout << "connection OK" << std::endl;
	}
	catch (const mqtt::exception& exc)
	{
        dzlog_error(" mqtt connect error %s", exc.what());
	}

    if (conntok == NULL)
    {
        dzlog_error(" mqtt connect ´íÎó clientID : %s", client.get_client_id().c_str());
    }
}

void MqttPublish::uninit()
{
	// Double check that there are no pending tokens

	std::vector<mqtt::idelivery_token_ptr> toks = client.get_pending_delivery_tokens();
	if (!toks.empty())
		std::cout << "Error: There are pending delivery tokens!" << std::endl;

	// Disconnect
	std::cout << "Disconnecting..." << std::flush;
	conntok = client.disconnect();
	conntok->wait_for_completion();
	conntok = NULL;
	std::cout << "Disconnect OK" << std::endl;
}

void MqttPublish::publishMsg(const std::string& topic, const std::string& msg)
{
    dzlog_info("topic:%s, ID:%d", topic.c_str(), ++msgId);
    //cout << "topic:" << topic << "ID:" << ++msgId << "  " << msg << endl;
	if (msg.empty())
	{
		return;
	}
    //std::cout << "Sending message..." << std::flush;
	try
	{
		publishMsg1(topic, msg);
	}
	catch (const mqtt::exception& exc)
	{
        dzlog_error("mqtt error %s code:%d", exc.what(), exc.get_reason_code());
	}
}

void MqttPublish::publishMsg1(const std::string& topic, const std::string& msg)
{
	pubmsg = std::make_shared<mqtt::message>(msg);
	pubmsg->set_qos(QOS);
	client.publish(topic, pubmsg)->wait_for_completion(TIMEOUT);
}

void MqttPublish::publishMsg2(const std::string& topic, const std::string& msg)
{
	pubtok = client.publish(topic, msg.c_str(), std::strlen(msg.c_str()), QOS, false);
	pubtok->wait_for_completion(TIMEOUT);

}

void MqttPublish::publishMsg3(const std::string& topic, const std::string& msg)
{
	action_listener listener;
	pubmsg = std::make_shared<mqtt::message>(msg);
	pubtok = client.publish(topic, pubmsg, nullptr, listener);
	pubtok->wait_for_completion();
}

void MqttPublish::publishMsg4(const std::string& topic, const std::string& msg)
{
	delivery_action_listener deliveryListener;
	pubmsg = std::make_shared<mqtt::message>(msg);
	client.publish(topic, pubmsg, nullptr, deliveryListener);

	while (!deliveryListener.is_done()) {
		sleep(100);
	}
}
