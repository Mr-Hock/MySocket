#pragma once
#include "afxwin.h"


// CreatRoomDlg 对话框

class CreatRoomDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CreatRoomDlg)

public:
	CreatRoomDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CreatRoomDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CREATROOM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton3();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CEdit m_Edit_Id;
	CString m_Edit_Id_Value;
	CEdit m_Edit_Name;
	CString m_Edit_Name_Value;
	CButton m_Button_Creat;
};
