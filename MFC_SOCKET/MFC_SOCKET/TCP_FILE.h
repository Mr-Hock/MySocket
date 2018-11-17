#pragma once
class TCP_FILE_SEND
{
private:
	WSADATA wsaData;
	SOCKET nServerSock;
	SOCKET nClientSock;
public:
	TCP_FILE_SEND(INT nPort);
	~TCP_FILE_SEND();

	void InitSeverSock(INT nPort);
	bool WaitAccept();

	bool SendFile(CString nFileName, INT &nFileSize);
};


class TCP_FILE_RECV
{
private:
	WSADATA wsaData;
	SOCKET nClientSock;
	SOCKADDR_IN addrSrv;


public:
	TCP_FILE_RECV();
	~TCP_FILE_RECV();

	void InitClientSock();
	bool LineServer(CString ip, int port);

	bool RecvFile(CString nFileName, INT &nFileSize);
};

