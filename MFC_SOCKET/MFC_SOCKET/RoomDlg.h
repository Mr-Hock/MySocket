#pragma once
#include "ClistCtrlEx.h"
#include "afxwin.h"
#include "afxcmn.h"


// RoomDlg 对话框

class RoomDlg : public CDialogEx
{
	DECLARE_DYNAMIC(RoomDlg)

public:
	RoomDlg(CWnd* pParent, CString nId, CString nName);   // 标准构造函数
	virtual ~RoomDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ROOM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	HICON m_hIcon;

	CString m_RoomId;
	CString m_RoomName;
	CEdit m_Edit_Recv;
	CEdit m_Edit_Send;
	CString m_Edit_Recv_Value;
	CString m_Edit_Send_Value;
	ClistCtrlEx m_List_Online;
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void PrintOnline(CString nData);
	afx_msg void OnDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton2();
	CStatic m_Picture_Pic;
	CStatic m_Static_Id;
	CStatic m_Static_Name;
};
