#ifndef NoteFile_H
#define NoteFile_H

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

DWORD FileInit(HWND);

BOOL FileOpenDlg(HWND, PTSTR, PTSTR);

BOOL FileSaveDlg(HWND, PTSTR, PTSTR);

BOOL MapFileRead(HWND, PTSTR);

BOOL FileWrite(HWND, PTSTR);

BOOL MapFileWrite(HWND, PTSTR);

BOOL is_str_gbk(PBYTE);

#endif // !NoteFile_H
