#ifndef RESPONSE_H
#define RESPONSE_H
#include <string>
#include <unistd.h>
#include <sys/stat.h>

#include "ioutils.h"

class Response
{
  private:
    const static char *response_head_template;
    static const size_t fail_size = static_cast<size_t>(-1);
    static std::string default_404_html;
    static size_t len_404;

    //检查响应路径
    static bool validate(const std::string &request_path);
    //获取文件大小
    static size_t get_file_size(const std::string &file_path);
    //向client_fd写入文件file_path的内容
    static size_t write_file_to(const std::string &file_path, int client_fd);

    std::string request_path;
    std::string buff;
    bool index_enabled;

    //获取响应类型
    const char *get_type(const std::string &path);
    //向client_fd发送响应头部
    size_t response_head_to(int client_fd, size_t reponse_code, const char *type, size_t content_len);
    //向client_fd发送响应主体
    size_t response_body_to(int client_fd, const std::string &file_path);


  public:
    static const size_t max_len = 1024;
    Response(const std::string &request_path="", bool index_enabled = true);
    Response(Response &&) noexcept = default;
    Response &operator=(Response &&) = default;

    //向client_fd发送响应
    size_t response_to(int client_fd);

    // 共用响应设置
    static void set_default_404(const std::string &default_index);
};

#endif