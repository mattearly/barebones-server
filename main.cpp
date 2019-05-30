#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

int main()
{
  // init winsock
  WSADATA winsockData;
  WORD ver = MAKEWORD(2, 2);
  
  int winsockOk = WSAStartup(ver, &winsockData);
  if (winsockOk != 0)
  {
    std::cerr << "can't init winsock.\n";
    return -1;
  }
  // create socket
  SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listeningSocket == INVALID_SOCKET)
  {
    std::cerr << "can't create socket.\n";
    return -1;
  }
  // bind ip address and port to a socket
  sockaddr_in hint;
  hint.sin_family = AF_INET;
  hint.sin_port = htons(54000);
  hint.sin_addr.S_un.S_addr = INADDR_ANY; // could use inet pton
  
  bind(listeningSocket, (sockaddr*)&hint, sizeof(hint));
  // tell winsock the socket is for listening
  listen(listeningSocket, SOMAXCONN);
  // wait for a connection
  sockaddr_in client;
  int clientSize = sizeof(client);

  SOCKET clientSocket = accept(listeningSocket, (sockaddr*)& client, &clientSize);
  if (clientSocket == INVALID_SOCKET)
  {
    std::cerr << "client socket problem.\n";
    return -1;
  }
  char host[NI_MAXHOST];   // clients remote name
  char service[NI_MAXHOST];  // service port the client is connected on

  ZeroMemory(host, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST)
  ZeroMemory(service, NI_MAXHOST);

  if (getnameinfo((sockaddr*)& client, sizeof(client), host, NI_MAXHOST,
    service, NI_MAXSERV, 0) == 0)
  {
    std::cout << host << " connected on port " << service << std::endl;
  }
  else
  {
    inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
    std::cout << host << " connect on port " << ntohs(client.sin_port) << std::endl;
  }
  // close listening socket
  closesocket(listeningSocket);
  // while loop: accept and echo message back to client
  char buf[4096];

  while (true)
  {
    ZeroMemory(buf, 4096);

    // wait for client to send data
    int bytesReceived = recv(clientSocket, buf, 4096, 0);
    if (bytesReceived == SOCKET_ERROR)
    {
      std::cerr << "errer in recieve\n";
      break;
    }

    if (bytesReceived == 0)
    {
      std::cout << "Client disconnected\n";
      break;
    }

    // echo message back to client
    send(clientSocket, buf, bytesReceived + 1, 0);

  }
  //close socket
  closesocket(clientSocket);
  //clean up winsock
  WSACleanup();
}