#define _CRT_SECURE_NO_WARNINGS //撤销sdl检查

#include <windows.h>
#include "strsafe.h"
#include "sysmets.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szAppName[] = TEXT("MyWindows");
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass;
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = NULL;//LoadCursor(NULL, IDI_QUESTION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass))
    {
        MessageBox(NULL, TEXT("这个程序需要在windows nt下运行"), szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow(szAppName,
        TEXT("滚动条"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL);

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

//LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    HDC hdc;
//    PAINTSTRUCT ps;
//    TEXTMETRIC tm;
//    TCHAR szBuffer[10];
//    static int cxChar, cxCaps, cyChar;
//    static int cxClient, cyClient;
//    static int iVscrollPos;
//    size_t iTarget;
//    int i, y;
//
//    switch (message)
//    {
//    case WM_CREATE:
//        hdc = GetDC(hwnd);
//
//        GetTextMetrics(hdc, &tm);
//        cxChar = tm.tmAveCharWidth;
//        cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
//        cyChar = tm.tmHeight + tm.tmExternalLeading;
//
//        ReleaseDC(hwnd, hdc);
//
//        SetScrollRange(hwnd, SB_VERT, 0, NUMLINES - 1, FALSE); // 设置指定滚动条的最小和最大滚动框位置
//        SetScrollPos(hwnd, SB_VERT, 0, TRUE);                  // 设置滚动条位置
//
//        return 0;
//
//    case WM_PAINT:
//        hdc = BeginPaint(hwnd, &ps);
//
//        for (i = 0; i < NUMLINES; i++)
//        {
//            y = cyChar * (i - iVscrollPos);
//
//            StringCchLength(sysmetrics[i].szLabel, 1024, &iTarget);
//            TextOut(hdc, 0, y, sysmetrics[i].szLabel, iTarget);
//
//            StringCchLength(sysmetrics[i].szDesc, 1024, &iTarget);
//            TextOut(hdc, 22 * cxCaps, y, sysmetrics[i].szDesc, iTarget);
//
//            SetTextAlign(hdc, TA_RIGHT | TA_TOP);   // 设置右对齐
//
//            StringCchPrintf(szBuffer, 10, TEXT("%5d"), GetSystemMetrics(sysmetrics[i].iIndex));
//            StringCchLength(szBuffer, 10, &iTarget);
//            TextOut(hdc, 22 * cxCaps + 40 * cxChar, y, szBuffer, iTarget);
//
//            StringCchPrintf(szBuffer, 10, TEXT("%5d"), i + 1);
//            StringCchLength(szBuffer, 10, &iTarget);
//            TextOut(hdc, 22 * cxCaps + 40 * cxChar + 15 * cxChar, y, szBuffer, iTarget);
//
//            SetTextAlign(hdc, TA_LEFT | TA_TOP);    // 设置左对齐
//        }
//        EndPaint(hwnd, &ps);
//        return 0;
//
//    case WM_SIZE:
//        cxClient = LOWORD(lParam);
//        cyClient = HIWORD(lParam);
//        return 0;
//
//    case WM_VSCROLL:
//        hdc = GetDC(hwnd);
//        SetTextAlign(hdc, TA_TOP | TA_RIGHT);
//
//        switch (LOWORD(wParam))
//        {
//        case SB_LINEUP:     // 向上滚动一行
//            iVscrollPos -= 1;
//            break;
//
//        case SB_LINEDOWN:   // 向下滚动一行
//            iVscrollPos += 1;
//            break;
//
//        case SB_PAGEUP:     // 向上滚动一页
//            iVscrollPos -= cyClient / cyChar;
//            break;
//
//        case SB_PAGEDOWN:   // 向下滚动一页
//            iVscrollPos += cyClient / cyChar;
//            break;
//
//        case SB_THUMBTRACK: // 拖拽滚动条
//            iVscrollPos = HIWORD(wParam);
//            break;
//        }
//        ReleaseDC(hwnd, hdc);              
//
//        iVscrollPos = max(0, min(iVscrollPos, NUMLINES - 1));   // 设置滚动条位置
//        if (iVscrollPos != GetScrollPos(hwnd, SB_VERT))
//        {
//            SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);   
//            InvalidateRect(hwnd, NULL, TRUE);       // 滑动滚动条后重绘窗口
//        }
//
//        return 0;
//
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        return 0;
//
//    }
//    return DefWindowProc(hwnd, message, wParam, lParam);
//
//}
//


/* -------------------------------------------------------------------
                 【API档案】ScrollWindow 函数使用例子
                     鱼C工作室 -- www.fishc.com
--------------------------------------------------------------------*/
//LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    HDC hdc;
//    PAINTSTRUCT ps;
//    TEXTMETRIC tm;
//    SCROLLINFO si;
//
//    static int xClient;     // 客户区的宽度
//    static int yClient;     // 客户区的高度
//    static int xClientMax;  // 客户区内容显示的最大宽度（超过此宽度则加入滚动条）
//
//    static int xChar;       // 字体的平均水平宽度
//    static int yChar;       // 字体的垂直高度
//    static int xUpper;      // 字体的大写字符的水平宽度
//
//    static int xPos;        // 当前水平滚动条滑块的位置
//    static int yPos;        // 当前垂直滚动条滑块的位置
//
//    int i;                  // 循环计数器
//    int x, y;               // 水平和垂直的坐标
//
//    int FirstLine;          // 失效区域的第一行（需重绘的第一行）
//    int LastLine;           // 失效区域的最后一行（（需重绘的最后一行））
//    HRESULT hr;
//    size_t abcLength;       // 用于存放 abc 数组的长度
//
//    // 创建要显示的行（呃，打印的内容我就不翻译了吧...）
//#define LINES 28 
//    static TCHAR* abc[] = {
//            TEXT("anteater"), TEXT("bear"), TEXT("cougar"),
//            TEXT("dingo"), TEXT("elephant"), TEXT("falcon"),
//            TEXT("gazelle"), TEXT("hyena"), TEXT("iguana"),
//            TEXT("jackal"), TEXT("kangaroo"), TEXT("llama"),
//            TEXT("moose"), TEXT("newt"), TEXT("octopus"),
//            TEXT("penguin"), TEXT("quail"), TEXT("rat"),
//            TEXT("squid"), TEXT("tortoise"), TEXT("urus"),
//            TEXT("vole"), TEXT("walrus"), TEXT("xylophone"),
//            TEXT("yak"), TEXT("zebra"),
//            TEXT("This line contains words, but no character. Go figure."),
//            TEXT("")
//    };
//
//    switch (message)
//    {
//    case WM_CREATE:
//        // 获得设备环境句柄
//        hdc = GetDC(hwnd);
//
//        // 获得文本中字体的大小 
//        GetTextMetrics(hdc, &tm);
//        xChar = tm.tmAveCharWidth;
//        xUpper = (tm.tmPitchAndFamily & 1 ? 3 : 2) * xChar / 2;
//        yChar = tm.tmHeight + tm.tmExternalLeading;
//
//        // 释放设备环境句柄 
//        ReleaseDC(hwnd, hdc);
//
//        // 设置客户区的最大宽度 
//        // （我们这里设置为 48 个小写字符的宽度 + 12 个大写字符的宽度）
//        xClientMax = 48 * xChar + 12 * xUpper;
//
//        return 0;
//
//    case WM_SIZE:
//        // 获得客户区的尺寸 
//        yClient = HIWORD(lParam);
//        xClient = LOWORD(lParam);
//
//        // 设置垂直滚动条范围和页面大小（设置页面大小将决定滑块的粗细）
//        si.cbSize = sizeof(si);
//        si.fMask = SIF_RANGE | SIF_PAGE;
//        si.nMin = 0;
//        si.nMax = LINES - 1;
//        si.nPage = yClient / yChar;
//        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
//
//        // 设置水平滚动条范围和页面大小（设置页面大小将决定滑块的粗细） 
//        si.cbSize = sizeof(si);
//        si.fMask = SIF_RANGE | SIF_PAGE;
//        si.nMin = 0;
//        si.nMax = 2 + xClientMax / xChar;
//        si.nPage = xClient / xChar;
//        SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
//
//        return 0;
//    case WM_HSCROLL:
//        // 获得水平滚动条的所有信息
//        si.cbSize = sizeof(si);
//        si.fMask = SIF_ALL;
//        GetScrollInfo(hwnd, SB_HORZ, &si);
//
//        // 保存当前滑块位置，迟些进行比较
//        xPos = si.nPos;
//        switch (LOWORD(wParam))
//        {
//            // 用户点击滚动条左边的三角形
//        case SB_LINELEFT:
//            si.nPos -= 1;
//            break;
//
//            // 用户点击滚动条右边的三角形
//        case SB_LINERIGHT:
//            si.nPos += 1;
//            break;
//
//            // 用户点击滑块左边的滚动条轴
//        case SB_PAGELEFT:
//            si.nPos -= si.nPage;
//            break;
//
//            // 用户点击滑块右边的滚动条轴
//        case SB_PAGERIGHT:
//            si.nPos += si.nPage;
//            break;
//
//            // 用户拖动滚动条
//        case SB_THUMBTRACK:
//            si.nPos = si.nTrackPos;
//            break;
//
//        default:
//            break;
//        }
//
//        // 设置滚动条滑块的新位置
//        si.fMask = SIF_POS;
//        SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
//
//        // 获得滚动条滑块的位置，由于窗口调整，它可能不是同一个值
//        GetScrollInfo(hwnd, SB_HORZ, &si);
//
//        // 与此前的保存的值进行比较，如果不同则滚动窗口
//        if (si.nPos != xPos)
//        {
//            ScrollWindow(hwnd, xChar * (xPos - si.nPos), 0, NULL, NULL);
//        }
//
//        return 0;
//
//    case WM_VSCROLL:
//        // 获得垂直滚动条的所有信息
//        si.cbSize = sizeof(si);
//        si.fMask = SIF_ALL;
//        GetScrollInfo(hwnd, SB_VERT, &si);
//
//        // 保存当前滑块位置，迟些进行比较
//        yPos = si.nPos;
//        switch (LOWORD(wParam))
//        {
//            // 用户点击键盘 Home 按键
//        case SB_TOP:
//            si.nPos = si.nMin;
//            break;
//
//            // 用户点击键盘 End 按键
//        case SB_BOTTOM:
//            si.nPos = si.nMax;
//            break;
//
//            // 用户点击滚动条上边的三角形
//        case SB_LINEUP:
//            si.nPos -= 1;
//            break;
//
//            // 用户点击滚动条下边的三角形
//        case SB_LINEDOWN:
//            si.nPos += 1;
//            break;
//
//            // 用户点击滑块上边的滚动条轴
//        case SB_PAGEUP:
//            si.nPos -= si.nPage;
//            break;
//
//            // 用户点击滑块下边的滚动条轴
//        case SB_PAGEDOWN:
//            si.nPos += si.nPage;
//            break;
//
//            // 用户拖动滚动条
//        case SB_THUMBTRACK:
//            si.nPos = si.nTrackPos;
//            break;
//
//        default:
//            break;
//        }
//
//        // 设置滚动条滑块的新位置
//        si.fMask = SIF_POS;
//        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
//
//        // 获得滚动条滑块的位置，由于窗口调整，它可能不是同一个值
//        GetScrollInfo(hwnd, SB_VERT, &si);
//
//        // 与此前的保存的值进行比较，如果不同则滚动窗口
//        if (si.nPos != yPos)
//        {
//            ScrollWindow(hwnd, 0, yChar * (yPos - si.nPos), NULL, NULL);
//            UpdateWindow(hwnd);
//        }
//
//        return 0;
//
//    case WM_PAINT:
//        // 准备绘制
//        hdc = BeginPaint(hwnd, &ps);
//
//        // 获得垂直滚动条的位置
//        si.cbSize = sizeof(si);
//        si.fMask = SIF_POS;
//        GetScrollInfo(hwnd, SB_VERT, &si);
//        yPos = si.nPos;
//
//        // 获得水平滚动条的位置
//        GetScrollInfo(hwnd, SB_HORZ, &si);
//        xPos = si.nPos;
//
//        // 计算需要重绘的区域
//        FirstLine = max(0, yPos + ps.rcPaint.top / yChar);
//        LastLine = min(LINES - 1, yPos + ps.rcPaint.bottom / yChar);
//
//        for (i = FirstLine; i <= LastLine; i++)
//        {
//            x = xChar * (1 - xPos);
//            y = yChar * (i - yPos);
//
//            // 使用安全字符串函数获得字符串长度
//            hr = StringCchLength(abc[i], 55, &abcLength);
//            if ((FAILED(hr)) || (abcLength == NULL))
//            {
//                // 获取字符串长度失败的提示...
//            }
//
//            // 在客户区绘制一行数据
//            TextOut(hdc, x, y, abc[i], abcLength);
//        }
//
//        // 绘制完毕
//        EndPaint(hwnd, &ps);
//        return 0;
//
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        return 0;
//    }
//
//    return DefWindowProc(hwnd, message, wParam, lParam);
//}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

}