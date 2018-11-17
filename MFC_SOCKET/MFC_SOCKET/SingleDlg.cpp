// SingleDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFC_SOCKET.h"
#include "SingleDlg.h"
#include "afxdialogex.h"
#include "TCP_CLIENT.h"
#include "MyHead.h"
#include "TCP_FILE.h"

#include <vector>
using std::vector;
extern CString g_LineIp;

HMODULE hModule;

INT g_RecvFileSize;
INT g_SendFileSize;
CString g_FileType;
CString g_SendFileRoute;
CString g_RecvFileRoute;
CString g_SendFileName;
CString g_RecvFileName;
CString g_SendIp(g_LineIp);
INT g_SendPort;
CString g_VideoIp;
INT g_VideoPort;
BOOL g_Videoing;

extern TCP_CLIENT m_Client;
extern CString g_Id;
extern CString g_Name;
extern CString SEG;
extern vector<CString>g_SingleMsg;
extern vector<CString>g_OpenSingleWin;
extern vector<CString> g_SingleRecord;
extern vector<CString>g_RecvFileMsg;
extern vector<CString>g_VideoMsg;

DWORD WINAPI SendFileThread(LPVOID lpThreadParameter);
DWORD WINAPI RecvFileThread(LPVOID lpThreadParameter);

IMPLEMENT_DYNAMIC(SingleDlg, CDialogEx)
SingleDlg::SingleDlg(CWnd* pParent, CString nId,CString nName,CString nPic)
	: CDialogEx(IDD_DIALOG_SINGLE, pParent)
	, m_Edit_Recv_Value(_T(""))
	, m_Edit_Send_Value(_T(""))
	, m_Id(nId)
	,m_Name(nName)
	, m_Pic(nPic)
{
	CString nDir = GetProgramDir();
	m_hIcon = ExtractIcon(AfxGetInstanceHandle(), nDir + TEXT("res\\") + m_Pic + TEXT(".ico"), 0);
	Create(IDD_DIALOG_SINGLE);

	CString nRoute = GetProgramDir();
	hModule = LoadLibrary(nRoute + TEXT("res\\LoadVideo.dll"));
	if (hModule)LoadVideoDll();
}
SingleDlg::~SingleDlg()
{
	FreeLibrary(hModule);
}
void SingleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Edit_Recv);
	DDX_Control(pDX, IDC_EDIT2, m_Edit_Send);
	DDX_Text(pDX, IDC_EDIT1, m_Edit_Recv_Value);
	DDX_Text(pDX, IDC_EDIT2, m_Edit_Send_Value);
	DDX_Control(pDX, IDC_SINGLEPIC, m_Pitcure_Pic);
	DDX_Control(pDX, IDC_SINGLEID, m_Static_Id);
	DDX_Control(pDX, IDC_SINGLENAME, m_Static_Name);
	DDX_Control(pDX, IDC_STATIC_Progress, m_Static_Progress);
	DDX_Control(pDX, IDC_BUTTON3, m_Button_SendFile);
	DDX_Control(pDX, IDC_BUTTON4, m_Button_RecvFile);
	DDX_Control(pDX, IDC_BUTTON5, m_Button_Video);
}
BEGIN_MESSAGE_MAP(SingleDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &SingleDlg::OnBnClickedButton1)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON2, &SingleDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &SingleDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &SingleDlg::OnBnClickedButton4)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON5, &SingleDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &SingleDlg::OnBnClickedButton6)
END_MESSAGE_MAP()


//发送私聊消息
void SingleDlg::OnBnClickedButton1()
{
	UpdateData(TRUE);

	CString nTime;
	GetTime(nTime);
	//包头，对方id，我的id，我的名字，时间，消息
	m_Client.SendData(TEXT("私聊") + SEG + m_Id + SEG + g_Id + SEG + g_Name + SEG + nTime + SEG + (m_Edit_Send_Value));

	m_Edit_Recv.SetSel(-1);
	m_Edit_Recv.ReplaceSel(g_Name + TEXT(" ") + nTime + L"\n");
	m_Edit_Recv.SetSel(-1);
	m_Edit_Recv.ReplaceSel(m_Edit_Send_Value + L"\n");
	m_Edit_Recv.SetSel(-1);
	m_Edit_Recv.ReplaceSel(L"\n");

	m_Edit_Send.SetWindowText(TEXT(""));
}

