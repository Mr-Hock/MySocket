// FindFriendDlg.cpp : ʵ���ļ�
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

// FindFriendDlg �Ի���

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


// FindFriendDlg ��Ϣ�������


void FindFriendDlg::OnBnClickedButton1()
{
	UpdateData(TRUE);
	if (m_Edit_Find_Value == TEXT(""))return;

	if (m_Edit_Find_Value == g_Id)
	{
		m_Edit_RecvStatus_Value = TEXT("��ǰ����");
		m_Edit_RecvId_Value = g_Id;
		m_Edit_RecvName_Value = g_Name;
		UpdateData(FALSE);
		m_Button_Add.EnableWindow(FALSE);
		return;
	}

	m_Client.SendData(TEXT("���Һ���") + SEG + m_Edit_Find_Value);


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

			if (nRecvText[0] == TEXT("������"))
			{
				m_Edit_RecvStatus_Value = TEXT("�����ڴ��ʺ�");
				m_Edit_RecvId_Value = TEXT("���޴���");
				m_Edit_RecvName_Value = TEXT("���޴���");
				UpdateData(FALSE);
				m_Button_Add.EnableWindow(FALSE);
			}
			else if (nRecvText[0] == TEXT("������"))
			{
				m_Edit_RecvStatus_Value = TEXT("��ǰ������");
				m_Edit_RecvId_Value = nRecvText[1];
				m_Edit_RecvName_Value = nRecvText[2];
				UpdateData(FALSE);
				m_Button_Add.EnableWindow(FALSE);
			}
			else
			{
				m_Edit_RecvStatus_Value = TEXT("��ǰ����");
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

	SetTimer(0x10005, 50, NULL);//���ö�ʱˢ��

	return TRUE; 
}


void FindFriendDlg::OnBnClickedButton3()
{
	for (auto &val: g_FriendList)
	{
		if (val== m_Edit_RecvId_Value)
		{
			MessageBox(TEXT("�Է��������ĺ��ѣ�"), TEXT("ʧ�ܣ�"),MB_ICONWARNING);
			return;
		}
	}
	m_Client.SendData(TEXT("������Ӻ���") + SEG + m_Edit_RecvId_Value + SEG + g_Id + SEG + g_Name);
	MessageBox(TEXT("�ѷ�����Ӻ�������"), TEXT("�ɹ���"), MB_ICONINFORMATION);
}
