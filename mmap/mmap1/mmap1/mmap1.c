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
        // ³��������� ���� � ����� "shared"
        map_view = (char*)MapViewOfFile(file_map, FILE_MAP_ALL_ACCESS, 0, 0, FILE_SIZE);
        if (map_view == NULL) {
            printf("�� ������� ���������� ���� � shared �����. ��� �������: %d\n", GetLastError());
            return;
        }
        printf("Shared ����� ���������.\n");
    }
    else {
        // ³��������� ���� � ����� "private"
        map_view = (char*)MapViewOfFile(file_map, FILE_MAP_COPY, 0, 0, FILE_SIZE);
        if (map_view == NULL) {
            printf("�� ������� ���������� ���� � private �����. ��� �������: %d\n", GetLastError());
            return;
        }
        printf("Private ����� ���������.\n");
    }

    // ������� ���������
    start_time = clock();

    // ��������� ������ � mmap
    for (size_t i = 0; i < FILE_SIZE; i += BUFFER_SIZE) {
        memcpy(map_view + i, "TestData", BUFFER_SIZE);
        data_size += BUFFER_SIZE;
    }

    // ���������� ���������� ����
    end_time = clock();

    // ��������� ����������
    double latency = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Latency: %f ������\n", latency);
    printf("Throughput: %zu ���� �� %f ������\n", data_size, latency);

    // ��������� ������ � mmap
    UnmapViewOfFile(map_view);
}

int main() {
    SetConsoleCP(1251); // ������������ ��������� Windows-1251 � ���� ��������
    SetConsoleOutputCP(1251); // ������������ ��������� Windows-1251 � ���� ���������
    HANDLE hFile, hMap;
    char file_name[] = "mmap_test_file.txt";

    // ��������� ����� ��� mmap
    hFile = CreateFile(file_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("�� ������� �������� ����. ��� �������: %d\n", GetLastError());
        return 1;
    }

    // ��������� ����������� ����� � ���'��� (mmap)
    hMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, FILE_SIZE, NULL);
    if (hMap == NULL) {
        printf("�� ������� �������� ����������� �����. ��� �������: %d\n", GetLastError());
        CloseHandle(hFile);
        return 1;
    }

    // ���� � ����� shared
    measure_latency_throughput(hMap, 1);

    // ���� � ����� private
    measure_latency_throughput(hMap, 0);

    // �������� �����������
    CloseHandle(hMap);
    CloseHandle(hFile);

    return 0;
}
