#include "server.h"
#include "worker.h"

int main()
{
    #ifdef TEST
    // Server::test_connection();
    Worker::test_connection();
    #endif
}