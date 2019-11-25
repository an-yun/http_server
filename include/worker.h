#ifndef WORKER_H
#define WORKER_H
#include <string>
#include <vector>
#include <memory>
#include <pthread.h>

#include "connection.h"

class Worker
{
private:
  /* data */
  //先使用pthread设施同步
  pthread_mutex_t connection_mutex;
  //对于单个连接
  std::shared_ptr<Connection> single_connection;
  //对于多个连接
  std::vector<Connection> connections;
  std::string web_root_path;
  /*
  需要处理三种事件（事件处理可以线程模型）：
  向主进程请求连接
  处理单个连接
  增加连接或者说接收连接

  需要一个事件结构，第二个和第三个事件需要放在一个同步的数据结构中，第一个事件的发生需要根据现在事件的多少去判断

  */
public:
  #ifdef TEST
  Worker() = default;
  #endif

  Worker(const std::string &web_root);
  
  /*
  以右值传入的参数，函数内部使用参数时都是左值（思考右值的含义）
  因此如果想以该参数再次调用右值函数，需要加move
  */
  bool set_connection(Connection &&con);
  bool add_connection(Connection &&con);
  //当前连接是否处理完成
  bool connection_finish();

  ~Worker();


  #ifdef TEST
  static void test_connection();
  #endif
};

#endif