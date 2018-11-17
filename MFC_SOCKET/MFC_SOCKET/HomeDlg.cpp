// HomeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFC_SOCKET.h"
#include "HomeDlg.h"
#include "afxdialogex.h"
#include "MyHead.h"
#include "SingleDlg.h"
#include "RoomDlg.h"
#include "FindFriendDlg.h"
#include "FindRoomDlg.h"
#include "CreatRoomDlg.h"
#include "ChangeInfoDlg.h"

#include <vector>
using std::vector;

extern TCP_CLIENT m_Client;//客户端
extern CString SEG ;//分隔符
extern CString g_Name;//名字
extern CString g_Id;//帐号
extern CString g_Pic;//头像
extern CString g_Sex;//性别
vector<CString>g_SingleMsg;//私聊消息容器
vector<CString>g_RoomMsg;//聊天室消息容器
vector<CString> g_RoomMember;//聊天室成员容器
vector<CString>g_OpenSingleWin;//私聊窗口
vector<CString>g_OpenRoomWin;//聊天室窗口
CString g_FindFriendRecv;//查找好友
CString g_FindRoomRecv;//查找聊天室
CString g_CreatRoomRecv;//创建聊天室
vector<CString>g_FriendList;//好友列表容器
vector<CString>g_RoomList;//聊天室容器
vector<CString> g_SingleRecord;//私聊记录容器
vector<CString> g_RoomRecord;//聊天室记录容器
vector<CString>g_RecvFileMsg;//收发文件信息容器
vector<CString>g_VideoMsg;//视频聊天信息容器

// HomeDlg 对话框

IMPLEMENT_DYNAMIC(HomeDlg, CDialogEx)

HomeDlg::HomeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_HOME, pParent)
{
	Create(IDD_DIALOG_HOME);
}

HomeDlg::~HomeDlg()
{
}

void HomeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List_Friend);
	DDX_Control(pDX, IDC_LIST2, m_List_Room);
	DDX_Control(pDX, IDC_LIST3, m_List_SystemMsg);
	DDX_Control(pDX, IDC_ID, m_Static_Id);
	DDX_Control(pDX, IDC_NAME, m_Static_Name);
	DDX_Control(pDX, IDC_HOMEPIC, m_Picture_Pic);
	DDX_Control(pDX, IDC_RADIO1, m_Radio_FriendList);
}

