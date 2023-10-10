#include "head\netcom.h"
#include <cstdio>
int port;
int main()
{
    scanf("%d", &port);
    getchar();
    for (;;)
    {
        printf("%s\n", net::Listen(port));
    }
    return 0;
}