# 使用C++实现一个高性能HTTP服务器

本项目目标是设计一个架构良好，高性能的HTTP服务器，核心设计思想如下：
- 采用OOP设计思想，通过良好的结构设计降低模块之间的耦合，资源管理和释放使用RAII
- 充分C++语言的现代特性和基础设施支持，如C++标准库中容器、算法、线程、互斥、条件变量和future的内建支持。
- 采用多进程，IO复用等技术，提高整体性能。

## 单进程静态HTTP服务器
本项目的第一个版本是单进程静态HTTP服务器，支持静态请求，单进程。这一版本主要核心技术点有两个：

1. HTTP 请求解析：需要解析的有请求路径，请求方法等HTTP字段。这一部分的工作虽然不难，但需要一些时间完成各个字段的细致处理。
2. HTTP 响应回复：对于静态请求，主要是判断文件是否存在可读，读取请求的文件，形成符合HTTP协议的响应回复，并设置响应的响应码，如文件不存在设置404.

整个工作流程如图所示
![工作流程如图](https://raw.githubusercontent.com/an-yun/http_server/master/img/单进程版HTTP服务器.jpg)

### HTTP协议

首先简单介绍HTTP协议，具体有HTTP请求格式与主要字段，响应格式与字段。HTTP请求格式是由多行组成，每行`[回车符][换行符]` 分隔，具体每一行格式如下

1. 首先第一行，格式为：<br>
   `请求方法(GET/POST) URL 协议版本` 
2. 接下来是请求头部，占据多行，每行说明一个参数，具体行数视请求所带参数多少而定。每一行格式如下<br>
   `头部字段名：值` 
3. 然后是一个空行
4. 最后是请求数据，这一部分可以没有，一般只有POST请求才有这一部分。

一个典型的HTTP请求报文如下

```html
GET /s?wd=HTTP HTTP/1.1
Host: www.baidu.com
Connection: keep-alive
Cache-Control: max-age=0
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.87 Safari/537.36
Accept: text/html
Accept-Encoding: gzip, deflate, br
Accept-Language: zh-CN,zh;q=0.9,en-US;q=0.8,en;q=0.7
Cookie: XXXX
```



如表所示

[HTTP字段](https://en.wikipedia.org/wiki/List_of_HTTP_header_fields)

了解HTTP协议，初步完成了HTTP服务器的单进程版（）。


类的功能和设计如下表：


## 多进程静态HTTP服务器
单进程的HTTP服务器缺点很明显，同一时刻只能响应一个HTTP请求，大分部的时间片都耗费在IO上。我们对服务器加入多进程的支持，就能在一定程度上解决这个问题。





## 多进程静态HTTP服务器
距离本项目最终的目标--多进程，<kbd>epoll</kbd>多路IO复用的高性能HTTP服务器，差两个关键的技术点：1.多进程，包含多进程通讯和同步，进程间传送文件描述符