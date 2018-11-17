#pragma once
#include "afxwin.h"


// RegisterDlg 对话框

class RegisterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(RegisterDlg)

public:
	RegisterDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~RegisterDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_REGISTER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	HICON m_hIcon;

	CString m_Edit_Id_Value;
	CString m_Edit_Name_Value;
	CString m_Edit_PassWord_Value;
	afx_msg void OnBnClickedButton2();
	CComboBox m_ComboBox_Sex;
	CString m_ComboBox_Sex_Value;
	virtual BOOL OnInitDialog();
	CStatic m_Pic;
	CComboBox m_ComboBox_Pic;
	CString m_ComboBox_Pic_Value;
	afx_msg void OnSelchangeCombo2();
};
