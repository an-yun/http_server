#include "server.h"

int main()
{
#ifdef TEST
    Server::test_connection();
#endif
}