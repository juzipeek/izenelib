/* 
 * File:   RawClient.hpp
 * Author: Paolo D'Apice
 *
 * Created on December 31, 2011, 2:46 PM
 */

#ifndef RAWCLIENT_HPP
#define	RAWCLIENT_HPP


#include "types.h"
#include <boost/asio.hpp> 
#include <exception>
#include <string>
#include <utility>


namespace ba = boost::asio;


namespace izenelib {
namespace net {
namespace sf1r {


/**
 * Client of the SF1 Driver.
 * Sends requests and get responses using the custom SF1 protocol:
 * - header
 * -- sequence number (4 bytes)
 * -- body length (4 bytes)
 * - body
 */
class RawClient : private boost::noncopyable {
public:
    
    /**
     * Creates the driver client.
     * @param service a reference to the IO service.
     * @param iterator a reference to the endpoint iterator
     * @throw boost::system::system_error if cannot connect.
     */
    RawClient(ba::io_service& service, 
              ba::ip::tcp::resolver::iterator& iterator);
    
    /// Destructor.
    ~RawClient();
    
    // TODO: keepalive
    
    /**
     * Checks the connection status.
     * @return true if connected, false otherwise.
     */
    bool isConnected() const {
        return socket.is_open();
    }
    
    /**
     * Send a request to SF1.
     * @param sequence request sequence number.
     * @param data request data.
     * @throw std::exception if errors occur
     */
    void sendRequest(const uint32_t& sequence, const std::string& data)
    throw(std::exception);
    
    /**
     * Get a response from SF1.
     * @returns a std::pair containg the sequence number of the corresponding 
     *          request and the response
     * @throw std::exception if errors occur
     */
    std::pair<uint32_t, std::string> getResponse()
    throw(std::exception);
    
private:

    ba::ip::tcp::socket socket;
    
};


}}} /* namespace izenelib::net::sf1r */

#endif	/* RAWCLIENT_HPP */