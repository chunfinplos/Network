#include "stdafx.h"
#include "WinClient.h"
#include "WinClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;

extern CWinClientApp _App;

CWinClientDlg::CWinClientDlg(CWnd* pParent /*=NULL*/) : CDialogEx(IDD_WINCLIENT_DIALOG, pParent) {
    _Hicon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWinClientDlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWinClientDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDBCONNECT, Click_bConnectToServer)
    ON_BN_CLICKED(IDBDISCONNECT, Click_bDisconnect)
    ON_BN_CLICKED(IDBSEND, Click_bSend)
    ON_BN_CLICKED(IDBCLEAR, Click_bClear)
    ON_WM_CLOSE()
    ON_WM_CLOSE()
END_MESSAGE_MAP()


BOOL CWinClientDlg::OnInitDialog() {
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
    _IPText = (CEdit*)GetDlgItem(IDC_IPADDRESS1);
    _PortText = (CEdit*)GetDlgItem(IDC_EDIT1);
    _PacketText = (CEdit*)GetDlgItem(IDC_EDIT2);

    _IPText->SetWindowTextW(L"127.0.0.1");
    _PortText->SetWindowTextW(L"2222");

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWinClientDlg::OnPaint() {
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
HCURSOR CWinClientDlg::OnQueryDragIcon() {
    return static_cast<HCURSOR>(_Hicon);
}

void CWinClientDlg::Click_bConnectToServer() {
    CString ip;
    _IPText->GetWindowText(ip);
    CT2CA ipConvertedAnsiString(ip);
    CString port;
    _PortText->GetWindowText(port);
    CT2CA portConvertedAnsiString(port);

    if (!port.IsEmpty()) {
        PrintMessage("CONNECTING TO SERVER.........");

        _bStart->EnableWindow(FALSE);
        _bStop->EnableWindow(FALSE);
        _bSend->EnableWindow(FALSE);

        _App.StartNetworkThread(string(ipConvertedAnsiString),
                                stoi(string(portConvertedAnsiString)));
    }
    else ::MessageBox(NULL, (LPCWSTR)L"Wrong Port", NULL, MB_OK | MB_ICONERROR);
}

void CWinClientDlg::Click_bDisconnect() {
    PrintMessage("DISCONNECTING.........");
    _App.StopNetworkThread();

    _bStart->EnableWindow(TRUE);
    _bStop->EnableWindow(FALSE);
    _bSend->EnableWindow(FALSE);
}

void CWinClientDlg::Click_bSend() {
    CString data;
    _PacketText->GetWindowText(data);
    CT2CA dataConvertedAnsiString(data);

    if (!data.IsEmpty()) {
        _App.SendPacketThread(string(dataConvertedAnsiString));
    }
    else ::MessageBox(NULL, (LPCWSTR)L"Empty DATA", NULL, MB_OK | MB_ICONERROR);
}

void CWinClientDlg::Click_bClear() {
    _OutText->SetWindowText(L"");
}

void CWinClientDlg::PrintMessage(string msg) {
    if (_OutText) {
        int end = _OutText->GetWindowTextLength();
        _OutText->SetSel(end, end);
        msg += "\n";
        _OutText->ReplaceSel(CString(msg.c_str()));
    }
}