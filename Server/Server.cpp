#include "head\netcom.h"
#include "head\tool.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <mutex>
#include <thread>
#include <windows.h>

const int MaxIP = 1e2 + 5;
const int MaxName = 1e2 + 5;
const int MaxCommand = 1e3 + 5;
const int MaxComputer = 1e2 + 5;

class computer
{
  public:
    bool online;
    char ip[MaxIP];
    char name[MaxName];
};

int computer_num;
char my_ip[MaxIP];
char command[MaxCommand];
computer computer_list[MaxComputer];
std::mutex Mutex;

/**
 *@初始化
 */
void init()
{
    system("chcp 65001");
    SetConsoleTitle("Kill v2.0 Alpha1.0");
    system("cls");
}

/**
 *@检测在列表中的每一台计算机是不是在线
 */
void liveTest()
{
    for (int i = 1; i <= computer_num; i++)
    {
        std::lock_guard<std::mutex> lock(Mutex);
        net::Send(computer_list[i].ip, 8800, "check");
        char *res = net::ListenNoWait(9902);
        if (res != nullptr && strcmp(res, "online") == 0)
        {
            computer_list[i].online = true;
        }
        else
        {
            computer_list[i].online = false;
        }
    }
}

/**
 *@后台服务进程
 */
void background()
{
    for (;;)
    {
        liveTest();
        _sleep(200);
    }
}

/**
 *@根据IP寻找计算机
 */
int findComputerIP(const char want_computer[])
{
    for (int i = 1; i <= computer_num; i++)
    {
        if (strcmp(computer_list[i].ip, want_computer) == 0)
        {
            return i;
        }
    }
    return -1;
}

/**
 *@根据名字寻找计算机
 */
int findComputerName(const char want_computer[])
{
    for (int i = 1; i <= computer_num; i++)
    {
        if (strcmp(computer_list[i].name, want_computer) == 0)
        {
            return i;
        }
    }
    return -1;
}

/**
 *@处理add命令，添加计算机
 */
void commandAdd(int last)
{
    int len = strlen(command);
    char ip[MaxCommand];
    ip[0] = '\0';
    getCommandWord(ip, last, command);
    if (!checkIP(ip))
    {
        printf("[ERROR]: IP is illegal\n");
    }
    else
    {
        char msg[MaxCommand];
        sprintf(msg, "connect %s", my_ip);
        if (net::Send(ip, 8801, msg))
        {
            char *name = net::ListenNoWait(9901);
            printf("%s\n", name);
            if (strlen(name) != 0)
            {
                std::lock_guard<std::mutex> lock(Mutex);
                computer_num++;
                strcpy(computer_list[computer_num].ip, ip);
                strcpy(computer_list[computer_num].name, name);
                computer_list[computer_num].online = true;
            }
        }
    }
}

/**
 *@设置自己的IP，为握手做准备
 */
void commandMyip(int last)
{
    char tmp_ip[MaxIP];
    tmp_ip[0] = '\0';
    getCommandWord(tmp_ip, last, command);
    if (!checkIP(tmp_ip))
    {
        printf("[ERROR]: IP is illegal\n");
    }
    else
    {
        strcpy(my_ip, tmp_ip);
    }
}

/**
 *@处理kill命令，让指定计算机执行命令
 */
void commandKill(int last)
{
    char kill_ip[MaxIP];
    char kill_command[MaxCommand];
    kill_ip[0] = '\0';
    kill_command[0] = '\0';
    int next = getCommandWord(kill_ip, last, command);
    if (findChar(command, '\"') != 0)
    {
        getCommandWord(kill_command, next + 1, command, "\"");
    }
    else
    {
        getCommandWord(kill_command, next, command, "\"");
    }
    printf("%s %s\n", kill_ip, kill_command);
    if (findChar(kill_ip, '.') == 3)
    {
        int kill_to = findComputerIP(kill_ip);
        if (kill_to == -1)
        {
            printf("[WARING]: The IP is not in the database\n");
        }
        char tmp_command[MaxCommand];
        sprintf(tmp_command, "c,%s", kill_command);
        net::Send(kill_ip, 8800, tmp_command);
    }
    else
    {
        int kill_to = findComputerName(kill_ip);
        if (kill_to == -1)
        {
            printf("[ERROR]: Computer not found\n");
        }
        else
        {
            char tmp_command[MaxCommand];
            sprintf(tmp_command, "c,%s", kill_command);
            net::Send(computer_list[kill_to].ip, 8800, tmp_command);
        }
    }
    // std::lock_guard<std::mutex> lock(Mutex);
}

/**
 *@处理命令ls，显示所有计算机
 */
void commandLs(int last)
{
    for (int i = 1; i <= computer_num; i++)
    {
        printf("%-20s %-20s %s\n", computer_list[i].name, computer_list[i].ip,
               computer_list[i].online ? "online" : "not online");
    }
}

void commandKeyListen(int last)
{
    char mode[MaxCommand];
}

/**
 *@命令解析
 */
void commandAnalysis()
{
    int len = strlen(command);
    char main_command[MaxCommand];
    main_command[0] = '\0';
    int next = getCommandWord(main_command, 0, command);
    if (strcmp(main_command, "add") == 0)
    {
        commandAdd(next);
    }
    else if (strcmp(main_command, "myip") == 0)
    {
        commandMyip(next);
    }
    else if (strcmp(main_command, "kill") == 0)
    {
        commandKill(next);
    }
    else if (strcmp(main_command, "ls") == 0)
    {
        commandLs(next);
    }
    else if (strcmp(main_command, "keylisten") == 0)
    {
    }
    else
    {
        printf("[ERROR]: Unkown command\n");
    }
}

int main(int argv, char *argc[])
{
    init();
    std::thread thread_server(background);
    for (;;)
    {
        printf("Kill>>> ");
        command[0] = '\0';
        std::cin.getline(command, MaxCommand);
        commandAnalysis();
    }
    return 0;
}
/*
加锁
std::lock_guard<std::mutex> lock(Mutex);
*/