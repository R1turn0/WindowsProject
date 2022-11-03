#include "NoteFont.h"

static LOGFONT logfont;
static HFONT   hFont;

void FontInit(HWND hwndEdit)
{
    GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT),
        (PTSTR)&logfont);

    hFont = CreateFontIndirect(&logfont);
    SendMessage(hwndEdit, WM_SETFONT, (WPARAM)hFont, 0);
}

BOOL FontChooseFont(HWND hwnd)
{
    CHOOSEFONT cf;

    cf.lStructSize = sizeof(CHOOSEFONT);                                // 结构的长度，以字节为单位。
    cf.hwndOwner = hwnd;                                                // 拥有对话框的窗口句柄。此成员可以是任何有效的窗口句柄，或者如果对话框没有所有者，它可以为NULL 。
    cf.hDC = NULL;                                                      // 打印机的设备上下文或信息上下文句柄
    cf.lpLogFont = &logfont;                                            // 
    cf.iPointSize = 0;                                                  // 所选字体的大小，以点的 1/10 为单位。
    cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_EFFECTS;    // 一组可用于初始化字体对话框的位标志。当对话框返回时，它会设置这些标志来指示用户输入。
    cf.rgbColors = 0;                                                   // 如果设置了CF_EFFECTS标志，则rgbColors指定初始文本颜色。
    cf.lCustData = 0;                                                   
    cf.lpfnHook = NULL;
    cf.lpTemplateName = NULL;
    cf.hInstance = NULL;
    cf.lpszStyle = NULL;
    cf.nFontType = 0;               // Returned from ChooseFont
    cf.nSizeMin = 0;
    cf.nSizeMax = 0;

    return ChooseFont(&cf);
}

void FontSetFont(HWND hwndEdit)
{
    HFONT hFontNew;
    RECT  rect;

    hFontNew = CreateFontIndirect(&logfont);
    SendMessage(hwndEdit, WM_SETFONT, (WPARAM)hFontNew, 0);
    DeleteObject(hFont);
    hFont = hFontNew;
    GetClientRect(hwndEdit, &rect);
    InvalidateRect(hwndEdit, &rect, TRUE);
}

void FontDeinitialize()
{
    DeleteObject(hFont);
}
