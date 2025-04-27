#include "websocket_handler.h"
#include <iostream>
#include <boost/asio/strand.hpp>

WebSocketHandler::WebSocketHandler(int port)
    : port_(port),
      acceptor_(io_context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
      running_(false)
{
    message_handler_ = [](const std::string&) {}; // Default empty handler
}

WebSocketHandler::~WebSocketHandler()
{
    running_ = false;
    io_context_.stop();
    if (thread_ && thread_->joinable()) {
        thread_->join();
    }
}

void WebSocketHandler::start()
{
    if (!running_) {
        running_ = true;
        do_accept();
        
        // Run the io_context in a separate thread
        thread_ = std::make_unique<std::thread>([this]() {
            try {
                io_context_.run();
            } catch (const std::exception& e) {
                std::cerr << "WebSocket error: " << e.what() << std::endl;
            }
        });
    }
}

void WebSocketHandler::do_accept()
{
    // Accept a new connection
    acceptor_.async_accept(
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
            if (!ec) {
                on_accept(ec, std::move(socket));
            }
            
            // Accept another connection if still running
            if (running_) {
                do_accept();
            }
        });
}

void WebSocketHandler::on_accept(boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
{
    if (ec) {
        std::cerr << "Accept error: " << ec.message() << std::endl;
        return;
    }
    
    // Launch a new session for this connection
    std::thread([this, s = std::move(socket)]() mutable {
        try {
            // Wrap the socket in a websocket stream
            boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws(std::move(s));
            
            // Accept the websocket handshake
            ws.accept();
            
            // Handle the session
            run_session(std::move(ws));
        } catch (const std::exception& e) {
            std::cerr << "Session error: " << e.what() << std::endl;
        }
    }).detach();
}

void WebSocketHandler::run_session(boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws)
{
    // Buffer for reading
    boost::beast::flat_buffer buffer;
    
    // Keep reading messages until an error occurs
    while (running_) {
        try {
            // Read a message
            buffer.clear();
            ws.read(buffer);
            
            // Convert to string
            std::string message(boost::beast::buffers_to_string(buffer.data()));
            
            // Call the message handler
            message_handler_(message);
        } catch (const boost::system::system_error& e) {
            if (e.code() == boost::beast::websocket::error::closed) {
                // Normal closure
                break;
            }
            std::cerr << "Read error: " << e.what() << std::endl;
            break;
        }
    }
}

void WebSocketHandler::send(const std::string& data)
{
    // This is a simple implementation that doesn't track connections
    // In a real application, you'd want to store active connections and send to all or specific ones
    
    // For now, just log that we're trying to send
    std::cout << "Would send: " << data << std::endl;
}

void WebSocketHandler::set_message_handler(std::function<void(const std::string&)> handler)
{
    message_handler_ = handler;
}