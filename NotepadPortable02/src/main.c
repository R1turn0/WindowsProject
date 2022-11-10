#include <windows.h>
#include <strsafe.h>
#include <string.h>
#include "resource.h"
#include "NoteFile.h"
#include "NoteFont.h"
#include "NotePrint.h"

#define CXCREAT 0
#define CYCREAT 0
#define NUMLINES 100
#define EDITID   1
#define UNHEADER TEXT("New File")
#define BUFFER(x, y) *(pBuffer + y * cxBuffer + x)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int AskAboutSave(HWND, TCHAR*);
BOOL DestroyOpen(HWND, TCHAR*, TCHAR*);

static TCHAR szAppName[] = TEXT("Notepad#");
static TCHAR szMenuName[] = TEXT("NotepadMenu");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szFileName[MAX_PATH];
    HWND hwnd;  // ���ھ��
    MSG msg;
    WNDCLASS wndclass = { 0 };
    HACCEL hAccel = NULL;
    HMENU hMenu;     // �˵��ľ����

    wndclass.style = CS_HREDRAW | CS_VREDRAW;                           // ������ȸ߶����»��ƴ���
    wndclass.lpfnWndProc = WndProc;                                     // ָ�����ڹ��̣������ǻص�����
    wndclass.cbClsExtra = 0;                                            // ����ռ䣬һ��Ϊ0
    wndclass.cbWndExtra = 0;                                            // ����ռ䣬һ��Ϊ0
    wndclass.hInstance = hInstance;                                     // Ӧ�ó����ʵ�����
    wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));    // ���л��ڸô�����Ĵ����趨��ͼ��
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);                     // ���л��ڸô�����Ĵ����趨�����ָ��
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);       // ����
    wndclass.lpszMenuName = szMenuName;                                 // �˵�
    wndclass.lpszClassName = szAppName;                                 // ����������

    // Unicode�ַ���
    if (!RegisterClass(&wndclass))  // ʹ��IsWindowUnicode�жϴ����Ƿ�ΪUnicode����
    {
        MessageBox(NULL, TEXT("���������Ҫ��Windows NT�����У�"), szAppName, MB_ICONERROR);
        return 0;
    }

    // ����Ӧ�ó���ʵ�������Ŀ�ִ�� (.exe) �ļ��м���ָ���Ĳ˵���Դ��
    hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));    
    hwnd = CreateWindow(
        szAppName,
        TEXT("Windows Test!"),
        WS_OVERLAPPEDWINDOW,  // WS_VSCROLL | WS_HSCROLL ��ֱ��ˮƽ������
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        hMenu,
        hInstance,
        NULL
    );

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    // ����ָ���ļ�����������ݼ�
    hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));   
    
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
    static HINSTANCE hInst = NULL;
   
    static int cxBuffer, cyBuffer;      // ����������Ϊ������棬������ܴ�Ŷ��ٸ��ַ�
    static int cxIcon, cyIcon;          // ͼ��Ĵ�С
    static TCHAR* pBuffer = NULL;       // ������ָ��
    static TCHAR szFileName[MAX_PATH];  // ��·����Ϣ���ļ���
    static TCHAR szTitleName[MAX_PATH]; // ��Ϊ�ļ���
    static TCHAR szBuffer[MAX_PATH];    // �����ı�������

    HDC hdc;
    HMENU hMenu;
    int x, y, i;
    int id;
    PAINTSTRUCT ps;
    TEXTMETRIC tm;
    static BOOL isSave = TRUE;
    static int szlParam = 0;
    //HINSTANCE hInstance;    // ʵ���ľ���� �����ڴ���ģ��Ļ�ַ��

    switch (message)
    {
    case WM_CREATE:
        if (hInst == NULL)
            hInst = ((LPCREATESTRUCT)lParam) -> hInstance;
        cxIcon = GetSystemMetrics(SM_CXICON);
        cyIcon = GetSystemMetrics(SM_CXICON);
        FileInit(hwnd);
        FontInit(hwndEdit);
        DoCaption(hwnd, szFileName);
        hwndEdit = CreateWindow(
            TEXT("EDIT"),
            TEXT("New File."),
            WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER | ES_LEFT | ES_MULTILINE | ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
            CXCREAT,
            CYCREAT,
            LOWORD(szlParam), // ��Ϊȡ��lParamΪ00 00 00 00�������·���WM_SIZE���ȡ��lParam��so�������»��ƴ���
            HIWORD(szlParam),
            hwnd,
            (HMENU)EDITID,
            hInst,
            NULL
        ); 
        id = GetWindowLong(hwndEdit, GWL_ID);
        SendMessage(hwndEdit, EM_LIMITTEXT, 32000, 0L);
        return 0;

    case WM_SIZE:
        szlParam = lParam;
        MoveWindow(hwndEdit, 0, 0, LOWORD(szlParam), HIWORD(szlParam), TRUE);   // ����ָ�����ڵ�λ�úͳߴ硣
        //UpdateWindow(hwnd);
        //InvalidateRect(hwnd, NULL, TRUE);
        //RedrawWindow(hwnd, );
        return 0;

    case WM_SETFOCUS:
        SetFocus(hwndEdit);
        return 0;

    case WM_COMMAND:
        hMenu = GetMenu(hwnd);
        if (lParam && LOWORD(wParam) == EDITID)
        {
            switch (HIWORD(wParam))
            {
            case EN_UPDATE:
                isSave = FALSE;
                break;
            case EN_ERRSPACE:   // ���༭�ؼ��޷������㹻���ڴ��������ض�����ʱ��������EN_ERRSPACE֪ͨ��Ϣ
            case EN_MAXTEXT:    // ��ǰ�ı����볬���༭�ؼ���ָ���ַ���ʱ��������EN_MAXTEXT֪ͨ��Ϣ���ı������ѱ��ضϡ�
                MessageBox(hwnd, TEXT("�༭�ؼ��ڴ治�㡣"),
                    szAppName, MB_OK | MB_ICONSTOP);
                break;
            }
            break;
            return 0;
        }

        switch (LOWORD(wParam))
        {
        case IDM_FILE_NEW:
            DestroyOpen(hwndEdit, szFileName, szTitleName);
            SendMessage(hwnd, WM_CREATE, 0, NULL);
            SendMessage(hwnd, WM_SETFOCUS, 0, NULL);
            break;

        case IDM_FILE_OPEN:
            if (!isSave && IDCANCEL == AskAboutSave(hwnd, szFileName))
                break;
            DestroyOpen(hwndEdit, szFileName, szTitleName);
            SendMessage(hwnd, WM_CREATE, 0, NULL);
            if (FileOpenDlg(hwnd, szFileName, szTitleName))
            {
                if (FALSE == MapFileRead(hwndEdit, szTitleName))
                {
                    StringCchPrintf(szBuffer, MAX_PATH, TEXT("�޷���ȡ�ļ�"), szTitleName[0] ? szTitleName : UNHEADER);
                    MessageBox(hwnd, szBuffer, szAppName, MB_OK | MB_ICONEXCLAMATION);
                    szFileName[0] = '\0';
                    szTitleName[0] = '\0';
                }
            }
            DoCaption(hwnd, szTitleName);
            isSave = TRUE;
            break;

        case IDM_FILE_SAVE:
            if (FileWrite(hwndEdit, szFileName))
            {
                isSave = TRUE;
                break;
            }
            // �˴�����Ҫbreak;
            
        case IDM_FILE_SAVEAS:
            if (FileSaveDlg(hwnd, szFileName, szTitleName))
            {
                DoCaption(hwnd, szTitleName);
                if (FileWrite(hwndEdit, szFileName))
                {
                    isSave = TRUE;
                    break;
                }
                else
                {
                    StringCchPrintf(szBuffer, MAX_PATH, TEXT("�޷����Ϊ..."), szTitleName[0] ? szTitleName : UNHEADER);
                    MessageBox(hwnd, szBuffer, szAppName, MB_OK | MB_ICONEXCLAMATION);
                    break;
                }
            }
            break;
        case IDM_FILE_PRINT:
            if (!PrintFile(hInst, hwnd, hwndEdit, szTitleName))
            {
                StringCchPrintf(szBuffer, MAX_PATH, TEXT("��ӡʧ��"), szTitleName[0] ? szTitleName : UNHEADER);
                MessageBox(hwnd, szBuffer, szAppName, MB_OK | MB_ICONEXCLAMATION);
            }
            break;

        case IDM_FILE_CLOSE:
            if (isSave)
                DestroyOpen(hwndEdit, szFileName, szTitleName);
            else if (AskAboutSave(hwndEdit, szFileName))
                DestroyOpen(hwndEdit, szFileName, szTitleName);
            break;

        case IDM_EXIT:
            SendMessage(hwnd, WM_CLOSE, NULL, 0);
            break;

        case ID_FORMAT_FONT:
            if (FontChooseFont(hwnd))
                FontSetFont(hwndEdit);
            break;

        default:
            break;
        }
        return 0;

    case WM_CLOSE:      // ����Ϊ���ڻ�Ӧ�ó���Ӧ��ֹ���ź�
        if (isSave)
            DestroyOpen(hwndEdit, szFileName, szTitleName);
        else if (AskAboutSave(hwndEdit, szFileName))
            DestroyOpen(hwndEdit, szFileName, szTitleName);
        if (MessageBox(hwnd, TEXT("�Ƿ��˳�"), TEXT("�˳�"), MB_YESNO) == IDYES)
            DestroyOpen(hwndEdit, szFileName, szTitleName);
        else
            return 0;

    case WM_DESTROY:
        FontDeinitialize();
        PostQuitMessage(0);
        return 0;
    }// message END

    return DefWindowProc(hwnd, message, wParam, lParam);
}

