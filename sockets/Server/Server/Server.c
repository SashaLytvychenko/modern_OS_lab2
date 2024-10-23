#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib") // ���� �������� ��� ������ � Winsock

#define SERVER_PORT 8888
#define BUFFER_SIZE 1024

int main() {
    SetConsoleCP(1251); // ������������ ��������� Windows-1251 � ���� ��������
    SetConsoleOutputCP(1251); // ������������ ��������� Windows-1251 � ���� ���������

    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server, client;
    int client_len, recv_size;
    char buffer[BUFFER_SIZE + 1];  // +1 ��� \0
    clock_t start_time, end_time;
    int totalBytesReceived = 0;

    // ����������� Winsock
    printf("����������� Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("������� WSAStartup: %d\n", WSAGetLastError());
        return 1;
    }

    // ��������� ���������� ������
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("�� ������� �������� �����: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // ������������ ������ �������
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(SERVER_PORT);

    // ����'���� ������
    if (bind(server_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("�� ������� ����'����� �����: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // ���������� ���������� �볺���
    listen(server_socket, 3);
    printf("���������� �'�������...\n");

    client_len = sizeof(struct sockaddr_in);
    client_socket = accept(server_socket, (struct sockaddr*)&client, &client_len);
    if (client_socket == INVALID_SOCKET) {
        printf("������� �'�������: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("�볺�� ����������.\n");

    // ������ �����
    start_time = clock(); // ������� ���������� ����
    while ((recv_size = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        totalBytesReceived += recv_size;

        if (recv_size < BUFFER_SIZE) {
            buffer[recv_size] = '\0';  // ʳ���� �����
        }
    }
    end_time = clock(); // ʳ���� ���������� ����

    if (recv_size == SOCKET_ERROR) {
        printf("������� �������: %d\n", WSAGetLastError());
    }
    else if (recv_size == 0) {
        printf("�������� ��������� �볺����.\n");
    }

    // ��������� ����������
    double latency = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("�������� ����: %d\n", totalBytesReceived);
    printf("��������: %f ������\n", latency);

    // �������� ������
    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();

    return 0;
}
