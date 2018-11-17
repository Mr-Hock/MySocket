// RoomDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFC_SOCKET.h"
#include "RoomDlg.h"
#include "afxdialogex.h"
#include "TCP_CLIENT.h"
#include "SingleDlg.h"
#include "MyHead.h"
#include <vector>
using std::vector;

extern TCP_CLIENT m_Client;
extern CString g_Id;
extern CString g_Name;
extern CString SEG;
extern vector<CString>g_RoomMsg;
extern vector<CString> g_RoomMember;
extern vector<CString>g_OpenRoomWin;
extern vector<CString>g_OpenSingleWin;
extern vector<CString> g_RoomRecord;
extern vector<CString>g_FriendList;

IMPLEMENT_DYNAMIC(RoomDlg, CDialogEx)

RoomDlg::RoomDlg(CWnd* pParent,CString nId,CString nName)
	: CDialogEx(IDD_DIALOG_ROOM, pParent)
	, m_Edit_Recv_Value(_T(""))
	, m_Edit_Send_Value(_T(""))
	, m_RoomId(nId)
	, m_RoomName(nName)
{
	CString nDir = GetProgramDir();
	m_hIcon = ExtractIcon(AfxGetInstanceHandle(), nDir + TEXT("res\\r.ico"), 0);

	Create(IDD_DIALOG_ROOM);
}

RoomDlg::~RoomDlg()
{
}

void RoomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Edit_Recv);
	DDX_Control(pDX, IDC_EDIT2, m_Edit_Send);
	DDX_Text(pDX, IDC_EDIT1, m_Edit_Recv_Value);
	DDX_Text(pDX, IDC_EDIT2, m_Edit_Send_Value);
	DDX_Control(pDX, IDC_LIST1, m_List_Online);
	DDX_Control(pDX, IDC_ROOMPIC, m_Picture_Pic);
	DDX_Control(pDX, IDC_ROOMID, m_Static_Id);
	DDX_Control(pDX, IDC_ROOMNAME, m_Static_Name);
}


BEGIN_MESSAGE_MAP(RoomDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &RoomDlg::OnBnClickedButton1)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &RoomDlg::OnDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON2, &RoomDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// RoomDlg 消息处理程序


void RoomDlg::OnBnClickedButton1()
{
	UpdateData(TRUE);
	if (m_Edit_Send_Value == TEXT(""))return;

	CString nTime;
	GetTime(nTime);
	m_Client.SendData(TEXT("聊天室") + SEG + m_RoomId + SEG + m_RoomName + SEG + g_Name + SEG + nTime + SEG + m_Edit_Send_Value);

	m_Edit_Send.SetWindowText(TEXT(""));
}


BOOL RoomDlg::OnInitDialog()
{
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	CDialogEx::OnInitDialog();

	m_List_Online.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_List_Online.InsertColumn(0, TEXT("成员"), 0, 60);
	m_List_Online.InsertColumn(1, TEXT("状态"), 0, 60);

	SetWindowText(m_RoomName + TEXT(" 聊天室ID:") + m_RoomId);

	SetTimer(0x10002, 100, NULL);//设置定时刷新
	SetTimer(0x10003, 1000, NULL);//设置定时刷新

	m_Client.SendData(TEXT("聊天室成员") + SEG + m_RoomId);

	m_Static_Id.SetWindowText(TEXT("聊天室ID：" + m_RoomId));
	m_Static_Name.SetWindowText(TEXT("聊天室名字：" + m_RoomName));
	m_Picture_Pic.SetIcon(m_hIcon);

	return TRUE; 
}


BOOL RoomDlg::PreTranslateMessage(MSG* pMsg)
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


void RoomDlg::OnClose()
{
	for (DWORD i = 0; i < g_OpenRoomWin.size(); i++)
	{
		if (g_OpenRoomWin[i] == m_RoomId)
		{
			g_OpenRoomWin.erase(g_OpenRoomWin.begin() + i);
			break;
		}
	}

	SetTimer(0x10002, 0, NULL);//设置定时刷新
	SetTimer(0x10003, 0, NULL);//设置定时刷新
	CDialogEx::OnClose();
	DestroyWindow();
}

void RoomDlg::PrintOnline(CString nData)
{
	if (nData.IsEmpty())
	{
		m_List_Online.DeleteAllItems();
		return;
	}

	vector<CString>nNameText;
	vector<CString>nTemp;
	DWORD nIndex = 0;
	DWORD nSub = 0;

	nIndex = m_List_Online.GetItemCount();

	SegmentText(nNameText, nData, TEXT("|||"));

	if (nIndex > nNameText.size())
	{
		nSub = nIndex - nNameText.size();
		for (DWORD i = 0; i < nSub; i++)
		{
			m_List_Online.DeleteItem(m_List_Online.GetItemCount() - 1);
		}
	}
	else if (nIndex < nNameText.size())
	{
		nSub = nNameText.size() - nIndex;
		for (DWORD i = 0; i < nSub; i++)
		{
			m_List_Online.InsertItem(m_List_Online.GetItemCount(), TEXT(""));
		}
	}


	for (DWORD i = 0; i < nNameText.size(); i++)
	{
		SegmentText(nTemp, nNameText[i], TEXT("#"));

		m_List_Online.SetItemData(i, nTemp[0]);
		m_List_Online.SetItemText(i, 0, nTemp[1]);
		m_List_Online.SetItemText(i, 1, nTemp[2]);
	}

}

