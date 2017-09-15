#pragma once


using namespace std;

struct stParams {
    string _IP;
    int _Port;

    stParams(string ip = "", int port = 0) : _IP(ip), _Port(port) {};
};

class CWinClientDlg : public CDialogEx {
public:
    CWinClientDlg(CWnd* pParent = NULL);

    void PrintMessage(string msg);

    #ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_WINCLIENT_DIALOG };
    #endif

    CButton* _bStart;
    CButton* _bStop;
    CButton* _bSend;

protected:
    HICON _Hicon;

    virtual void DoDataExchange(CDataExchange* pDX);

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

private:
    CEdit* _OutText;
    CEdit* _IPText;
    CEdit* _PortText;
    CEdit* _PacketText;

    void Click_bConnectToServer();
    void Click_bDisconnect();
    void Click_bSend();
    void Click_bClear();
};
