#ifndef RESPONSE_H
#define RESPONSE_H
#include <string>

class Response
{
  private:
    static std::string default_index_html;
    static std::string default_404_html;

    std::string request_path;
    std::string buff;
    bool index_enabled;
    //检查响应路径
    bool validate();

    //向client_fd发送响应头部
    size_t response_head_to(int client_fd);
    //向client_fd发送响应主体
    size_t response_body_to(int client_fd);

  public:
    static const size_t max_len = 1024;
    Response(const std::string &request_path="", bool index_enabled = true);
    Response(Response &&) = default;
    Response &operator=(Response &&) = default;

    //向client_fd发送响应
    size_t response_to(int client_fd);

    // 共用响应设置
    static void set_default_index(const std::string &default_index);
    static void set_default_404(const std::string &default_index);
};

#endif