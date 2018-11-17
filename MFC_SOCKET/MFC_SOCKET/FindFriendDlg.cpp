// FindFriendDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFC_SOCKET.h"
#include "FindFriendDlg.h"
#include "afxdialogex.h"
#include "TCP_CLIENT.h"
#include "MyHead.h"
#include <vector>
using std::vector;

extern TCP_CLIENT m_Client;
extern CString g_Id;
extern CString g_Name;
extern CString SEG;
extern CString g_FindFriendRecv;
extern vector<CString>g_FriendList;

// FindFriendDlg 对话框

IMPLEMENT_DYNAMIC(FindFriendDlg, CDialogEx)

FindFriendDlg::FindFriendDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_FINDFRIEND, pParent)
	, m_Edit_Find_Value(_T(""))
	, m_Edit_RecvId_Value(_T(""))
	, m_Edit_RecvName_Value(_T(""))
	, m_Edit_RecvStatus_Value(_T(""))
{

}

FindFriendDlg::~FindFriendDlg()
{
}

void FindFriendDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Edit_Find);
	DDX_Text(pDX, IDC_EDIT1, m_Edit_Find_Value);
	DDX_Control(pDX, IDC_EDIT2, m_Edit_RecvId);
	DDX_Text(pDX, IDC_EDIT2, m_Edit_RecvId_Value);
	DDX_Control(pDX, IDC_EDIT4, m_Edit_RecvName);
	DDX_Text(pDX, IDC_EDIT4, m_Edit_RecvName_Value);
	DDX_Control(pDX, IDC_BUTTON3, m_Button_Add);
	DDX_Control(pDX, IDC_EDIT5, m_Edit_RecvStatus);
	DDX_Text(pDX, IDC_EDIT5, m_Edit_RecvStatus_Value);
}


BEGIN_MESSAGE_MAP(FindFriendDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &FindFriendDlg::OnBnClickedButton1)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON3, &FindFriendDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// FindFriendDlg 消息处理程序


void FindFriendDlg::OnBnClickedButton1()
{
	UpdateData(TRUE);
	if (m_Edit_Find_Value == TEXT(""))return;

	if (m_Edit_Find_Value == g_Id)
	{
		m_Edit_RecvStatus_Value = TEXT("当前在线");
		m_Edit_RecvId_Value = g_Id;
		m_Edit_RecvName_Value = g_Name;
		UpdateData(FALSE);
		m_Button_Add.EnableWindow(FALSE);
		return;
	}

	m_Client.SendData(TEXT("查找好友") + SEG + m_Edit_Find_Value);


}


void FindFriendDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 0x10005)
	{

		if (g_FindFriendRecv !=TEXT(""))
		{
			vector<CString>nRecvText;
			SegmentText(nRecvText, g_FindFriendRecv, TEXT("|||"));
			g_FindFriendRecv = TEXT("");

			if (nRecvText[0] == TEXT("不存在"))
			{
				m_Edit_RecvStatus_Value = TEXT("不存在此帐号");
				m_Edit_RecvId_Value = TEXT("查无此人");
				m_Edit_RecvName_Value = TEXT("查无此人");
				UpdateData(FALSE);
				m_Button_Add.EnableWindow(FALSE);
			}
			else if (nRecvText[0] == TEXT("不在线"))
			{
				m_Edit_RecvStatus_Value = TEXT("当前不在线");
				m_Edit_RecvId_Value = nRecvText[1];
				m_Edit_RecvName_Value = nRecvText[2];
				UpdateData(FALSE);
				m_Button_Add.EnableWindow(FALSE);
			}
			else
			{
				m_Edit_RecvStatus_Value = TEXT("当前在线");
				m_Edit_RecvId_Value = nRecvText[0];
				m_Edit_RecvName_Value = nRecvText[1];
				UpdateData(FALSE);
				m_Button_Add.EnableWindow(TRUE);
			}
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}


BOOL FindFriendDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetTimer(0x10005, 50, NULL);//设置定时刷新

	return TRUE; 
}


void FindFriendDlg::OnBnClickedButton3()
{
	for (auto &val: g_FriendList)
	{
		if (val== m_Edit_RecvId_Value)
		{
			MessageBox(TEXT("对方已是您的好友！"), TEXT("失败！"),MB_ICONWARNING);
			return;
		}
	}
	m_Client.SendData(TEXT("请求添加好友") + SEG + m_Edit_RecvId_Value + SEG + g_Id + SEG + g_Name);
	MessageBox(TEXT("已发送添加好友请求！"), TEXT("成功！"), MB_ICONINFORMATION);
}
