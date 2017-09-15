#include "stdafx.h"
#include "Mgr_Net_Client.h"
#include "WinClient.h"


using namespace std;

extern CWinClientApp _App;

Mgr_Net_Client::Mgr_Net_Client() {
    _Address.host = ENET_HOST_ANY;
    _Address.port = -1;
    _Client = nullptr;
    _Peer = nullptr;

    assert(!enet_initialize() && "Failed loading ENET");
    _Client = enet_host_create(nullptr, 1, 2, 0, 0);
    assert(_Client && "ERROR in client creation");
}

Mgr_Net_Client::~Mgr_Net_Client() {
    enet_host_destroy(_Client);
    enet_deinitialize();
}

void Mgr_Net_Client::Tick(float DeltaSeconds) {
    if (enet_host_service(_Client, &_Event, 1000) > 0) {
        ostringstream ss;
        switch (_Event.type) {
            //case ENET_EVENT_TYPE_CONNECT:
            //    char hostName[50];
            //    enet_address_get_host_ip(&_Event.peer->address, hostName, 50);
            //    ss << "CONNECTED TO: " << hostName << ":" << _Event.peer->address.port;

            //    _Event.peer->data = "Server information";
            //    _App.OnConnected();
            //    break;
            case ENET_EVENT_TYPE_RECEIVE:
                ss << "PACKET: Size(" << _Event.packet->dataLength << "), Data["
                    << _Event.packet->data << "], Peer(" << (char*)_Event.peer->data << "), Channel: "
                    << (unsigned int)_Event.channelID;

                enet_packet_destroy(_Event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                ss << "DISCONNECTED: " << (char*)_Event.peer->data;
                _Event.peer->data = NULL;
                _App.OnDisconnected();
            default:
                ss << "Unknown Event";
        }
        _pLog(ss.str());
    }
}

bool Mgr_Net_Client::ConnectToServer(string host, int port) {
    enet_address_set_host(&_Address, host.c_str());
    _Address.port = port;

    /* Initiate the connection, allocating the two channels 0 and 1. */
    _Peer = enet_host_connect(_Client, &_Address, 2, 0);

    if (_Peer &&
        enet_host_service(_Client, &_Event, 5000) > 0 &&
        _Event.type == ENET_EVENT_TYPE_CONNECT) {

        ostringstream ss;
        char hostName[50];

        enet_address_get_host_ip(&_Event.peer->address, hostName, 50);
        ss << "CONNECTED TO: " << hostName << ":" << _Event.peer->address.port;

        _Event.peer->data = "Server information";
        _pLog(ss.str());
        SendPacket("Init Packet");
        _App.OnConnected();
    }
    else {
        ::MessageBox(NULL, (LPCWSTR)L"Can't connect to server", NULL, MB_OK | MB_ICONERROR);
        return false;
    }
    return true;
}

int Mgr_Net_Client::DisconnectFromServer() {
    if (_Peer && _Peer->state == ENET_PEER_STATE_CONNECTED) enet_peer_disconnect(_Peer, 0);
    return 0;
}

int Mgr_Net_Client::SendPacket(string data) {
    if (_Peer && _Peer->state == ENET_PEER_STATE_CONNECTED) {
        /* Create a reliable packet of size 7 containing "packet\0" */
        ENetPacket* packet = enet_packet_create(data.c_str(), strlen(data.c_str()) + 1,
                                                ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(_Peer, 1, packet);
    }
    else ::MessageBox(NULL, (LPCWSTR)L"Not Connected!!", NULL, MB_OK | MB_ICONERROR);
    return 0;
}