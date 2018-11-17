
// MFC_SOCKETDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "HomeDlg.h"
#include "TCP_CLIENT.h"


// CMFC_SOCKETDlg �Ի���
class CMFC_SOCKETDlg : public CDialogEx
{
// ����
public:
	CMFC_SOCKETDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_SOCKET_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_Ip;
	WORD m_Port ;

	CString m_Edit_Account_Value;
	CString m_Edit_PassWord_Value;
	CEdit m_Edit_Account;
	CEdit m_Edit_PassWord;


	CButton m_Button_Login;
	CButton m_Button_Register;
	afx_msg void OnBnClickedButton1();

	afx_msg void OnBnClickedButton2();

	HomeDlg *pHomeDlg = nullptr;

	CButton m_Check_Rember;
	BOOL m_Check_Rember_Value;
};