//判断是否打开过私聊
BOOL IsOpenSingleWin(CString nId)
{
	for (auto &val : g_OpenSingleWin)
	{
		if (val == nId)return TRUE;
	}
	return false;
}
//判断是否打开过聊天室
BOOL IsOpenRoomWin(CString nRoomId)
{
	for (auto &val : g_OpenRoomWin)
	{
		if (val == nRoomId)return TRUE;
	}
	return false;
}
BEGIN_MESSAGE_MAP(HomeDlg, CDialogEx)
	ON_WM_TIMER()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &HomeDlg::OnDblclkList1)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, &HomeDlg::OnDblclkList2)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &HomeDlg::OnRclickList1)
	ON_COMMAND(ID_FINDFRIEND, &HomeDlg::OnFindfriend)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST3, &HomeDlg::OnDblclkList3)
	ON_COMMAND(ID_DELETEFRIEND, &HomeDlg::OnDeletefriend)
	ON_COMMAND(ID_OUTROOM, &HomeDlg::OnOutroom)
	ON_COMMAND(ID_FINDROOM, &HomeDlg::OnFindroom)
	ON_NOTIFY(NM_RCLICK, IDC_LIST2, &HomeDlg::OnRclickList2)
	ON_COMMAND(ID_CREATROOM, &HomeDlg::OnCreatroom)
	ON_BN_CLICKED(IDC_RADIO1, &HomeDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &HomeDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_BUTTON1, &HomeDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

//接受服务端消息，处理线程
DWORD WINAPI RefershThread(LPVOID lpThreadParameter)
{
	HomeDlg *nDlg = (HomeDlg*)lpThreadParameter; 
	vector<CString>nRecvText;
	CString nRecvData;
	BOOL nRet = FALSE;
	CString nState;
	DWORD nHead = 0;

	while (true)
	{

		nRet = m_Client.RecvData(nRecvData);

		if (nRet && nRecvData!=TEXT(""))
		{
			SegmentText(nRecvText, nRecvData, SEG);
			nState = nRecvText[0];


			if (nState == TEXT("聊天室"))
			{
				//聊天室id，聊天室名字，对方名字，对方时间，对方信息
				g_RoomMsg.push_back(nRecvText[1] + SEG + nRecvText[2] + SEG + nRecvText[3] + SEG + nRecvText[4] + SEG + nRecvText[5]);

				BOOL nHaveMsg = FALSE;
				nHaveMsg = IsOpenRoomWin(nRecvText[1]);

				if (nHaveMsg == FALSE)
				{
					CString nId;
					for (int i = 0; i < nDlg->m_List_SystemMsg.GetItemCount(); i++)
					{
						nId = nDlg->m_List_SystemMsg.GetItemDataStr(i);
						if (nId == nRecvText[1])
						{
							nDlg->m_List_SystemMsg.SetItemText(i, 1, nRecvText[2]);
							nDlg->m_List_SystemMsg.SetItemText(i, 2, nRecvText[3] + nRecvText[5]);
							nHaveMsg = TRUE;
							break;
						}
					}


					if (nHaveMsg == FALSE)
					{
						nDlg->m_List_SystemMsg.InsertItem(0, TEXT("收到聊天室信息"),0);
						nDlg->m_List_SystemMsg.SetItemData(0, nRecvText[1]);
						nDlg->m_List_SystemMsg.SetItemText(0, 1, nRecvText[2]);
						nDlg->m_List_SystemMsg.SetItemText(0, 2, nRecvText[3] + nRecvText[5]);
					}
				}


			}
			else if (nState == TEXT("私聊"))
			{
				//我的id，对方id，对方名字，对方时间，对方信息
				g_SingleMsg.push_back(nRecvText[1] + SEG + nRecvText[2] + SEG + nRecvText[3] + SEG + nRecvText[4] + SEG + nRecvText[5]);

				BOOL nHaveMsg = FALSE;

				nHaveMsg = IsOpenSingleWin(nRecvText[2]);

				if (nHaveMsg == FALSE)
				{
					CString nId;
					for (int i = 0; i < nDlg->m_List_SystemMsg.GetItemCount(); i++)
					{
						nId = nDlg->m_List_SystemMsg.GetItemDataStr(i);
						if (nId == nRecvText[2])
						{
							nDlg->m_List_SystemMsg.SetItemText(i, 1, nRecvText[3] );
							nDlg->m_List_SystemMsg.SetItemText(i, 2, nRecvText[5]);
							nHaveMsg = TRUE;
							break;
						}
					}

					if (nHaveMsg == FALSE)
					{
						nDlg->m_List_SystemMsg.InsertItem(0, TEXT("收到私聊信息"), 0);
						nDlg->m_List_SystemMsg.SetItemData(0, nRecvText[2]);
						nDlg->m_List_SystemMsg.SetItemText(0, 1, nRecvText[3] );
						nDlg->m_List_SystemMsg.SetItemText(0, 2, nRecvText[5]);
					}
				}
			}
			else if (nState == TEXT("发送文件"))
			{
				//我的id，对方id，对方名字，文件名
				g_RecvFileMsg.push_back(nRecvText[0] + SEG + nRecvText[1] + SEG + nRecvText[2] + SEG + nRecvText[3] + SEG + nRecvText[4]);


				BOOL nHaveMsg = FALSE;

				nHaveMsg = IsOpenSingleWin(nRecvText[2]);

				if (nHaveMsg == FALSE)
				{
					CString nId;
					for (int i = 0; i < nDlg->m_List_SystemMsg.GetItemCount(); i++)
					{
						nId = nDlg->m_List_SystemMsg.GetItemDataStr(i);
						if (nId == nRecvText[2])
						{
							nDlg->m_List_SystemMsg.SetItemText(i, 1, nRecvText[3]);
							nDlg->m_List_SystemMsg.SetItemText(i, 2, TEXT("向你发送文件:")+nRecvText[4]);
							nHaveMsg = TRUE;
							break;
						}
					}

					if (nHaveMsg == FALSE)
					{
						nDlg->m_List_SystemMsg.InsertItem(0, TEXT("收到私聊信息"), 0);
						nDlg->m_List_SystemMsg.SetItemData(0, nRecvText[2]);
						nDlg->m_List_SystemMsg.SetItemText(0, 1, nRecvText[3]);
						nDlg->m_List_SystemMsg.SetItemText(0, 2, TEXT("向你发送文件:") + nRecvText[4]);
					}
				}

			}
			else if (nState == TEXT("拒绝文件"))
			{
				//我的id，对方id
				g_RecvFileMsg.push_back(nRecvText[0] + SEG + nRecvText[1] + SEG + nRecvText[2]);
			}
			else if (nState == TEXT("接受文件"))
			{
				//我的id，对方id
				g_RecvFileMsg.push_back(nRecvText[0] + SEG + nRecvText[1] + SEG + nRecvText[2]);
			}
			else if (nState == TEXT("准备发送"))
			{
				//我的id，对方id，IP，端口
				g_RecvFileMsg.push_back(nRecvText[0] + SEG + nRecvText[1] + SEG + nRecvText[2] + SEG + nRecvText[3] + SEG + nRecvText[4] + SEG + nRecvText[5]);
			}
			else if (nState == TEXT("视频聊天"))
			{
				//我的id，对方id，对方名字，端口，IP
				g_VideoMsg.push_back(nRecvText[0] + SEG + nRecvText[1] + SEG + nRecvText[2] + SEG + nRecvText[3] + SEG + nRecvText[4] + SEG + nRecvText[5]);
			

				BOOL nHaveMsg = FALSE;

				nHaveMsg = IsOpenSingleWin(nRecvText[2]);

				if (nHaveMsg == FALSE)
				{
					CString nId;
					for (int i = 0; i < nDlg->m_List_SystemMsg.GetItemCount(); i++)
					{
						nId = nDlg->m_List_SystemMsg.GetItemDataStr(i);
						if (nId == nRecvText[2])
						{
							nDlg->m_List_SystemMsg.SetItemText(i, 1, nRecvText[3]);
							nDlg->m_List_SystemMsg.SetItemText(i, 2, TEXT("邀请你视频聊天"));
							nHaveMsg = TRUE;
							break;
						}
					}

					if (nHaveMsg == FALSE)
					{
						nDlg->m_List_SystemMsg.InsertItem(0, TEXT("收到私聊信息"), 0);
						nDlg->m_List_SystemMsg.SetItemData(0, nRecvText[2]);
						nDlg->m_List_SystemMsg.SetItemText(0, 1, nRecvText[3]);
						nDlg->m_List_SystemMsg.SetItemText(0, 2, TEXT("邀请你视频聊天"));
					}
				}

			}
			else if (nState == TEXT("接受视频"))
			{
				//我的id，对方id，对方名字，端口，IP
				g_VideoMsg.push_back(nRecvText[0] + SEG + nRecvText[1] + SEG + nRecvText[2] + SEG + nRecvText[3] + SEG + nRecvText[4] + SEG + nRecvText[5]);
			}
			else if (nState == TEXT("拒绝视频"))
			{
				//我的id，对方id，对方IP
				g_VideoMsg.push_back(nRecvText[0] + SEG + nRecvText[1] + SEG + nRecvText[2]);
			}
			else if (nState == TEXT("关闭视频"))
			{
				//我的id，对方id，对方IP
				g_VideoMsg.push_back(nRecvText[0] + SEG + nRecvText[1] + SEG + nRecvText[2]);
			}
			else if (nState == TEXT("列表信息"))
			{
				nDlg->PrintFriend(nRecvText[1]);
				nDlg->PrintRoom(nRecvText[2]);
			}
			else if (nState == TEXT("聊天室成员"))
			{
				g_RoomMember.push_back(nRecvText[1] + SEG + nRecvText[2]);
			}
			else if (nState == TEXT("查找好友"))
			{
				g_FindFriendRecv = nRecvText[1];
			}
			else if (nState == TEXT("请求添加好友"))
			{
				BOOL nHaveMsg = FALSE;

				CString nId;
				for (int i = 0; i < nDlg->m_List_SystemMsg.GetItemCount(); i++)
				{
					nId = nDlg->m_List_SystemMsg.GetItemDataStr(i);
					if (nId == nRecvText[1])
					{
						nHaveMsg = TRUE;
						break;
					}
				}

				if (nHaveMsg == FALSE)
				{
					nDlg->m_List_SystemMsg.InsertItem(0, TEXT("请求添加好友"), 0);
					nDlg->m_List_SystemMsg.SetItemData(0, nRecvText[1]);
					nDlg->m_List_SystemMsg.SetItemText(0, 1, TEXT("帐号：") + nRecvText[1]);
					nDlg->m_List_SystemMsg.SetItemText(0, 2, TEXT("昵称：") + nRecvText[2]);
				}

			}
			else if (nState == TEXT("同意添加好友"))
			{
				nDlg->m_List_SystemMsg.InsertItem(0, TEXT("同意添加好友"), 0);
				nDlg->m_List_SystemMsg.SetItemText(0, 1, TEXT("帐号：") + nRecvText[1]);
				nDlg->m_List_SystemMsg.SetItemText(0, 2, TEXT("昵称：") + nRecvText[2]);
			}
			else if (nState == TEXT("拒绝添加好友"))
			{
				nDlg->m_List_SystemMsg.InsertItem(0, TEXT("拒绝添加好友"), 0);
				nDlg->m_List_SystemMsg.SetItemText(0, 1, TEXT("帐号：") + nRecvText[1]);
				nDlg->m_List_SystemMsg.SetItemText(0, 2, TEXT("昵称：") + nRecvText[2]);
			}
			else if (nState == TEXT("查看私聊记录"))
			{
				g_SingleRecord.push_back(nRecvText[1] + SEG + nRecvText[2]);
			}
			else if (nState == TEXT("查看聊天室记录"))
			{
				g_RoomRecord.push_back(nRecvText[1] + SEG + nRecvText[2]);
			}
			else if (nState == TEXT("查找聊天室"))
			{
				g_FindRoomRecv = nRecvText[1];
			}
			else if (nState == TEXT("创建聊天室"))
			{
				g_CreatRoomRecv = nRecvText[1];
			}
		}

		nHead++;
		if (nHead>=2000)
		{
			nHead = 0;
			m_Client.SendData(TEXT("心跳包") + SEG + g_Id);
		}
		Sleep(1);
	}
}
//刷新好友列表
void HomeDlg::PrintFriend(CString &nData)
{
	if (nData.IsEmpty())
	{
		m_List_Friend.DeleteAllItems();
		return;
	}

	vector<CString>nNameText;
	vector<CString>nTemp;
	DWORD nIndex = 0;
	DWORD nSub = 0;

	nIndex = m_List_Friend.GetItemCount();

	SegmentText(nNameText, nData, TEXT("|||"));

	if (nIndex > nNameText.size())
	{
		nSub = nIndex - nNameText.size();
		for (DWORD i = 0;i<nSub;i++)
		{
			m_List_Friend.DeleteItem(m_List_Friend.GetItemCount()-1);
		}
	}
	else if (nIndex < nNameText.size())
	{
		nSub =  nNameText.size()- nIndex ;
		for (DWORD i = 0; i < nSub; i++)
		{
			m_List_Friend.InsertItem(m_List_Friend.GetItemCount(),TEXT(""));
		}
	}


	for (DWORD i = 0; i < nNameText.size(); i++)
	{
		SegmentText(nTemp, nNameText[i], TEXT("#"));

		INT nHead = 0;
		CString szHead;
		CString bkHead;
		bkHead = nTemp[3];
		_stscanf_s(nTemp[3], TEXT("%d"), &nHead);
		nHead += 6;
		szHead.Format(TEXT("%d"), nHead);

		if (nTemp[2] == TEXT("离线"))nTemp[3] = szHead;

		m_List_Friend.SetItemData(i, nTemp[0]);
		m_List_Friend.SetItemImageEx(i, 0, nTemp[0], nTemp[3]);
		m_List_Friend.SetItemText(i, 0, nTemp[0]);
		m_List_Friend.SetItemText(i, 1, nTemp[1]);
		m_List_Friend.SetItemText(i, 2, nTemp[2]);
		m_List_Friend.SetItemText(i, 3, bkHead);

	}
}
//刷新聊天室列表
void HomeDlg::PrintRoom(CString &nData)
{
	if (nData.IsEmpty())
	{
		m_List_Room.DeleteAllItems();
		return;
	}

	vector<CString>nNameText;
	vector<CString>nTemp;
	DWORD nIndex = 0;
	DWORD nSub = 0;

	nIndex = m_List_Room.GetItemCount();

	SegmentText(nNameText, nData, TEXT("|||"));

	if (nIndex > nNameText.size())
	{
		nSub = nIndex - nNameText.size();
		for (DWORD i = 0; i < nSub; i++)
		{
			m_List_Room.DeleteItem(m_List_Room.GetItemCount() - 1);
		}
	}
	else if (nIndex < nNameText.size())
	{
		nSub = nNameText.size() - nIndex;
		for (DWORD i = 0; i < nSub; i++)
		{
			m_List_Room.InsertItem(m_List_Room.GetItemCount(), TEXT(""));
		}
	}


	for (DWORD i = 0; i < nNameText.size(); i++)
	{
		SegmentText(nTemp, nNameText[i], TEXT("#"));

		m_List_Room.SetItemData(i, nTemp[0]);
		m_List_Room.SetItemImageEx(i, 0, nTemp[0],0);
		m_List_Room.SetItemText(i, 0, nTemp[0]);
		m_List_Room.SetItemText(i, 1, nTemp[1]);
	}
}
//主界面初始化
BOOL HomeDlg::OnInitDialog()
{
	CString nDir = GetProgramDir();
	m_hIcon = ExtractIcon(AfxGetInstanceHandle(), nDir + TEXT("res\\") + g_Pic + TEXT(".ico"), 0);
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	CDialogEx::OnInitDialog();

	m_Menu.LoadMenu(IDR_MENU1);
	SetMenu(&m_Menu);
	m_Menu.GetSubMenu(0)->EnableMenuItem(ID_LOOK, TRUE);
	m_Menu.GetSubMenu(0)->EnableMenuItem(ID_DELETEFRIEND, TRUE);
	m_Menu.GetSubMenu(1)->EnableMenuItem(ID_OUTROOM, TRUE);

	m_List_Friend.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_List_Room.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_List_SystemMsg.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_List_Friend.InsertColumn(0, TEXT("ID"), 0, 120);
	m_List_Friend.InsertColumn(1, TEXT("名字"), 0, 150);
	m_List_Friend.InsertColumn(2, TEXT("状态"), 0, 80);
	m_List_Friend.InsertColumn(3, TEXT("图片"), 0, 0);

	m_List_Room.InsertColumn(0, TEXT("聊天室ID"), 0, 150);
	m_List_Room.InsertColumn(1, TEXT("聊天室名字"), 0, 200);

	m_List_SystemMsg.InsertColumn(0, TEXT("系统消息"), 0, 130);
	m_List_SystemMsg.InsertColumn(1, TEXT(""), 0, 120);
	m_List_SystemMsg.InsertColumn(2, TEXT(""), 0, 100);

	m_List_Friend.ShowWindow(TRUE);
	m_List_Room.ShowWindow(FALSE);

	//SetTimer(0x10000, 1000, NULL);//设置定时刷新
	CreateThread(NULL, NULL, RefershThread, (LPVOID)this, NULL, NULL);

	SetWindowText(g_Name);

	m_Client.SendData(TEXT("心跳包") + SEG + g_Id);

	m_Static_Id.SetWindowText(TEXT("帐号：") + g_Id);
	m_Static_Name.SetWindowText(TEXT("昵称：") + g_Name);

	LoadFriendImage();
	LoadRoomImage();
	LoadMsgImage();
	LoadMyHeadImage(g_Pic);

	m_Radio_FriendList.SetCheck(TRUE);
	return TRUE; 
}
//载入好友头像
void HomeDlg::LoadFriendImage()
{
	m_FriendImageList.Create(24, 24, ILC_COLOR32 | ILC_MASK, 2, 2);
	//创建一个CImageList
	HICON hSmallIco;
	CString nStr;
	CString nDir = GetProgramDir();
	for (int i=0;i<6;i++)
	{
		nStr.Format(TEXT("%d"), i);
		//取出资源目录的ico文件，获取一个ico句柄
		hSmallIco = ExtractIcon(AfxGetInstanceHandle(), nDir + TEXT("res\\") + nStr + TEXT(".ico"), 0);
		//加入到ImageList里面
		m_FriendImageList.Add(hSmallIco);
		//释放资源
		DestroyIcon(hSmallIco);
	}

	for (int i = 0; i < 6; i++)
	{
		nStr.Format(TEXT("%d"), i);
		hSmallIco = ExtractIcon(AfxGetInstanceHandle(), nDir + TEXT("res\\") + nStr + nStr+ TEXT(".ico"), 0);
		m_FriendImageList.Add(hSmallIco);
		DestroyIcon(hSmallIco);
	}

	//将ImageList设置到List控件内即可
	m_List_Friend.SetImageList(&m_FriendImageList, LVSIL_SMALL);

}
//载入聊天室头像
void HomeDlg::LoadRoomImage()
{
	m_RoomImageList.Create(24, 24, ILC_COLOR32 | ILC_MASK, 2, 2);

	HICON hSmallIco;
	CString nDir = GetProgramDir();

	hSmallIco = ExtractIcon(AfxGetInstanceHandle(), nDir + TEXT("res\\r.ico"), 0);
	m_RoomImageList.Add(hSmallIco);
	DestroyIcon(hSmallIco);

	m_List_Room.SetImageList(&m_RoomImageList, LVSIL_SMALL);
}
//载入系统消息图标
void HomeDlg::LoadMsgImage()
{
	m_MsgImageList.Create(24, 24, ILC_COLOR32 | ILC_MASK, 2, 2);

	HICON hSmallIco;
	CString nDir = GetProgramDir();

	hSmallIco = ExtractIcon(AfxGetInstanceHandle(), nDir + TEXT("res\\m.ico"), 0);
	m_MsgImageList.Add(hSmallIco);
	DestroyIcon(hSmallIco);

	m_List_SystemMsg.SetImageList(&m_MsgImageList, LVSIL_SMALL);

}
//载入个人头像
void HomeDlg::LoadMyHeadImage(CString nNum)
{
	CString nDir = GetProgramDir();
	HICON nHicon;
	nHicon = ExtractIcon(AfxGetInstanceHandle(), nDir + TEXT("res\\") + nNum + TEXT(".ico"), 0);
	m_Picture_Pic.SetIcon(nHicon);
	//DestroyIcon(nHicon);
}
//time事件循环
void HomeDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent==0x10000)
	{
		//m_Client.SendData(TEXT("心跳包") + SEG + g_Name + SEG );

	}

	CDialogEx::OnTimer(nIDEvent);
}

