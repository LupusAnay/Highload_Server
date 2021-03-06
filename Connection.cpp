//
// Created by lupusanay on 27.12.17.
//

#include "Connection.h"

Connection::Connection(io_service &service)
        : connectionStrand(service), connectionSocket(service) {
}

ip::tcp::socket &Connection::getSocket() {
    return connectionSocket;
}

void Connection::start() {
    connectionSocket.async_read_some(buffer(charBuffer), connectionStrand.wrap(
            boost::bind(&Connection::handleRead, shared_from_this(), _1, _2)));
}

void Connection::handleRead(const boost::system::error_code &error, size_t bytes) {
    HTTPHandler handler(string(charBuffer.data()));

    string response = handler.getResponse();

    async_write(connectionSocket, buffer(response),
                connectionStrand.wrap(boost::bind(&Connection::handleWrite, shared_from_this(), _1)));
}

void Connection::handleWrite(boost::system::error_code const &error) {
    if (error) {
        cout << "Error in write: " << error << endl;
    }
    boost::system::error_code code;
    connectionSocket.shutdown(ip::tcp::socket::shutdown_both, code);
}
