#pragma once
#include "afxwin.h"


// ChangeInfoDlg �Ի���

class ChangeInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ChangeInfoDlg)

public:
	ChangeInfoDlg(CWnd* pParent, CString nPic, CString nName, CString nSex);   // ��׼���캯��
	virtual ~ChangeInfoDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CHANGEINFO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_Combo_Pic;
	CStatic m_Picture_Pic;
	CEdit m_Edit_Name;
	CString m_Edit_Name_Value;
	CComboBox m_Combo_Sex;
	afx_msg void OnBnClickedButton2();
	virtual BOOL OnInitDialog();

	CString m_Pic;
	CString m_Name;
	CString m_Sex;
	CString m_Combo_Pic_Value;
	CString m_Combo_Sex_Value;
	afx_msg void OnSelchangeCombo2();
};
