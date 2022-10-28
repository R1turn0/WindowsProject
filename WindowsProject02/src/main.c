#define _CRT_SECURE_NO_WARNINGS //����sdl���

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
        MessageBox(NULL, TEXT("���������Ҫ��windows nt������"), szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow(szAppName,
        TEXT("������"),
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
//        SetScrollRange(hwnd, SB_VERT, 0, NUMLINES - 1, FALSE); // ����ָ������������С����������λ��
//        SetScrollPos(hwnd, SB_VERT, 0, TRUE);                  // ���ù�����λ��
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
//            SetTextAlign(hdc, TA_RIGHT | TA_TOP);   // �����Ҷ���
//
//            StringCchPrintf(szBuffer, 10, TEXT("%5d"), GetSystemMetrics(sysmetrics[i].iIndex));
//            StringCchLength(szBuffer, 10, &iTarget);
//            TextOut(hdc, 22 * cxCaps + 40 * cxChar, y, szBuffer, iTarget);
//
//            StringCchPrintf(szBuffer, 10, TEXT("%5d"), i + 1);
//            StringCchLength(szBuffer, 10, &iTarget);
//            TextOut(hdc, 22 * cxCaps + 40 * cxChar + 15 * cxChar, y, szBuffer, iTarget);
//
//            SetTextAlign(hdc, TA_LEFT | TA_TOP);    // ���������
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
//        case SB_LINEUP:     // ���Ϲ���һ��
//            iVscrollPos -= 1;
//            break;
//
//        case SB_LINEDOWN:   // ���¹���һ��
//            iVscrollPos += 1;
//            break;
//
//        case SB_PAGEUP:     // ���Ϲ���һҳ
//            iVscrollPos -= cyClient / cyChar;
//            break;
//
//        case SB_PAGEDOWN:   // ���¹���һҳ
//            iVscrollPos += cyClient / cyChar;
//            break;
//
//        case SB_THUMBTRACK: // ��ק������
//            iVscrollPos = HIWORD(wParam);
//            break;
//        }
//        ReleaseDC(hwnd, hdc);              
//
//        iVscrollPos = max(0, min(iVscrollPos, NUMLINES - 1));   // ���ù�����λ��
//        if (iVscrollPos != GetScrollPos(hwnd, SB_VERT))
//        {
//            SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);   
//            InvalidateRect(hwnd, NULL, TRUE);       // �������������ػ洰��
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
                 ��API������ScrollWindow ����ʹ������
                     ��C������ -- www.fishc.com
--------------------------------------------------------------------*/
//LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    HDC hdc;
//    PAINTSTRUCT ps;
//    TEXTMETRIC tm;
//    SCROLLINFO si;
//
//    static int xClient;     // �ͻ����Ŀ��
//    static int yClient;     // �ͻ����ĸ߶�
//    static int xClientMax;  // �ͻ���������ʾ������ȣ������˿��������������
//
//    static int xChar;       // �����ƽ��ˮƽ���
//    static int yChar;       // ����Ĵ�ֱ�߶�
//    static int xUpper;      // ����Ĵ�д�ַ���ˮƽ���
//
//    static int xPos;        // ��ǰˮƽ�����������λ��
//    static int yPos;        // ��ǰ��ֱ�����������λ��
//
//    int i;                  // ѭ��������
//    int x, y;               // ˮƽ�ʹ�ֱ������
//
//    int FirstLine;          // ʧЧ����ĵ�һ�У����ػ�ĵ�һ�У�
//    int LastLine;           // ʧЧ��������һ�У������ػ�����һ�У���
//    HRESULT hr;
//    size_t abcLength;       // ���ڴ�� abc ����ĳ���
//
//    // ����Ҫ��ʾ���У�������ӡ�������ҾͲ������˰�...��
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
//        // ����豸�������
//        hdc = GetDC(hwnd);
//
//        // ����ı�������Ĵ�С 
//        GetTextMetrics(hdc, &tm);
//        xChar = tm.tmAveCharWidth;
//        xUpper = (tm.tmPitchAndFamily & 1 ? 3 : 2) * xChar / 2;
//        yChar = tm.tmHeight + tm.tmExternalLeading;
//
//        // �ͷ��豸������� 
//        ReleaseDC(hwnd, hdc);
//
//        // ���ÿͻ���������� 
//        // ��������������Ϊ 48 ��Сд�ַ��Ŀ�� + 12 ����д�ַ��Ŀ�ȣ�
//        xClientMax = 48 * xChar + 12 * xUpper;
//
//        return 0;
//
//    case WM_SIZE:
//        // ��ÿͻ����ĳߴ� 
//        yClient = HIWORD(lParam);
//        xClient = LOWORD(lParam);
//
//        // ���ô�ֱ��������Χ��ҳ���С������ҳ���С����������Ĵ�ϸ��
//        si.cbSize = sizeof(si);
//        si.fMask = SIF_RANGE | SIF_PAGE;
//        si.nMin = 0;
//        si.nMax = LINES - 1;
//        si.nPage = yClient / yChar;
//        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
//
//        // ����ˮƽ��������Χ��ҳ���С������ҳ���С����������Ĵ�ϸ�� 
//        si.cbSize = sizeof(si);
//        si.fMask = SIF_RANGE | SIF_PAGE;
//        si.nMin = 0;
//        si.nMax = 2 + xClientMax / xChar;
//        si.nPage = xClient / xChar;
//        SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
//
//        return 0;
//    case WM_HSCROLL:
//        // ���ˮƽ��������������Ϣ
//        si.cbSize = sizeof(si);
//        si.fMask = SIF_ALL;
//        GetScrollInfo(hwnd, SB_HORZ, &si);
//
//        // ���浱ǰ����λ�ã���Щ���бȽ�
//        xPos = si.nPos;
//        switch (LOWORD(wParam))
//        {
//            // �û������������ߵ�������
//        case SB_LINELEFT:
//            si.nPos -= 1;
//            break;
//
//            // �û�����������ұߵ�������
//        case SB_LINERIGHT:
//            si.nPos += 1;
//            break;
//
//            // �û����������ߵĹ�������
//        case SB_PAGELEFT:
//            si.nPos -= si.nPage;
//            break;
//
//            // �û���������ұߵĹ�������
//        case SB_PAGERIGHT:
//            si.nPos += si.nPage;
//            break;
//
//            // �û��϶�������
//        case SB_THUMBTRACK:
//            si.nPos = si.nTrackPos;
//            break;
//
//        default:
//            break;
//        }
//
//        // ���ù������������λ��
//        si.fMask = SIF_POS;
//        SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
//
//        // ��ù����������λ�ã����ڴ��ڵ����������ܲ���ͬһ��ֵ
//        GetScrollInfo(hwnd, SB_HORZ, &si);
//
//        // ���ǰ�ı����ֵ���бȽϣ������ͬ���������
//        if (si.nPos != xPos)
//        {
//            ScrollWindow(hwnd, xChar * (xPos - si.nPos), 0, NULL, NULL);
//        }
//
//        return 0;
//
//    case WM_VSCROLL:
//        // ��ô�ֱ��������������Ϣ
//        si.cbSize = sizeof(si);
//        si.fMask = SIF_ALL;
//        GetScrollInfo(hwnd, SB_VERT, &si);
//
//        // ���浱ǰ����λ�ã���Щ���бȽ�
//        yPos = si.nPos;
//        switch (LOWORD(wParam))
//        {
//            // �û�������� Home ����
//        case SB_TOP:
//            si.nPos = si.nMin;
//            break;
//
//            // �û�������� End ����
//        case SB_BOTTOM:
//            si.nPos = si.nMax;
//            break;
//
//            // �û�����������ϱߵ�������
//        case SB_LINEUP:
//            si.nPos -= 1;
//            break;
//
//            // �û�����������±ߵ�������
//        case SB_LINEDOWN:
//            si.nPos += 1;
//            break;
//
//            // �û���������ϱߵĹ�������
//        case SB_PAGEUP:
//            si.nPos -= si.nPage;
//            break;
//
//            // �û���������±ߵĹ�������
//        case SB_PAGEDOWN:
//            si.nPos += si.nPage;
//            break;
//
//            // �û��϶�������
//        case SB_THUMBTRACK:
//            si.nPos = si.nTrackPos;
//            break;
//
//        default:
//            break;
//        }
//
//        // ���ù������������λ��
//        si.fMask = SIF_POS;
//        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
//
//        // ��ù����������λ�ã����ڴ��ڵ����������ܲ���ͬһ��ֵ
//        GetScrollInfo(hwnd, SB_VERT, &si);
//
//        // ���ǰ�ı����ֵ���бȽϣ������ͬ���������
//        if (si.nPos != yPos)
//        {
//            ScrollWindow(hwnd, 0, yChar * (yPos - si.nPos), NULL, NULL);
//            UpdateWindow(hwnd);
//        }
//
//        return 0;
//
//    case WM_PAINT:
//        // ׼������
//        hdc = BeginPaint(hwnd, &ps);
//
//        // ��ô�ֱ��������λ��
//        si.cbSize = sizeof(si);
//        si.fMask = SIF_POS;
//        GetScrollInfo(hwnd, SB_VERT, &si);
//        yPos = si.nPos;
//
//        // ���ˮƽ��������λ��
//        GetScrollInfo(hwnd, SB_HORZ, &si);
//        xPos = si.nPos;
//
//        // ������Ҫ�ػ������
//        FirstLine = max(0, yPos + ps.rcPaint.top / yChar);
//        LastLine = min(LINES - 1, yPos + ps.rcPaint.bottom / yChar);
//
//        for (i = FirstLine; i <= LastLine; i++)
//        {
//            x = xChar * (1 - xPos);
//            y = yChar * (i - yPos);
//
//            // ʹ�ð�ȫ�ַ�����������ַ�������
//            hr = StringCchLength(abc[i], 55, &abcLength);
//            if ((FAILED(hr)) || (abcLength == NULL))
//            {
//                // ��ȡ�ַ�������ʧ�ܵ���ʾ...
//            }
//
//            // �ڿͻ�������һ������
//            TextOut(hdc, x, y, abc[i], abcLength);
//        }
//
//        // �������
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