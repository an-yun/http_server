#include "response.h"

std::string Response::default_index_html;
std::string Response::default_404_html;

Response::Response(const std::string &request_path, bool index_enabled)
    : request_path(request_path), 
    buff(Response::max_len, ' '), 
    index_enabled(index_enabled)
{
}

size_t Response::response_to(int client_fd)
{
    if(index_enabled)
        ;       // do something
    // todo
    return 0;
}

size_t Response::response_head_to(int client_fd)
{
    // todo
    return 0;
}
size_t Response::response_body_to(int client_fd)
{
    // todo
    return 0;
}

// set default index html
void Response::set_default_index(const std::string &default_index)
{
    default_index_html = default_index;
}

// set default 404 html
void Response::set_default_404(const std::string &default_404)
{
    default_404_html = default_404;
}