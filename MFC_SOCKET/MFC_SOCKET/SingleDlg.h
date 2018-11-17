#pragma once
#include "afxwin.h"


// SingleDlg 对话框

class SingleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SingleDlg)

public:
	SingleDlg(CWnd* pParent, CString nId, CString nName, CString nPic);   // 标准构造函数
	virtual ~SingleDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SINGLE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	HICON m_hIcon;

	CString m_Id;
	CString m_Name;
	CString m_Pic;

	CEdit m_Edit_Recv;
	CEdit m_Edit_Send;
	CString m_Edit_Recv_Value;
	CString m_Edit_Send_Value;
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButton2();
	CStatic m_Pitcure_Pic;
	CStatic m_Static_Id;
	CStatic m_Static_Name;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	CStatic m_Static_Progress;
	CButton m_Button_SendFile;
	CButton m_Button_RecvFile;
	afx_msg void OnDropFiles(HDROP hDropInfo);
	CButton m_Button_Video;
	afx_msg void OnBnClickedButton5();

	void LoadVideoDll();

	typedef  void  (WINAPI*pMoveWindows)(int, int, int);
	typedef  void  (WINAPI*pSetWindowsSize)(int, int, int);
	typedef  void  (WINAPI*pSetNewParent)(int, int);

	typedef  int  (WINAPI*pLoadVideo_Send)(void);
	typedef  int  (WINAPI*pLineVideo_Send)(char *, int);
	typedef  void  (WINAPI*pShowVideo_Send)(void);
	typedef  void  (WINAPI*pCloseVideo_Send)(void);

	typedef  int  (WINAPI*pLoadVideo_Recv)(void);
	typedef  void  (WINAPI*pBindPort_Recv)(int);
	typedef  void  (WINAPI*pShowVideo_Recv)(void);
	typedef  void  (WINAPI*pCloseVideo_Recv)(void);


	pSetWindowsSize SetWindowsSize;
	pMoveWindows MoveWindows;
	pSetNewParent SetNewParent;

	pLoadVideo_Send LoadVideo_Send;
	pLineVideo_Send LineVideo_Send;
	pShowVideo_Send ShowVideo_Send;
	pCloseVideo_Send CloseVideo_Send;

	pLoadVideo_Recv LoadVideo_Recv;
	pBindPort_Recv BindPort_Recv;
	pShowVideo_Recv ShowVideo_Recv;
	pCloseVideo_Recv CloseVideo_Recv;
	afx_msg void OnBnClickedButton6();
};
