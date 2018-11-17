#pragma once
#include <vector>
using std::vector;

CString EncryData(CString nData);
INT GetRandNum(INT Min, INT Max);
CString GetRoute(CString type, TCHAR *nFileName);
TCHAR* GetProgramDir();
CString GetIni(CString nFileName, CString n1, CString n2);
VOID SetIni(CString nFileName, CString nApp, CString nKey, CString nValue);
VOID GetTime(CString &szTime);
DWORD SegmentText(vector<CString>&nText, CString &nSrc, CString nSeg);
CString GetFileFullName(CString csFilePath);