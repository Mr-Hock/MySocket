// HomeDlg.cpp : ʵ���ļ�
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

extern TCP_CLIENT m_Client;//�ͻ���
extern CString SEG ;//�ָ���
extern CString g_Name;//����
extern CString g_Id;//�ʺ�
extern CString g_Pic;//ͷ��
extern CString g_Sex;//�Ա�
vector<CString>g_SingleMsg;//˽����Ϣ����
vector<CString>g_RoomMsg;//��������Ϣ����
vector<CString> g_RoomMember;//�����ҳ�Ա����
vector<CString>g_OpenSingleWin;//˽�Ĵ���
vector<CString>g_OpenRoomWin;//�����Ҵ���
CString g_FindFriendRecv;//���Һ���
CString g_FindRoomRecv;//����������
CString g_CreatRoomRecv;//����������
vector<CString>g_FriendList;//�����б�����
vector<CString>g_RoomList;//����������
vector<CString> g_SingleRecord;//˽�ļ�¼����
vector<CString> g_RoomRecord;//�����Ҽ�¼����
vector<CString>g_RecvFileMsg;//�շ��ļ���Ϣ����
vector<CString>g_VideoMsg;//��Ƶ������Ϣ����

// HomeDlg �Ի���

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

//�ж��Ƿ�򿪹�˽��
BOOL IsOpenSingleWin(CString nId)
{
	for (auto &val : g_OpenSingleWin)
	{
		if (val == nId)return TRUE;
	}
	return false;
}
//�ж��Ƿ�򿪹�������
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

