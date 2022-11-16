#include "NoteFile.h"

static OPENFILENAME ofn;

static LPVOID  lpAddr;                     // �ļ�ӳ�������ڴ�ָ��

DWORD FileInit(HWND hwnd)
{
    static TCHAR szFilter[] = TEXT("Text Files (*.TXT)\0*.txt\0")  \
        TEXT("ASCII Files (*.ASC)\0*.asc\0") \
        TEXT("All Files (*.*)\0*.*\0\0");

    // OPENFILENAME
    ofn.lStructSize = sizeof(OPENFILENAME); // �ṹ�ĳ��ȣ����ֽ�Ϊ��λ��
    ofn.hwndOwner = hwnd;                   // ӵ�жԻ���Ĵ��ڵľ��
    ofn.hInstance = NULL;                   // �����Ի���ģ����ڴ����ľ�� ���� lpTemplateName ��Ա�����ĶԻ���ģ���ģ��ľ��
    ofn.lpstrFilter = szFilter;             // ������ null ��β��ɸѡ���ַ����ԵĻ�����
    ofn.lpstrCustomFilter = NULL;           // һ����̬�����������а���һ���� null ��β��ɸѡ���ַ��������ڱ����û�ѡ���ɸѡ��ģʽ
    ofn.nMaxCustFilter = 0;                 // �� lpstrCustomFilter ��ʶ�Ļ������Ĵ�С�����ַ�Ϊ��λ��
    ofn.nFilterIndex = 0;                   // �ļ����Ϳؼ��е�ǰ��ѡɸѡ��������
    ofn.lpstrFile = NULL;                   // ���ڳ�ʼ���ļ����༭�ؼ��� �ļ���
    ofn.nMaxFile = MAX_PATH;                // lpstrFile ָ��Ļ������Ĵ�С�����ַ�Ϊ��λ��
    ofn.lpstrFileTitle = NULL;              // �ļ�������չ����û����ѡ�ļ���·����Ϣ��
    ofn.nMaxFileTitle = MAX_PATH;           // lpstrFileTitle ָ��Ļ������Ĵ�С�����ַ�Ϊ��λ��
    ofn.lpstrInitialDir = NULL;             // ��ʼĿ¼
    ofn.lpstrTitle = NULL;                  // Ҫ�����ڶԻ���������е��ַ���
    ofn.Flags = 0;                          // һ������ڳ�ʼ���Ի����λ��־
    ofn.nFileOffset = 0;                    // ��·���Ŀ�ͷ�� �� lpstrFile ָ����ַ������ļ����Ĵ��㿪ʼ��ƫ���������ַ�Ϊ��λ��
    ofn.nFileExtension = 0;                 // ��·���Ŀ�ͷ���ַ������� lpstrFile ָ����ļ���չ���Ĵ��㿪ʼ��ƫ���������ַ�Ϊ��λ��
    ofn.lpstrDefExt = TEXT("txt");          // Ĭ����չ
    ofn.lCustData = 0L;                     // ϵͳ���ݸ� lpfnHook ��Ա��ʶ�Ĺҹ����̵�Ӧ�ó���������
    ofn.lpfnHook = NULL;                    // ָ��ҹ����̵�ָ��
    ofn.lpTemplateName = NULL;              // hInstance ��Ա��ʶ��ģ���еĶԻ���ģ����Դ������

    return 0;
}

//���Open���ļ�
BOOL FileOpenDlg(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = pstrFileName;
    ofn.lpstrFileTitle = pstrTitleName;
    ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;

    return GetOpenFileName(&ofn);
}

// Save File
BOOL FileSaveDlg(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = pstrFileName;
    ofn.lpstrFileTitle = pstrTitleName;
    ofn.Flags = OFN_OVERWRITEPROMPT;

    return GetSaveFileName(&ofn);
}

