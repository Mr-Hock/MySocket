
// MFC_SOCKET.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMFC_SOCKETApp: 
// �йش����ʵ�֣������ MFC_SOCKET.cpp
//

class CMFC_SOCKETApp : public CWinApp
{
public:
	CMFC_SOCKETApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMFC_SOCKETApp theApp;