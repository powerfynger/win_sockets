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
    //  �������� ���� ������ (memset)
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    /*
    *   ������������� ���������� Windows Sockets 
    * 
    *   MAKEWORD() return word, ������ ���������� Windows Sockets
    * 
    *   � wsa_data ������������ �������� � ���������� ���������� ���������� Windows Sockets 
    * 
    *   � ������ ������ ��������� 0  
    */
    result = WSAStartup(MAKEWORD(1, 1), &wsa_data);
    if (result != 0)
    {
        fprintf(stderr, "WSAStartup failed with error: %d\n", result);
        return WINSOCK_API_INIT_ERROR;
    }
    /*
    *   [in, optional] PCSTR pNodeName �������� ����� ��� �����;
    * 
    *   [in, optional] PCSTR pServiceName, �������� ������� ��� ����� �����;
    * 
    *   [in, optional] ADDRINFOA *pHints ��������� �� ��������� ������� �������� ���������� � ���� ������;
    * 
    *   [out] PADDRINFOA *ppResult ��������� �� addinfo, ������� ������ ���������� � �����;
    * 
    *   � ������ ������ ��������� 0
    */
    result = getaddrinfo(NULL, (PCSTR)DEFAULT_PORT, &hints, &result_addr);
    if (result != 0)
    {
        fprintf(stderr, "getaddrinfo failed with error: %d\n", result);
        WSACleanup();
        return GET_ADDR_INFO_ERROR;
    }

    /*
    *   �������� ������
    *    
    *   � ������ ������ ��������� ���������� ���� SOCKET, ����� ��� �������������� � ������� ���� �������������� ����� ����;
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
    *   �������� ������ � ������;
    * 
    *   [in] SOCKET s, ����������;
    * 
    *   [in] sockaddr *addr, ��������� �� �������������� ����� ���������; 
    * 
    *   [in] int namelen, ����� ��������� addr � ������; 
    * 
    *   � ������ ������ ��������� 0;
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
    *   ������� ����������� ���������� ������ �������� getaddrinfo()
    */
    freeaddrinfo(result_addr);

    /*
    *   ������� ������ � ��������� �������� �����������
    * 
    *   [in] SOCKET s, ����������;
    * 
    *   [in] int backlog, ������������ ����� ������� �����������, SOMAXCONN ������������;
    * 
    *   � ������ ������ ��������� 0
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
    *   ��������� ������� ��������� ���������� � ������;
    * 
    *   [in] SOCKET s, ����������;
    * 
    *   [out] sockaddr *addr, ��������� �� ����� ��� �������� ������� �����������;
    * 
    *   [in, out] omt *addrlen, ��������� �� ����� ��������� ����;
    * 
    *   � ������ ������ ��������� ���������� ������ ������ ���� SOCKET;
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
        *   ���������� ������ �� �������� ������;
        * 
        *   [in] SOCKET s, ����������;
        * 
        *   [in] char *buf, ��������� �� ������������ ������;
        * 
        *   [in] int len, ����� ������;
        * 
        *   [in] int flags, ����� ������ ��� ��������� �����������;
        * 
        *   � ������ ������ ��������� ����� ������������ ����, ������� ����� ���� ������ len;
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
    *   ����������� ��������� WSADATA
    * 
    */ 
    
    WSACleanup();
}