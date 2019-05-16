#include "request.h"
#include <cstring>
Request::Request(const char * requst_str, size_t n)
{
    if(n == SIZE_MAX)
        n = std::strlen(requst_str);
    
}
