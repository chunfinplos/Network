#pragma once

#include "Mgr.h"
#include <enet\enet.h>


using namespace std;

class Mgr_Net_Server : public Mgr {
public:
    Mgr_Net_Server();
    ~Mgr_Net_Server();

    void Tick(float DeltaSeconds);

    int StartServer(int port);
    int SendPacket(string data);

private:
    ENetAddress _Address;
    ENetPeer* _Peer;

    ENetHost* _Server;
    ENetEvent _Event;
};