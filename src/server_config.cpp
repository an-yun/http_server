#include "server_config.h"

const char * const Config::default_404_response = "HTTP/1.1 404 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length:99\r\n\r\n<html><head><title>404</title></ head><body><h1>文件不存在</h1><p>404错误</p></body></html>";
