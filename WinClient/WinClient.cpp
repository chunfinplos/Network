#include "stdafx.h"
#include "WinClient.h"
#include "WinClientDlg.h"
#include "Mgr_Net_Client.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CWinClientApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


using namespace std;

CWinClientApp _App;
CWinClientDlg* _pDialog;
volatile int _State = 2;

/************************* COMMUNICATION BETWEEN THREADS *****************************************/

class ConcurrentQueue {
private:
    string _Data[10];
    int _Pos = -1;
    CCriticalSection _CritSect;

public:
    ConcurrentQueue() : _Pos(-1) {}
    ~ConcurrentQueue() {}

    void PutElem(string elem) {
        _CritSect.Lock();
        if (_Pos < 9) {
            _Pos++;
            _Data[_Pos] = elem;
        }
        _CritSect.Unlock();
    }
    int ReadElems(string buff[]) {
        _CritSect.Lock();
        int i = 0;
        if (_Pos >= 0) {
            for (i = 0; i <= _Pos; i++) buff[i] = _Data[i];
            _Pos = -1;
        }
        _CritSect.Unlock();
        return i;
    }
};

ConcurrentQueue _Queue;

/************************************** THREAD ***************************************************/
CWinThread* _Thread;

void PrintMessage(string msg);
void PrintMessage(string msg) {
    _pDialog->PrintMessage(msg);
}

void PostThread(int exitCode);
void PostThread(int exitCode) {
    if (exitCode == -1) {
        _pDialog->_bStart->EnableWindow(TRUE);
        _pDialog->_bStop->EnableWindow(FALSE);
        _pDialog->_bSend->EnableWindow(FALSE);
    }
}

UINT ThreadProc(LPVOID pParam);
UINT ThreadProc(LPVOID pParam) {
    int result = 0;
    string buffer[10];

    stParams* params = (stParams*)pParam;
    assert(params && "ERROR Bad Thread params");
    Mgr_Net_Client* _Mgr_Network = new Mgr_Net_Client();
    assert(_Mgr_Network && "ERROR Mgr_Network not valid");

    _Mgr_Network->_pLog = PrintMessage;
    if (_Mgr_Network->ConnectToServer(params->_IP, params->_Port)) {
        _State = 1;
        while (_State) {
            _Mgr_Network->Tick(0);
            int n = _Queue.ReadElems(buffer);
            for (int i = 0; i < n; i++) {
                _Mgr_Network->SendPacket(buffer[i]);
            }
        }
        _Mgr_Network->DisconnectFromServer();
        _State = 2;
    }
    else result = -1;

    delete params;
    delete _Mgr_Network;

    PostThread(result);

    return result;
}

/************************************** APP ******************************************************/
CWinClientApp::CWinClientApp() {
    // support Restart Manager
    m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

BOOL CWinClientApp::InitInstance() {
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    AfxEnableControlContainer();

    // Create the shell manager, in case the dialog contains
    // any shell tree view or shell list view controls.
    CShellManager *pShellManager = new CShellManager;

    // Activate "Windows Native" visual manager for enabling themes in MFC controls
    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
    SetRegistryKey(_T("Network Project - alexdev"));

    CWinClientDlg Dialog;
    m_pMainWnd = &Dialog;
    _pDialog = &Dialog;
    INT_PTR nResponse = Dialog.DoModal();
    if (nResponse == -1) {
        TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
        TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
    }
    
    StopNetworkThread();
    if (_Thread) ::WaitForSingleObject(_Thread->m_hThread, INFINITE);

    // Delete the shell manager created above.
    if (pShellManager != NULL) delete pShellManager;

    #ifndef _AFXDLL
    ControlBarCleanUp();
    #endif

    return FALSE;
}

void CWinClientApp::StartNetworkThread(string ip, int port) {
    stParams* params = new stParams(ip, port);
    _Thread = AfxBeginThread(ThreadProc, params);
}

void CWinClientApp::StopNetworkThread() {
    _State = 0;
}

void CWinClientApp::SendPacketThread(string data) {
    _Queue.PutElem(data);
}

void CWinClientApp::OnConnected() {
    //_pDialog->_bStart->EnableWindow(FALSE);
    _pDialog->_bStop->EnableWindow(TRUE);
    _pDialog->_bSend->EnableWindow(TRUE);
}

void CWinClientApp::OnDisconnected() {
    _pDialog->_bSend->EnableWindow(FALSE);
}