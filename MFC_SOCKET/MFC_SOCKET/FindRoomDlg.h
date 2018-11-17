#pragma once
#include "afxwin.h"


// FindRoomDlg 对话框

class FindRoomDlg : public CDialogEx
{
	DECLARE_DYNAMIC(FindRoomDlg)

public:
	FindRoomDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~FindRoomDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FINDROOM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_Edit_Find;
	CString m_Edit_Find_Value;
	CEdit m_Edit_RecvId;
	CString m_Edit_RecvId_Value;
	CEdit m_Edit_RecvName;
	CString m_Edit_RecvName_Value;
	CEdit m_Edit_RecvNum;
	CString m_Edit_RecvNum_Value;
	CButton m_Button_Add;
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButton3();
};
