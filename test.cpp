#include <cstdio>
#include <cstring>
#include <iostream>
char s[10005];
int main()
{
    freopen("attack.py", "r", stdin);
    freopen("a.cpp", "w", stdout);
    for (int l = 1; l <= 215; l++)
    {
        std::cin.getline(s, 10005);
        printf("\"");
        for (int i = 0; i < strlen(s); i++)
        {
            switch (s[i])
            {
            case ('\"'): {
                printf("\\\"");
                break;
            }
            case ('\''): {
                printf("\\\'");
                break;
            }
            case ('\\'): {
                printf("\\\\");
                break;
            }
            default: {
                printf("%c", s[i]);
            }
            }
        }
        printf("\",\n");
    }
    return 0;
}