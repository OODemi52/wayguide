#ifndef WEBSOCKET_HANDLER_H
#define WEBSOCKET_HANDLER_H

#include <string>
#include <memory>
#include <functional>
#include <thread>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>

class WebSocketHandler {
public:
    WebSocketHandler(int port);
    ~WebSocketHandler();
    
    void start();
    void send(const std::string& data);
    
    // Set a callback for received messages
    void set_message_handler(std::function<void(const std::string&)> handler);
    
private:
    // Start accepting connections
    void do_accept();
    
    // Handle a new websocket session
    void on_accept(boost::system::error_code ec, boost::asio::ip::tcp::socket socket);
    
    // Run the message loop for a client
    void run_session(boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws);
    
    int port_;
    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::unique_ptr<std::thread> thread_;
    std::function<void(const std::string&)> message_handler_;
    bool running_;
};

#endif