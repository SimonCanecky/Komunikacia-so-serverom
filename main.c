// API 2023
//gcc main.c -o main -lwsock32 -lWs2_32

#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_BUFLEN 4096
#define Server "147.175.115.34"
#define Port "777"

int iResult;
SOCKET ConnectSocket;

int nastavenia(){
    WSADATA wsaData;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0){
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }
    struct addrinfo *result = NULL, *ptr = NULL;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo(Server, Port, &hints, &result);
    if (iResult != 0){
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    else
        printf("getaddrinfo did not fail...\n");

    ConnectSocket = INVALID_SOCKET;
    ptr = result;
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,ptr->ai_protocol);

    if (ConnectSocket == INVALID_SOCKET){
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    else
        printf("Error at socket DID NOT occur...\n");

    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR)
        printf("Not connected to server…\n");
    else
        printf("Connected to server!\n\n");

    if (iResult == SOCKET_ERROR){
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        WSACleanup();
        return 1;
    }

    Sleep(250);
    return 0;
}

void sifra(char *recvbuf) {
    for (int i = 0; i < 150; i++) {
        recvbuf[i] = recvbuf[i] ^ 55;
    }
}

int komunikacia(char message[], FILE *subor){
    char sendbuf[4096];
    int x, y, posledneX, fakeY, velkost, j;
    int recvbuflen = DEFAULT_BUFLEN;
    char recvbuf[DEFAULT_BUFLEN];

    SetConsoleOutputCP(CP_UTF8);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    velkost = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    x = consoleInfo.dwCursorPosition.X;
    y = consoleInfo.dwCursorPosition.Y;
    COORD point = {x,y};
    SetConsoleCursorPosition(hConsole, point);

    //odosielanie
    fakeY = y;
    for (int i=1; consoleInfo.dwCursorPosition.Y != fakeY; i++){
        posledneX = consoleInfo.dwCursorPosition.X + i;
        point.X += 1;
        SetConsoleCursorPosition(hConsole, point);
        GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
        printf("1");
    }

    strcpy(sendbuf,message);
    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    SetConsoleTextAttribute(hConsole, 9);
    printf("Me:\n%s\n",sendbuf);
    fprintf(subor, "Me:\n%s\n\n", sendbuf);

    //prijimanie
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
        SetConsoleTextAttribute(hConsole, 10);
        point.X = velkost/2;
        SetConsoleCursorPosition(hConsole, point);
        printf("Morpheus:\n");
        fprintf(subor, "Morpheus:\n");
        point.X = velkost/2;
        SetConsoleCursorPosition(hConsole, point);

        if (strcmp(message,"123") == 0)
            sifra(recvbuf);

        for (int i=0; recvbuf[i]!='\0' && recvbuf[i]!='\n'; i++) {
            GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
            x = consoleInfo.dwCursorPosition.X;
            if (x+2 > velkost){
                printf("\n");
                point.X = velkost/2;
                SetConsoleCursorPosition(hConsole, point);
            }
            else if(recvbuf[i] == ' '){
                j = i+1;
                while (recvbuf[j] != ' ')
                    j++;

                j -= i;
                GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
                x = consoleInfo.dwCursorPosition.X;
                if (x+j+1 > velkost){
                    printf("\n");
                    point.X = velkost/2;
                    SetConsoleCursorPosition(hConsole, point);
                    i += 1;
                }
            }
            printf("%c", recvbuf[i]);
            Sleep(100);
        }
        printf("\n");
        fprintf(subor, "%s\n\n", recvbuf);

    }
    else if (iResult == 0)
        printf("Connection closed\n");
    else
        printf("recv failed with error: %d\n", WSAGetLastError());

    SetConsoleTextAttribute(hConsole, 7);
}

void vypocetID(char* ID){
    int vysledok = 0;

    for (int i=0; i<5; i++)
        vysledok += (ID[i]-48);
    vysledok = vysledok%(ID[4]-48);
    sprintf(ID,"%d",vysledok);
}

int integral(char retazec[]){
    int suradnica = atoi(retazec);

    return suradnica;
}

int prvocisla(int cislo){
    if (cislo < 2)
        return 1;

    for (int i=2; i < cislo; i++){
        if (cislo%i == 0)
            return 1;
    }

    return 0;
}

void spajanieZnakov(char *sifra){
    char message[10];

    for (int i=0; sifra[i] != '\0'; i++){
        if (prvocisla(i) == 0)
            sprintf(message, "%s%c", message, sifra[i-1]);
    }
    strcpy(sifra,message);
}

int main() {
    int returnValue, parameter;
    char ID[] = "120397";
    char suradnica[4];
    char message[] = "XLOZGP.R.ACLHCOGAGTER";
    FILE *subor;

    subor = fopen("zaznam_komunikacie.txt", "w");
    if (subor == NULL) {
        printf("Subor neexistuje!");
        return 1;
    }

    returnValue = nastavenia();
    if (returnValue == 1)
        return 1;

    komunikacia(" ", subor);
    komunikacia(ID, subor);
    komunikacia("co teda chces", subor);
    komunikacia("8484848", subor);
    komunikacia("753422", subor);
    vypocetID(ID);
    komunikacia(ID, subor);
    komunikacia("333222333", subor);
    komunikacia("123", subor);
    komunikacia("Statue of Liberty", subor);
    parameter = integral("40");
    sprintf(suradnica, "%d", parameter);
    komunikacia(suradnica, subor);
    parameter = integral("-74");
    sprintf(suradnica, "%d", parameter);
    komunikacia(suradnica, subor);
    komunikacia("S.O.L.", subor);
    komunikacia("PRIMENUMBER", subor);
    spajanieZnakov(message);
    komunikacia(message, subor);
    komunikacia("Trinity", subor);
    komunikacia("32", subor);
    komunikacia("DQPSK", subor);

    fclose(subor);
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}
