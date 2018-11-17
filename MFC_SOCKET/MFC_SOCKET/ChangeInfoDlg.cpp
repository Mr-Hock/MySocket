// ChangeInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFC_SOCKET.h"
#include "ChangeInfoDlg.h"
#include "afxdialogex.h"
#include "MyHead.h"
#include "TCP_CLIENT.h"

extern TCP_CLIENT m_Client;
extern CString g_Id;
extern CString SEG;
extern CString g_Name;
extern CString g_Pic;
extern CString g_Sex;


// ChangeInfoDlg 对话框

IMPLEMENT_DYNAMIC(ChangeInfoDlg, CDialogEx)

ChangeInfoDlg::ChangeInfoDlg(CWnd* pParent,CString nPic,CString nName,CString nSex)
	: CDialogEx(IDD_DIALOG_CHANGEINFO, pParent)
	, m_Edit_Name_Value(_T(""))
	, m_Pic(nPic)
	,m_Name(nName)
	,m_Sex(nSex)
	, m_Combo_Pic_Value(_T(""))
	, m_Combo_Sex_Value(_T(""))
{

}

ChangeInfoDlg::~ChangeInfoDlg()
{
}

void ChangeInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO2, m_Combo_Pic);
	DDX_Control(pDX, IDC_PICINFO, m_Picture_Pic);
	DDX_Control(pDX, IDC_EDIT3, m_Edit_Name);
	DDX_Text(pDX, IDC_EDIT3, m_Edit_Name_Value);
	DDX_Control(pDX, IDC_COMBO1, m_Combo_Sex);
	DDX_CBString(pDX, IDC_COMBO2, m_Combo_Pic_Value);
	DDX_CBString(pDX, IDC_COMBO1, m_Combo_Sex_Value);
}


BEGIN_MESSAGE_MAP(ChangeInfoDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &ChangeInfoDlg::OnBnClickedButton2)
	ON_CBN_SELCHANGE(IDC_COMBO2, &ChangeInfoDlg::OnSelchangeCombo2)
END_MESSAGE_MAP()


// ChangeInfoDlg 消息处理程序


void ChangeInfoDlg::OnBnClickedButton2()
{
	UpdateData(TRUE);

	if (m_Edit_Name_Value == TEXT(""))return;


	m_Client.SendData(TEXT("修改资料") + SEG + g_Id + SEG + m_Combo_Pic_Value + SEG + m_Edit_Name_Value + SEG + m_Combo_Sex_Value);

	g_Pic = m_Combo_Pic_Value;
	g_Name = m_Edit_Name_Value;
	g_Sex = m_Combo_Sex_Value;

	MessageBox(TEXT(" 修改成功！"), TEXT("成功"), MB_ICONINFORMATION);
	SendMessage(WM_CLOSE, 0, 0);

}


BOOL ChangeInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Combo_Sex.AddString(TEXT("男"));
	m_Combo_Sex.AddString(TEXT("女"));
	if(m_Sex==TEXT("男"))m_Combo_Sex.SetCurSel(0);
	else m_Combo_Sex.SetCurSel(1);
	
	m_Combo_Pic.AddString(TEXT("0"));
	m_Combo_Pic.AddString(TEXT("1"));
	m_Combo_Pic.AddString(TEXT("2"));
	m_Combo_Pic.AddString(TEXT("3"));
	m_Combo_Pic.AddString(TEXT("4"));
	m_Combo_Pic.AddString(TEXT("5"));

	INT nSelect;
	_stscanf_s(m_Pic, TEXT("%d"), &nSelect);
	m_Combo_Pic.SetCurSel(nSelect);


	CString nDir = GetProgramDir();
	HICON nHicon;
	nHicon = ExtractIcon(AfxGetInstanceHandle(), nDir + TEXT("res\\") + m_Pic + TEXT(".ico"), 0);
	m_Picture_Pic.SetIcon(nHicon);

	m_Edit_Name.SetWindowText(m_Name);

	return TRUE;
}


void ChangeInfoDlg::OnSelchangeCombo2()
{
	UpdateData(TRUE);
	CString nDir = GetProgramDir();
	HICON nHicon;
	nHicon = ExtractIcon(AfxGetInstanceHandle(), nDir + TEXT("res\\") + m_Combo_Pic_Value + TEXT(".ico"), 0);
	m_Picture_Pic.SetIcon(nHicon);
	DestroyIcon(nHicon);
}
