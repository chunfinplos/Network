#pragma once


using namespace std;

struct stParams {
    string _IP;
    int _Port;

    stParams(string ip = "", int port = 0) : _IP(ip), _Port(port) {};
};

class CWinServerDlg : public CDialogEx {
public:
    CWinServerDlg(CWnd* pParent = NULL);
    
    void PrintMessage(string msg);

    #ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_WINSERVER_DIALOG };
    #endif

    CButton* _bStart;
    CButton* _bStop;
    CButton* _bSend;

protected:
    HICON _Hicon;

    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

private:
    CEdit* _OutText;
    CEdit* _PortText;
    CEdit* _PacketText;

    void Click_bStart();
    void Click_bStop();
    void Click_bSend();
    void Click_bClear();
};
