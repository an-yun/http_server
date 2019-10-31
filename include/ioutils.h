#ifndef IOUTILS_HPP
#define IOUTILS_HPP

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <type_traits>

// 打印普通类型
template <typename T>
inline void print(const T &t)
{
    std::cout << t;
}
// 打印bool类型
template <>
inline void print<bool>(const bool &b)
{
    std::cout<<(b?"true":"false");
}
// tag dispatch
template<typename T> struct is_vector : public std::false_type 
{
    static inline void print_delimiter() { print(", "); }
};

template<typename T, typename A>
struct is_vector<std::vector<T, A>> : public std::true_type
{
    static inline void print_delimiter() { print(",\n "); }
};

//打印vector,利用函数重载(不是部分特例化)达到的
template <typename T>
inline void print(const std::vector<T> &t)
{
    
    print('[');
    size_t t_size = t.size();
    if (t_size > 0)
    {
        for (size_t i = 1; i < t_size; i++)
        {
            print(t[i - 1]);
            is_vector<T>::print_delimiter();
        }
        print(t[t_size - 1]) ;
    }
    print(']');
}
// 泛化的打印函数
template <typename T, typename... Args>
inline void print(const T &t, const Args... rest)
{
    print(t);
    print(", ");
    print(rest...);
}

//输出空行
void println();

template <typename T>
inline void println(const T &t)
{
    print(t);
    std::cout << std::endl;
}


template <typename T, typename... Args>
inline void println(const T &t, const Args... rest)
{
    print(t, rest...);
    std::cout << std::endl;
}
inline bool startwith(const std::string &str, const std::string& start)
{
    size_t str_len = str.length(), start_len = start.length();
    return str_len>= start_len && std::equal(str.begin(),str.begin()+start_len,start.begin());
}

inline bool endwith(const std::string &str, const std::string& end)
{
    size_t str_len = str.length(), end_len = end.length();
    return str_len>= end_len && std::equal(str.end()-end_len,str.end(),end.begin());
}



#endif