BOOL HomeDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialogEx::PreTranslateMessage(pMsg);
}


//双击好友弹出私聊窗口
void HomeDlg::OnDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	INT nIndex = m_List_Friend.GetSelectEx();
	if (nIndex == -1)return;

	CString nId = m_List_Friend.GetItemDataStr(nIndex);
	CString nName = m_List_Friend.GetItemText(nIndex, 1);
	CString nPic = m_List_Friend.GetItemText(nIndex, 3);

	if (IsOpenSingleWin(nId))return;

	g_OpenSingleWin.push_back(nId);

	SingleDlg *pSingleDlg;
	pSingleDlg = new SingleDlg(NULL, nId, nName, nPic);
	pSingleDlg->ShowWindow(SW_SHOW);

}
//双击聊天室弹出窗口
void HomeDlg::OnDblclkList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	INT nIndex = m_List_Room.GetSelectEx();
	if (nIndex == -1)return;


	g_FriendList.swap(vector<CString>());
	for (int i = 0; i < m_List_Friend.GetItemCount(); i++)
	{
		g_FriendList.push_back(m_List_Friend.GetItemDataStr(i));
	}

	CString nId;
	nId = m_List_Room.GetItemDataStr(nIndex);
	CString nName = m_List_Room.GetItemText(nIndex, 1);

	if (IsOpenRoomWin(nId))return;

	g_OpenRoomWin.push_back(nId);


	RoomDlg *pRoomDlg;
	pRoomDlg = new RoomDlg(NULL, nId, nName);
	pRoomDlg->ShowWindow(SW_SHOW);

}
//关闭主界面结束进程
void HomeDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	PostQuitMessage(0);

	CDialogEx::OnClose();
}




