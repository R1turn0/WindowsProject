#ifndef NoteFont_H
#define NoteFont_H

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <commdlg.h>

BOOL NotepadFontChooseFont(HWND hwnd);
void NotepadFontInitialize(HWND hwndEdit);
void NotepadFontSetFont(HWND hwndEdit);
void NotepadFontDeinitialize(void);


#endif // !NoteFont_H