BOOL MapFileRead(HWND hwndEdit, PTSTR pstrFileName)
{
    BYTE    bySwap;
    DWORD   dwBytesRead;
    DWORD   dwFileMapSize;              // ӳ���ļ���С   
    HANDLE  hFile;                      // �ļ����
    HANDLE  hMapFile;                   // �ļ�ӳ����
    //LPVOID  lpAddr;                     // �ļ�ӳ�������ڴ�ָ��
    PBYTE   dwTextHand, dwTextTail;     // ���ļ�ָ��
    int     i, iFileLength, iUniTest;
    PBYTE   pBuffer, pText, pConv;

    // �����ļ����
    hFile = CreateFile(
        pstrFileName,                           // Ҫ������򿪵��ļ����豸�����ơ�
        GENERIC_READ | GENERIC_WRITE,           // ��������ļ����豸�����Ի���Ϊ��ȡ��д��� 0����ָʾ���߶�û��)
        FILE_SHARE_READ | FILE_SHARE_WRITE,     // �ļ����豸��������ģʽ�����Զ�ȡ��д�롢ɾ��������Щ���� (�����±�) �� �����Ի���չ���Եķ��������ܴ˱�־��Ӱ�졣
        NULL,                                   // ָ��һ�� SECURITY_ATTRIBUTES �ṹ��ָ�룬�ýṹ����������������ص����ݳ�Ա����ѡ�İ�ȫ���������Լ�һ������ֵ����ֵȷ�����صľ���Ƿ�������ӽ��̼̳С�
        OPEN_EXISTING,                          // �Դ��ڻ򲻴��ڵ��ļ����豸ִ�еĲ�����
        0,                                      // �ļ����豸���Ժͱ�־�� FILE_ATTRIBUTE_NORMAL ���ļ������Ĭ��ֵ��
        NULL                                    // ���� GENERIC_READ ����Ȩ�޵�ģ���ļ�����Ч����� ģ���ļ�Ϊ���ڴ������ļ��ṩ�ļ����Ժ���չ���ԡ�
    );
    if (INVALID_HANDLE_VALUE == hFile)
        goto CLOSE;

    // �����ļ�ӳ����
    hMapFile = CreateFileMapping(
        hFile,              // �����ļ�ӳ�����ľ��
        NULL,               // ָ�� SECURITY_ATTRIBUTES �ṹ��ָ�룬�ýṹȷ�����صľ���Ƿ�������ӽ��̼̳�
        PAGE_READWRITE,     // ����Ϊֻ����д��ʱ���ƻ��ȡ/д�����ӳ����ͼ
        0,                  // �ļ�ӳ����������С�ĸ߽� DWORD
        0,                  // �ļ�ӳ���������С�ĵ��� DWORD
        pstrFileName        // �ļ�ӳ���������ơ�
    );
    if (NULL == hMapFile)
        goto CLOSE;

    // ���ļ�ӳ�䵽�����ڴ��У������ڴ�ָ��
    // FILE_MAP_COPY
    // FILE_MAP_ALL_ACCESS
    lpAddr = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (NULL == lpAddr)
        goto CLOSE;

    // ��ȡ�ļ���С
    dwFileMapSize = GetFileSize(hFile, NULL);
    // ���ļ�
    dwTextHand = (PBYTE)lpAddr;
    dwTextTail = ((PBYTE)lpAddr + 0x20);

    // Test to see if the text is unicode
    // UTF - 8������ı��ĵ����еĴ���BOM(Byte Order Mark, �ֽ����־)����0xEF, 0xBB, 0xBF
    iUniTest = IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_REVERSE_SIGNATURE;
    if (IsTextUnicode(lpAddr, dwFileMapSize, &iUniTest))
    {
        SetWindowText(hwndEdit, (PTSTR)dwTextHand);
    }
    //else if (IsDBCSLeadByte(*dwTextHand))
    //else if (ANSI_CHARSET == GetTextCharset(lpAddr))
    else if (is_str_gbk(dwTextHand))
    {
        pConv = (PBYTE)malloc(dwFileMapSize * 2 + 2);
        MultiByteToWideChar(CP_ACP, 0, (LPCSTR)dwTextHand, -1, (PTSTR)pConv, dwFileMapSize);
        SetWindowText(hwndEdit, (PTSTR)pConv);
        free(pConv);
    }
    else
    {
        pConv = (PBYTE)malloc(dwFileMapSize * 2 + 2);
        // ���ַ���ӳ�䵽 UTF-16 (���ַ�) �ַ����� �ַ�����һ�����Զ��ֽ��ַ�����
        MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)dwTextHand, -1, (PTSTR)pConv, dwFileMapSize);
        SetWindowText(hwndEdit, (PTSTR)pConv);
        free(pConv);
    }

    // д�ļ�
    //*dwTextHand = 0x41414141;

    // ǿ�Ƹ��»��棬Ӧ�ó������ļ�����ʱǿ��д���ļ�
    //FlushViewOfFile(dwTextHand, 0);

    // �ر���Դ
    CloseHandle(hFile);
    CloseHandle(hMapFile);
    UnmapViewOfFile(lpAddr);
    return TRUE;

CLOSE:
    CloseHandle(hFile);
    CloseHandle(hMapFile);
    UnmapViewOfFile(lpAddr);
    return FALSE;
}

