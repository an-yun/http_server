#include "request.h"

Request::Request(const char *request_str, size_t n)
{
    if (request_str)
    {
        if (n == SIZE_MAX)
            n = std::strlen(request_str);
        parse_request(request_str, n);
    }
}

Request::Request(const string & requst_str)
{
    parse_request(requst_str.c_str(), requst_str.size());
}

std::string Request::get_request_path() const
{
    return request_path;
}
Request::size_t Request::parse_request(const char *request_str, size_t n)
{
    size_t at = 0;
    //解析请求方法
    if (std::strncmp("GET ", request_str, 4) == 0)
    {
        method = GET;
        at = 4;
    }
    else if (std::strncmp("POST ", request_str, 5) == 0)
    {
        method = POST;
        at = 5;
    }
    else
    {
        error_message = "requst type error!";
        method = OTHER;
        while (at < n && request_str[at] != ' ')
            ++at;
    }
    //解析请求路径
    if(request_str[at]== '/')
        ++at;
    size_t url_start = at;
    while (at < n && request_str[at] != ' ' && request_str[at] != '?')
        ++at;
    request_path = string(request_str + url_start, request_str + at);
    //解析请求参数
    if (request_str[at++] == '?')
    {
        while (request_str[at] != ' ')
        {
            auto ch = request_str[at];
            if (ch == '&')
                ++at;
            else
            {
                size_t name_start = at;
                while ((ch = request_str[at]) != '&' && ch != '=' && ch != ' ')
                    ++at;
                if (name_start == at) //para_str start with '='
                {
                    error_message = "parameter format error";
                    while (request_str[++at] != ' ')
                        ;
                    break;
                }
                string para_name(request_str + name_start, request_str + at);
                if (ch == '=')
                {
                    size_t value_start = ++at;
                    while ((ch = request_str[at]) != '&' && ch != '=' && ch != ' ')
                        ++at;
                    request_parameter[para_name] = string(request_str + value_start, request_str + at);
                }
            }
        }
    }
    //解析HTTP类别
    if (std::strncmp("HTTP/", ++at + request_str, 5) == 0)
    {
        auto http_start = at += 5;
        while (request_str[at] != ' ' && request_str[at] != '\r' && request_str[at] != '\n')
            ++at;
        http_type = string(request_str + http_start, request_str + at);
    }
    //解析host， connection等其他请求参数
    while (at < n)
    {
        while (at < n && (request_str[at] == '\r' || request_str[at] == '\n'))
            ++at;
        // auto current = request_str + at;
        if (std::strncmp("Host:", request_str + at, 5) == 0)
            at += parse_hosts(request_str + at, n - at);
        else
        {
            ++at;
        }
    }
    return at;
}
Request::size_t Request::parse_hosts(const char *request_str, size_t n)
{
    size_t at = 6;
    while (request_str[at] != '\r' && request_str[at] != '\n')
        ++at;
    host = string(request_str + 6, request_str + at);
    return at;
}

size_t Request::parse_connection_type(const char *request_str, size_t n)
{
    return 1;
}
