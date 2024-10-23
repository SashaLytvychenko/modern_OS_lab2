#include <stdio.h>
#include <windows.h>
#include <time.h>

#define SHARED_MEMORY_SIZE 1024 * 1024 * 500 // 10 MB
#define BUFFER_SIZE 1024 // 1 KB
#define SHARED_MEMORY_NAME "SharedMemoryExample"

void measure_latency_throughput(char* shared_memory, int is_writer) {
    clock_t start_time, end_time;
    size_t data_size = 0;

    if (is_writer) {
        printf("Процес записує дані у спільну пам'ять.\n");

        // Початок вимірювань
        start_time = clock();

        // Записуємо дані у спільну пам'ять
        for (size_t i = 0; i < SHARED_MEMORY_SIZE; i += BUFFER_SIZE) {
            memcpy(shared_memory + i, "TestData", BUFFER_SIZE);
            data_size += BUFFER_SIZE;
        }

        // Завершення вимірювання часу
        end_time = clock();

        // Виведення результатів
        double latency = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        printf("Latency (запис): %f секунд\n", latency);
        printf("Throughput (запис): %zu байт за %f секунд\n", data_size, latency);

        // Додати затримку для процесу writer, щоб reader встиг підключитися
        printf("Writer завершив запис. Очікування для процесу reader...\n");
        Sleep(5000); // Затримка на 5 секунд
    }
    else {
        printf("Процес зчитує дані зі спільної пам'яті.\n");

        // Початок вимірювань
        start_time = clock();

        // Читання даних зі спільної пам'яті
        for (size_t i = 0; i < SHARED_MEMORY_SIZE; i += BUFFER_SIZE) {
            char buffer[BUFFER_SIZE];
            memcpy(buffer, shared_memory + i, BUFFER_SIZE);
            data_size += BUFFER_SIZE;
        }

        // Завершення вимірювання часу
        end_time = clock();

        // Виведення результатів
        double latency = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        printf("Latency (читання): %f секунд\n", latency);
        printf("Throughput (читання): %zu байт за %f секунд\n", data_size, latency);
    }
}

int main(int argc, char* argv[]) {
    SetConsoleCP(1251); // Встановлення кодування Windows-1251 в потік введення
    SetConsoleOutputCP(1251); // Встановлення кодування Windows-1251 в потік виведення

    HANDLE hMapFile;
    char* shared_memory;
    int is_writer = (argc > 1 && strcmp(argv[1], "writer") == 0);

    // Створення або відкриття спільної області пам'яті
    if (is_writer) {
        hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SHARED_MEMORY_SIZE, SHARED_MEMORY_NAME);
        if (hMapFile == NULL) {
            printf("Не вдалося створити спільну пам'ять. Код помилки: %d\n", GetLastError());
            return 1;
        }
    }
    else {
        hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHARED_MEMORY_NAME);
        if (hMapFile == NULL) {
            printf("Не вдалося відкрити спільну пам'ять. Код помилки: %d\n", GetLastError());
            return 1;
        }
    }

    // Відображення спільної пам'яті в адресний простір процесу
    shared_memory = (char*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SHARED_MEMORY_SIZE);
    if (shared_memory == NULL) {
        printf("Не вдалося відобразити спільну пам'ять. Код помилки: %d\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    // Вимірювання latency та throughput
    measure_latency_throughput(shared_memory, is_writer);

    // Завершення роботи з спільною пам'яттю
    UnmapViewOfFile(shared_memory);
    CloseHandle(hMapFile);

    return 0;
}