BOOL MapFileWrite(HWND hwndEdit, PTSTR pstrFileName)
{
    PDWORD   dwTextHand, dwTextTail;     // ���ļ�ָ��

    dwTextHand = (PDWORD)lpAddr;

    // ���ļ�ӳ����ͼ�ڵ��ֽڷ�Χд�����
    FlushViewOfFile(dwTextHand, 0);

    return TRUE;
}

BOOL FileWrite(HWND hwndEdit, PTSTR pstrFileName)
{
    DWORD  dwBytesWritten;
    HANDLE hFile;
    int    iLength;
    PTSTR  pstrBuffer;
    WORD   wByteOrderMark = 0xFEFF;

    // Open the file, creating it if necessary
    if (INVALID_HANDLE_VALUE ==
        (hFile = CreateFile(pstrFileName, GENERIC_WRITE, 0,
            NULL, CREATE_ALWAYS, 0, NULL)))
        return FALSE;

    // Get the number of characters in the edit control and allocate
    // memory for them.
    iLength = GetWindowTextLength(hwndEdit);
    pstrBuffer = (PTSTR)malloc((iLength + 1) * sizeof(TCHAR));

    if (!pstrBuffer)
    {
        CloseHandle(hFile);
        return FALSE;
    }

    // If the edit control will return Unicode text, write the
    // byte order mark to the file.
#ifdef UNICODE
    WriteFile(hFile, &wByteOrderMark, 2, &dwBytesWritten, NULL);
#endif

    // Get the edit buffer and write that out to the file.
    GetWindowText(hwndEdit, pstrBuffer, iLength + 1);
    WriteFile(hFile, pstrBuffer, iLength * sizeof(TCHAR),
        &dwBytesWritten, NULL);

    if ((iLength * sizeof(TCHAR)) != (int)dwBytesWritten)
    {
        CloseHandle(hFile);
        free(pstrBuffer);
        return FALSE;
    }

    CloseHandle(hFile);
    free(pstrBuffer);

    return TRUE;
}

BOOL is_str_gbk(char* str)
{
    unsigned int nBytes = 0;    // GBK��1-2���ֽڱ���,�������� ,Ӣ��һ��
    unsigned char chr = *str;
    unsigned int i = 0;
    BOOL bAllAscii = TRUE; //���ȫ������ASCII,

    // GBKʹ��˫�ֽڱ��룬��һ�ֽ�0xA1-0xF7, �ڶ��ֽ�0xA1-0xFE
    for (i = 0; str[i] != '\0'; ++i) {
        chr = str[i];
        if ((chr & 0x80) != 0 && nBytes == 0) { // �ж��Ƿ�ASCII����,�������,˵���п�����GBK
            bAllAscii = FALSE;
        }
        if (nBytes == 0) {
            if (chr >= 0x80) {
                if (chr >= 0xA1 && chr <= 0xF7) {
                    nBytes = 2;
                }
                else {
                    return FALSE;
                }
                nBytes--;
            }
        }
        else {
            if (chr < 0xA1 || chr > 0xFE) {
                return FALSE;
            }
            nBytes--;
        }//else end
    }
    if (nBytes != 0) {   //Υ������
        return FALSE;
    }
    if (bAllAscii) { //���ȫ������ASCII, Ҳ��GBK
        return TRUE;
    }
    return TRUE;
}


////gb2312 to unicode
//int wLen = MultiByteToWideChar(CP_ACP, 0, lpszText, -1, NULL, 0);
//LPWSTR wStr = new WCHAR[wLen];
//MultiByteToWideChar(CP_ACP, 0, lpszText, -1, wStr, wLen);

////unicode to utf8
//int aLen = WideCharToMultiByte(CP_UTF8, 0, wStr, -1, NULL, 0, NULL, NULL);
//char* converted = new char[aLen];
//WideCharToMultiByte(CP_UTF8, 0, wStr, -1, converted, aLen, NULL, NULL);


////utf8 to unicode
//int wLen2 = MultiByteToWideChar(CP_UTF8, 0, converted, -1, NULL, 0);
//LPWSTR wStr2 = new WCHAR[wLen2];
//MultiByteToWideChar(CP_UTF8, 0, converted, -1, wStr2, wLen2);


////unicode to gb2312
//int aLen2 = WideCharToMultiByte(CP_ACP, 0, wStr2, -1, NULL, 0, NULL, NULL);
//char* converted2 = new char[aLen2];
//WideCharToMultiByte(CP_ACP, 0, wStr, -1, converted2, aLen2, NULL, NULL);
