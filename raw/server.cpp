#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        cout << "WSAStartup failed\n";
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cout << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(54000);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Bind failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        cout << "Listen failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Server running on port 54000...\nWaiting for a client...\n";

    sockaddr_in clientAddr{};
    int clientSize = sizeof(clientAddr);
    SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Accept failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Client connected!\nType messages (type '/quit' to exit)\n\n";

    char buffer[512];
    string message;
    int bytesReceived = 0;

    while (true) {
        cout << "You: ";
        getline(cin, message);

        if (message == "/quit")
            break;

        int sendResult = send(clientSocket, message.c_str(), (int)message.size(), 0);
        if (sendResult == SOCKET_ERROR) {
            cout << "Send failed. Closing.\n";
            break;
        }

        bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            cout << "Client disconnected.\n";
            break;
        }
        buffer[bytesReceived] = '\0';
        cout << "Client: " << buffer << "\n";
    }

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    cout << "Server closed.\n";
    return 0;
}
