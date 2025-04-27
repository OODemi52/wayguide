#ifndef ASIO_COMPATIBILITY_HPP
#define ASIO_COMPATIBILITY_HPP

#include <boost/version.hpp>
#include <boost/asio.hpp>

// Define compatibility layer for different Boost versions
namespace websocketpp {
namespace lib {
namespace asio {

#if BOOST_VERSION >= 106600 // Boost 1.66.0 and newer
    // io_service -> io_context
    using io_service = boost::asio::io_context;
    
    // strand implementation for newer Boost
    template <typename IoContext>
    class strand {
    private:
        boost::asio::strand<boost::asio::io_context::executor_type> impl;
    public:
        explicit strand(IoContext& io_context) 
            : impl(boost::asio::make_strand(io_context.get_executor())) {}
        
        template <typename Handler>
        auto wrap(Handler&& handler) -> decltype(boost::asio::bind_executor(impl, std::forward<Handler>(handler))) {
            return boost::asio::bind_executor(impl, std::forward<Handler>(handler));
        }
    };
    
    // Replacement for io_service::work
    template <typename IoContext>
    class io_work_guard {
    private:
        typename boost::asio::executor_work_guard<typename IoContext::executor_type> guard;
    public:
        explicit io_work_guard(IoContext& io_context)
            : guard(boost::asio::make_work_guard(io_context)) {}
    };
    
    // work definition for io_service
    class io_service_work {
    public:
        explicit io_service_work(io_service& ios) : io_context_(ios) {}
    private:
        io_service& io_context_;
    };
    
    // forward declare for TCP resolver
    namespace ip {
        class tcp {
        public:
            class resolver {
            public:
                using iterator = boost::asio::ip::tcp::resolver::results_type::iterator;
                
                class query {
                public:
                    query(const std::string& host, const std::string& service)
                        : host_(host), service_(service) {}
                    
                    const std::string& host() const { return host_; }
                    const std::string& service() const { return service_; }
                    
                private:
                    std::string host_;
                    std::string service_;
                };
            };
        };
    }
    
    // Helper for post operations
    template<typename IoContext, typename Handler>
    void post(IoContext* io_context, Handler&& handler) {
        boost::asio::post(*io_context, std::forward<Handler>(handler));
    }
    
    // Timer expiry functions
    template <typename Timer, typename Duration>
    void expires_from_now(Timer& timer, Duration duration) {
        timer.expires_after(duration);
    }
    
    template <typename Timer>
    auto expires_from_now(Timer& timer) -> decltype(timer.expiry() - std::chrono::steady_clock::now()) {
        return timer.expiry() - std::chrono::steady_clock::now();
    }
    
    // Helper to check if a duration is negative
    template <typename Duration>
    bool is_neg(Duration duration) {
        return duration.count() < 0;
    }
    
    // Socket options
    namespace socket_base {
        constexpr int max_connections = SOMAXCONN;
    }
    
#else
    // Use Boost's definitions directly for older versions
    using boost::asio::io_service;
    using boost::asio::strand;
    
    template <typename IoContext>
    class io_work_guard {
    private:
        std::shared_ptr<typename IoContext::work> work;
    public:
        explicit io_work_guard(IoContext& io_context)
            : work(std::make_shared<typename IoContext::work>(io_context)) {}
    };
    
    template<typename IoContext, typename Handler>
    void post(IoContext* io_context, Handler&& handler) {
        io_context->post(std::forward<Handler>(handler));
    }
    
    template <typename Timer, typename Duration>
    void expires_from_now(Timer& timer, Duration duration) {
        timer.expires_from_now(duration);
    }
    
    template <typename Timer>
    auto expires_from_now(Timer& timer) -> decltype(timer.expires_from_now()) {
        return timer.expires_from_now();
    }
    
    template <typename Duration>
    bool is_neg(Duration duration) {
        return duration.is_negative();
    }
#endif

} // namespace asio
} // namespace lib
} // namespace websocketpp

#endif // ASIO_COMPATIBILITY_HPP