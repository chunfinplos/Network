#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"

using namespace std;

class CWinClientApp : public CWinApp {
public:
    CWinClientApp();

    void StartNetworkThread(string ip, int port);
    void StopNetworkThread();
    void SendPacketThread(string data);

    void OnConnected();
    void OnDisconnected();

    virtual BOOL InitInstance();
    DECLARE_MESSAGE_MAP()
};

extern CWinClientApp _App;