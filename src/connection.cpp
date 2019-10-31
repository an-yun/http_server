#include "connection.h"

Connection::Connection()
:buff(Connection::max_len,' '),request()
{
}

const Request &Connection::get_request()
{
    return request;
}

size_t Connection::send(const char *content, size_t len)
{
    return ::send(client_st, content, len, 0);
}

size_t Connection::send(const std::string &content)
{
    return send(content.c_str(), content.length());
}

std::string Connection::receive(size_t len)
{
    if(len > buff.length()) buff.resize(len+16);
    size_t n = ::recv(client_st, buff.data(), len, 0);
    return buff.substr(0,n);
}

std::string Connection::get_client_ip()
{
    return inet_ntoa(client_address.sin_addr);;
}

bool Connection::close()
{
    ::close(client_st);
    return true;
}
Connection::~Connection()
{
    close();
}