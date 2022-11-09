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

BOOL FileOpenDlg(HWND, PTSTR, PTSTR);

BOOL MapFileWrite(HWND, PTSTR);

#endif // !NoteFile_H
