#include "worker.h"


Worker::Worker(const std::string &web_root)
    :web_root_path(web_root)
{
#ifdef TEST
    printf("ok\n");
#endif
}

bool Worker::set_connection(Connection &&con)
{
#ifdef TEST
    printf("set_connection right\n");
#endif
    single_connection = std::make_shared<Connection>(std::move(con));
    return true;
}

bool Worker::add_connection(Connection &&con)
{
    connections.push_back(std::move(con));
    return true;
}



Worker::~Worker()
{
}

#ifdef TEST
void Worker::test_connection()
{
      Connection con;
      Connection con1;
      Connection con2;
      Worker w;
      w.set_connection(std::move(con));
      w.add_connection(std::move(con1));
      w.add_connection(std::move(con2));
}
#endif