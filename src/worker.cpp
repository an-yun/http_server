#include "worker.h"


Worker::Worker(Connection &&con, const std::string &web_root)
    :connection(std::move(con)),web_root_path(web_root)
{
    printf("ok\n");
}

Worker::~Worker()
{
}
