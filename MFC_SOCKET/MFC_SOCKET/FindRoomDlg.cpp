// FindRoomDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFC_SOCKET.h"
#include "FindRoomDlg.h"
#include "afxdialogex.h"
#include "TCP_CLIENT.h"
#include "MyHead.h"
#include <vector>
using std::vector;

extern TCP_CLIENT m_Client;
extern CString g_Id;
extern CString g_Name;
extern CString SEG;
extern CString g_FindRoomRecv;
extern vector<CString>g_RoomList;

// FindRoomDlg 对话框

IMPLEMENT_DYNAMIC(FindRoomDlg, CDialogEx)

FindRoomDlg::FindRoomDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_FINDROOM, pParent)
	, m_Edit_Find_Value(_T(""))
	, m_Edit_RecvId_Value(_T(""))
	, m_Edit_RecvName_Value(_T(""))
	, m_Edit_RecvNum_Value(_T(""))
{

}

FindRoomDlg::~FindRoomDlg()
{
}

void FindRoomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Edit_Find);
	DDX_Text(pDX, IDC_EDIT1, m_Edit_Find_Value);
	DDX_Control(pDX, IDC_EDIT2, m_Edit_RecvId);
	DDX_Text(pDX, IDC_EDIT2, m_Edit_RecvId_Value);
	DDX_Control(pDX, IDC_EDIT4, m_Edit_RecvName);
	DDX_Text(pDX, IDC_EDIT4, m_Edit_RecvName_Value);
	DDX_Control(pDX, IDC_EDIT5, m_Edit_RecvNum);
	DDX_Text(pDX, IDC_EDIT5, m_Edit_RecvNum_Value);
	DDX_Control(pDX, IDC_BUTTON3, m_Button_Add);
}


BEGIN_MESSAGE_MAP(FindRoomDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &FindRoomDlg::OnBnClickedButton1)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON3, &FindRoomDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// FindRoomDlg 消息处理程序


void FindRoomDlg::OnBnClickedButton1()
{
	UpdateData(TRUE);
	if (m_Edit_Find_Value == TEXT(""))return;

	m_Client.SendData(TEXT("查找聊天室") + SEG + m_Edit_Find_Value);
}


BOOL FindRoomDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetTimer(0x10010, 50, NULL);//设置定时刷新

	return TRUE; 
}


void FindRoomDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 0x10010)
	{

		if (g_FindRoomRecv != TEXT(""))
		{
			vector<CString>nRecvText;
			SegmentText(nRecvText, g_FindRoomRecv, TEXT("|||"));
			g_FindRoomRecv = TEXT("");

			if (nRecvText[0] == TEXT("不存在"))
			{
				m_Edit_RecvNum_Value = TEXT("0人");
				m_Edit_RecvId_Value = TEXT("查无此聊天室");
				m_Edit_RecvName_Value = TEXT("查无此聊天室");
				UpdateData(FALSE);
				m_Button_Add.EnableWindow(FALSE);
			}
			else
			{
				m_Edit_RecvId_Value = nRecvText[0];
				m_Edit_RecvName_Value = nRecvText[1];
				m_Edit_RecvNum_Value = nRecvText[2] + TEXT("人");
				UpdateData(FALSE);
				m_Button_Add.EnableWindow(TRUE);
			}
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}


void FindRoomDlg::OnBnClickedButton3()
{
	for (auto &val : g_RoomList)
	{
		if (val == m_Edit_RecvId_Value)
		{
			MessageBox(TEXT("您已经在此聊天室！"), TEXT("失败！"), MB_ICONWARNING);
			return;
		}
	}
	m_Client.SendData(TEXT("加入聊天室") + SEG + m_Edit_RecvId_Value + SEG + g_Id + SEG + g_Name);
	MessageBox(TEXT("成功加入聊天室！"), TEXT("成功！"),MB_ICONINFORMATION);
	g_RoomList.push_back(m_Edit_RecvId_Value);
}
