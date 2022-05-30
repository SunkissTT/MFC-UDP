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
	m_pDlg = pDlg;// DataSocket�� ���� �� ��ȭ���ڿ� ������ �� �ֵ��� �Ѵ�.
}

CDataSocket::~CDataSocket()
{
}


// CDataSocket member functions


void CDataSocket::OnReceive(int nErrorCode)
{
	// TODO: Add your specialized code here and/or call the base class

	CSocket::OnReceive(nErrorCode);
	m_pDlg->ProcessReceive(this, nErrorCode);// Client���� ���� �����͸� ���� �� �ֵ��� �Ѵ�.
}