void RoomDlg::OnTimer(UINT_PTR nIDEvent)
{
	vector<CString>nTemp;
	vector<CString>nRecord;
	vector<CString>nNameAndData;

	if (nIDEvent == 0x10002)
	{
		int nMsgNum = g_RoomMsg.size();
		int nRecordNum = g_RoomRecord.size();

		if (nMsgNum <= 0 && nRecordNum <= 0)return;

		for (int i = nMsgNum - 1; i >= 0; i--)
		{
			if (g_RoomMsg[i] == "")break;

			SegmentText(nTemp, g_RoomMsg[i], SEG);
			if (nTemp[0] == m_RoomId)
			{
				m_Edit_Recv.SetSel(-1);
				m_Edit_Recv.ReplaceSel(nTemp[2] + TEXT(" ") + nTemp[3] + L"\n");
				m_Edit_Recv.SetSel(-1);
				m_Edit_Recv.ReplaceSel(nTemp[4] + L"\n");
				m_Edit_Recv.SetSel(-1);
				m_Edit_Recv.ReplaceSel(L"\n");
				g_RoomMsg.erase(g_RoomMsg.begin() + i);
			}
		}


		for (int i = nRecordNum - 1; i >= 0; i--)
		{
			SegmentText(nTemp, g_RoomRecord[i], SEG);

			if (nTemp[0] == m_RoomId)//如果id等于聊天框id
			{
				if (nTemp[1] == "")//如果聊天记录为空
				{
					g_RoomRecord.erase(g_RoomRecord.begin() + i);
					continue;
				}

				m_Edit_Recv.SetWindowText(TEXT(""));

				SegmentText(nRecord, nTemp[1], TEXT("|*|"));
				for (auto &val : nRecord)
				{
					SegmentText(nNameAndData, val, TEXT("|||"));

					m_Edit_Recv.SetSel(-1);
					m_Edit_Recv.ReplaceSel(nNameAndData[0] + TEXT(" ") + nNameAndData[1] + L"\n");
					m_Edit_Recv.SetSel(-1);
					m_Edit_Recv.ReplaceSel(nNameAndData[2] + L"\n");
					m_Edit_Recv.SetSel(-1);
					m_Edit_Recv.ReplaceSel(L"\n");
				}

				g_RoomRecord.erase(g_RoomRecord.begin() + i);
			}
		
		}

	}
	else if (nIDEvent == 0x10003)
	{
		m_Client.SendData(TEXT("聊天室成员") + SEG + m_RoomId);

		for (int i = g_RoomMember.size() - 1; i >= 0; i--)
		{
			SegmentText(nRecord, g_RoomMember[i], SEG);

			if (nRecord[0] == m_RoomId)
			{
				PrintOnline(nRecord[1]);
				g_RoomMember.erase(g_RoomMember.begin() + i);
				break;
			}
			
			
		}
		
	}
	CDialogEx::OnTimer(nIDEvent);
}

BOOL IsFriend(CString nId)
{
	for (auto &val : g_FriendList)
	{
		if (val == nId)
		{
			return TRUE;
		}

	}
	return FALSE;
}

void RoomDlg::OnDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;


	DWORD nIndex = m_List_Online.GetSelectEx();
	if (nIndex == -1)return;

	CString nId;
	nId = m_List_Online.GetItemDataStr(nIndex);
	CString nName = m_List_Online.GetItemText(nIndex, 0);

	if (nId == g_Id)return;

	BOOL nIsFriend = IsFriend(nId);
	if (nIsFriend == FALSE)
	{
		DWORD nRet;
		nRet = MessageBox(TEXT("帐号：") + nId + TEXT("\n昵称：") + nName + TEXT("\n还不是您的好友！无法私聊！\n是否添加对方为好友？"),
			TEXT("提示"), MB_YESNO + MB_ICONASTERISK);
		if (nRet == 6)
		{
			m_Client.SendData(TEXT("请求添加好友") + SEG + nId + SEG + g_Id + SEG + g_Name);
			MessageBox(TEXT("已发送添加好友请求！"), TEXT("成功！"));
		}
		return;
	}

	for (auto &val : g_OpenSingleWin)
	{
		if (val == nId)return;
	}
	g_OpenSingleWin.push_back(nId);

	SingleDlg *pSingleDlg;
	pSingleDlg = new SingleDlg(NULL, nId, nName,TEXT("0"));
	pSingleDlg->ShowWindow(SW_SHOW);
}


void RoomDlg::OnBnClickedButton2()
{
	m_Client.SendData(TEXT("查看聊天室记录") + SEG + m_RoomId);

}
