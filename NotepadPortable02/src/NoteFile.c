#include "NoteFile.h"

static OPENFILENAME ofn;

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

BOOL FileRead(HWND hwndEdit, PTSTR pstrFileName)
{
    BYTE   bySwap;
    DWORD  dwBytesRead;
    HANDLE hFile;
    int    i, iFileLength, iUniTest;
    PBYTE  pBuffer, pText, pConv;

    // Open the file.
    hFile = CreateFile(
        pstrFileName, 
        GENERIC_READ, 
        FILE_SHARE_READ, 
        NULL, 
        OPEN_EXISTING, 
        0, 
        NULL
    );
    if (INVALID_HANDLE_VALUE == hFile)
        return FALSE;

    // Get file size in bytes and allocate memory for read.
    // Add an extra two bytes for zero termination.
    iFileLength = GetFileSize(hFile, NULL);
    pBuffer = (PBYTE)malloc(iFileLength + 2);

    // �ƶ�ָ���ļ����ļ�ָ��
    //SetFilePointer(hFile, 1, NULL, FILE_BEGIN);  

    // Read file and input terminating zeros at end.
    ReadFile(hFile, pBuffer, iFileLength, &dwBytesRead, NULL);
    CloseHandle(hFile);
    pBuffer[iFileLength] = '\0';
    pBuffer[iFileLength + 1] = '\0';

    // Test to see if the text is unicode
    iUniTest = IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_REVERSE_SIGNATURE;
    if (IsTextUnicode(pBuffer, iFileLength, &iUniTest))
    {
        pText = pBuffer + 2;
        iFileLength -= 2;
        if (iUniTest & IS_TEXT_UNICODE_REVERSE_SIGNATURE)
        {
            for (i = 0; i < iFileLength / 2; i++)
            {
                bySwap = ((BYTE*)pText)[2 * i];
                ((BYTE*)pText)[2 * i] = ((BYTE*)pText)[2 * i + 1];
                ((BYTE*)pText)[2 * i + 1] = bySwap;
            }
        }

        // Allocate memory for possibly converted string
        pConv = (PBYTE)malloc(iFileLength + 2);

#ifndef UNICODE
        // If the edit control is not Unicode, convert Unicode text to 
        // non-Unicode (ie, in general, wide character).
        WideCharToMultiByte(CP_ACP, 0, (PWSTR)pText, -1, pConv,
            iFileLength + 2, NULL, NULL);
#else
        // If the edit control is Unicode, just copy the string
        lstrcpy((PTSTR)pConv, (PTSTR)pText);
#endif
    }
    else      // the file is not Unicode
    {
        pText = pBuffer;

        // Allocate memory for possibly converted string.
        pConv = (PBYTE)malloc(2 * iFileLength + 2);
        // If the edit control is Unicode, convert ASCII text.
#ifdef UNICODE
        // ���ַ���ӳ�䵽 UTF-16 (���ַ�) �ַ����� �ַ�����һ�����Զ��ֽ��ַ�����
        MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pText, -1, (PTSTR)pConv, iFileLength + 1);
#else
        // If not, just copy buffer
        lstrcpy((PTSTR)pConv, (PTSTR)pText);
#endif
    }
    SetWindowText(hwndEdit, (PTSTR)pConv);
    free(pBuffer);
    free(pConv);

    return TRUE;
}

BOOL MapFileRead(HWND hwndEdit, PTSTR pstrFileName)
{
    BYTE    bySwap;
    DWORD   dwBytesRead;
    DWORD   dwFileMapSize;              // ӳ���ļ���С   
    HANDLE  hFile;                      // �ļ����
    HANDLE  hMapFile;                   // �ļ�ӳ����
    LPVOID  lpAddr;                     // �ļ�ӳ�������ڴ�ָ��
    PDWORD   dwTextHand, dwTextTail;     // ���ļ�ָ��
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
    lpAddr = MapViewOfFile(hMapFile, FILE_MAP_COPY, 0, 0, 0);
    if (NULL == lpAddr)
        goto CLOSE;

    // ��ȡ�ļ���С
    dwFileMapSize = GetFileSize(hFile, NULL);

    // ���ļ�
    dwTextHand = (PDWORD)lpAddr;
    dwTextTail = ((PDWORD)lpAddr + 0x20);

    // Test to see if the text is unicode
    iUniTest = IS_TEXT_UNICODE_SIGNATURE | IS_TEXT_UNICODE_REVERSE_SIGNATURE;
    if (IsTextUnicode(lpAddr, dwFileMapSize, &iUniTest))
    {
        SetWindowText(hwndEdit, (PTSTR)dwTextHand);
    }
    else
    {
        pConv = (PBYTE)malloc(2 * dwFileMapSize + 2);
        // ���ַ���ӳ�䵽 UTF-16 (���ַ�) �ַ����� �ַ�����һ�����Զ��ֽ��ַ�����
        MultiByteToWideChar(CP_ACP, 0, (LPCSTR)dwTextHand, -1, (PTSTR)pConv, dwFileMapSize);
        SetWindowText(hwndEdit, (PTSTR)pConv);
    }
    // д�ļ�
    //*(PDWORD)lpAddr = 0xFFFFFFFF;

    // ǿ�Ƹ��»��棬Ӧ�ó������ļ�����ʱǿ��д���ļ�
    //FlushViewOfFile(((PDWORD)lpAddr), 4);

    // �ر���Դ
    UnmapViewOfFile(lpAddr);
    CloseHandle(hFile);
    CloseHandle(hMapFile);
    return TRUE;

CLOSE:
    CloseHandle(hFile);
    CloseHandle(hMapFile);
    UnmapViewOfFile(lpAddr);
    return FALSE;
}


BOOL FileWrite(HWND hwndEdit, PTSTR pstrFileName)
{
    DWORD  dwBytesWritten;
    HANDLE hFile;
    int    iLength;
    PTSTR  pstrBuffer;
    WORD   wByteOrderMark = 0xFEFF;

    // Open the file, creating it if necessary
    if (INVALID_HANDLE_VALUE == (hFile = CreateFile(pstrFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL)))
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

#ifdef UNICODE
    // If the edit control will return Unicode text, write the
    // byte order mark to the file.
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
