#include <iostream>
#include "netClient.h"
#include "mqttClient.h"

using namespace std;

int main()
{
    int port = 10088;
    int port2 = 5188;
    std::string ip = "183.62.250.18";
    std::string ip2 = "172.0.0.1";

    NetClient* ClientHandler = new NetClient();
    ClientHandler->initClient(ip, port);
    ClientHandler->netSocket();
    ClientHandler->netConnect();

    //init mqtt client
    mqtt::async_client client(ADDRESS, CLIENTID);
    callback cb;
    client.set_callback(cb);
    mqtt::itoken_ptr conntok = client.connect();

    try{
        std::cout << "the mqtt waiting for the connection..." << std::flush;
        conntok->wait_for_completion();
        std::cout << "OK" << std::endl;
    }catch (const mqtt::exception& exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
    }
    
    std::string test_message = "this is a test meesage...";
    mqtt::message_ptr pubmsg = std::make_shared<mqtt::message>(test_message);
    pubmsg->set_qos(QOS);
    client.publish(TOPIC, pubmsg)->wait_for_completion(TIMEOUT);

    bool log_flag = ClientHandler->verifyLogin();
    std::string str="";
    while(log_flag)
    {
        ClientHandler->recvData( str, 100 );
        str="";
    }

    std::vector<mqtt::idelivery_token_ptr> toks = client.get_pending_delivery_tokens();
    if (!toks.empty())
    {
        std::cout << "Error: There are pending delivery tokens!" << std::endl;
    }
    
    // mqtt disconnect
    std::cout << "Disconnecting..." << std::flush;
    conntok = client.disconnect();
    conntok->wait_for_completion();
    std::cout << "OK" << std::endl;

    delete ClientHandler;
    return 0;


}