int DoCaption(HWND hwnd, TCHAR* szFileName)
{
    TCHAR szCaption[1 + MAX_PATH];
    wsprintf(szCaption, TEXT("%s - %s"), szAppName, szFileName[0] ? szFileName : UNHEADER);
    SetWindowText(hwnd, szCaption);
    return 0;
}

BOOL AskAboutSave(HWND hwnd, TCHAR* szFileName)
{
    TCHAR szBuffer[MAX_PATH + 64];
    int ret;

    StringCchPrintf(szBuffer, MAX_PATH, TEXT("�Ƿ񱣴浱ǰ�ļ�%s?"), szFileName[0] ? szFileName : UNHEADER);
    ret = MessageBox(hwnd, szBuffer, szFileName, MB_YESNOCANCEL | MB_ICONQUESTION);
    if (ret == IDCANCEL)
    {
        ret = FALSE;
    }
    else
    {
        if(IDYES == ret)
            SendMessage(hwnd, WM_COMMAND, IDM_FILE_SAVE, 0);
        ret = TRUE;
    }
    return ret;
}

BOOL DestroyOpen(HWND hwnd, TCHAR *szFileName, TCHAR *szTitleName)
{
    DestroyWindow(hwnd);
    memset(szFileName, 0, sizeof(*szFileName));
    memset(szTitleName, 0, sizeof(*szTitleName));
    return TRUE;
}


/*
Creating a File View
https://learn.microsoft.com/en-us/windows/win32/memory/creating-a-file-view

MapViewOfFile function (memoryapi.h)
https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-mapviewoffile

*/