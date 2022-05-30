#pragma once

// CDataSocket command target
class CUDPClient_thdDlg;

class CDataSocket : public CSocket
{
public:
	CDataSocket(CUDPClient_thdDlg *pDlg);
	virtual ~CDataSocket();
	CUDPClient_thdDlg *m_pDlg;
	virtual void OnReceive(int nErrorCode);
};


