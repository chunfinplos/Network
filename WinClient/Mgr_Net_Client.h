#pragma once

#include "Mgr.h"
#include <enet\enet.h>


using namespace std;

class Mgr_Net_Client : public Mgr {
public:
    Mgr_Net_Client();
    ~Mgr_Net_Client();

    void Tick(float DeltaSeconds);

    bool ConnectToServer(string host, int port);
    int DisconnectFromServer();
    int SendPacket(string data);

private:
    ENetAddress _Address;
    ENetPeer* _Peer;

    ENetHost* _Client;
    ENetEvent _Event;
};