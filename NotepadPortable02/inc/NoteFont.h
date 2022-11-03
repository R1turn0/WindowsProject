#ifndef NoteFont_H
#define NoteFont_H

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <commdlg.h>

void FontInit(HWND);

BOOL FontChooseFont(HWND);

void FontSetFont(HWND);

void FontDeinitialize();


#endif // !NoteFont_H
