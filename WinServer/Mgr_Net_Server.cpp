#include "stdafx.h"
#include "Mgr_Net_Server.h"
#include "WinServer.h"


using namespace std;

extern CWinServerApp _App;

Mgr_Net_Server::Mgr_Net_Server() {
    _Address.host = ENET_HOST_ANY;
    _Address.port = -1;
    _Server = nullptr;
    _Peer = nullptr;
}

Mgr_Net_Server::~Mgr_Net_Server() {
    enet_host_destroy(_Server);
    enet_deinitialize();
}

void Mgr_Net_Server::Tick(float DeltaSeconds) {
    if (enet_host_service(_Server, &_Event, 1000) > 0) {
        ostringstream ss;
        switch (_Event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                char hostName[50];
                enet_address_get_host_ip(&_Event.peer->address, hostName, 50);
                if (!_Peer) {
                    _Peer = _Event.peer;
                    ss << "CONNECTION ACCEPTED FROM: " << hostName << ":" << _Event.peer->address.port;

                    /* Store any relevant client information here. */
                    _Event.peer->data = "Client information";
                    SendPacket("Init Packet");
                    _App.OnConnected();
                }
                else {
                    ss << "CONNECTION REFUSED FROM: " << hostName << ":" << _Event.peer->address.port;
                    enet_peer_disconnect_now(_Event.peer, 0);
                }
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                ss << "PACKET: Size(" << _Event.packet->dataLength << "), Data["
                   << _Event.packet->data << "], Peer(" << (char*)_Event.peer->data << "), Channel: "
                   << (unsigned int)_Event.channelID;
                
                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy(_Event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                ss << "DISCONNECTED: " << (char*)_Event.peer->data;
                /* Reset the peer's client information. */
                _Event.peer->data = NULL;
                _Peer = nullptr;
                _App.OnDisconnected();
                break;
            default:
                ss << "Unknown Event";
        }
        _pLog(ss.str());
    }
}

int Mgr_Net_Server::StartServer(int port) {
    assert(!enet_initialize() && "Failed loading ENET");

    _Address.port = port;
    _Server = enet_host_create(&_Address, 32, 2, 0, 0);

    assert(_Server && "ERROR in server creation");
    return 0;
}

int Mgr_Net_Server::SendPacket(string data) {
    /* Create a reliable packet of size 7 containing "packet\0" */
    ENetPacket* packet = enet_packet_create(data.c_str(), strlen(data.c_str()) + 1,
                                            ENET_PACKET_FLAG_RELIABLE);
    if (_Peer) enet_peer_send(_Peer, 1, packet);
    return 0;
}