#define _CRT_SECURE_NO_WARNINGS
#define _AFXDLL

#include <iostream>
#include <string>
#include <stdio.h>

//Libreria para CString
#include <atlstr.h>
//Libreria para time
#include <time.h>
//Libreria para creacion de archivos
//#include <C_archiv.hpp>
#include <fstream>
//Libreria para socket de windows
#include <WS2tcpip.h>
//Liberia para ventana
#include <cstdlib>
#include <Windows.h>
#include <WinUser.h>
#include <conio.h>

#pragma comment (lib, "ws2_32.lib")

struct M0001Recibe
{
	int iEstado;
	char cVentana[1024];
};

struct M0001Envia
{
	int iEstado;
	bool bResultado;
};

//Globales
char cLog[1024]={0};
char SendBuff[sizeof(M0001Envia)], GetBuff[sizeof(M0001Recibe)];
M0001Recibe skRecibe;
M0001Envia skEnvia;

void darSeguimiento (const char *cSuceso);
void buscaVentana(const char *cVentana);

int main (int argc, char *argv[])
{
	//Inicializamos el socket
	sprintf(cLog, "...  I N I C I A    V A L I D A V E N T A N A  ...");
	darSeguimiento(cLog);
	WSAData wsData;
	WORD ver = MAKEWORD(2, 2);
	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != ERROR)
	{
		sprintf(cLog,"No se pudo inicializar el socket. iEstado %d", -1);
		darSeguimiento(cLog);
		skEnvia.iEstado=-1;
	}
	//Configuramos el sockt
	sockaddr_in local;
	local.sin_family=AF_INET;
	local.sin_port=htons(54000); //asignamos el puerto
				//host to network short
	local.sin_addr.S_un.S_addr=INADDR_ANY;

	//Creamos el socket
	SOCKET escuchaSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (escuchaSocket==INVALID_SOCKET)
	{
		sprintf(cLog,"No se pudo configurar el socket. iEstado %d",-2);
		darSeguimiento(cLog);
		skEnvia.iEstado=-2;
	}
	//Asosiamos la IP y puerto
	int resp = bind(escuchaSocket, (sockaddr*)&local, sizeof(local));
	if (resp==SOCKET_ERROR)
	{
		closesocket(escuchaSocket);
		WSACleanup();
		skEnvia.iEstado=-3;
		sprintf(cLog, "No se pudo asociar el socket. iEstado %d", -3);
		darSeguimiento(cLog);
	}

	if (listen(escuchaSocket, 1)==SOCKET_ERROR)
	{
		closesocket(escuchaSocket);
		WSACleanup();
		skEnvia.iEstado=-4;
		sprintf(cLog, "Error al habilitar conexiones entrantes. iEstado %d", -4);
		darSeguimiento(cLog);
	}
	
	SOCKET clienteSocket;
	while (true)
	{
		sprintf(cLog, "Esperando conexiones entrantes...");
		darSeguimiento(cLog);
		skEnvia.iEstado=0;
		skEnvia.bResultado=false;
		//Aceptamos conexion entrantes
		int stSize = sizeof(sockaddr);
		sockaddr_in cliente;
		clienteSocket=accept(escuchaSocket, (sockaddr*)&cliente, &stSize);
		if (clienteSocket==INVALID_SOCKET)
		{
			closesocket(escuchaSocket);
			WSACleanup();
			skEnvia.iEstado=-5;
			sprintf(cLog, "Error al aceptar conexiones. iEstado %d", -5);
			darSeguimiento(cLog);
		}
		sprintf(cLog, "Conexion desde %s ", inet_ntoa(cliente.sin_addr));
		darSeguimiento(cLog);
		recv(clienteSocket, GetBuff, sizeof(M0001Recibe), 0);
		memcpy(&skRecibe, GetBuff, sizeof(M0001Recibe));
		sprintf(cLog,"Datos recibidos. iEstado = : %d ", skRecibe.iEstado);
		darSeguimiento(cLog);
		if (skRecibe.iEstado==1)
		{
			sprintf(cLog, "Ventana a checar: %s", skRecibe.cVentana);
			darSeguimiento(cLog);
			buscaVentana(skRecibe.cVentana);
			memcpy(SendBuff, &skEnvia, sizeof(M0001Envia));
		}
		send(clienteSocket, SendBuff, sizeof(skEnvia),0);
		
		char * vOut = skEnvia.bResultado? "true" : "false";
		sprintf(cLog, "Se envia respuesta al cliente... skEnvia.iEstado= %d , skEnvia.bResultado= %s", skEnvia.iEstado, vOut);
		darSeguimiento(cLog);
	}
	closesocket(clienteSocket);
	sprintf(cLog, "Se cierra el socket");
	darSeguimiento(cLog);
	WSACleanup();
}

void darSeguimiento(const char *cSuceso)
{
	CString sTexto="";
	char cTexto[2048]={0}, cArchivo[100]={0};
	time_t tTiempo;
	tm *tHora;

	sTexto.Format("%s", cSuceso);
	for (char cPos = 1; cPos < 32; cPos++)
	{
		sTexto.Replace(cPos, 32);
	}

	tTiempo=time(NULL);
	tHora=localtime(&tTiempo);
	
	sprintf(cTexto, "[%02d/%02d/%04d %02d:%02d:%02d] :: %s",
		tHora->tm_mday,
		tHora->tm_mon+1,
		tHora->tm_year+1900,
		tHora->tm_hour,
		tHora->tm_min,
		tHora->tm_sec,
		sTexto);

	sprintf ( cArchivo, "C:\\SYS\\MEM\\LOGVALIDAVENTANA.TXT");
	std::fstream file;
	file.open(cArchivo, std::fstream::in|std::fstream::out|std::fstream::app);
	file<<cTexto<<'\n';
	file.close();
}

void buscaVentana(const char *cVentana)
{
	sprintf(cLog, "Dentro de buscaVentana");
	darSeguimiento(cLog);
	skEnvia.iEstado=1;

	LPCTSTR WindowName = cVentana;
	HWND Find=FindWindow(NULL, WindowName);
	if (Find)
	{
		sprintf(cLog, "Se encontro la ventana: %s", cVentana);
		darSeguimiento(cLog);
		skEnvia.bResultado=true;
	}
	else
	{
		sprintf(cLog, "NO se encontro la ventana: %s", cVentana);
		darSeguimiento(cLog);
		skEnvia.bResultado=false;
	}
}