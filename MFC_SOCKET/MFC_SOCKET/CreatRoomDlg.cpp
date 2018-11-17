// CreatRoomDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFC_SOCKET.h"
#include "CreatRoomDlg.h"
#include "afxdialogex.h"
#include "TCP_CLIENT.h"
#include "MyHead.h"
#include <vector>
using std::vector;

extern TCP_CLIENT m_Client;
extern CString g_Id;
extern CString g_Name;
extern CString SEG;
extern CString g_CreatRoomRecv;

IMPLEMENT_DYNAMIC(CreatRoomDlg, CDialogEx)

CreatRoomDlg::CreatRoomDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_CREATROOM, pParent)
	, m_Edit_Id_Value(_T(""))
	, m_Edit_Name_Value(_T(""))
{

}

CreatRoomDlg::~CreatRoomDlg()
{
}

void CreatRoomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Edit_Id);
	DDX_Text(pDX, IDC_EDIT1, m_Edit_Id_Value);
	DDX_Control(pDX, IDC_EDIT2, m_Edit_Name);
	DDX_Text(pDX, IDC_EDIT2, m_Edit_Name_Value);
	DDX_Control(pDX, IDC_BUTTON3, m_Button_Creat);
}


BEGIN_MESSAGE_MAP(CreatRoomDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON3, &CreatRoomDlg::OnBnClickedButton3)
	ON_WM_TIMER()
END_MESSAGE_MAP()



BOOL CreatRoomDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetTimer(0x10011, 50, NULL);//���ö�ʱˢ��

	return TRUE;
}


void CreatRoomDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 0x10011)
	{

		if (g_CreatRoomRecv != TEXT(""))
		{
			

			if (g_CreatRoomRecv == TEXT("�����ɹ�"))
			{
				g_CreatRoomRecv = TEXT("");
				MessageBox(TEXT("�ɹ����������ң�"), TEXT("�ɹ���"), MB_ICONINFORMATION);
				SendMessage(WM_CLOSE, 0, 0);
			}
			else
			{
				g_CreatRoomRecv = TEXT("");
				MessageBox(TEXT("������ID�Ѵ��ڣ������ID���ԣ�"), TEXT("ʧ�ܣ�"), MB_ICONERROR);
				m_Button_Creat.EnableWindow(TRUE);
			}
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CreatRoomDlg::OnBnClickedButton3()
{
	UpdateData(TRUE);
	if (m_Edit_Id_Value == TEXT("") || m_Edit_Name_Value == TEXT(""))
	{
		MessageBox(TEXT("����д��ȷ�����ϣ�"), TEXT("ʧ�ܣ�"), MB_ICONERROR);
		return;
	}

	m_Client.SendData(TEXT("����������") + SEG + m_Edit_Id_Value + SEG + m_Edit_Name_Value + SEG + g_Id);
	m_Button_Creat.EnableWindow(FALSE);
}