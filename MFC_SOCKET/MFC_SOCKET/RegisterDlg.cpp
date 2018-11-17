// RegisterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFC_SOCKET.h"
#include "RegisterDlg.h"
#include "afxdialogex.h"
#include "TCP_CLIENT.h"
#include "MyHead.h"
#include <vector>
using std::vector;

extern TCP_CLIENT m_Client;
extern CString g_Id;
extern CString SEG;

IMPLEMENT_DYNAMIC(RegisterDlg, CDialogEx)

RegisterDlg::RegisterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_REGISTER, pParent)
	, m_Edit_Id_Value(_T(""))
	, m_Edit_Name_Value(_T(""))
	, m_Edit_PassWord_Value(_T(""))
	, m_ComboBox_Sex_Value(_T(""))
	, m_ComboBox_Pic_Value(_T(""))
{
	CString nDir = GetProgramDir();
	m_hIcon = ExtractIcon(AfxGetInstanceHandle(), nDir + TEXT("res\\0.ico"), 0);
}

RegisterDlg::~RegisterDlg()
{
}

void RegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_Edit_Id_Value);
	DDX_Text(pDX, IDC_EDIT3, m_Edit_Name_Value);
	DDX_Text(pDX, IDC_EDIT2, m_Edit_PassWord_Value);
	DDX_Control(pDX, IDC_COMBO1, m_ComboBox_Sex);
	DDX_CBString(pDX, IDC_COMBO1, m_ComboBox_Sex_Value);
	DDX_Control(pDX, IDC_PIC, m_Pic);
	DDX_Control(pDX, IDC_COMBO2, m_ComboBox_Pic);
	DDX_CBString(pDX, IDC_COMBO2, m_ComboBox_Pic_Value);
}


BEGIN_MESSAGE_MAP(RegisterDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &RegisterDlg::OnBnClickedButton2)
	ON_CBN_SELCHANGE(IDC_COMBO2, &RegisterDlg::OnSelchangeCombo2)
END_MESSAGE_MAP()


BOOL RegisterDlg::OnInitDialog()
{
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	CDialogEx::OnInitDialog();

	m_ComboBox_Sex.AddString(TEXT("男"));
	m_ComboBox_Sex.AddString(TEXT("女"));
	m_ComboBox_Sex.SetCurSel(0);


	m_ComboBox_Pic.AddString(TEXT("0"));
	m_ComboBox_Pic.AddString(TEXT("1"));
	m_ComboBox_Pic.AddString(TEXT("2"));
	m_ComboBox_Pic.AddString(TEXT("3"));
	m_ComboBox_Pic.AddString(TEXT("4"));
	m_ComboBox_Pic.AddString(TEXT("5"));
	m_ComboBox_Pic.SetCurSel(0);


	CString nDir = GetProgramDir();
	HICON nHicon;
	nHicon = ExtractIcon(AfxGetInstanceHandle(), nDir + TEXT("res\\0.ico"), 0);
	m_Pic.SetIcon(nHicon);

	return TRUE;
}



void RegisterDlg::OnBnClickedButton2()
{
	BOOL nRet = FALSE;
	CString nRecvData;
	CString nState;
	vector<CString>nRecvText;

	UpdateData(TRUE);

	if (m_Edit_Id_Value == TEXT("") || m_Edit_PassWord_Value == TEXT("") || m_Edit_Name_Value == TEXT(""))return;


	m_Client.SendData(TEXT("注册") + SEG + m_Edit_Id_Value + SEG + m_Edit_PassWord_Value + SEG + m_Edit_Name_Value + SEG + m_ComboBox_Sex_Value + SEG + m_ComboBox_Pic_Value);


	while (true)
	{
		nRet = m_Client.RecvData(nRecvData);

		if (nRet)
		{
			SegmentText(nRecvText, nRecvData, SEG);
			if (nRecvText.size())
			{
				nState = nRecvText[0];
				break;
			}

		}
		Sleep(5);
	}


	if (nState == TEXT("注册成功"))
	{
		MessageBox(TEXT(" 注册成功！"), TEXT("成功"),MB_ICONINFORMATION);
		SendMessage(WM_CLOSE, 0, 0);
	}
	else
	{
		MessageBox(TEXT("帐号已存在！"), TEXT("失败"),MB_ICONWARNING);
	}

	// TODO: 在此添加控件通知处理程序代码
}




void RegisterDlg::OnSelchangeCombo2()
{
	UpdateData(TRUE);
	CString nDir = GetProgramDir();
	HICON nHicon;
	nHicon = ExtractIcon(AfxGetInstanceHandle(), nDir + TEXT("res\\") + m_ComboBox_Pic_Value + TEXT(".ico"), 0);
	m_Pic.SetIcon(nHicon);
	DestroyIcon(nHicon);
}
