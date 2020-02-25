#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main()
{
	string ipAddress = "127.0.0.1"; //Direccion Ip del servidor
	int port = 54000;				//Puerto de escucha del servidor

	//Inicializar winsock
	WSADATA data;
	WORD ver=MAKEWORD(2,2);
	int wsResult=WSAStartup(ver, &data);
	if (wsResult!=ERROR)
	{
		cerr<<"No se pudo inicializar winsok, Error #"<<wsResult<<endl;
		return;
	}

	//Crear el socket
						//ver 4 de tcpip //ver 6 de ip AF_INET6
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cerr<<"No se pudo crear el socket, Error #"<<WSAGetLastError()<<endl;
		WSACleanup();
		return;
	}

	//Llenar estructura
	sockaddr_in hint;
	hint.sin_family=AF_INET;
	hint.sin_port=htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	//Conectar al servidor
	int connResult = connect (sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult==SOCKET_ERROR)
	{
		cerr<<"No se pudo conectar al servidor, Error #"<<WSAGetLastError()<<endl;
		closesocket(sock);
		WSACleanup();
		return;
	}
	
	//Ciclo do-while, para enviar y recibir datos
	char buf[4096];
	string userInput;
	do
	{
		//Pedir texto al usuario
		cout<<"> ";
		getline(cin, userInput);

		if (userInput.size()>0)//se valida que no se haya tecleado algo
		{
			//Enviar el texto
			int sendResult= send(sock, userInput.c_str(), userInput.size()+1, 0); //Se le suma 1, porque en C, finaliza con un 0
			if (sendResult!=SOCKET_ERROR)
			{
				//Esperar respuesta
				ZeroMemory(buf, 4096);
				int byteRecived=recv(sock, buf, 4096, 0);
				if (byteRecived>0)
				{
					//Echo, respuesta por consola
					cout<<"SERVER> "<< string (buf,0, byteRecived)<<endl;
				}
			}
		}
	} while (userInput.size()>0);
	
	//Cerrar y limpiar todo
	closesocket(sock);
	WSACleanup();
}