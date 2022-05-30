
// UDPClient_thdDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "UDPClient_thd.h"
#include "UDPClient_thdDlg.h"
#include "afxdialogex.h"
#include "DataSocket.h"
#include "afxsock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCriticalSection tx_cs;// tx_thread�� critical section�� ������ �� lock, unlock�� �� �ֵ��� �Ѵ�.
CCriticalSection rx_cs;// rx_thread�� critical section�� ������ �� lock, unlock�� �� �ֵ��� �Ѵ�.


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CUDPClient_thdDlg ��ȭ ����



CUDPClient_thdDlg::CUDPClient_thdDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_UDPCLIENT_THD_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUDPClient_thdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_tx_edit_short);
	DDX_Control(pDX, IDC_EDIT2, m_tx_edit);
	DDX_Control(pDX, IDC_EDIT3, m_rx_edit);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ipaddr);
}

BEGIN_MESSAGE_MAP(CUDPClient_thdDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEND, &CUDPClient_thdDlg::OnBnClickedSend)
END_MESSAGE_MAP()

UINT RXThread(LPVOID arg)// ���ۿ��� �����͸� �������� ������
{
	ThreadArg *pArg = (ThreadArg *)arg;
	CStringList *plist = pArg->pList;
	CUDPClient_thdDlg *pDlg = (CUDPClient_thdDlg *)pArg->pDlg;
	while (pArg->Thread_run) {// Thread_run�� 1�̸� ���ѷ����� ������.
		POSITION pos = plist->GetHeadPosition();
		POSITION current_pos;
		while (pos != NULL) {
			current_pos = pos;

			rx_cs.Lock();// lock�� unlock�� ���� ����ȭ ������ ���Ѵ�.
			CString str = plist->GetNext(pos);// buffer���� �����͸� �����´�.
			rx_cs.Unlock();
			//			AfxMessageBox(str);
			// ���� �޽����� edit control�� ����Ѵ�.
			CString message;
			pDlg->m_rx_edit.GetWindowText(message);
			message += str;
			pDlg->m_rx_edit.SetWindowText(message);
			pDlg->m_rx_edit.LineScroll(pDlg->m_rx_edit.GetLineCount());
			//			pDlg->m_rx_edit.SetFocus();

			plist->RemoveAt(current_pos);// �޾ƿ� �޽����� �ݺ������� ������� �ʵ��� �����Ѵ�.
		}
		Sleep(10);
	}

	return 0;

}

UINT TXThread(LPVOID arg)// ���� �����͸� ���ۿ��� ������ �����ϴ� ������
{
	ThreadArg *pArg = (ThreadArg *)arg;
	CStringList *plist = pArg->pList;
	CUDPClient_thdDlg *pDlg = (CUDPClient_thdDlg *)pArg->pDlg;

	while (pArg->Thread_run) {// Thread_run�� 1�̸� ���ѷ����� ������.
		POSITION pos = plist->GetHeadPosition();
		POSITION current_pos;
		while (pos != NULL) {
			current_pos = pos;

			tx_cs.Lock();// ����ȭ������ ���Ѵ�.
			CString str = plist->GetNext(pos);// ���ۿ� ����ִ� ������ �����͸� �����´�.
			tx_cs.Unlock();

			CString message;
			message += str;// message�� ������ ��´�
			//AfxMessageBox(message);

			CString PeerAddr;
			UINT PeerPort;
			pDlg->m_pDataSocket->GetPeerNameEx(PeerAddr, PeerPort); ���� ���ľ���
			pDlg->m_pDataSocket->SendToEx((LPCTSTR)message, (message.GetLength() + 1) * sizeof(TCHAR), PeerPort, PeerAddr);// DataSocket�� Send()�� ���� ������.

			plist->RemoveAt(current_pos);// message�� ���ѹݺ��ϸ� ������ �ʵ��� �������ش�.
		}
		Sleep(10);
	}

	return 0;
}


// CUDPClient_thdDlg �޽��� ó����

BOOL CUDPClient_thdDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ���� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CStringList* newlist = new CStringList;
	arg1.pList = newlist;
	arg1.Thread_run = 1;
	arg1.pDlg = this;

	CStringList* newlist2 = new CStringList;
	arg2.pList = newlist2;
	arg2.Thread_run = 1;
	arg2.pDlg = this;

	WSADATA wsa;
	int error_code;
	if ((error_code = WSAStartup(MAKEWORD(2, 2), &wsa)) != 0) {
		TCHAR buffer[256];
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, 256, NULL);
		AfxMessageBox(buffer, MB_ICONERROR);
	}

	m_pDataSocket = NULL;
	m_ipaddr.SetWindowTextW(_T("127.0.0.1"));// IP Addr ����
	
	m_pDataSocket = NULL;
	m_pDataSocket = new CDataSocket(this);
	if (m_pDataSocket->Create(8000, SOCK_DGRAM)) { ���� Ȯ���ؾ���
		
		pThread1 = AfxBeginThread(TXThread, (LPVOID)&arg1);// TXThread ����
		pThread2 = AfxBeginThread(RXThread, (LPVOID)&arg2);// RXThread ����
		m_pDataSocket = NULL;

		m_tx_edit_short.SetFocus();
		return TRUE;
	}
	else {
		return FALSE;
	}
	/*
	if (m_pDataSocket == NULL) {
		m_pDataSocket = new CDataSocket(this);
		m_pDataSocket->Create();
		CString addr;
		m_ipaddr.GetWindowText(addr);
		if (m_pDataSocket->Connect(addr, 8000)) {// ������ �Ǹ� ���� ������ �Ѵ�.
			MessageBox(_T("������ ���� ����!"), _T("�˸�"), MB_ICONINFORMATION);

			arg1.Thread_run = 1;// ����Ǹ� �����带 �Ҵ�(�� ��° �̻��� ���ῡ�� �ش� �ڵ尡 ������ �������� ������ �����ؾ� ������ ������)
			arg2.Thread_run = 1;// ����Ǹ� �����带 �Ҵ�(�� ��° �̻��� ���ῡ�� �ش� �ڵ尡 ������ �������� ������ �����ؾ� ������ ������)

			pThread1 = AfxBeginThread(TXThread, (LPVOID)&arg1);// ó�� ����� �� �����带 ����
			pThread2 = AfxBeginThread(RXThread, (LPVOID)&arg2);// ó�� ����� �� �����带 ����

			m_tx_edit_short.SetFocus();

			return TRUE;
		}
		else {// ������ �ȵǾ����� �޽��� ����ϰ� DataSocket �����ϰ� ���� ������ �غ� �Ѵ�.
			MessageBox(_T("������ ���� ����!"), _T("�˸�"), MB_ICONERROR);
			delete m_pDataSocket;
			m_pDataSocket = NULL;

			return FALSE;
		}
	}
	else {// �̹� ����Ǿ������� �˷��ش�.
		MessageBox(_T("������ �̹� ���ӵ�!"), _T("�˸�"), MB_ICONINFORMATION);
		m_rx_edit.SetFocus();

		return FALSE;
	}
	*/
}

void CUDPClient_thdDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CUDPClient_thdDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CUDPClient_thdDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CUDPClient_thdDlg::ProcessReceive(CDataSocket* pSocket, int nErrorCode)
{
	TCHAR pBuf[1024 + 1];
	CString strData;
	int nbytes;
	CString PeerAddr;
	UINT PeerPort;

	m_pDataSocket->GetPeerNameEx(PeerAddr, PeerPort);
	nbytes = pSocket->ReceiveFromEx(pBuf, 1024, PeerAddr, PeerPort);
	pBuf[nbytes] = NULL;
	strData = (LPCTSTR)pBuf;


	rx_cs.Lock();// ����ȭ ������ ���Ѵ�.
	arg2.pList->AddTail((LPCTSTR)strData);
	rx_cs.Unlock();
}


void CUDPClient_thdDlg::OnBnClickedSend()
{
	// TODO: Add your control notification handler code here
	CString tx_message;
	m_tx_edit_short.GetWindowText(tx_message);
	tx_message += _T("\r\n");
	tx_cs.Lock();
	arg1.pList->AddTail(tx_message);
	tx_cs.Unlock();
	m_tx_edit_short.SetWindowText(_T(""));
	m_tx_edit_short.SetFocus();

	int len = m_tx_edit.GetWindowTextLengthW();
	m_tx_edit.SetSel(len, len);
	m_tx_edit.ReplaceSel(tx_message);
}