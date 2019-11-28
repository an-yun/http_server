# 使用C++实现一个高性能HTTP服务器

## 目录
- [使用C++实现一个高性能HTTP服务器](#使用c实现一个高性能http服务器)
  - [目录](#目录)
  - [介绍](#介绍)
  - [单进程HTTP服务器](#单进程http服务器)
    - [HTTP协议](#http协议)
    - [架构设计和主要工作](#架构设计和主要工作)
    - [版本进展和计划](#版本进展和计划)
  - [多进程HTTP服务器](#多进程http服务器)
  - [多进程结合IO复用HTTP服务器](#多进程结合io复用http服务器)

## 介绍
本项目目标是设计一个架构良好，高性能的HTTP服务器，核心设计思想如下：
- 采用OOP设计思想，通过良好的结构设计降低模块之间的耦合，资源管理和释放使用RAII
- 充分C++语言的现代特性和基础设施支持，如C++标准库中容器、算法、线程、互斥、条件变量和future的内建支持。
- 采用多进程，IO复用等技术，提高整体性能。

项目分预计发布三个版本，依次是单进程静态HTTP服务器，多进程HTTP服务器和多进程IO复用HTTP服务器。

## 单进程HTTP服务器
本项目的第一个版本是单进程静态HTTP服务器，支持静态请求，单进程串行处理客户端连接。

### HTTP协议

首先简单介绍HTTP协议，具体有HTTP请求格式与主要字段，响应格式与字段。HTTP请求格式是由多行组成，每行`[回车符][换行符]` 分隔，具体每一行格式如下

1. 首先第一行，格式为：<br>
   `请求方法(GET/POST) URL 协议版本` 
2. 接下来是请求头部，占据多行，每行说明一个参数，具体行数视请求所带参数多少而定。每一行格式如下<br>
   `头部字段名：值` 
3. 然后是一个空行
4. 最后是请求数据，这一部分可以没有，一般只有POST请求才有这一部分。

具体HTTP各字段含义见[HTTP字段](https://en.wikipedia.org/wiki/List_of_HTTP_header_fields)
。一个典型的HTTP请求报文如下：

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


### 架构设计和主要工作
这一版本的整个工作流程如下图所示

![工作流程如图](https://raw.githubusercontent.com/an-yun/http_server/master/img/单进程版HTTP服务器.jpg)
其中涉及类的具体信息如下表：

需要完成的主要工作有：
1. HTTP 请求解析：需要解析的有请求路径，请求方法等HTTP字段。这一部分的工作虽然不难，但需要一些时间完成各个字段的细致处理。
2. HTTP 响应回复：对于静态请求，主要是判断文件是否存在可读，读取请求的文件，形成符合HTTP协议的响应回复，并设置响应的响应码，如文件不存在设置404.



### 版本进展和计划
HTTP服务器的单进程版的进展和To-do list如下:
1. 简单单文件版的HTTP服务器，展现整个HTPP请求流程。     `√2019 3-25` 
2. 初步设计Server类，添加监听等方法，并加入响应终止信号（如Ctrl+C）的处理。`√2019 04-17`
3. 添加Request类解析请求，包括各类请求参数。    `√2019 05-23`
4. 设计Connection类处理客户端连接，包括接收，发送内容，关闭连接等等。`√2019 10-25`
5. 向server添加wait_connection方法，将Connection的构造函数设为私有，只能由Server监听客户端连接生成。`√2019 10-30`
6. 实现初版HTTPServer(见single_http_server.cpp文件): 响应静态请求，对文件访问加以判断，生成响应。`√ 2019 10-31`


初版优化进展和To-do list:
1. 添加初版文档，测试一些复杂网页。 `√ 2019 11-08`
2. 优化server的wait_connection方法，尽量不使用友元。 Connection的parse_request方法改为public，加入健壮性判断。`√ 2019 11-11`
3. 设计Response类，负责生成响应头和响应内容。Connection加入reponse方法，减少与Server的耦合。`√ 2019 11-15`
4. 增加文件目录浏览功能。



新增类的功能和设计如下表：


## 多进程HTTP服务器
单进程的HTTP服务器缺点很明显，同一时刻只能响应一个HTTP请求，大分部的时间片都耗费在IO上。我们对服务器加入多进程的支持，就能在一定程度上解决这个问题。

多进程版进展和To-do list:

1. 验证多进程通讯技术(详见[进程间传送文件描述符](https://gihub.com/an-yun/http_server/doc/file_descriptor_transfer.md))：
    - 管道通讯：通讯可行，多进程同步竞争连接不便。 `√ 2019 10-31`
    - 共享存储：通讯竞争可行，但直接跨进程读写文件描述符失败。 `√ 2019 11-07`
    - socketpair双工管道：参照apue第15章，成功传送文件描述符。`√ 2019 11-07`
2. 结合共享存储与互斥量，实现各个进程对连接的竞争。√ 2019 11-08 
3. 实现Woker类串行处理单个Connection：
    - 添加connection_finish方法，返回当前连接是否处理完成.  `√2019 11-25`
    - 添加set_connection方法用于设置新的待处理的连接，并向Connection中添加移动构造和移动赋值方法。 `√2019 11-10`
    - 添加handle方法，循环处理连接。
4. 增加config类管理服务器相关的配置：
    - 404页面管理和自定义
    - 启用目录默认index
    - 允许目录浏览等等
5. 实现Woker类串行并发处理多个Connection：
    - 加入事件处理机制
    - 使用同步信号操作
    - 使用线程异步处理每个事件




## 多进程结合IO复用HTTP服务器
距离本项目最终的目标--多进程，<kbd>epoll</kbd>多路IO复用的高性能HTTP服务器，差两个关键的技术点：1.多进程，包含多进程通讯和同步，进程间传送文件描述符。

多进程IO复用版进展和To-do list:
1. 加入设计多连接设计：
    - Server中修改连接竞争机制，考虑各个进程间负载均衡。
    - Server加入索引连接的数据结构，添加epoll设计。
    - Connection 中加入生成epoll事件的方法，实现IO复用设计。