//���ܷ������Ϣ�������߳�
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


			if (nState == TEXT("������"))
			{
				//������id�����������֣��Է����֣��Է�ʱ�䣬�Է���Ϣ
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
						nDlg->m_List_SystemMsg.InsertItem(0, TEXT("�յ���������Ϣ"),0);
						nDlg->m_List_SystemMsg.SetItemData(0, nRecvText[1]);
						nDlg->m_List_SystemMsg.SetItemText(0, 1, nRecvText[2]);
						nDlg->m_List_SystemMsg.SetItemText(0, 2, nRecvText[3] + nRecvText[5]);
					}
				}


			}
			else if (nState == TEXT("˽��"))
			{
				//�ҵ�id���Է�id���Է����֣��Է�ʱ�䣬�Է���Ϣ
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
						nDlg->m_List_SystemMsg.InsertItem(0, TEXT("�յ�˽����Ϣ"), 0);
						nDlg->m_List_SystemMsg.SetItemData(0, nRecvText[2]);
						nDlg->m_List_SystemMsg.SetItemText(0, 1, nRecvText[3] );
						nDlg->m_List_SystemMsg.SetItemText(0, 2, nRecvText[5]);
					}
				}
			}
			else if (nState == TEXT("�����ļ�"))
			{
				//�ҵ�id���Է�id���Է����֣��ļ���
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
							nDlg->m_List_SystemMsg.SetItemText(i, 2, TEXT("���㷢���ļ�:")+nRecvText[4]);
							nHaveMsg = TRUE;
							break;
						}
					}

					if (nHaveMsg == FALSE)
					{
						nDlg->m_List_SystemMsg.InsertItem(0, TEXT("�յ�˽����Ϣ"), 0);
						nDlg->m_List_SystemMsg.SetItemData(0, nRecvText[2]);
						nDlg->m_List_SystemMsg.SetItemText(0, 1, nRecvText[3]);
						nDlg->m_List_SystemMsg.SetItemText(0, 2, TEXT("���㷢���ļ�:") + nRecvText[4]);
					}
				}

			}
			else if (nState == TEXT("�ܾ��ļ�"))
			{
				//�ҵ�id���Է�id
				g_RecvFileMsg.push_back(nRecvText[0] + SEG + nRecvText[1] + SEG + nRecvText[2]);
			}
			else if (nState == TEXT("�����ļ�"))
			{
				//�ҵ�id���Է�id
				g_RecvFileMsg.push_back(nRecvText[0] + SEG + nRecvText[1] + SEG + nRecvText[2]);
			}
			else if (nState == TEXT("׼������"))
			{
				//�ҵ�id���Է�id��IP���˿�
				g_RecvFileMsg.push_back(nRecvText[0] + SEG + nRecvText[1] + SEG + nRecvText[2] + SEG + nRecvText[3] + SEG + nRecvText[4] + SEG + nRecvText[5]);
			}
			else if (nState == TEXT("��Ƶ����"))
			{
				//�ҵ�id���Է�id���Է����֣��˿ڣ�IP
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
							nDlg->m_List_SystemMsg.SetItemText(i, 2, TEXT("��������Ƶ����"));
							nHaveMsg = TRUE;
							break;
						}
					}

					if (nHaveMsg == FALSE)
					{
						nDlg->m_List_SystemMsg.InsertItem(0, TEXT("�յ�˽����Ϣ"), 0);
						nDlg->m_List_SystemMsg.SetItemData(0, nRecvText[2]);
						nDlg->m_List_SystemMsg.SetItemText(0, 1, nRecvText[3]);
						nDlg->m_List_SystemMsg.SetItemText(0, 2, TEXT("��������Ƶ����"));
					}
				}

			}
			else if (nState == TEXT("������Ƶ"))
			{
				//�ҵ�id���Է�id���Է����֣��˿ڣ�IP
				g_VideoMsg.push_back(nRecvText[0] + SEG + nRecvText[1] + SEG + nRecvText[2] + SEG + nRecvText[3] + SEG + nRecvText[4] + SEG + nRecvText[5]);
			}
			else if (nState == TEXT("�ܾ���Ƶ"))
			{
				//�ҵ�id���Է�id���Է�IP
				g_VideoMsg.push_back(nRecvText[0] + SEG + nRecvText[1] + SEG + nRecvText[2]);
			}
			else if (nState == TEXT("�ر���Ƶ"))
			{
				//�ҵ�id���Է�id���Է�IP
				g_VideoMsg.push_back(nRecvText[0] + SEG + nRecvText[1] + SEG + nRecvText[2]);
			}
			else if (nState == TEXT("�б���Ϣ"))
			{
				nDlg->PrintFriend(nRecvText[1]);
				nDlg->PrintRoom(nRecvText[2]);
			}
			else if (nState == TEXT("�����ҳ�Ա"))
			{
				g_RoomMember.push_back(nRecvText[1] + SEG + nRecvText[2]);
			}
			else if (nState == TEXT("���Һ���"))
			{
				g_FindFriendRecv = nRecvText[1];
			}
			else if (nState == TEXT("������Ӻ���"))
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
					nDlg->m_List_SystemMsg.InsertItem(0, TEXT("������Ӻ���"), 0);
					nDlg->m_List_SystemMsg.SetItemData(0, nRecvText[1]);
					nDlg->m_List_SystemMsg.SetItemText(0, 1, TEXT("�ʺţ�") + nRecvText[1]);
					nDlg->m_List_SystemMsg.SetItemText(0, 2, TEXT("�ǳƣ�") + nRecvText[2]);
				}

			}
			else if (nState == TEXT("ͬ����Ӻ���"))
			{
				nDlg->m_List_SystemMsg.InsertItem(0, TEXT("ͬ����Ӻ���"), 0);
				nDlg->m_List_SystemMsg.SetItemText(0, 1, TEXT("�ʺţ�") + nRecvText[1]);
				nDlg->m_List_SystemMsg.SetItemText(0, 2, TEXT("�ǳƣ�") + nRecvText[2]);
			}
			else if (nState == TEXT("�ܾ���Ӻ���"))
			{
				nDlg->m_List_SystemMsg.InsertItem(0, TEXT("�ܾ���Ӻ���"), 0);
				nDlg->m_List_SystemMsg.SetItemText(0, 1, TEXT("�ʺţ�") + nRecvText[1]);
				nDlg->m_List_SystemMsg.SetItemText(0, 2, TEXT("�ǳƣ�") + nRecvText[2]);
			}
			else if (nState == TEXT("�鿴˽�ļ�¼"))
			{
				g_SingleRecord.push_back(nRecvText[1] + SEG + nRecvText[2]);
			}
			else if (nState == TEXT("�鿴�����Ҽ�¼"))
			{
				g_RoomRecord.push_back(nRecvText[1] + SEG + nRecvText[2]);
			}
			else if (nState == TEXT("����������"))
			{
				g_FindRoomRecv = nRecvText[1];
			}
			else if (nState == TEXT("����������"))
			{
				g_CreatRoomRecv = nRecvText[1];
			}
		}

		nHead++;
		if (nHead>=2000)
		{
			nHead = 0;
			m_Client.SendData(TEXT("������") + SEG + g_Id);
		}
		Sleep(1);
	}
}
//ˢ�º����б�
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

		if (nTemp[2] == TEXT("����"))nTemp[3] = szHead;

		m_List_Friend.SetItemData(i, nTemp[0]);
		m_List_Friend.SetItemImageEx(i, 0, nTemp[0], nTemp[3]);
		m_List_Friend.SetItemText(i, 0, nTemp[0]);
		m_List_Friend.SetItemText(i, 1, nTemp[1]);
		m_List_Friend.SetItemText(i, 2, nTemp[2]);
		m_List_Friend.SetItemText(i, 3, bkHead);

	}
}
//ˢ���������б�
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
//�������ʼ��
BOOL HomeDlg::OnInitDialog()
{
	CString nDir = GetProgramDir();
	m_hIcon = ExtractIcon(AfxGetInstanceHandle(), nDir + TEXT("res\\") + g_Pic + TEXT(".ico"), 0);
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
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
	m_List_Friend.InsertColumn(1, TEXT("����"), 0, 150);
	m_List_Friend.InsertColumn(2, TEXT("״̬"), 0, 80);
	m_List_Friend.InsertColumn(3, TEXT("ͼƬ"), 0, 0);

	m_List_Room.InsertColumn(0, TEXT("������ID"), 0, 150);
	m_List_Room.InsertColumn(1, TEXT("����������"), 0, 200);

	m_List_SystemMsg.InsertColumn(0, TEXT("ϵͳ��Ϣ"), 0, 130);
	m_List_SystemMsg.InsertColumn(1, TEXT(""), 0, 120);
	m_List_SystemMsg.InsertColumn(2, TEXT(""), 0, 100);

	m_List_Friend.ShowWindow(TRUE);
	m_List_Room.ShowWindow(FALSE);

	//SetTimer(0x10000, 1000, NULL);//���ö�ʱˢ��
	CreateThread(NULL, NULL, RefershThread, (LPVOID)this, NULL, NULL);

	SetWindowText(g_Name);

	m_Client.SendData(TEXT("������") + SEG + g_Id);

	m_Static_Id.SetWindowText(TEXT("�ʺţ�") + g_Id);
	m_Static_Name.SetWindowText(TEXT("�ǳƣ�") + g_Name);

	LoadFriendImage();
	LoadRoomImage();
	LoadMsgImage();
	LoadMyHeadImage(g_Pic);

	m_Radio_FriendList.SetCheck(TRUE);
	return TRUE; 
}
//�������ͷ��
void HomeDlg::LoadFriendImage()
{
	m_FriendImageList.Create(24, 24, ILC_COLOR32 | ILC_MASK, 2, 2);
	//����һ��CImageList
	HICON hSmallIco;
	CString nStr;
	CString nDir = GetProgramDir();
	for (int i=0;i<6;i++)
	{
		nStr.Format(TEXT("%d"), i);
		//ȡ����ԴĿ¼��ico�ļ�����ȡһ��ico���
		hSmallIco = ExtractIcon(AfxGetInstanceHandle(), nDir + TEXT("res\\") + nStr + TEXT(".ico"), 0);
		//���뵽ImageList����
		m_FriendImageList.Add(hSmallIco);
		//�ͷ���Դ
		DestroyIcon(hSmallIco);
	}

	for (int i = 0; i < 6; i++)
	{
		nStr.Format(TEXT("%d"), i);
		hSmallIco = ExtractIcon(AfxGetInstanceHandle(), nDir + TEXT("res\\") + nStr + nStr+ TEXT(".ico"), 0);
		m_FriendImageList.Add(hSmallIco);
		DestroyIcon(hSmallIco);
	}

	//��ImageList���õ�List�ؼ��ڼ���
	m_List_Friend.SetImageList(&m_FriendImageList, LVSIL_SMALL);

}
//����������ͷ��
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
//����ϵͳ��Ϣͼ��
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
//�������ͷ��
void HomeDlg::LoadMyHeadImage(CString nNum)
{
	CString nDir = GetProgramDir();
	HICON nHicon;
	nHicon = ExtractIcon(AfxGetInstanceHandle(), nDir + TEXT("res\\") + nNum + TEXT(".ico"), 0);
	m_Picture_Pic.SetIcon(nHicon);
	//DestroyIcon(nHicon);
}
//time�¼�ѭ��
void HomeDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nIDEvent==0x10000)
	{
		//m_Client.SendData(TEXT("������") + SEG + g_Name + SEG );

	}

	CDialogEx::OnTimer(nIDEvent);
}

