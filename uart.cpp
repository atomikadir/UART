#include <iostream>
#include <windows.h>
#include <string>
#include <thread>

void senderThread() {
    HANDLE hSerial;
    DCB dcbSerialParams = { 0 };
    COMMTIMEOUTS timeouts = { 0 };
    LPCWSTR comPort = L"COM1";  // COM portunu uygun þekilde deðiþtirin
    DWORD bytesWritten;

    // COM portunu aç
    hSerial = CreateFile(comPort, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "COM port acilamadi." << std::endl;
        return;
    }

    // COM portu yapýlandýrma
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "GetCommState hatasi." << std::endl;
        CloseHandle(hSerial);
        return;
    }

    dcbSerialParams.BaudRate = CBR_9600;  // Baud hýzýný ayarlayýn (örnekte 9600)
    dcbSerialParams.ByteSize = 8;         // Veri bitleri (8 bit)
    dcbSerialParams.Parity = NOPARITY;     // Parite (yok)
    dcbSerialParams.StopBits = ONESTOPBIT; // Durma biti (1 bit)

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "SetCommState hatasi." << std::endl;
        CloseHandle(hSerial);
        return;
    }

    // Timeout ayarlarý
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        std::cerr << "SetCommTimeouts hatasi." << std::endl;
        CloseHandle(hSerial);
        return;
    }

    // Basýnç sensörü simulasyon verileri
    srand(static_cast<unsigned int>(time(0)));

    // Döngüde kullanýlacak zaman aralýðý (milisaniye cinsinden)
    int interval = 1000;

    // Veri gönderme döngüsü
    while (true) {
        // Basýnç deðerini güncelle
        double pressure = (rand() % 1000) / 10.0;

        // Basýnç deðerini string formatýna dönüþtür
        std::string pressureString = std::to_string(pressure);
        // Basýnç deðerini UART üzerinden gönder
        DWORD bytesToWrite = pressureString.size();
        if (!WriteFile(hSerial, pressureString.c_str(), bytesToWrite, &bytesWritten, NULL)) {
            std::cerr << "Veri gonderme hatasi." << std::endl;
        }

        // Belirli bir süre bekleyerek simülasyon hýzýný kontrol et
        Sleep(interval);
    }

    // COM portunu kapat
    CloseHandle(hSerial);
}

void receiverThread() {
    HANDLE hSerial;
    DCB dcbSerialParams = { 0 };
    COMMTIMEOUTS timeouts = { 0 };
    LPCWSTR comPort = L"COM2";  // COM portunu uygun þekilde deðiþtirin
    DWORD bytesRead;

    // COM portunu aç
    hSerial = CreateFile(comPort, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "COM port acilamadi." << std::endl;
        return;
    }

    // COM portu yapýlandýrma
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "GetCommState hatasi." << std::endl;
        CloseHandle(hSerial);
        return;
    }

    dcbSerialParams.BaudRate = CBR_9600;  // Baud hýzýný ayarlayýn (örnekte 9600)
    dcbSerialParams.ByteSize = 8;         // Veri bitleri (8 bit)
    dcbSerialParams.Parity = NOPARITY;     // Parite (yok)
    dcbSerialParams.StopBits = ONESTOPBIT; // Durma biti (1 bit)

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "SetCommState hatasi." << std::endl;
        CloseHandle(hSerial);
        return;
    }

    // Timeout ayarlarý
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        std::cerr << "SetCommTimeouts hatasi." << std::endl;
        CloseHandle(hSerial);
        return;
    }

    // Veri okuma döngüsü
    while (true) {
        char buffer[256];
        memset(buffer, 0, sizeof(buffer));

        if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
            // Okunan veriyi iþleme
            std::cout << "Okunan veri: " << buffer << std::endl;
        }
        else {
            std::cerr << "Veri okuma hatasi." << std::endl;
        }
    }

    // COM portunu kapat
    CloseHandle(hSerial);
}

int main() {
    // Sender thread'i baþlat
    std::thread sender(senderThread);

    // Receiver thread'i baþlat
    std::thread receiver(receiverThread);

    // Ana thread'in bekleme iþlemi
    sender.join(); 
    receiver.join(); 

    return 0;
}