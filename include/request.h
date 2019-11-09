#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <cstddef>
#include <limits>
#include <map>
#include <cstring>

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
        //错误信息
        string error_message;
    public:
        Request(const char * request_str= nullptr, size_t n = SIZE_MAX) ;
        Request(const string & requst_str);
        Request(const Request &) = default;
        Request(Request &&) = default;

        size_t parse_request(const char * request_str, size_t n);
        string get_request_path() const;
        const map<string, string> &get_request_parameter();
        enum RequestMethod get_request_type();
        string get_error_message();
    
    private:
        
        size_t parse_hosts(const char *request_str, size_t n);
        size_t parse_connection_type(const char *request_str, size_t n);
        size_t parse_user_agent(const char *request_str, size_t n);
};

#endif
