#include <stdio.h>
#include <winsock2.h>
#include <windows.h> // ��� SetConsoleCP � SetConsoleOutputCP
#include <ws2tcpip.h> // ��� inet_pton
#include <time.h>    // ��� ���������� ����

#pragma comment(lib, "ws2_32.lib") // ���� �������� ��� ������ � Winsock

#define SERVER_ADDRESS "127.0.0.1" // ��������� ������
#define SERVER_PORT 8888
#define BUFFER_SIZE 1024

void send_data(SOCKET client_socket, size_t data_size) {
    char buffer[BUFFER_SIZE];
    int totalBytesSent = 0;
    clock_t start_time, end_time;
    double latency;

    // ���������� ������ ��������� ������
    memset(buffer, 'A', BUFFER_SIZE); // ���������� ����� �������� 'A'

    // ������� ���������� ����
    start_time = clock();

    // ³������� ����� ���������
    while (totalBytesSent < data_size) {
        int bytesToSend = (data_size - totalBytesSent) < BUFFER_SIZE ? (data_size - totalBytesSent) : BUFFER_SIZE;
        if (send(client_socket, buffer, bytesToSend, 0) == SOCKET_ERROR) {
            printf("������� ����������� �����: %d\n", WSAGetLastError());
            break;
        }
        totalBytesSent += bytesToSend;
    }

    // ���������� ���������� ����
    end_time = clock();
    latency = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // ��������� ����������
    printf("³��������� ����: %d\n", totalBytesSent);
    printf("��� �����������: %f ������\n", latency);
    printf("��������� ��������: %f ����/�������\n", totalBytesSent / latency);
}

int main(int argc, char* argv[]) {
    SetConsoleCP(1251); // ������������ ��������� Windows-1251 � ���� ��������
    SetConsoleOutputCP(1251); // ������������ ��������� Windows-1251 � ���� ���������

    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server;
    size_t data_size = 1024 * 1024 * 500; 

    // ���� �������� ���������, ������ ����� �����
    if (argc > 1) {
        data_size = atoi(argv[1]) * 1024 * 1024; // ����������� MB � �����
    }

    // ����������� Winsock
    printf("����������� Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("������� WSAStartup: %d\n", WSAGetLastError());
        return 1;
    }

    // ��������� �볺�������� ������
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("�� ������� �������� �����: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // ������������ ������ �������
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_ADDRESS, &server.sin_addr) <= 0) {
        printf("������ ������ ��� ������ �� �����������\n");
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    // ϳ��������� �� �������
    if (connect(client_socket, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("������� ���������� �� �������\n");
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    printf("ϳ�������� �� �������.\n");

    // ³������� �����
    send_data(client_socket, data_size);

    // �������� ������
    closesocket(client_socket);
    WSACleanup();

    return 0;
}
