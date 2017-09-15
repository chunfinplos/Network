#include "stdafx.h"
#include "WinServer.h"
#include "WinServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;

extern CWinServerApp _App;

CWinServerDlg::CWinServerDlg(CWnd* pParent /*=NULL*/) : CDialogEx(IDD_WINSERVER_DIALOG, pParent) {
    _Hicon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWinServerDlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWinServerDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDBSTART, Click_bStart)
    ON_BN_CLICKED(IDBSTOP, Click_bStop)
    ON_BN_CLICKED(IDBSEND, Click_bSend)
    ON_BN_CLICKED(IDBCLEAR, Click_bClear)
    ON_WM_CLOSE()
END_MESSAGE_MAP()


BOOL CWinServerDlg::OnInitDialog() {
    CDialogEx::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(_Hicon, TRUE);
    SetIcon(_Hicon, FALSE);

    _bStart = (CButton*)GetDlgItem(IDBSTART);
    _bStop = (CButton*)GetDlgItem(IDBSTOP);
    _bSend = (CButton*)GetDlgItem(IDBSEND);

    _bStop->EnableWindow(FALSE);
    _bSend->EnableWindow(FALSE);

    _OutText = (CEdit*)GetDlgItem(IDTOUT);
    _PortText = (CEdit*)GetDlgItem(IDC_EDIT2);
    _PacketText = (CEdit*)GetDlgItem(IDC_EDIT3);

    _PortText->SetWindowTextW(L"2222");

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWinServerDlg::OnPaint() {
    if (IsIconic()) {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, _Hicon);
    }
    else {
        CDialogEx::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWinServerDlg::OnQueryDragIcon() {
    return static_cast<HCURSOR>(_Hicon);
}

void CWinServerDlg::Click_bStart() {
    CString port;
    _PortText->GetWindowText(port);
    CT2CA portConvertedAnsiString(port);

    if (!port.IsEmpty()) {
        _bStart->EnableWindow(FALSE);
        _bStop->EnableWindow(TRUE);
        _bSend->EnableWindow(FALSE);

        _App.StartNetworkThread("", stoi(string(portConvertedAnsiString)));
        PrintMessage("------SERVER STARTED------");
    }
    else ::MessageBox(NULL, (LPCWSTR)L"Wrong Port", NULL, MB_OK | MB_ICONERROR);
}

void CWinServerDlg::Click_bStop() {
    _App.StopNetworkThread();
    
    _bStart->EnableWindow(TRUE);
    _bStop->EnableWindow(FALSE);
    _bSend->EnableWindow(FALSE);
    PrintMessage("------SERVER STOPPED------");
}

void CWinServerDlg::Click_bSend() {
    CString data;
    _PacketText->GetWindowText(data);
    CT2CA dataConvertedAnsiString(data);

    if (!data.IsEmpty()) {
        _App.SendPacketThread(string(dataConvertedAnsiString));
    }
    else ::MessageBox(NULL, (LPCWSTR)L"Empty DATA", NULL, MB_OK | MB_ICONERROR);
}

void CWinServerDlg::Click_bClear() {
    _OutText->SetWindowText(L"");
}

void CWinServerDlg::PrintMessage(string msg) {
    int end = _OutText->GetWindowTextLength();
    _OutText->SetSel(end, end);
    msg += "\n";
    _OutText->ReplaceSel(CString(msg.c_str()));
}