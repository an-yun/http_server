#include <algorithm>
#include <iostream>
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

void println()
{
    std::cout << std::endl;
}

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
