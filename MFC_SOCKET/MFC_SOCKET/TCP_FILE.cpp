#include "stdafx.h"
#include "TCP_FILE.h"
#include <WS2tcpip.h>


TCP_FILE_SEND::TCP_FILE_SEND(INT nPort)
{
	InitSeverSock(nPort);
}


TCP_FILE_SEND::~TCP_FILE_SEND()
{
	closesocket(nServerSock);
	//WSACleanup();//释放初始化Ws2_32.dll所分配的资源。
}

void TCP_FILE_SEND::InitSeverSock(INT nPort)
{
	nServerSock = socket(AF_INET, SOCK_STREAM, 0);                     //创建了可识别套接字
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);                         //ip地址
	addr.sin_port = htons(nPort);                                          //绑定端口
	bind(nServerSock, (SOCKADDR*)&addr, sizeof(SOCKADDR));                 //绑定完成
	listen(nServerSock, 10);

}

bool TCP_FILE_SEND::WaitAccept()
{
	sockaddr_in clientsocket;//地址结构体用于储存客户信息
	int len = sizeof(sockaddr);
	nClientSock = accept(nServerSock, (SOCKADDR*)&clientsocket, &len);
	return true;
}

bool TCP_FILE_SEND::SendFile(CString nFileName, INT &nFileSize)
{
	FILE *pFile;
	fopen_s(&pFile, CStringA(nFileName), CStringA("rb"));

	rewind(pFile);

	int nCount;
	char *buff = new char[1024];
	nFileSize = 0;
	while ((nCount = fread(buff, 1, 1024, pFile)) > 0)
	{
		nFileSize += 1024;
		send(nClientSock, buff, nCount, 0);
	}
	fclose(pFile);
	delete[]buff;

	shutdown(nClientSock, SD_SEND); //文件读取完毕，断开输出流，向客户端发送FIN包
	return true;
}





TCP_FILE_RECV::TCP_FILE_RECV()
{
	InitClientSock();
}

TCP_FILE_RECV::~TCP_FILE_RECV()
{
	closesocket(nClientSock);
	//WSACleanup();//释放初始化Ws2_32.dll所分配的资源。
}

void TCP_FILE_RECV::InitClientSock()
{
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		MessageBox(NULL, TEXT("初始化Winsock失败"), TEXT("错误"), MB_ICONWARNING);
		return;
	}


	nClientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (SOCKET_ERROR == nClientSock)
	{
		MessageBox(NULL, TEXT("创建套接字失败"), TEXT("错误"), MB_ICONWARNING);
		return;
	}
}

bool TCP_FILE_RECV::LineServer(CString ip,INT port)
{
	CStringA nTempIp(ip);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(port);//端口号
	inet_pton(AF_INET, nTempIp, &addrSrv.sin_addr.S_un.S_addr);

	if (connect(nClientSock, (sockaddr*)&addrSrv, sizeof(addrSrv)) == INVALID_SOCKET)
	{
		MessageBox(NULL, TEXT("连接失败传输服务器"), TEXT("错误"), MB_ICONWARNING);
		return false;
	}
	return true;
}

bool TCP_FILE_RECV::RecvFile(CString nFileName, INT &nFileSize)
{
	FILE *pFile;
	fopen_s(&pFile, CStringA(nFileName), "wb");
	fclose(pFile);
	fopen_s(&pFile, CStringA(nFileName), "ab");


	char *buff = new char[1024];
	nFileSize = 0;
	int nCount;
	while ((nCount = recv(nClientSock, buff, 1024, 0)) > 0)
	{
		nFileSize += nCount;
		fwrite(buff, nCount, 1, pFile);
	}
	fclose(pFile);
	delete[]buff;

	return true;

}