BOOL HomeDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialogEx::PreTranslateMessage(pMsg);
}


//˫�����ѵ���˽�Ĵ���
void HomeDlg::OnDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
//˫�������ҵ�������
void HomeDlg::OnDblclkList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
//�ر��������������
void HomeDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	PostQuitMessage(0);

	CDialogEx::OnClose();
}




//�Ҽ����������б�˵�
void HomeDlg::OnRclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
//�Ҽ������������б�˵�
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
//�������Һ��Ѵ���
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
//ȡ������ͷ��
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
//˫������ϵͳ��Ϣ����
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

	if (nMsg == TEXT("������Ӻ���"))
	{
		nId = m_List_SystemMsg.GetItemDataStr(nIndex);
		nName = m_List_SystemMsg.GetItemText(nIndex, 2);

		int nRet = MessageBox(nId + TEXT("\n") + nName + TEXT("\n���������Ϊ���ѡ�\n�Ƿ�ͬ�⣿"), 
			TEXT("������Ӻ���"), MB_YESNO + MB_ICONASTERISK);

		if (nRet == 6)
		{
			m_Client.SendData(TEXT("ͬ����Ӻ���") + SEG + nId + SEG + g_Id + SEG + g_Name);
		}
		else
		{
			m_Client.SendData(TEXT("�ܾ���Ӻ���") + SEG + nId + SEG + g_Id + SEG + g_Name);
		}
	}
	else if (nMsg == TEXT("ͬ����Ӻ���"))
	{
		MessageBox(TEXT("�ʺţ�") + nId + TEXT("\n") + TEXT("�ǳƣ�") + nName +
			TEXT("\n��ͬ�������Ϊ���ѡ�"), TEXT("�ɹ�"), MB_ICONASTERISK);
	}
	else if (nMsg == TEXT("�ܾ���Ӻ���"))
	{
		MessageBox(TEXT("�ʺţ�") + nId + TEXT("\n") + TEXT("�ǳƣ�") + nName +
			TEXT("\n�Ѿܾ������Ϊ���ѡ�"), TEXT("ʧ��"), MB_ICONWARNING);
	}
	else if (nMsg == TEXT("�յ�˽����Ϣ"))
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
	else if (nMsg == TEXT("�յ���������Ϣ"))
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
//ɾ������
void HomeDlg::OnDeletefriend()
{
	INT nIndex = m_List_Friend.GetSelectEx();

	CString nId;
	CString nName;

	nId = m_List_Friend.GetItemDataStr(nIndex);
	nName = m_List_Friend.GetItemText(nIndex, 0);

	int nRet = MessageBox(TEXT("�ʺţ�") + nId + TEXT("\n") + TEXT("�ǳƣ�") + nName +
		TEXT("\n�Ƿ�Ҫɾ���˺��ѣ�"), TEXT("ɾ������"), MB_YESNO + MB_ICONASTERISK);
	if (nRet == 6)
	{
		m_Client.SendData(TEXT("ɾ������") + SEG + g_Id + SEG + nId);
	}
}
//�˳�������
void HomeDlg::OnOutroom()
{
	INT nIndex = m_List_Room.GetSelectEx();

	CString nId;
	CString nName;

	nId = m_List_Room.GetItemDataStr(nIndex);
	nName = m_List_Room.GetItemText(nIndex, 0);

	int nRet = MessageBox(TEXT("������ID��") + nId + TEXT("\n") + TEXT("���������ƣ�") + nName +
		TEXT("\n�Ƿ�Ҫ�˳��������ң�"), TEXT("�˳�������"), MB_YESNO + MB_ICONASTERISK);
	if (nRet == 6)
	{
		m_Client.SendData(TEXT("�˳�������") + SEG + nId + SEG + g_Id);
	}
}
//����������
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
//����������
void HomeDlg::OnCreatroom()
{

	CreatRoomDlg nCreatRoomDlg;
	nCreatRoomDlg.DoModal();
}

//�л������б�
void HomeDlg::OnBnClickedRadio1()
{

	m_List_Friend.ShowWindow(TRUE);
	m_List_Room.ShowWindow(FALSE);
}

//�л��������б�
void HomeDlg::OnBnClickedRadio2()
{
	m_List_Friend.ShowWindow(FALSE);
	m_List_Room.ShowWindow(TRUE);
}


//�޸�����
void HomeDlg::OnBnClickedButton1()
{
	ChangeInfoDlg nDlg(NULL,g_Pic,g_Name, g_Sex);
	nDlg.DoModal();

	m_Static_Id.SetWindowText(TEXT("�ʺţ�") + g_Id);
	m_Static_Name.SetWindowText(TEXT("�ǳƣ�") + g_Name);
	LoadMyHeadImage(g_Pic);
	SetWindowText(g_Name);

}
