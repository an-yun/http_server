#include "connection.h"

#ifdef TEST
Connection::Connection()
{
    printf("Connection default ok\n");
}
#endif

Connection::Connection(const std::string &web_root_path, int client_st, const sockaddr_in &client_address)
    : client_st(client_st),
     client_address(client_address),
     buff(Connection::max_len,' '),
     web_root_path(web_root_path)
{
    size_t n = receive();
    request = Request(buff.c_str(),n);
#ifdef TEST
    printf("Connection manual ok\n");
#endif
}

Connection::Connection(Connection &&con) noexcept
    : client_st(con.client_st),
      buff(std::move(con.buff)),
      request(std::move(con.request)),
      response(std::move(con.response)),
      web_root_path(std::move(con.web_root_path))
      
{
    client_address = con.client_address;
    con.client_st = -1;
#ifdef TEST
    printf("Connection move ok\n");
#endif
}

const Request &Connection::get_request()
{
    return request;
}

size_t Connection::response_to_client()
{
    return response.response_to(client_st, web_root_path, request.get_request_path());
}

size_t Connection::send(const char *content, size_t len)
{
    return ::send(client_st, content, len, 0);
}

size_t Connection::send(const std::string &content)
{
    return send(content.c_str(), content.length());
}

size_t Connection::receive(size_t len)
{
    if (len > buff.length())
        buff.resize(len + 16);
    return ::recv(client_st, (void *)buff.data(), len, 0);
}

std::string Connection::get_client_ip() const
{
    return inet_ntoa(client_address.sin_addr);
}

const std::string &Connection::get_request_path() const
{
    return request.get_request_path();
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