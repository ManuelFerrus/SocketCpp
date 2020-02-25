#include <iostream>
#include <string>
//libreria para socket de windows
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main()
{
	//iniciarlizar winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	int wsOk=WSAStartup(ver, &wsData);
	if (wsOk != ERROR)
	{
		cerr<< "No se pudo inicializar winsock"<<endl;
		return;
	}
	//crear el socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr<<"No se pudo crear el socket"<<endl;
		return;
	}
	//bind al socket la ip y puerto
	sockaddr_in hint;
	hint.sin_family=AF_INET;
	hint.sin_port=htons(54000); //asignamos el puerto
		//htons = host to network short
	hint.sin_addr.S_un.S_addr=INADDR_ANY; //tambien se puede usar inet_pton ...
	bind(listening, (sockaddr*)&hint, sizeof(hint));
	//poner el socket en escucha
	listen(listening, SOMAXCONN);
	//esperar conexiones
	sockaddr_in client;
	int clientSize=sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	
	char host[NI_MAXHOST];		//Nombre del cliente remoto
	char service[NI_MAXHOST];	//Servicio (i.e. puerto) el cliente esta conectado

	ZeroMemory(host, NI_MAXHOST); //es lo mismo que memset(host, 0, NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXHOST, 0)==0)
	{
		cout<<host<<"conectado al puerto"<<service<<endl;
	}
	else
	{
			inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
			cout<<host<<"2 conectado al puerto"<<ntohs(client.sin_port)<<endl;
	}
	//dejar de escuchar
	closesocket(listening);
	//while: aceptar conexiones y regresar un mensaje de regreso al cliente
	char buf[4096];
	while (true)
	{
		ZeroMemory(buf, 4096);
		//esperar los datos enviados del cliente
		int byteReceived=recv(clientSocket, buf, 4096, 0);
		//if (strcmp(buf, "\r\n")!=0) //Se valida que no sea un saldo de linea
		{
			if (byteReceived==SOCKET_ERROR)
			{
				cerr<<"Error recv()."<<endl;
				break;
			}
			if (byteReceived==ERROR)
			{
				cout<<"Cliente desconectado"<<endl;
				break;
			}
			cout<<string(buf,0,byteReceived)<<endl;
			//cout<<string(buf, 0, byteReceived)<<endl;
			//echo, mensaje de regreso al cliente
			//std::string answer("Server: ");
			//answer.append(buf);
			//char*response = const_cast<char*>(answer.c_str());
			//int byteSend = strlen(response);
			//send(clientSocket, response, byteSend+1, 0); //se le tiene que sumar un valor de memoria a los byte recibidos
			send(clientSocket, buf, byteReceived+1, 0); //se le tiene que sumar un valor de memoria a los byte recibidos
		}
		//else//Se envia el salto de linea
		//{
		//	if (byteReceived==SOCKET_ERROR)
		//	{
		//		cerr<<"Error recv()."<<endl;
		//		break;
		//	}
		//	if (byteReceived==ERROR)
		//	{
		//		cout<<"Cliente desconectado"<<endl;
		//		break;
		//	}
		//	send(clientSocket, buf, byteReceived+1, 0); //se le tiene que sumar un valor de memoria a los byte recibidos
		//}
	}
	//cerrar el socket
	closesocket(clientSocket);
	//Cleanup winsock
	WSACleanup();
	system("pause");
}