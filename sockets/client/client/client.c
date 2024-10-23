#include <stdio.h>
#include <winsock2.h>
#include <windows.h> // для SetConsoleCP і SetConsoleOutputCP
#include <ws2tcpip.h> // для inet_pton
#include <time.h>    // для вимірювання часу

#pragma comment(lib, "ws2_32.lib") // Додає бібліотеку для роботи з Winsock

#define SERVER_ADDRESS "127.0.0.1" // Локальний сервер
#define SERVER_PORT 8888
#define BUFFER_SIZE 1024

void send_data(SOCKET client_socket, size_t data_size) {
    char buffer[BUFFER_SIZE];
    int totalBytesSent = 0;
    clock_t start_time, end_time;
    double latency;

    // Заповнення буфера тестовими даними
    memset(buffer, 'A', BUFFER_SIZE); // Заповнюємо буфер символом 'A'

    // Початок вимірювання часу
    start_time = clock();

    // Відправка даних частинами
    while (totalBytesSent < data_size) {
        int bytesToSend = (data_size - totalBytesSent) < BUFFER_SIZE ? (data_size - totalBytesSent) : BUFFER_SIZE;
        if (send(client_socket, buffer, bytesToSend, 0) == SOCKET_ERROR) {
            printf("Помилка відправлення даних: %d\n", WSAGetLastError());
            break;
        }
        totalBytesSent += bytesToSend;
    }

    // Завершення вимірювання часу
    end_time = clock();
    latency = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // Виведення результатів
    printf("Відправлено байт: %d\n", totalBytesSent);
    printf("Час відправлення: %f секунд\n", latency);
    printf("Пропускна здатність: %f байт/секунду\n", totalBytesSent / latency);
}

int main(int argc, char* argv[]) {
    SetConsoleCP(1251); // Встановлення кодування Windows-1251 в потік введення
    SetConsoleOutputCP(1251); // Встановлення кодування Windows-1251 в потік виведення

    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server;
    size_t data_size = 1024 * 1024 * 500; 

    // Якщо аргумент переданий, задаємо розмір даних
    if (argc > 1) {
        data_size = atoi(argv[1]) * 1024 * 1024; // Переведення MB в байти
    }

    // Ініціалізація Winsock
    printf("Ініціалізація Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Помилка WSAStartup: %d\n", WSAGetLastError());
        return 1;
    }

    // Створення клієнтського сокету
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Не вдалося створити сокет: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Налаштування адреси сервера
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_ADDRESS, &server.sin_addr) <= 0) {
        printf("Невірна адреса або адреса не підтримується\n");
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    // Підключення до сервера
    if (connect(client_socket, (struct sockaddr*)&server, sizeof(server)) < 0) {
        printf("Помилка підключення до сервера\n");
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    printf("Підключено до сервера.\n");

    // Відправка даних
    send_data(client_socket, data_size);

    // Закриття сокету
    closesocket(client_socket);
    WSACleanup();

    return 0;
}
