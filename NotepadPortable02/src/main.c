#include <windows.h>
#include <strsafe.h>
#include "NoteFile.h"
#include "NoteFont.h"

#define NUMLINES 100
#define EDITID   1
#define BUFFER(x, y) *(pBuffer + y * cxBuffer + x) 
#define UNTITLED TEXT("New File")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static TCHAR szAppName[] = TEXT("Notepad#");
static TCHAR szMenuName[] = TEXT("NotepadMenu");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szFileName[MAX_PATH];
    HWND hwnd;  // ���ھ��
    MSG msg;
    WNDCLASS wndclass = { 0 };
    HACCEL hAccel = NULL;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;                       // ������ȸ߶����»��ƴ���
    wndclass.lpfnWndProc = WndProc;                                 // ָ�����ڹ��̣������ǻص�����
    wndclass.cbClsExtra = 0;                                        // ����ռ䣬һ��Ϊ0
    wndclass.cbWndExtra = 0;                                        // ����ռ䣬һ��Ϊ0
    wndclass.hInstance = hInstance;                                 // Ӧ�ó����ʵ�����
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);               // ���л��ڸô�����Ĵ����趨��ͼ��
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);                 // ���л��ڸô�����Ĵ����趨�����ָ��
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);   // ����
    wndclass.lpszMenuName = szMenuName;                             // �˵�
    wndclass.lpszClassName = szAppName;                             // ����������

    // Unicode�ַ���
    if (!RegisterClass(&wndclass))  // ʹ��IsWindowUnicode�жϴ����Ƿ�ΪUnicode����
    {
        MessageBox(NULL, TEXT("���������Ҫ��Windows NT�����У�"), szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow(
        szAppName,
        TEXT("Windows Test!"),
        WS_OVERLAPPEDWINDOW,  // WS_VSCROLL | WS_HSCROLL ��ֱ��ˮƽ������
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    hAccel = LoadAccelerators(hInstance, szMenuName);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(hwnd, hAccel, &msg))
        {
            TranslateMessage(&msg); // �����ַ���Ϣ��������ϢͶ�ݵ���Ϣ������
            DispatchMessage(&msg);  // ��
        }
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static DWORD dwCharSet = DEFAULT_CHARSET;
    static HWND hwndEdit;
    static HINSTANCE hInst;
    
    static int cxChar, cyChar;
    static int cxClient, cyClient;
    static int cxBuffer, cyBuffer;  // ����������Ϊ������棬������ܴ�Ŷ��ٸ��ַ�
    static int xCaret, yCaret;
    
    static TCHAR* pBuffer = NULL;
    static TCHAR szFileName[MAX_PATH];
    HDC hdc;
    int x, y, i;
    PAINTSTRUCT ps;
    TEXTMETRIC tm;

    switch (message)
    {
    case WM_CREATE:
        hInst = ((LPCREATESTRUCT)lParam) -> hInstance;
        hwndEdit = CreateWindow(
            TEXT("EDIT"),
            TEXT("**���Ǳ༭�� ����������������**"),
            WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
            0,
            10,
            1000,
            500,
            hwnd,
            (HMENU)EDITID,
            hInst,
            NULL
        );

        NotepadFileInitialize(hwnd);
        NotepadFontInitialize(hwndEdit);
        DoCaption(hwndEdit, szFileName);
        return 0;

        //����Ϊ�˱༭���ȡ������������Ϣ
    case WM_SETFOCUS:

        SetFocus(hwndEdit);
        return 0;

        //���ڵĴ�С�����仯�󣬻��յ������Ϣ��

    case WM_SIZE:
        MoveWindow(hwndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        return 0;

    case WM_CLOSE:      // ����Ϊ���ڻ�Ӧ�ó���Ӧ��ֹ���ź�
        if (MessageBox(hwnd, TEXT("�Ƿ��˳�"), TEXT("�˳�"), MB_YESNO) == IDYES)
            DestroyWindow(hwnd);
        else
            return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }// message END

    return DefWindowProc(hwnd, message, wParam, lParam);
}

int DoCaption(HWND hwnd, TCHAR* szTitleName)
{
    TCHAR szCaption[1 + MAX_PATH];
    wsprintf(szCaption, TEXT("%s - %s"), szAppName, szTitleName[0] ? szTitleName : UNTITLED);
    SetWindowText(hwnd, szCaption);
    return 0;
}