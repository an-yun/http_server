#include "worker.h"


Worker::Worker(const std::string &web_root)
    :web_root_path(web_root)
{
    if(pthread_mutex_init(&connection_mutex,nullptr)<0)
    {
        println("init pthread mutex error!");
    }
#ifdef TEST
    printf("ok\n");
#endif
}

bool Worker::set_connection(Connection &&con)
{
#ifdef TEST
    printf("set_connection right\n");
#endif
    if(pthread_mutex_trylock(&connection_mutex)== EBUSY)
        return false;
    single_connection = std::make_shared<Connection>(std::move(con));
    pthread_mutex_unlock(&connection_mutex);
    return true;
}

bool Worker::add_connection(Connection &&con)
{
    connections.push_back(std::move(con));
    return true;
}

bool Worker::connection_finish()
{
    if(pthread_mutex_trylock(&connection_mutex)== EBUSY)
        return false;
    pthread_mutex_unlock(&connection_mutex);
    return true;
}

Worker::~Worker()
{
    pthread_mutex_destroy(&connection_mutex);
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