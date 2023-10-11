#include "head\netcom.h"
#include <cstdio>
int port;
char s[1005];
char ip[1005];
int main()
{
    scanf("%s %d", ip, &port);
    getchar(int argv, char *argc[]);
    for (;;)
    {
        scanf("%[^\n]s", s);
        getchar();
        net::Send(ip, port, s);
    }
    return 0;
}