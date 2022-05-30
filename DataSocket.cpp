// DataSocket.cpp : implementation file
//

#include "stdafx.h"
#include "UDPServer_thd.h"
#include "DataSocket.h"
#include "UDPServer_thdDlg.h"
#include "afxsock.h"

// CDataSocket

CDataSocket::CDataSocket(CUDPServer_thdDlg *pDlg)
{
	m_pDlg = pDlg;// DataSocket의 생성 시 대화상자에 접근할 수 있도록 한다.
}

CDataSocket::~CDataSocket()
{
}


// CDataSocket member functions


void CDataSocket::OnReceive(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class

	CSocket::OnReceive(nErrorCode);
	m_pDlg->ProcessReceive(this, nErrorCode);// Client에서 보낸 데이터를 받을 수 있도록 한다.
}
