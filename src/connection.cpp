#include "connection.h"

Connection::Connection()
    : request(), client_st(-1), buff(Connection::max_len, ' ')
{
}

Connection::Connection(Connection &&con)
    : request(std::move(con.request)),
      request_path(std::move(con.request_path)),
      buff(std::move(con.buff))
{
    client_st = con.client_st;
    client_address = con.client_address;
    con.client_st = -1;
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
    if (len > buff.length())
        buff.resize(len + 16);
    size_t n = ::recv(client_st, (void *)buff.data(), len, 0);
    return buff.substr(0, n);
}

std::string Connection::get_client_ip() const
{
    return inet_ntoa(client_address.sin_addr);
}

std::string Connection::get_request_path() const
{
    return request_path;
}

bool Connection::close()
{
    if(client_st != -1)
        ::close(client_st);
    client_st = -1;
    return true;
}
Connection::~Connection()
{
    if (client_st != -1)
        close();
}