#include "response.h"

std::string Response::default_404_html;
size_t Response::len_404=0;
const char * Response::response_head_template = "HTTP/1.1 %u OK\r\nContent-Type: %s; charset=UTF-8\r\nContent-Length:%u\r\n\r\n";

Response::Response(bool index_enabled)
    :
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
    if(S_ISDIR(t.st_mode))
        return dir_size;
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



size_t Response::response_to(int client_fd, const std::string &request_path, bool index)
{

    //依次是文件链接，文件名，日期，大小(带单位)
    static const char * const one_file_template = "\r\n<a href=\"%s\">%s</a>                          %s     %s";

    //依次是目录名，目录名
    static const char *const dir_content_template = "<html><head><title>Index of %s</title></head><body bgcolor=\"white\"><h1>Index of %s</h1><hr><pre><a href=\"../\">../</a></pre><hr></body></html>";

    //依次是响应长度，响应内容
    static const char *const dir_response_template = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nContent-Length:%u\r\n\r\n";

    //响应头buff
    static char head_buff[128]; 
    std::string response_path = request_path;
    if(validate(response_path))
    {
        if(index_enabled && index)
        {
            //处理默认目录的index
            std::string index_path = response_path + "index.html";
            if(validate(index_path))
                response_path = index_path;
        }
        size_t content_len = get_file_size(response_path);
        if(content_len == dir_size)
        {
            DIR *dp;
            struct dirent *dirp;

        }
        else if (content_len != fail_size)
            return response_head_to(client_fd,200,get_type(response_path),content_len) + response_body_to(client_fd,response_path);
    }

    return response_head_to(client_fd, 404,"text/html", len_404) + response_body_to(client_fd,default_404_html);
}

const char *Response::get_type(const std::string &path)
{
        //响应类型判断
    if (endwith(path, ".html"))
        return "text/html";
    else if (endwith(path, ".txt") || endwith(path, ".cpp"))
        return "text/plain";
    else if (endwith(path,".css"))
        return "text/css";
    else if (endwith(path,".js"))
        return "application/x-javascript";
    // else if(endwith(file_name, ".jpg"))
    //     type = "application/x-jpg";
    else
        return ".*";
}
size_t Response::response_head_to(int client_fd, size_t reponse_code, const char *type,  size_t content_len)
{
    char *char_buff = const_cast<char *>(buff.data());
    size_t n = sprintf(char_buff, response_head_template, reponse_code, type, content_len);
    write(client_fd, char_buff, n);
    return 0;
}

size_t Response::response_body_to(int client_fd,const std::string &file_path)
{
    return write_file_to(file_path,client_fd);
}


// set default 404 html
void Response::set_default_404(const std::string &default_404)
{
    default_404_html = default_404;
    len_404 = get_file_size(default_404_html);
}