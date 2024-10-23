#include <stdio.h>
#include <windows.h>
#include <time.h>

#define FILE_SIZE 1024 * 1024 * 5 // 10 MB
#define BUFFER_SIZE 1024 // 1 KB

void measure_latency_throughput(HANDLE file_map, int shared_mode) {
    char* map_view;
    clock_t start_time, end_time;
    size_t data_size = 0;

    if (shared_mode) {
        // Відображаємо файл у режимі "shared"
        map_view = (char*)MapViewOfFile(file_map, FILE_MAP_ALL_ACCESS, 0, 0, FILE_SIZE);
        if (map_view == NULL) {
            printf("Не вдалося відобразити файл у shared режимі. Код помилки: %d\n", GetLastError());
            return;
        }
        printf("Shared режим увімкнений.\n");
    }
    else {
        // Відображаємо файл у режимі "private"
        map_view = (char*)MapViewOfFile(file_map, FILE_MAP_COPY, 0, 0, FILE_SIZE);
        if (map_view == NULL) {
            printf("Не вдалося відобразити файл у private режимі. Код помилки: %d\n", GetLastError());
            return;
        }
        printf("Private режим увімкнений.\n");
    }

    // Початок вимірювань
    start_time = clock();

    // Симуляція запису в mmap
    for (size_t i = 0; i < FILE_SIZE; i += BUFFER_SIZE) {
        memcpy(map_view + i, "TestData", BUFFER_SIZE);
        data_size += BUFFER_SIZE;
    }

    // Завершення вимірювання часу
    end_time = clock();

    // Виведення результатів
    double latency = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Latency: %f секунд\n", latency);
    printf("Throughput: %zu байт за %f секунд\n", data_size, latency);

    // Завершуємо роботу з mmap
    UnmapViewOfFile(map_view);
}

int main() {
    SetConsoleCP(1251); // Встановлення кодування Windows-1251 в потік введення
    SetConsoleOutputCP(1251); // Встановлення кодування Windows-1251 в потік виведення
    HANDLE hFile, hMap;
    char file_name[] = "mmap_test_file.txt";

    // Створення файлу для mmap
    hFile = CreateFile(file_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Не вдалося створити файл. Код помилки: %d\n", GetLastError());
        return 1;
    }

    // Створюємо відображення файлу в пам'ять (mmap)
    hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, FILE_SIZE, NULL);
    if (hMap == NULL) {
        printf("Не вдалося створити відображення файлу. Код помилки: %d\n", GetLastError());
        CloseHandle(hFile);
        return 1;
    }

    // Тест у режимі shared
    measure_latency_throughput(hMap, 1);

    // Тест у режимі private
    measure_latency_throughput(hMap, 0);

    // Закриття дескрипторів
    CloseHandle(hMap);
    CloseHandle(hFile);

    return 0;
}
