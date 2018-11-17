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
	//WSACleanup();//�ͷų�ʼ��Ws2_32.dll���������Դ��
}

void TCP_FILE_SEND::InitSeverSock(INT nPort)
{
	nServerSock = socket(AF_INET, SOCK_STREAM, 0);                     //�����˿�ʶ���׽���
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);                         //ip��ַ
	addr.sin_port = htons(nPort);                                          //�󶨶˿�
	bind(nServerSock, (SOCKADDR*)&addr, sizeof(SOCKADDR));                 //�����
	listen(nServerSock, 10);

}

bool TCP_FILE_SEND::WaitAccept()
{
	sockaddr_in clientsocket;//��ַ�ṹ�����ڴ���ͻ���Ϣ
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

	shutdown(nClientSock, SD_SEND); //�ļ���ȡ��ϣ��Ͽ����������ͻ��˷���FIN��
	return true;
}





TCP_FILE_RECV::TCP_FILE_RECV()
{
	InitClientSock();
}

TCP_FILE_RECV::~TCP_FILE_RECV()
{
	closesocket(nClientSock);
	//WSACleanup();//�ͷų�ʼ��Ws2_32.dll���������Դ��
}

void TCP_FILE_RECV::InitClientSock()
{
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		MessageBox(NULL, TEXT("��ʼ��Winsockʧ��"), TEXT("����"), MB_ICONWARNING);
		return;
	}


	nClientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (SOCKET_ERROR == nClientSock)
	{
		MessageBox(NULL, TEXT("�����׽���ʧ��"), TEXT("����"), MB_ICONWARNING);
		return;
	}
}

bool TCP_FILE_RECV::LineServer(CString ip,INT port)
{
	CStringA nTempIp(ip);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(port);//�˿ں�
	inet_pton(AF_INET, nTempIp, &addrSrv.sin_addr.S_un.S_addr);

	if (connect(nClientSock, (sockaddr*)&addrSrv, sizeof(addrSrv)) == INVALID_SOCKET)
	{
		MessageBox(NULL, TEXT("����ʧ�ܴ��������"), TEXT("����"), MB_ICONWARNING);
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
