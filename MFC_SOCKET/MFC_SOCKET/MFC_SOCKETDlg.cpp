
// MFC_SOCKETDlg.cpp : ʵ���ļ�
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
CString g_LineIp("127.0.0.1");//����IP
INT g_LinePort = 9999;//���Ӷ˿�




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
	CString A("���ļ���");
	CString B;
	CString C;
	B =EncryData(A);
	MessageBox(B, 0, 0);
	C = EncryData(B);
	MessageBox(C, 0, 0);*/

	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	
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

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMFC_SOCKETDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
	m_Client.SendData(TEXT("��¼")+ SEG + m_Edit_Account_Value + SEG + m_Edit_PassWord_Value);

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

	if (nState == TEXT("��¼�ɹ�"))
	{
		g_Name = nRecvText[1];
		g_Id = m_Edit_Account_Value;
		g_Pic = nRecvText[2];
		g_Sex = nRecvText[3];
		//MessageBox(g_Name + TEXT(" ��¼�ɹ���"), TEXT("�ɹ�"));
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
	else if (nState == TEXT("�ʺ��ѵ�¼"))
	{
		MessageBox(TEXT("�ʺ��ѵ�¼��"), TEXT("ʧ��"),MB_ICONWARNING);

	}
	else
	{
		MessageBox(TEXT("�ʺ��������"), TEXT("ʧ��"),MB_ICONERROR);
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


	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
