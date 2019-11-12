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

    size_t response_head_to(int client_fd);
    size_t response_body_to(int client_fd);

  public:
    static const size_t max_len = 1024;
    Response(const std::string &request_path, bool index_enabled = true);
    size_t response_to(int client_fd);

    // set response common config
    static void set_default_index(const std::string &default_index);
    static void set_default_404(const std::string &default_index);
};

#endif