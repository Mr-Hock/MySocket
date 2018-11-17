#pragma once
#include "afxwin.h"
#include "TCP_CLIENT.h"
#include "afxcmn.h"
#include "ClistCtrlEx.h"


// HomeDlg 对话框

class HomeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(HomeDlg)

public:
	HomeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~HomeDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_HOME };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	ClistCtrlEx m_List_Friend;
	ClistCtrlEx m_List_Room;
	ClistCtrlEx m_List_SystemMsg;
	CMenu m_Menu;
	CImageList m_FriendImageList;
	CImageList  m_RoomImageList;
	CImageList  m_MsgImageList;

	void PrintFriend(CString &nData);
	void PrintRoom(CString &nData);
	afx_msg void OnDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClose();
	afx_msg void OnDblclkList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnFindfriend();
	afx_msg void OnDblclkList3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeletefriend();
	afx_msg void OnOutroom();
	afx_msg void OnFindroom();
	afx_msg void OnRclickList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCreatroom();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	void LoadFriendImage();
	void LoadRoomImage();
	void LoadMsgImage();
	void LoadMyHeadImage(CString nNum);
	CString HomeDlg::GetFriendHeadImage(CString nId);
	CStatic m_Static_Id;
	CStatic m_Static_Name;
	CStatic m_Picture_Pic;
	CButton m_Radio_FriendList;
	afx_msg void OnBnClickedButton1();
};
