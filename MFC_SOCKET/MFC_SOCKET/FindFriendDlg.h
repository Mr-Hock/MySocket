#pragma once
#include "afxwin.h"


// FindFriendDlg �Ի���

class FindFriendDlg : public CDialogEx
{
	DECLARE_DYNAMIC(FindFriendDlg)

public:
	FindFriendDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~FindFriendDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FINDFRIEND };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_Edit_Find;
	CString m_Edit_Find_Value;
	CEdit m_Edit_RecvId;
	CString m_Edit_RecvId_Value;
	CEdit m_Edit_RecvName;
	CString m_Edit_RecvName_Value;
	CButton m_Button_Add;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton3();
	CEdit m_Edit_RecvStatus;
	CString m_Edit_RecvStatus_Value;
};
