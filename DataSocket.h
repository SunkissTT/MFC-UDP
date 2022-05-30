#pragma once

// CDataSocket command target
class CUDPServer_thdDlg;

class CDataSocket : public CSocket
{
public:
	CDataSocket(CUDPServer_thdDlg *pDlg);
	virtual ~CDataSocket();
	CUDPServer_thdDlg *m_pDlg;
	virtual void OnReceive(int nErrorCode);
};


