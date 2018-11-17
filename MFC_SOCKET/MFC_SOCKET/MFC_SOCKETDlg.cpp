
// MFC_SOCKETDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFC_SOCKET.h"
#include "MFC_SOCKETDlg.h"
#include "afxdialogex.h"
#include "MyHead.h"
#include "HomeDlg.h"
#include "RegisterDlg.h"

#include <vector>
#include <ws2tcpip.h>
using std::vector;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BOOL nLine = FALSE;
TCP_CLIENT m_Client;
CString SEG = TEXT("|#|");
CString g_Name ;
CString g_Id;
CString g_Pic;
CString g_Sex;
CString g_LineIp("127.0.0.1");//连接IP
INT g_LinePort = 9999;//连接端口




CMFC_SOCKETDlg::CMFC_SOCKETDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFC_SOCKET_DIALOG, pParent)
	, m_Edit_Account_Value(_T(""))
	, m_Edit_PassWord_Value(_T(""))
	, m_Check_Rember_Value(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_Ip = g_LineIp;
	m_Port = g_LinePort;
}

void CMFC_SOCKETDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_Button_Login);
	DDX_Control(pDX, IDC_BUTTON2, m_Button_Register);
	DDX_Text(pDX, IDC_EDIT1, m_Edit_Account_Value);
	DDX_Text(pDX, IDC_EDIT2, m_Edit_PassWord_Value);
	DDX_Control(pDX, IDC_EDIT1, m_Edit_Account);
	DDX_Control(pDX, IDC_EDIT2, m_Edit_PassWord);
	DDX_Control(pDX, IDC_CHECK1, m_Check_Rember);
	DDX_Check(pDX, IDC_CHECK1, m_Check_Rember_Value);
}

BEGIN_MESSAGE_MAP(CMFC_SOCKETDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFC_SOCKETDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFC_SOCKETDlg::OnBnClickedButton2)
END_MESSAGE_MAP()



BOOL CMFC_SOCKETDlg::OnInitDialog()
{
/*
	CString A("中文加密");
	CString B;
	CString C;
	B =EncryData(A);
	MessageBox(B, 0, 0);
	C = EncryData(B);
	MessageBox(C, 0, 0);*/

	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	
	CString nRemember,nId, nPass;

	nRemember = GetIni(TEXT("set.ini"), TEXT("Login"), TEXT("remember"));
	nId = GetIni(TEXT("set.ini"), TEXT("Login"), TEXT("id"));

	if (nRemember!=TEXT("")|| nRemember == TEXT("true"))
	{
		nPass = GetIni(TEXT("set.ini"), TEXT("Login"), TEXT("pass"));
		m_Check_Rember_Value = TRUE;
	}
	m_Edit_Account_Value = nId;
	m_Edit_PassWord_Value = nPass;
	UpdateData(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFC_SOCKETDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFC_SOCKETDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


DWORD LoginServer(CString &nStrData,PCHAR &nSendData)
{
	DWORD nSize = nStrData.GetLength()*sizeof(TCHAR);
	nSendData = new CHAR[nSize]{};
	memcpy(nSendData,&*nStrData, nSize);
	return nSize;
}


void CMFC_SOCKETDlg::OnBnClickedButton1()
{

	BOOL nRet = FALSE;
	CString nRecvData ;
	CString nState;
	vector<CString>nRecvText;

	if (!nLine)
	{
		nRet = m_Client.LineServer(m_Ip, m_Port);
		if (!nRet) return;
	}
	nLine = TRUE;

	UpdateData(TRUE);
	m_Client.SendData(TEXT("登录")+ SEG + m_Edit_Account_Value + SEG + m_Edit_PassWord_Value);

	while (true)
	{
		nRet = m_Client.RecvData(nRecvData);

		if (nRet)
		{
			SegmentText(nRecvText, nRecvData, SEG);
			if(nRecvText.size())
			{
				nState = nRecvText[0];
				break;
			}

		}
		Sleep(5);
	}

	if (nState == TEXT("登录成功"))
	{
		g_Name = nRecvText[1];
		g_Id = m_Edit_Account_Value;
		g_Pic = nRecvText[2];
		g_Sex = nRecvText[3];
		//MessageBox(g_Name + TEXT(" 登录成功！"), TEXT("成功"));
		ShowWindow(SW_HIDE);
		pHomeDlg = new HomeDlg;
		pHomeDlg->ShowWindow(SW_SHOW);


		SetIni(TEXT("set.ini"), TEXT("Login"), TEXT("remember"),m_Check_Rember_Value?TEXT("true"):TEXT("false"));
		if (m_Check_Rember_Value) 
		{ 
			SetIni(TEXT("set.ini"), TEXT("Login"), TEXT("id"), m_Edit_Account_Value);
			SetIni(TEXT("set.ini"), TEXT("Login"), TEXT("pass"), m_Edit_PassWord_Value);
		}

	}
	else if (nState == TEXT("帐号已登录"))
	{
		MessageBox(TEXT("帐号已登录！"), TEXT("失败"),MB_ICONWARNING);

	}
	else
	{
		MessageBox(TEXT("帐号密码错误！"), TEXT("失败"),MB_ICONERROR);
	}
}


void CMFC_SOCKETDlg::OnBnClickedButton2()
{
	BOOL nRet = FALSE;
	if (!nLine)
	{
		nRet = m_Client.LineServer(m_Ip, m_Port);
		if (!nRet) return;
	}
	nLine = TRUE;

	RegisterDlg nRegisterDlg;
	nRegisterDlg.DoModal();


	// TODO: 在此添加控件通知处理程序代码
}
