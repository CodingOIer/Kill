#include <cstdio>
#include <cstring>

/**
 *@检测IP地址是否合法
 */
bool checkIP(char *ip)
{
    if (ip == nullptr)
    {
        return false;
    }
    int len = strlen(ip);
    int num = 0;
    int partCount = 0;
    for (int i = 0; i < len; ++i)
    {
        char c;
        c = ip[i];
        if (c >= '0' && c <= '9')
        {
            num = num * 10 + (c - '0');
            if (num > 255)
            {
                return false;
            }
        }
        else if (c == '.')
        {
            if (num < 0 || num > 255 || partCount >= 3)
            {
                return false;
            }
            num = 0;
            partCount++;
        }
        else
        {
            return false;
        }
    }
    if (num < 0 || num > 255 || partCount != 3)
    {
        return false;
    }
    return true;
}

/**
 *@在字符串中查找指定字符出现次数
 */
int findChar(const char find_string[], char want)
{
    int res = 0;
    int len = strlen(find_string);
    for (int i = 0; i < len; i++)
    {
        if (find_string[i] == want)
        {
            res++;
        }
    }
    return res;
}

/**
 *@获取命令中下一个单词
 */
int getCommandWord(char res[], int start, const char command[], const char mid[] = " ")
{
    int len = strlen(command);
    int w = start;
    for (; w < len && findChar(mid, command[w]) == 0; w++)
    {
        sprintf(res, "%s%c", res, command[w]);
    }
    return w + 1;
}