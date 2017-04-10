#include <iostream>
#include <cstdlib>
#include <string>
#include <thread>    // For sleep
#include <chrono>
#include <cstring>
#include "mqtt/async_client.h"

const std::string ADDRESS("tcp://localhost:1883");
const std::string CLIENTID("AsyncPublisher");
const std::string TOPIC("chen_mqtt");

const int  QOS = 0;
const long TIMEOUT = 1000000L;

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
