#include "server_config.h"

const char * const Config::default_404_response = "HTTP/1.1 404 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length:98\r\n\r\n<html><head><title>404</title></ head><body><h1>文件不存在</h1><p>404错误</p></body></html>";

//依次是文件链接，文件名，日期，大小(带单位)
const char * const Config::one_file_template = "\r\n<a href=\"%s\">%s</a>                          %s     %s";

//依次是目录名，目录名
const char *const Config::dir_content_template = "<html><head><title>Index of %s</title></head><body bgcolor=\"white\"><h1>Index of %s</h1><hr><pre><a href=\"../\">../</a></pre><hr></body></html>";

//依次是响应长度，响应内容
const char *const Config::dir_response_template = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length:%u\r\n\r\n%s";