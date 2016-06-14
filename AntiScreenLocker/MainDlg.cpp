// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"

CMainDlg::CMainDlg()
    : m_hMouseMoveThread(NULL)
    , m_hTerminateEvent(NULL)
{
}

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
    return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
    UIUpdateChildWindows();
    return FALSE;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    // center the dialog on the screen
    CenterWindow();

    // set icons
    HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
    SetIcon(hIcon, TRUE);
    HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
    SetIcon(hIconSmall, FALSE);

    // register object for message filtering and idle updates
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);
    pLoop->AddIdleHandler(this);

    UIAddChildWindowContainer(m_hWnd);

    // Install the tray icon
    WTL::CString toolTip;
    toolTip.LoadString(IDR_MAINFRAME);
    InstallIcon(toolTip, hIconSmall, IDR_POPUP);

    // Start mouse moving thread
    // uncomment if it need
    //StartMouseDancing();

    return TRUE;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    StopMouseDancing();

    // unregister message filtering and idle updates
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->RemoveMessageFilter(this);
    pLoop->RemoveIdleHandler(this);

    return 0;
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CAboutDlg dlg;
    dlg.DoModal();
    return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // TODO: Add validation code 
    //CloseDialog(wID);
    return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    //CloseDialog(wID);
    ShowWindow(SW_HIDE);
    return 0;
}

LRESULT CMainDlg::OnAppExit(WORD, WORD, HWND, BOOL &)
{
    CloseDialog(IDOK);
    return LRESULT();
}

void CMainDlg::CloseDialog(int nVal)
{
    DestroyWindow();
    ::PostQuitMessage(nVal);
}

void CMainDlg::StartMouseDancing()
{
    if (NULL != m_hMouseMoveThread)
    {
        StopMouseDancing();
    }
    m_hTerminateEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
    m_hMouseMoveThread = ::CreateThread(NULL, 0, MouseDancingThreadProc, this, 0, NULL);
}

void CMainDlg::StopMouseDancing()
{
    if (NULL != m_hMouseMoveThread)
    {
        ::SetEvent(m_hTerminateEvent);
        ::WaitForSingleObject(m_hMouseMoveThread, INFINITE);
        ::CloseHandle(m_hMouseMoveThread);
        m_hMouseMoveThread = NULL;
        ::CloseHandle(m_hTerminateEvent);
        m_hTerminateEvent = NULL;
    }
    
}

DWORD CMainDlg::MouseDancingThreadProc(LPVOID pData)
{
    auto *pThis = reinterpret_cast<CMainDlg *>(pData);

    int nScreenSaverTimeout = 0; // in seconds
    ::SystemParametersInfo(SPI_GETSCREENSAVETIMEOUT, 0, &nScreenSaverTimeout, 0);

    int nWaitTimeout = nScreenSaverTimeout * 1000 - 500;
    if (0 > nWaitTimeout)
    {
        // strange case
        return 1;
    }
    bool moveDown = true;
    while (WAIT_TIMEOUT == ::WaitForSingleObject(pThis->m_hTerminateEvent, nWaitTimeout))
    {
        INPUT input;
        input.type = INPUT_MOUSE;
        input.mi.dx = moveDown?1:-1;
        input.mi.dy = moveDown?1:-1;
        input.mi.mouseData = 0;
        input.mi.dwFlags = MOUSEEVENTF_MOVE;
        input.mi.time = 0;
        input.mi.dwExtraInfo = 0;
        ::SendInput(1, &input, sizeof(input));
        moveDown = !moveDown;
    }

    return 0;
}
