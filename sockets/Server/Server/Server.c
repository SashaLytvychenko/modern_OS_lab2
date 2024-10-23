#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib") // Додає бібліотеку для роботи з Winsock

#define SERVER_PORT 8888
#define BUFFER_SIZE 1024

int main() {
    SetConsoleCP(1251); // Встановлення кодування Windows-1251 в потік введення
    SetConsoleOutputCP(1251); // Встановлення кодування Windows-1251 в потік виведення

    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server, client;
    int client_len, recv_size;
    char buffer[BUFFER_SIZE + 1];  // +1 для \0
    clock_t start_time, end_time;
    int totalBytesReceived = 0;

    // Ініціалізація Winsock
    printf("Ініціалізація Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Помилка WSAStartup: %d\n", WSAGetLastError());
        return 1;
    }

    // Створення серверного сокету
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Не вдалося створити сокет: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Налаштування адреси сервера
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(SERVER_PORT);

    // Прив'язка сокету
    if (bind(server_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Не вдалося прив'язати сокет: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Очікування підключення клієнта
    listen(server_socket, 3);
    printf("Очікування з'єднання...\n");

    client_len = sizeof(struct sockaddr_in);
    client_socket = accept(server_socket, (struct sockaddr*)&client, &client_len);
    if (client_socket == INVALID_SOCKET) {
        printf("Помилка з'єднання: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Клієнт підключений.\n");

    // Прийом даних
    start_time = clock(); // Початок вимірювання часу
    while ((recv_size = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        totalBytesReceived += recv_size;

        if (recv_size < BUFFER_SIZE) {
            buffer[recv_size] = '\0';  // Кінець рядка
        }
    }
    end_time = clock(); // Кінець вимірювання часу

    if (recv_size == SOCKET_ERROR) {
        printf("Помилка прийому: %d\n", WSAGetLastError());
    }
    else if (recv_size == 0) {
        printf("Передача завершена клієнтом.\n");
    }

    // Виведення результатів
    double latency = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Прийнято байт: %d\n", totalBytesReceived);
    printf("Затримка: %f секунд\n", latency);

    // Закриття сокетів
    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();

    return 0;
}
