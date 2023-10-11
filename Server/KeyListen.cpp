#include "head\netcom.h"
#include <cstdio>
#include <cstring>

int main(int argv, char *argc[])
{
    for (;;)
    {
        printf("%s\n", net::Listen(9903));
    }
    return 0;
}