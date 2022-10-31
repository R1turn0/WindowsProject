#ifndef NoteFile_H
#define NoteFile_H

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void NotepadFileInitialize(HWND hwnd);
BOOL NotepadFileOpenDlg(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName);
BOOL NotepadFileSaveDlg(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName);
BOOL NotepadFileRead(HWND hwndEdit, PTSTR pstrFileName);
BOOL NotepadFileWrite(HWND hwndEdit, PTSTR pstrFileName);
BOOL NotepadFileOpenDlg(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName);

#endif // !NoteFile_H
