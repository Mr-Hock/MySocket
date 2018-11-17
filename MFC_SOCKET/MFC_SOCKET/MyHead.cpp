#include "stdafx.h"
#include "MyHead.h"
#include <vector>
using std::vector;

CString EncryData(CString nData)
{
	INT nSize = nData.GetLength() * sizeof(TCHAR) + sizeof(TCHAR);
	TCHAR *nTemp = new TCHAR[nSize]{};
	_tcscpy_s(nTemp, nSize, nData);

	for (int i = 0; i < nSize; i++)
	{
		nTemp[i] = nTemp[i] ^ 10;
	}

	return nTemp;
}

INT GetRandNum(INT Min, INT Max)
{
	//srand(time(NULL));
	return rand() % Max + Min;
}

CString GetFileFullName(CString csFilePath)
{
	int nPos = csFilePath.ReverseFind('\\'); // 文件路径，以'\'斜杠分隔的路径  
	CString csFileFullName;
	csFileFullName = csFilePath.Right(csFilePath.GetLength() - nPos - 1); // 获取文件全名，包括文件名和扩展名  
	return csFileFullName;
}


CString GetRoute(CString type, TCHAR *nFileName)
{

	TCHAR szFileName[MAX_PATH] = { 0 };
	_tcscpy_s(szFileName, MAX_PATH, nFileName);


	OPENFILENAME openFileName = { 0 };
	openFileName.lStructSize = sizeof(OPENFILENAME);
	openFileName.nMaxFile = MAX_PATH;  //这个必须设置，不设置的话不会出现打开文件对话框
	openFileName.lpstrFile = szFileName;
	openFileName.nFilterIndex = 1;
	openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	//openFileName.lpstrFilter = "所有文件(*.map)\0*.map\0";

	if (type==TEXT("save"))
	{
		if (GetSaveFileName(&openFileName))
		{
			return openFileName.lpstrFile;
		}

	}
	else if (type == TEXT("open"))
	{
		if (GetOpenFileName(&openFileName))
		{
			return openFileName.lpstrFile;
		}
	}

	return szFileName;
}



TCHAR* GetProgramDir()
{
	static TCHAR exeFullPath[MAX_PATH]; // Full path
	TCHAR *nWeak;

	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	nWeak = _tcsrchr(exeFullPath, '\\');

	nWeak[1] = 0;
	return exeFullPath;
	system("pause");

}

CString GetIni(CString nFileName, CString nApp, CString nKey)
{
	TCHAR nTemp[256]{};//储存配置文件全路径
	TCHAR nRoute[256]{};//保存当前运行目录
	_tcscpy_s(nRoute, GetProgramDir());//取出当前运行目录
	_tcscat_s(nRoute, nFileName);//组成配置文件全路径

	GetPrivateProfileString(nApp, nKey, TEXT(""), nTemp, _countof(nTemp), nRoute);
	return nTemp;
}
VOID SetIni(CString nFileName, CString nApp, CString nKey,CString nValue)
{
	TCHAR nRoute[256]{};//保存当前运行目录
	_tcscpy_s(nRoute, GetProgramDir());//取出当前运行目录
	_tcscat_s(nRoute, nFileName);//组成配置文件全路径
	WritePrivateProfileString(nApp, nKey, nValue, nRoute);
}

VOID GetTime(CString &szTime)
{
	SYSTEMTIME nTime{};
	GetLocalTime(&nTime);
	szTime.Format(TEXT("%d:%d:%d"), nTime.wHour, nTime.wMinute, nTime.wSecond);
}

CStringArray* DivString(CString test, CString nSeg, DWORD &nNum)
{
	CStringArray* m_result = new CStringArray;
	while (TRUE)
	{
		int index = test.Find(nSeg);
		if (index == -1)
		{
			nNum++;
			m_result->Add(test);
			return m_result;
		}
		CString test1 = test.Left(index);
		m_result->Add(test1);
		test = test.Right(test.GetLength() - index - nSeg.GetLength());
		nNum++;
	}
}
DWORD SegmentText(vector<CString>&nText, CString &nSrc, CString nSeg)
{
	nText.swap(vector<CString>());

	DWORD nNum = 0;
	CStringArray* result = DivString(nSrc, nSeg, nNum);
	for (DWORD i = 0;i<nNum;i++)
	{
		nText.push_back(result->GetAt(i));
	}
	delete result;
	return nText.size();
}


