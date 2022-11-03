#ifndef NoteFile_H
#define NoteFile_H

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void FileInit(HWND);

BOOL FileOpenDlg(HWND, PTSTR, PTSTR);

BOOL FileSaveDlg(HWND, PTSTR, PTSTR);

BOOL FileRead(HWND, PTSTR);

BOOL FileWrite(HWND, PTSTR);

BOOL FileOpenDlg(HWND, PTSTR, PTSTR);

#endif // !NoteFile_H
