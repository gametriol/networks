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

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cout << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in servAddr{};
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(54000);
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // change to server IP if needed

    if (connect(sock, (sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        cout << "Connection failed\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    cout << "Connected to server!\nType messages (type '/quit' to exit)\n\n";

    char buffer[512];
    string message;
    int bytesReceived = 0; // declare before use

    while (true) {
        // Wait for server to send (in this turn-based protocol the server sends first)
        bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            cout << "Server disconnected.\n";
            break;
        }
        buffer[bytesReceived] = '\0';
        cout << "Server: " << buffer << "\n";

        cout << "You: ";
        getline(cin, message);

        if (message == "/quit")
            break;

        int sendResult = send(sock, message.c_str(), (int)message.size(), 0);
        if (sendResult == SOCKET_ERROR) {
            cout << "Send failed. Closing.\n";
            break;
        }
        // Loop continues: server will recv this, then server will send next reply
    }

    closesocket(sock);
    WSACleanup();
    cout << "Client closed.\n";
    return 0;
}
