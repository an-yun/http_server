#include "request.h"
#include <algorithm>
#include <cstring>

Request::Request(const char * request_str, size_t n)
{
    if(n == SIZE_MAX)
        n = std::strlen(request_str);
    parse_request(request_str, n);
}

void Request::parse_request(const char * request_str, size_t n)
{
    size_t at = 0;
    //解析请求方法
    if(std::strncmp("GET ",request_str,4) == 0)
    {
        method = GET;
        at = 4;
    }
    else if(std::strncmp("POST ",request_str,5) == 0)
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
    size_t url_start = at;
    while (at < n && request_str[at] != ' '&& request_str[at] != '?')
        ++at;
    request_path = string(request_str + url_start, request_str + at);
    //解析请求参数
    if(request_str[at++] == '?')
    {
        while(request_str[at] != ' ')
        {
            auto ch = request_str[at];
            if(ch == '&') ++at;
            else
            {
                size_t name_start = at;
                while ((ch = request_str[at]) != '&' && ch != '=' && ch != ' ')
                    ++at;
                if(name_start == at)//para_str start with '='
                {
                    error_message = "parameter format error";
                    while (request_str[++at] != ' ')
                        ;
                    break;
                }
                string para_name(request_str + name_start, request_str + at);
                if(ch == '=')
                {
                    
                }
            }
        }
    }
}