//初始化私聊窗口
BOOL SingleDlg::OnInitDialog()
{
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	CDialogEx::OnInitDialog();

	//载入信息，显示头像，名字，帐号
	SetWindowText(TEXT("与 ") + m_Name + TEXT(" 聊天"));
	m_Static_Id.SetWindowText(TEXT("帐号：" + m_Id));
	m_Static_Name.SetWindowText(TEXT("昵称：" + m_Name));
	m_Pitcure_Pic.SetIcon(m_hIcon);

	SetTimer(0x10001, 100, NULL);//设置定时刷新
	SetTimer(0x10002, 50, NULL);//设置定时刷新
	SetTimer(0x10003, 50, NULL);//设置定时刷新
	SetTimer(0x10004, 50, NULL);//设置定时刷新



	return TRUE;  
}

//响应回车键发送消息
BOOL SingleDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYUP && pMsg->hwnd == m_Edit_Send.m_hWnd)
	{
		if (pMsg->wParam == 13)
		{
			OnBnClickedButton1();
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

//关闭私聊界面
void SingleDlg::OnClose()
{
	for (DWORD i = 0; i < g_OpenSingleWin.size(); i++)
	{
		if (g_OpenSingleWin[i] == m_Id)
		{
			g_OpenSingleWin.erase(g_OpenSingleWin.begin() + i);
			break;
		}
	}


	SetTimer(0x10001, 0, NULL);//设置定时刷新
	SetTimer(0x10002, 0, NULL);//设置定时刷新
	SetTimer(0x10003, 0, NULL);//设置定时刷新
	SetTimer(0x10004, 0, NULL);//设置定时刷新

	if (g_Videoing == TRUE)
	{
/*
		CloseVideo_Recv();
		CloseVideo_Send();*/
		m_Client.SendData(TEXT("关闭视频") + SEG + m_Id + SEG + g_Id);
	}

	CDialogEx::OnClose();
	DestroyWindow();


}


void SingleDlg::OnTimer(UINT_PTR nIDEvent)
{
	vector<CString>nTemp;
	vector<CString>nRecord;
	vector<CString>nNameAndData;

	//处理私聊消息
	if (nIDEvent == 0x10001)
	{
		int nMsgNum = g_SingleMsg.size();
		int nRecordNum = g_SingleRecord.size();

		if (nMsgNum <= 0 && nRecordNum <= 0)return;

		for (int i = nMsgNum - 1; i >= 0; i--)
		{
			if (g_SingleMsg[i] == "")break;

			SegmentText(nTemp, g_SingleMsg[i], SEG);
			if (nTemp[1] == m_Id)
			{
				m_Edit_Recv.SetSel(-1);
				m_Edit_Recv.ReplaceSel(nTemp[2] + TEXT(" ") + nTemp[3] + L"\n");
				m_Edit_Recv.SetSel(-1);
				m_Edit_Recv.ReplaceSel(nTemp[4] + L"\n");
				m_Edit_Recv.SetSel(-1);
				m_Edit_Recv.ReplaceSel( L"\n"); 
				g_SingleMsg.erase(g_SingleMsg.begin() + i);
				if (nTemp[4]==TEXT("对方目前不在线！无法发送文件！"))
				{
					g_FileType = "";
					m_Button_SendFile.EnableWindow(TRUE);
				}
				else if (nTemp[4] == TEXT("对方目前不在线！无法进行视频聊天！"))
				{
/*
					CloseVideo_Recv();
					CloseVideo_Send();*/
					SetWindowsSize((int)m_hWnd, 456, 587);//调整聊天框尺寸 原来尺寸456,587
					m_Button_Video.EnableWindow(TRUE);
				}
			}
		}


		for (int i = nRecordNum - 1; i >= 0; i--)
		{
			SegmentText(nTemp, g_SingleRecord[i], SEG);

			if (nTemp[0] == m_Id)//如果id等于聊天框id
			{
				if (nTemp[1] == "")//如果聊天记录为空
				{
					g_SingleRecord.erase(g_SingleRecord.begin() + i);
					continue;
				}

				m_Edit_Recv.SetWindowText(TEXT(""));

				SegmentText(nRecord, nTemp[1], TEXT("|*|"));
				for (auto &val: nRecord)
				{
					SegmentText(nNameAndData, val, TEXT("|||"));

					m_Edit_Recv.SetSel(-1);
					m_Edit_Recv.ReplaceSel(nNameAndData[0] + TEXT(" ") + nNameAndData[1] + L"\n");
					m_Edit_Recv.SetSel(-1);
					m_Edit_Recv.ReplaceSel(nNameAndData[2] + L"\n");
					m_Edit_Recv.SetSel(-1);
					m_Edit_Recv.ReplaceSel(L"\n");
				}


			}

			g_SingleRecord.erase(g_SingleRecord.begin() + i);
		}


	}
	//刷新收发文件信息
	else if (nIDEvent == 0x10002)
	{
		if (g_FileType==TEXT("发送文件"))
		{
			CString nStr;
			nStr.Format(TEXT("%d"), g_SendFileSize / 1024 / 1024);
			m_Static_Progress.SetWindowText(TEXT("已发送：")+ nStr+TEXT("MB"));
		}
		else if (g_FileType == TEXT("接受文件"))
		{
			CString nStr;
			nStr.Format(TEXT("%d"), g_RecvFileSize / 1024 / 1024);
			m_Static_Progress.SetWindowText(TEXT("已接受：") + nStr + TEXT("MB"));
		}
		else
		{
			m_Static_Progress.SetWindowText(TEXT(""));
		}
	}
	//处理收发文件信息
	else if (nIDEvent == 0x10003)
	{
		int nMsgNum = g_RecvFileMsg.size();

		if (nMsgNum <= 0)return;

		for (int i = nMsgNum - 1; i >= 0; i--)
		{
			if (g_RecvFileMsg[i] == "")break;

			SegmentText(nTemp, g_RecvFileMsg[i], SEG);
			if (nTemp[2] == m_Id)
			{
				g_RecvFileMsg.erase(g_RecvFileMsg.begin() + i);
				
				if (nTemp[0] == TEXT("发送文件"))
				{
					m_Button_RecvFile.EnableWindow(TRUE);
					m_Button_SendFile.EnableWindow(FALSE);
					g_RecvFileName = nTemp[4];
					int nRet = MessageBox(m_Name + TEXT(" 给你发送文件：") + g_RecvFileName + TEXT("\n是否接受？"), TEXT("提示"), MB_YESNO + MB_ICONASTERISK);
					if (nRet == 6)
					{
						OnBnClickedButton4();
					}
					else
					{
						m_Button_SendFile.EnableWindow(TRUE);
						m_Button_RecvFile.EnableWindow(FALSE);
						m_Client.SendData(TEXT("拒绝文件") + SEG + m_Id + SEG + g_Id);
					}
				}
				else if (nTemp[0] == TEXT("拒绝文件"))
				{
					g_FileType = "";
					m_Button_RecvFile.EnableWindow(FALSE);
					m_Button_SendFile.EnableWindow(TRUE);

					MessageBox(m_Name + TEXT(" 拒绝接受你发送的文件：") + g_SendFileName, TEXT("提示"), MB_ICONWARNING);
				}
				else if (nTemp[0] == TEXT("接受文件"))
				{
					srand( (DWORD)time(NULL));
					g_SendPort = GetRandNum(5000,8000);
					CString nSendPort;
					nSendPort.Format(TEXT("%d"), g_SendPort);

					g_FileType = TEXT("发送文件");
					CreateThread(NULL, NULL, SendFileThread, (LPVOID)this, NULL, NULL);

					m_Client.SendData(TEXT("准备发送") + SEG + m_Id + SEG + g_Id + SEG + g_SendIp + SEG + nSendPort);

				}
				else if (nTemp[0] == TEXT("准备发送"))
				{
					g_SendIp = nTemp[3];//本机ip
					g_SendIp = nTemp[5];//服务端获取ip
					_stscanf_s(nTemp[4], TEXT("%d"), &g_SendPort);
					g_FileType = TEXT("接受文件");
					CreateThread(NULL, NULL, RecvFileThread, (LPVOID)this, NULL, NULL);

				}

			}
		}
	}
	//处理视频聊天信息
	else if (nIDEvent == 0x10004)
	{
		int nMsgNum = g_VideoMsg.size();

		if (nMsgNum <= 0)return;

		for (int i = nMsgNum - 1; i >= 0; i--)
		{
			if (g_VideoMsg[i] == "")break;

			SegmentText(nTemp, g_VideoMsg[i], SEG);
			if (nTemp[2] == m_Id)
			{
				g_VideoMsg.erase(g_VideoMsg.begin() + i);

				if (nTemp[0] == TEXT("视频聊天"))
				{
					m_Button_Video.EnableWindow(FALSE);

					int nRet = MessageBox(m_Name + TEXT(" 邀请你视频聊天\n是否接受？"), TEXT("提示"), MB_YESNO + MB_ICONASTERISK);
					if (nRet == 6)
					{

						//我的id，对方id，对方名字，端口，IP
						g_VideoIp = nTemp[5];
						_stscanf_s(nTemp[4], TEXT("%d"), &g_VideoPort);
						OnBnClickedButton6();
					}
					else
					{
						m_Button_Video.EnableWindow(TRUE);
						m_Client.SendData(TEXT("拒绝视频") + SEG + m_Id + SEG + g_Id);
					}
				}
				else if (nTemp[0] == TEXT("接受视频"))
				{
					//我的id，对方id，对方名字，端口，IP
					g_VideoIp = nTemp[5];
					_stscanf_s(nTemp[4], TEXT("%d"), &g_VideoPort);

					CHAR nIp[16]{};
					memcpy_s(nIp, 16, &*CStringA(g_VideoIp), 16);
					INT nRet = LineVideo_Send(nIp, g_VideoPort);
					if(!nRet)
					{
						CloseVideo_Recv();
						CloseVideo_Send();
						SetWindowsSize((int)m_hWnd, 456, 587);//调整聊天框尺寸 原来尺寸456,587
						m_Button_Video.EnableWindow(TRUE);
						m_Client.SendData(TEXT("关闭视频") + SEG + m_Id + SEG + g_Id);

					}
					g_Videoing = TRUE;
				}
				else if (nTemp[0] == TEXT("拒绝视频"))
				{
					CloseVideo_Recv();
					CloseVideo_Send();
					SetWindowsSize((int)m_hWnd, 456, 587);//调整聊天框尺寸 原来尺寸456,587
					m_Button_Video.EnableWindow(TRUE);
					MessageBox(m_Name + TEXT(" 拒绝了你的视频聊天邀请"), TEXT("提示"), MB_ICONWARNING);

				}
				else if (nTemp[0] == TEXT("关闭视频"))
				{
					CloseVideo_Recv();
					CloseVideo_Send();
					SetWindowsSize((int)m_hWnd, 456, 587);//调整聊天框尺寸 原来尺寸456,587
					m_Button_Video.EnableWindow(TRUE);
					g_Videoing = FALSE;
				}

			}
		}

	}

	CDialogEx::OnTimer(nIDEvent);
}



void SingleDlg::OnBnClickedButton2()
{
	m_Client.SendData(TEXT("查看私聊记录") + SEG + m_Id + SEG + g_Id);
}

//发送文件线程
DWORD WINAPI SendFileThread(LPVOID lpThreadParameter)
{
	SingleDlg *nDlg = (SingleDlg*)lpThreadParameter;

	TCP_FILE_SEND nSend(g_SendPort);

	nSend.WaitAccept();

	nSend.SendFile(g_SendFileRoute, g_SendFileSize);
	//nSend.sendfile(TEXT("F:\\梦精记2.BD1280高清韩语中字.mkv"), g_SendFileSize);

	MessageBox(0, TEXT("发送完毕！"), TEXT("成功"), MB_ICONINFORMATION);
	nDlg->m_Button_SendFile.EnableWindow(TRUE);
	g_FileType = TEXT("");

	return 0;
}
void SingleDlg::OnBnClickedButton3()
{
	g_SendFileRoute = "";
	g_SendFileRoute = GetRoute(TEXT("open"),TEXT(""));

	if (g_SendFileRoute != "")
	{
		g_SendFileName = GetFileFullName(g_SendFileRoute);

		m_Client.SendData(TEXT("发送文件") + SEG + m_Id + SEG + g_Id + SEG + g_Name + SEG + g_SendFileName);

		m_Button_SendFile.EnableWindow(FALSE);

	}

}

//接受文件线程
DWORD WINAPI RecvFileThread(LPVOID lpThreadParameter)
{
	SingleDlg *nDlg = (SingleDlg*)lpThreadParameter;
	TCP_FILE_RECV nRecv;

	nRecv.LineServer(g_SendIp,g_SendPort);

	//nRecv.recvfile(TEXT("F:\\z接受文件.mkv"), g_RecvFileSize);
	nRecv.RecvFile(g_RecvFileRoute, g_RecvFileSize);

	MessageBox(0, TEXT("接受完毕！"), TEXT("成功"), MB_ICONINFORMATION);
	nDlg->m_Button_SendFile.EnableWindow(TRUE);
	g_FileType = TEXT("");
	return 0;
}
void SingleDlg::OnBnClickedButton4()
{
	g_RecvFileRoute = "";
	g_RecvFileRoute = GetRoute(TEXT("save"), (TCHAR*)&*g_RecvFileName);
	if (g_RecvFileRoute != "")
	{
		m_Button_RecvFile.EnableWindow(FALSE);
		m_Client.SendData(TEXT("接受文件") + SEG + m_Id + SEG + g_Id);

	}
	else
	{
		m_Button_SendFile.EnableWindow(TRUE);
		m_Button_RecvFile.EnableWindow(FALSE);
		m_Client.SendData(TEXT("拒绝文件") + SEG + m_Id + SEG + g_Id);
	}

}


void SingleDlg::OnDropFiles(HDROP hDropInfo)
{
	CDialogEx::OnDropFiles(hDropInfo);

	if (!m_Button_SendFile.IsWindowEnabled())return;

	TCHAR buff[MAX_PATH]{};

	DragQueryFile(hDropInfo, 0, buff, MAX_PATH);

	g_SendFileRoute = buff;
	g_SendFileName = GetFileFullName(g_SendFileRoute);

	m_Client.SendData(TEXT("发送文件") + SEG + m_Id + SEG + g_Id + SEG + g_Name + SEG + g_SendFileName);

	m_Button_SendFile.EnableWindow(FALSE);

	DragFinish(hDropInfo);
}

//接受视频聊天邀请
void SingleDlg::OnBnClickedButton6()
{
	INT nVideoPort = GetRandNum(10000, 20000);
	CString nTempVideoPort;
	nTempVideoPort.Format(TEXT("%d"), nVideoPort);
	m_Client.SendData(TEXT("接受视频") + SEG + m_Id + SEG + g_Id + SEG + g_Name + SEG + nTempVideoPort);


	int nHwndRecv = LoadVideo_Recv();
	SetNewParent(nHwndRecv, (int)m_hWnd);//将摄像头窗口置父聊天框
	SetWindowsSize((int)m_hWnd, 830, 587);//调整聊天框尺寸 原来尺寸456,587
	MoveWindows(nHwndRecv, 480, 50);//移动摄像头窗口
	BindPort_Recv(nVideoPort);
	ShowVideo_Recv();



	int nHwndSend = LoadVideo_Send();
	SetNewParent(nHwndSend, (int)m_hWnd);//将摄像头窗口置父聊天框
	MoveWindows(nHwndSend, 480, 300);//移动摄像头窗口
	ShowVideo_Send();

	CHAR nIp[16]{};
	memcpy_s(nIp, 16, &*CStringA(g_VideoIp), 16);
	INT nRet = LineVideo_Send(nIp, g_VideoPort);
	if (!nRet)
	{
		CloseVideo_Recv();
		CloseVideo_Send();
		SetWindowsSize((int)m_hWnd, 456, 587);//调整聊天框尺寸 原来尺寸456,587
		m_Button_Video.EnableWindow(TRUE);
		m_Client.SendData(TEXT("关闭视频") + SEG + m_Id + SEG + g_Id);
		return;
	}
	g_Videoing = TRUE;

}
//发送视频聊天邀请
void SingleDlg::OnBnClickedButton5()
{

	m_Button_Video.EnableWindow(FALSE);
	INT nVideoPort = GetRandNum(10000, 20000);
	CString nTempVideoPort;
	nTempVideoPort.Format(TEXT("%d"), nVideoPort);


	int nHwndRecv = LoadVideo_Recv();
	SetNewParent(nHwndRecv, (int)m_hWnd);//将摄像头窗口置父聊天框
	SetWindowsSize((int)m_hWnd, 830, 587);//调整聊天框尺寸 原来尺寸456,587
	MoveWindows(nHwndRecv, 480, 50);//移动摄像头窗口

	BindPort_Recv(nVideoPort);

	ShowVideo_Recv();

	//CloseVideo_Recv();

	int nHwndSend = LoadVideo_Send();
	SetNewParent(nHwndSend, (int)m_hWnd);//将摄像头窗口置父聊天框
	MoveWindows(nHwndSend, 480, 300);//移动摄像头窗口

	ShowVideo_Send();

	m_Client.SendData(TEXT("视频聊天") + SEG + m_Id + SEG + g_Id + SEG + g_Name + SEG + nTempVideoPort);

	//CloseVideo_Send();

}

//载入视频dll函数
void SingleDlg::LoadVideoDll()
{
	SetWindowsSize = (pSetWindowsSize)GetProcAddress(hModule, "SetWindowsSize");
	MoveWindows = (pMoveWindows)GetProcAddress(hModule, "MoveWindows");
	SetNewParent = (pSetNewParent)GetProcAddress(hModule, "SetNewParent");

	LoadVideo_Send = (pLoadVideo_Send)GetProcAddress(hModule, "LoadVideo_Send");
	LineVideo_Send = (pLineVideo_Send)GetProcAddress(hModule, "LineVideo_Send");
	ShowVideo_Send = (pShowVideo_Send)GetProcAddress(hModule, "ShowVideo_Send");
	CloseVideo_Send = (pCloseVideo_Send)GetProcAddress(hModule, "CloseVideo_Send");

	LoadVideo_Recv = (pLoadVideo_Recv)GetProcAddress(hModule, "LoadVideo_Recv");
	BindPort_Recv = (pBindPort_Recv)GetProcAddress(hModule, "BindPort_Recv");
	ShowVideo_Recv = (pShowVideo_Recv)GetProcAddress(hModule, "ShowVideo_Recv");
	CloseVideo_Recv = (pCloseVideo_Recv)GetProcAddress(hModule, "CloseVideo_Recv");
}



