#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <string.h>

#pragma comment (lib, "Ws2_32.lib")
#define DEFAULT_PORT             "12345"

#define WINSOCK_API_INIT_ERROR   0xFF
#define GET_ADDR_INFO_ERROR      0xFE
#define SOCKET_INIT_ERROR        0xFD
#define BIND_ERROR               0xFC
#define LISTEN_ERROR             0xFB
#define ACCEPT_ERROR             0xFA


int main(void)
{
    puts("Waiting for connection . . .");
    WSADATA wsa_data;
    SOCKET listen_socket = INVALID_SOCKET;
    struct addrinfo hints, * result_addr = NULL;
    int result = 0;
    //  Очистить блок памяти (memset)
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    /*
    *   Инициалищация интерфейса Windows Sockets 
    * 
    *   MAKEWORD() return word, версия интерфейса Windows Sockets
    * 
    *   В wsa_data записываются сведения о конкретной реализации интерфейса Windows Sockets 
    * 
    *   В случае успеха ожидается 0  
    */
    result = WSAStartup(MAKEWORD(1, 1), &wsa_data);
    if (result != 0)
    {
        fprintf(stderr, "WSAStartup failed with error: %d\n", result);
        return WINSOCK_API_INIT_ERROR;
    }
    /*
    *   [in, optional] PCSTR pNodeName название хоста или номер;
    * 
    *   [in, optional] PCSTR pServiceName, название сервиса или номер хоста;
    * 
    *   [in, optional] ADDRINFOA *pHints указатель на структуры которая содержит информацию о типе сокета;
    * 
    *   [out] PADDRINFOA *ppResult указатель на addinfo, которая хранит информацию о хосте;
    * 
    *   В случае успеха ожидается 0
    */
    result = getaddrinfo(NULL, (PCSTR)DEFAULT_PORT, &hints, &result_addr);
    if (result != 0)
    {
        fprintf(stderr, "getaddrinfo failed with error: %d\n", result);
        WSACleanup();
        return GET_ADDR_INFO_ERROR;
    }

    /*
    *   Создание сокета
    *    
    *   В случае успеха ожидается дескриптор типа SOCKET, далее все взаимодействия с сокетом буду осуществляться через него;
    */   
    listen_socket = socket(result_addr->ai_family, result_addr->ai_socktype, result_addr->ai_protocol);
    if (listen_socket == INVALID_SOCKET)
    {
        fprintf(stderr, "socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result_addr);
        WSACleanup();
        return SOCKET_INIT_ERROR;
    }
    /*
    *   Привязка адреса к сокету;
    * 
    *   [in] SOCKET s, дескриптор;
    * 
    *   [in] sockaddr *addr, указатель на подготовленную ранее структуру; 
    * 
    *   [in] int namelen, длина структуры addr в байтах; 
    * 
    *   В случае успеха ожидается 0;
    */
    result = bind(listen_socket, result_addr->ai_addr, (int)result_addr->ai_addrlen);
    if (result == SOCKET_ERROR)
    {
        fprintf(stderr, "bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result_addr);
        closesocket(listen_socket);
        WSACleanup();
        return BIND_ERROR;
    }
    /*
    *   Очистка динамически выделенной памяти функцией getaddrinfo()
    */
    freeaddrinfo(result_addr);

    /*
    *   Перевод сокета в состояние ожидания подключения
    * 
    *   [in] SOCKET s, дескриптор;
    * 
    *   [in] int backlog, максимальная длина очереди подключений, SOMAXCONN максимальное;
    * 
    *   В случае успеха ожидается 0
    */

    result = listen(listen_socket, SOMAXCONN);
    if (result == SOCKET_ERROR)
    {
        fprintf(stderr, "listen failed with error: %d\n", WSAGetLastError());
        closesocket(listen_socket);
        WSACleanup();
        return LISTEN_ERROR;
    }
    SOCKET client_socket = INVALID_SOCKET;
        
    /*
    *   Разрешает попытку входящего подключеия к сокету;
    * 
    *   [in] SOCKET s, дескриптор;
    * 
    *   [out] sockaddr *addr, указатель на буфер для хранения адресса подключения;
    * 
    *   [in, out] omt *addrlen, указатель на длину структуры выше;
    * 
    *   В случае успеха ожидается дескриптор нового сокета типа SOCKET;
    */

    client_socket = accept(listen_socket, NULL, NULL);
    if (client_socket == INVALID_SOCKET)
    {
        fprintf(stderr, "accept failed with error: %d\n", WSAGetLastError());
        closesocket(listen_socket);
        WSACleanup();
        return ACCEPT_ERROR;
    }
    while (1)
    {
        system("cls");
        char file_name[100] = { '\0' }, result[2];
        printf("Connection established\nEnter path to file or \"0\" to exit:\n");
        fgets(file_name, sizeof(file_name), stdin);
        file_name[strlen(file_name) - 1] = '\0';
        if (file_name[0] == '0') {
            break;
        }
        /*
        *   Отправляет данные по текущему сокету;
        * 
        *   [in] SOCKET s, дескриптор;
        * 
        *   [in] char *buf, указатель на отправляемую строку;
        * 
        *   [in] int len, длина строки;
        * 
        *   [in] int flags, набор флагов для изменения подключения;
        * 
        *   В случае успеха ожидается сумма отправленных байт, которое может быть меньше len;
        */

        int connect = send(client_socket, file_name, sizeof(file_name), 0);
        if (connect == SOCKET_ERROR) {
            puts("Connection lost.");
            system("pause");
            return 0;
        }
        recv(client_socket, result, sizeof(result), NULL);
        if (result[0] == '0') {
            puts("File was deleted!");
        }
        else {
            puts("File was not deleted!");
        }
        system("pause");
    }
    closesocket(listen_socket);
    /*
    * 
    *   Уничтожение структуры WSADATA
    * 
    */ 
    
    WSACleanup();
}