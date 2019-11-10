#ifndef WORKER_H
#define WORKER_H
#include <string>
#include <vector>
#include <memory>

#include "connection.h"

class Worker
{
private:
    /* data */
  Connection connection;
  std::string web_root_path;

public:
  Worker(Connection &&con, const std::string &web_root);
  bool set_connectino(Connection &&con);
  ~Worker();
};




#endif