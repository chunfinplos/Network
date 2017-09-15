#include "stdafx.h"
#include "WinServer.h"
#include "WinServerDlg.h"
#include "Mgr_Net_Server.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CWinServerApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


using namespace std;

CWinServerApp _App;
CWinServerDlg* _pDialog;
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

UINT ThreadProc(LPVOID pParam);
UINT ThreadProc(LPVOID pParam) {
    string buffer[10];

    stParams* params = (stParams*)pParam;
    assert(params && "ERROR Bad Thread params");
    Mgr_Net_Server* _Mgr_Network = new Mgr_Net_Server();
    assert(_Mgr_Network && "ERROR Mgr_Network not valid");

    _Mgr_Network->_pLog = PrintMessage;
    _Mgr_Network->StartServer(params->_Port);
    
    _State = 1;
    while (_State) {
        _Mgr_Network->Tick(0);
        int n = _Queue.ReadElems(buffer);
        for (int i = 0; i < n; i++) {
            _Mgr_Network->SendPacket(buffer[i]);
        }
    }
    delete params;
    delete _Mgr_Network;

    _State = 2;
    return 0;
}

/************************************** APP ******************************************************/
CWinServerApp::CWinServerApp() {
    // support Restart Manager
    m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

BOOL CWinServerApp::InitInstance() {
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

    CWinServerDlg Dialog;
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

void CWinServerApp::StartNetworkThread(string ip, int port) {
    stParams* params = new stParams(ip, port);
    _Thread = AfxBeginThread(ThreadProc, params);
}

void CWinServerApp::StopNetworkThread() {
    _State = 0;
}

void CWinServerApp::SendPacketThread(string data) {
    _Queue.PutElem(data);
}

void CWinServerApp::OnConnected() {
    _pDialog->_bSend->EnableWindow(TRUE);
}

void CWinServerApp::OnDisconnected() {
    _pDialog->_bSend->EnableWindow(FALSE);
}