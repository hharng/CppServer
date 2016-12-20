/*!
    \file tcp_client.h
    \brief TCP client definition
    \author Ivan Shynkarenka
    \date 15.12.2016
    \copyright MIT License
*/

#ifndef CPPSERVER_ASIO_TCP_CLIENT_H
#define CPPSERVER_ASIO_TCP_CLIENT_H

#include "service.h"

#include "system/uuid.h"

#include <mutex>
#include <vector>

namespace CppServer {
namespace Asio {

//! TCP client
/*!
    TCP client is used to read and write data from the connected TCP server.

    Not thread-safe.
*/
class TCPClient : public std::enable_shared_from_this<TCPClient>
{
public:
    //! Initialize TCP client with a given IP address and port number
    /*!
        \param service - Asio service
        \param address - IP address
        \param port - Port number
    */
    explicit TCPClient(std::shared_ptr<Service> service, const std::string& address, int port);
    TCPClient(const TCPClient&) = delete;
    TCPClient(TCPClient&&) = default;
    virtual ~TCPClient() = default;

    TCPClient& operator=(const TCPClient&) = delete;
    TCPClient& operator=(TCPClient&&) = default;

    //! Get the Asio service
    std::shared_ptr<Service> service() noexcept { return _service; }

    //! Get the client Id
    const CppCommon::UUID& id() const noexcept { return _id; }

    //! Is the client connected?
    bool IsConnected() const noexcept { return _connected; };

    //! Connect the client
    /*!
        \return 'true' if the client was successfully connected, 'false' if the client failed to connect
    */
    bool Connect();
    //! Disconnect the client
    /*!
        \return 'true' if the client was successfully disconnected, 'false' if the client is already disconnected
    */
    bool Disconnect();

    //! Send data to the server
    /*!
        \param buffer - Buffer to send
        \param size - Buffer size
        \return Count of pending bytes in the send buffer
    */
    size_t Send(const void* buffer, size_t size);

protected:
    //! Handle client connected notification
    virtual void onConnected() {}
    //! Handle client disconnected notification
    virtual void onDisconnected() {}

    //! Handle buffer received notification
    /*!
        Notification is called when another chunk of buffer was received
        from the server.

        Default behavior is to handle all bytes from the received buffer.
        If you want to wait for some more bytes from the server return the
        size of the buffer you want to keep until another chunk is received.

        \param buffer - Received buffer
        \param size - Received buffer size
        \return Count of handled bytes
    */
    virtual size_t onReceived(const void* buffer, size_t size) { return size; }
    //! Handle buffer sent notification
    /*!
        Notification is called when another chunk of buffer was sent
        to the server.

        This handler could be used to send another buffer to the server
        for instance when the pending size is zero.

        \param sent - Size of sent buffer
        \param pending - Size of pending buffer
    */
    virtual void onSent(size_t sent, size_t pending) {}

    //! Handle error notification
    /*!
        \param error - Error code
        \param category - Error category
        \param message - Error message
    */
    virtual void onError(int error, const std::string& category, const std::string& message) {}

private:
    // Session Id
    CppCommon::UUID _id;
    // Asio service
    std::shared_ptr<Service> _service;
    // Client socket & endpoint
    asio::ip::tcp::socket _socket;
    asio::ip::tcp::endpoint _endpoint;
    std::atomic<bool> _connected;
    // Receive & send buffers
    std::mutex _send_lock;
    std::vector<uint8_t> _recive_buffer;
    std::vector<uint8_t> _send_buffer;
    bool _reciving;
    bool _sending;

    static const size_t CHUNK = 8192;

    //! Try to receive data
    void TryReceive();
    //! Try to send data
    void TrySend();
};

/*! \example tcp_chat_client.cpp TCP chat client example */

} // namespace Asio
} // namespace CppServer

#include "tcp_client.inl"

#endif // CPPSERVER_ASIO_TCP_CLIENT_H