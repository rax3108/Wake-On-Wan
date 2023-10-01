#include <iostream>
#include <string>
#include<fstream>
#include <Winsock2.h>
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main() {
    int a;
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed." << endl;
        return 1;
        cin >> a;
    }

    //get MAC and IP
    string MAC;
    string IP_;
    string port_str;
    int port;
    ifstream input;
    input.open("input.txt");
    getline(input, MAC);
    getline(input, IP_);
    getline(input, port_str);
    input.close();
    struct hostent* hostInfo = gethostbyname(IP_.c_str());
    if (hostInfo == NULL) {
        cout << "Failed to resolve hostname." << endl;
        WSACleanup();
    }
    cout << *((unsigned long*)hostInfo->h_addr);
    port= stoi(port_str);

    //process
    //MAC to mac[6]
    unsigned char mac[6];
    for(int i= 0; i< 6; i++){
        string str_temp= MAC.substr(3*i, 2);
        str_temp= "0x"+str_temp;
        mac[i] = stoi(str_temp, nullptr, 16);
    }

    //create packet
    //FF->6times, mac[6]->16times
    unsigned char wowPacket[102];
    for(int i= 0; i< 6; i++){
        wowPacket[i]= 0xFF;
    }
    int j= 6;
    while(j< 102){
        for(int i= 0;i< 6; i++){
            wowPacket[j]= mac[i];
            j++;
        }
    }

    // Set destination address to intermediate server
    sockaddr_in destAddr;
    memset(&destAddr, 0, sizeof(destAddr));
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(port);
    destAddr.sin_addr.s_addr = *((unsigned long*)hostInfo->h_addr);

    //socket(使用IPv4,UDP)存入sockfd
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        cout << "Socket creation failed." << endl;
        WSACleanup();
    }

    //sent packet and broadcast
    int bytesSent = sendto(sockfd, (const char*)wowPacket, sizeof(wowPacket), 0, (struct sockaddr*)&destAddr, sizeof(destAddr));
    if (bytesSent < 0) {
        cout << "Sending WoW packet failed." << endl;
    } else {
        cout << "WoW packet sent successfully." << endl;
    }

    closesocket(sockfd);
    WSACleanup();
    system("pause");
    return 0;
}
