#include <iostream>
#include <cstdlib>
#include <string>
#include <thread>   // For sleep
#include <chrono>
#include <cstring>
#include <fstream>

#include "mqtt/async_client.h"

#include "tcpRequest.h"

//const std::string ADDRESS("tcp://101.37.33.121:1883");
const std::string ADDRESS("tcp://localhost:1883");
const std::string CLIENTID("AsyncPublisher");
const std::string TOPIC("chen_mqtt");

const char* PAYLOAD1 = "Hello World!";
const char* PAYLOAD2 = "Hi there!";
const char* PAYLOAD3 = "Is anyone listening?";
const char* PAYLOAD4 = "Someone is always listening.";

const int  QOS = 1;
const long TIMEOUT = 10000L;

inline void sleep(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}


/**
 * A callback class for use with the main MQTT client.
 */
class callback : public virtual mqtt::callback
{
public:
    virtual void connection_lost(const std::string& cause) {
        std::cout << "\nConnection lost" << std::endl;
        if (!cause.empty())
            std::cout << "\tcause: " << cause << std::endl;
    }

    // We're not subscribed to anything, so this should never be called.
    virtual void message_arrived(const std::string& topic, mqtt::message_ptr msg) {}

    virtual void delivery_complete(mqtt::idelivery_token_ptr tok) {
        std::cout << "Delivery complete for token: " 
            << (tok ? tok->get_message_id() : -1) << std::endl;
    }
};

/////////////////////////////////////////////////////////////////////////////

/**
 * A base action listener.
 */
class action_listener : public virtual mqtt::iaction_listener
{
protected:
    virtual void on_failure(const mqtt::itoken& tok) {
        std::cout << "\n\tListener: Failure on token: " 
            << tok.get_message_id() << std::endl;
    }

    virtual void on_success(const mqtt::itoken& tok) {
        std::cout << "\n\tListener: Success on token: " 
            << tok.get_message_id() << std::endl;
    }
};

/////////////////////////////////////////////////////////////////////////////

/**
 * A derived action listener for publish events.
 */
class delivery_action_listener : public action_listener
{
    bool done_;

    virtual void on_failure(const mqtt::itoken& tok) {
        action_listener::on_failure(tok);
        done_ = true;
    }

    virtual void on_success(const mqtt::itoken& tok) {
        action_listener::on_success(tok);
        done_ = true;
    }

public:
    delivery_action_listener() : done_(false) {}
    bool is_done() const { return done_; }
};

std::string getDate()
{
    struct tm *t;
    time_t s;
    time(&s);
    t = localtime(&s);

    char str[32]={0};

    snprintf(str, sizeof(str),
                "%4d-%02d-%02d",
                t->tm_year+1900,
                t->tm_mon+1,
                t->tm_mday
                );
    return str;
}

std::string getDateAndTime()
{   
    struct tm *t;
    time_t s;
    time(&s);
    t = localtime(&s);
    char str[32]={0};
    snprintf(str, sizeof(str),
        "%4d-%02d-%02d %02d%02d%02d",
        t->tm_year+1900,
        t->tm_mon+1,
        t->tm_mday,
        t->tm_hour,
        t->tm_min,
        t->tm_sec
        );
        return str;
}


/////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
    mqtt::async_client client(ADDRESS, CLIENTID);
    
    callback cb;
    client.set_callback(cb);
    
    std::string date = getDate();
    std::cout << getDateAndTime() << std::endl;
    std::string filename = date +".log";
    std::ofstream out_file(filename, ios::app);
    
    try {
        mqtt::itoken_ptr conntok = client.connect();
        std::cout << "Waiting for the connection..." << std::flush;
        conntok->wait_for_completion();
        std::cout << "OK" << std::endl;

        int host_port= 10088;
        char host_name[] = "183.62.250.18";    //  183.62.250.18     
        int bytecount = 0;
        int hsock = 0;
        int buffer_len = 2048;

        string recvData = "";

        TcpRequest * tmpRequest = new TcpRequest(host_port, host_name, bytecount, hsock, buffer_len);
        tmpRequest->socketInit();

//        out_file << "chenxun" << std::endl;
        while(1)
        {  
            tmpRequest->tcpRecv(recvData);
            std::cout << "The received data:" << recvData << std::endl;
            out_file << getDateAndTime() << "[The received data:]" << recvData << std::endl << std::endl;;

            mqtt::message_ptr pubmsg = std::make_shared<mqtt::message>(recvData.c_str());
            pubmsg->set_qos(QOS);
            client.publish(TOPIC, pubmsg)->wait_for_completion(TIMEOUT);        
    
            std::cout << "Send message OK!" <<std::endl;
            recvData="";
        } 
        
//        client.publish(TOPIC, pubmsg)->wait_for_completion(TIMEOUT);        

        std::cout << "OK" << std::endl;


        std::cout << "Disconnecting..." << std::flush;
        conntok = client.disconnect();
        conntok->wait_for_completion();
        std::cout << "OK" << std::endl;
    }
    catch (const mqtt::exception& exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
        return 1;
    }

    return 0;
}