//右键弹出好友列表菜单
void HomeDlg::OnRclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	CMenu *nMenu = m_Menu.GetSubMenu(0);

	INT nIndex = m_List_Friend.GetSelectEx();

	if (nIndex == -1)
	{
		nMenu->EnableMenuItem(ID_LOOK, TRUE);
		nMenu->EnableMenuItem(ID_DELETEFRIEND, TRUE);
	}
	else
	{
		nMenu->EnableMenuItem(ID_LOOK, FALSE);
		nMenu->EnableMenuItem(ID_DELETEFRIEND, FALSE);

	}
	POINT nMenuPos;
	GetCursorPos(&nMenuPos);
	nMenu->TrackPopupMenu(TPM_LEFTALIGN, nMenuPos.x, nMenuPos.y, this);

}
//右键弹出聊天室列表菜单
void HomeDlg::OnRclickList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	CMenu *nMenu = m_Menu.GetSubMenu(1);

	INT nIndex = m_List_Room.GetSelectEx();

	if (nIndex == -1)
	{
		nMenu->EnableMenuItem(ID_OUTROOM, TRUE);
	}
	else
	{
		nMenu->EnableMenuItem(ID_OUTROOM, FALSE);
	}
	POINT pos;
	GetCursorPos(&pos);
	nMenu->TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, this);
}
//弹出查找好友窗口
void HomeDlg::OnFindfriend()
{
	g_FriendList.swap(vector<CString>());
	for (int i=0;i<m_List_Friend.GetItemCount();i++)
	{
		g_FriendList.push_back(m_List_Friend.GetItemDataStr(i));
	}
	
	FindFriendDlg nFindFriendDlg;
	nFindFriendDlg.DoModal();

}
//取出好友头像
CString HomeDlg::GetFriendHeadImage(CString nId)
{
	INT nNum= m_List_Friend.GetItemCount();
	for (INT i = 0;i<nNum;i++)
	{
		if (m_List_Friend.GetItemDataStr(i)== nId)
		{
			return m_List_Friend.GetItemText(i, 3);
		}
	}
	return TEXT("0");
}
//双击弹出系统消息处理
void HomeDlg::OnDblclkList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;


	INT nIndex = m_List_SystemMsg.GetSelectEx();
	if (nIndex == -1)return;

	CString nMsg;
	CString nId;
	CString nName;
	nMsg = m_List_SystemMsg.GetItemText(nIndex, 0);
	nId = m_List_SystemMsg.GetItemText(nIndex, 1);
	nName = m_List_SystemMsg.GetItemText(nIndex, 2);

	if (nMsg == TEXT("请求添加好友"))
	{
		nId = m_List_SystemMsg.GetItemDataStr(nIndex);
		nName = m_List_SystemMsg.GetItemText(nIndex, 2);

		int nRet = MessageBox(nId + TEXT("\n") + nName + TEXT("\n请求添加您为好友。\n是否同意？"), 
			TEXT("请求添加好友"), MB_YESNO + MB_ICONASTERISK);

		if (nRet == 6)
		{
			m_Client.SendData(TEXT("同意添加好友") + SEG + nId + SEG + g_Id + SEG + g_Name);
		}
		else
		{
			m_Client.SendData(TEXT("拒绝添加好友") + SEG + nId + SEG + g_Id + SEG + g_Name);
		}
	}
	else if (nMsg == TEXT("同意添加好友"))
	{
		MessageBox(TEXT("帐号：") + nId + TEXT("\n") + TEXT("昵称：") + nName +
			TEXT("\n已同意您添加为好友。"), TEXT("成功"), MB_ICONASTERISK);
	}
	else if (nMsg == TEXT("拒绝添加好友"))
	{
		MessageBox(TEXT("帐号：") + nId + TEXT("\n") + TEXT("昵称：") + nName +
			TEXT("\n已拒绝您添加为好友。"), TEXT("失败"), MB_ICONWARNING);
	}
	else if (nMsg == TEXT("收到私聊信息"))
	{
		CString nId = m_List_SystemMsg.GetItemDataStr(nIndex);
		CString nName = m_List_SystemMsg.GetItemText(nIndex, 1);
		CString nPic = GetFriendHeadImage(nId);
		if (IsOpenSingleWin(nId))return;

		g_OpenSingleWin.push_back(nId);



		SingleDlg *pSingleDlg;
		pSingleDlg = new SingleDlg(NULL, nId, nName, nPic);
		pSingleDlg->ShowWindow(SW_SHOW);

	}
	else if (nMsg == TEXT("收到聊天室信息"))
	{
		CString nId;
		nId = m_List_SystemMsg.GetItemDataStr(nIndex);
		CString nName = m_List_SystemMsg.GetItemText(nIndex, 1);

		if (IsOpenRoomWin(nId))return;

		g_FriendList.swap(vector<CString>());
		for (int i = 0; i < m_List_Friend.GetItemCount(); i++)
		{
			g_FriendList.push_back(m_List_Friend.GetItemDataStr(i));
		}

		g_OpenRoomWin.push_back(nId);

		RoomDlg *pRoomDlg;
		pRoomDlg = new RoomDlg(NULL, nId, nName);
		pRoomDlg->ShowWindow(SW_SHOW);

	}


	
	m_List_SystemMsg.DeleteItem(nIndex);
}
//删除好友
void HomeDlg::OnDeletefriend()
{
	INT nIndex = m_List_Friend.GetSelectEx();

	CString nId;
	CString nName;

	nId = m_List_Friend.GetItemDataStr(nIndex);
	nName = m_List_Friend.GetItemText(nIndex, 0);

	int nRet = MessageBox(TEXT("帐号：") + nId + TEXT("\n") + TEXT("昵称：") + nName +
		TEXT("\n是否要删除此好友？"), TEXT("删除好友"), MB_YESNO + MB_ICONASTERISK);
	if (nRet == 6)
	{
		m_Client.SendData(TEXT("删除好友") + SEG + g_Id + SEG + nId);
	}
}
//退出聊天室
void HomeDlg::OnOutroom()
{
	INT nIndex = m_List_Room.GetSelectEx();

	CString nId;
	CString nName;

	nId = m_List_Room.GetItemDataStr(nIndex);
	nName = m_List_Room.GetItemText(nIndex, 0);

	int nRet = MessageBox(TEXT("聊天室ID：") + nId + TEXT("\n") + TEXT("聊天室名称：") + nName +
		TEXT("\n是否要退出此聊天室？"), TEXT("退出聊天室"), MB_YESNO + MB_ICONASTERISK);
	if (nRet == 6)
	{
		m_Client.SendData(TEXT("退出聊天室") + SEG + nId + SEG + g_Id);
	}
}
//查找聊天室
void HomeDlg::OnFindroom()
{
	g_RoomList.swap(vector<CString>());
	for (int i = 0; i < m_List_Room.GetItemCount(); i++)
	{
		g_RoomList.push_back(m_List_Room.GetItemDataStr(i));
	}

	FindRoomDlg nFindRoomDlg;
	nFindRoomDlg.DoModal();
}
//创建聊天室
void HomeDlg::OnCreatroom()
{

	CreatRoomDlg nCreatRoomDlg;
	nCreatRoomDlg.DoModal();
}

//切换好友列表
void HomeDlg::OnBnClickedRadio1()
{

	m_List_Friend.ShowWindow(TRUE);
	m_List_Room.ShowWindow(FALSE);
}

//切换聊天室列表
void HomeDlg::OnBnClickedRadio2()
{
	m_List_Friend.ShowWindow(FALSE);
	m_List_Room.ShowWindow(TRUE);
}


//修改资料
void HomeDlg::OnBnClickedButton1()
{
	ChangeInfoDlg nDlg(NULL,g_Pic,g_Name, g_Sex);
	nDlg.DoModal();

	m_Static_Id.SetWindowText(TEXT("帐号：") + g_Id);
	m_Static_Name.SetWindowText(TEXT("昵称：") + g_Name);
	LoadMyHeadImage(g_Pic);
	SetWindowText(g_Name);

}
