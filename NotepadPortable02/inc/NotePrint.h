#ifndef NotePrint_H
#define NotePrint_H

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <commdlg.h>

BOOL CALLBACK PrintDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK AbortProc(HDC hPrinterDC, int iCode);

BOOL PrintFile(HINSTANCE hInst, HWND hwnd, HWND hwndEdit, PTSTR szTitleName);

#endif // !NotePrint_H
