#include "response.h"

std::string Response::default_404_html;
const char * Response::response_head_template = "HTTP/1.1 %u OK\r\nContent-Type: %s; charset=UTF-8\r\nContent-Length:%u\r\n\r\n";

Response::Response(const std::string &request_path, bool index_enabled)
    : request_path(request_path), 
    buff(Response::max_len, ' '), 
    index_enabled(index_enabled)
{
}

bool Response::validate(const std::string &request_path)
{
    if(request_path.size()== 0)
        return false;
    if (access(request_path.c_str(), F_OK | R_OK) == -1)
        return false;
    return true;
}


size_t Response::get_file_size(const std::string &file_path)
{
    struct stat t;
    if(stat(file_path.c_str(), &t)== -1)
        return fail_size;
    /*判断输入的文件路径是否目录，若是目录，则往下执行，分析目录下的文件*/
    //if(S_ISDIR(t.st_mode))
    if(S_ISREG(t.st_mode))
        return t.st_size;
    return fail_size;
}


size_t Response::write_file_to(const std::string &file_path, int client_fd)
{
    const size_t max_len = 1024;
    static char buff[max_len];
    FILE *html_file = fopen(file_path.c_str(), "rb");
    if(!html_file) return fail_size;
    int read_len = 0;
    size_t total_size = 0;
    while ((read_len = read(fileno(html_file), buff, max_len)) > 0)
    {
        write(client_fd, buff, read_len);
        total_size += read_len;
    }
    fclose(html_file);
    return total_size;
}

size_t Response::response_404_to(int client_fd)
{
    return write_file_to(default_404_html, client_fd);
}

size_t Response::response_to(int client_fd)
{
    if(validate(request_path))
    {
        if(index_enabled && endwith(request_path,"/"))
        {
            std::string index_path = request_path + "index.html";
            if(validate(index_path))
                request_path = index_path;
        }
        println(request_path);
        if (get_file_size(request_path) != fail_size)
            return response_head_to(client_fd) + response_body_to(client_fd);
    }
    return response_404_to(client_fd);
}

size_t Response::response_head_to(int client_fd)
{
    const char *type;
    //响应类型判断
    if (endwith(request_path, ".html"))
        type = "text/html";
    else if (endwith(request_path, ".txt") || endwith(request_path, ".cpp"))
    type = "text/plain";
    // else if(endwith(file_name, ".jpg"))
    //     type = "application/x-jpg";
    else
        type = ".*";
    
    char *char_buff = const_cast<char *>(buff.data());
    size_t n = sprintf(char_buff, response_head_template, 200, type, get_file_size(request_path));
    write(client_fd, char_buff, n);
    return 0;
}

size_t Response::response_body_to(int client_fd)
{
    return write_file_to(request_path,client_fd);
}


// set default 404 html
void Response::set_default_404(const std::string &default_404)
{
    default_404_html = default_404;
}