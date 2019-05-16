#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <cstddef>
#include <limits>
#include <map>

enum RequestMethod
{
    GET,
    POST,
    OTHER
};

enum ConnectionType
{
    KeepAlive,
    Upgrade
};
class Request
{
    public:
        using size_t  = std::size_t;
        using string  = std::string;
        template<class K, class V> using map = std::map<K,V>;

    private:
        enum RequestMethod method;
        string request_path;
        map<string, string> request_parameter;
        string http_type;
        string host;
        string user_agent;
        string content_type;
        size_t content_length;
        enum ConnectionType connection;
        //错误请求
        string error_message;
    public:
        Request(const char * request_str, size_t n = SIZE_MAX) ;
        Request(const string & requst_str);
        
        string get_request_path();
        const map<string, string> &get_request_parameter();
        enum RequestMethod get_request_type();
        string get_error_message();
    
    private:
        void parse_request(const char * request_str, size_t n);
};

#endif
