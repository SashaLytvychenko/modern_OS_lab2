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
        printf("������ ������ ��� � ������ ���'���.\n");

        // ������� ���������
        start_time = clock();

        // �������� ��� � ������ ���'���
        for (size_t i = 0; i < SHARED_MEMORY_SIZE; i += BUFFER_SIZE) {
            memcpy(shared_memory + i, "TestData", BUFFER_SIZE);
            data_size += BUFFER_SIZE;
        }

        // ���������� ���������� ����
        end_time = clock();

        // ��������� ����������
        double latency = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        printf("Latency (�����): %f ������\n", latency);
        printf("Throughput (�����): %zu ���� �� %f ������\n", data_size, latency);

        // ������ �������� ��� ������� writer, ��� reader ����� �����������
        printf("Writer �������� �����. ���������� ��� ������� reader...\n");
        Sleep(5000); // �������� �� 5 ������
    }
    else {
        printf("������ ����� ��� � ������ ���'��.\n");

        // ������� ���������
        start_time = clock();

        // ������� ����� � ������ ���'��
        for (size_t i = 0; i < SHARED_MEMORY_SIZE; i += BUFFER_SIZE) {
            char buffer[BUFFER_SIZE];
            memcpy(buffer, shared_memory + i, BUFFER_SIZE);
            data_size += BUFFER_SIZE;
        }

        // ���������� ���������� ����
        end_time = clock();

        // ��������� ����������
        double latency = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        printf("Latency (�������): %f ������\n", latency);
        printf("Throughput (�������): %zu ���� �� %f ������\n", data_size, latency);
    }
}

int main(int argc, char* argv[]) {
    SetConsoleCP(1251); // ������������ ��������� Windows-1251 � ���� ��������
    SetConsoleOutputCP(1251); // ������������ ��������� Windows-1251 � ���� ���������

    HANDLE hMapFile;
    char* shared_memory;
    int is_writer = (argc > 1 && strcmp(argv[1], "writer") == 0);

    // ��������� ��� �������� ������ ������ ���'��
    if (is_writer) {
        hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SHARED_MEMORY_SIZE, SHARED_MEMORY_NAME);
        if (hMapFile == NULL) {
            printf("�� ������� �������� ������ ���'���. ��� �������: %d\n", GetLastError());
            return 1;
        }
    }
    else {
        hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHARED_MEMORY_NAME);
        if (hMapFile == NULL) {
            printf("�� ������� ������� ������ ���'���. ��� �������: %d\n", GetLastError());
            return 1;
        }
    }

    // ³���������� ������ ���'�� � �������� ������ �������
    shared_memory = (char*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SHARED_MEMORY_SIZE);
    if (shared_memory == NULL) {
        printf("�� ������� ���������� ������ ���'���. ��� �������: %d\n", GetLastError());
        CloseHandle(hMapFile);
        return 1;
    }

    // ���������� latency �� throughput
    measure_latency_throughput(shared_memory, is_writer);

    // ���������� ������ � ������� ���'����
    UnmapViewOfFile(shared_memory);
    CloseHandle(hMapFile);

    return 0;
}
