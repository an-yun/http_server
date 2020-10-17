#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H
#include <string>
#include <vector>

class Config
{
public:
    Config();

private:
    std::string web_root_path;
    /*  一些二值设定:
    *   是否允许目录浏览
    *   是否启用默认index
    *   
    */
    uint32_t bool_configs;

    //静态设置
    static const char *const default_404_response;
    static const char *const one_file_template;
    static const char *const dir_response_template;
    static const char *const dir_content_template;

};

#endif