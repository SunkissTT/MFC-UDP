
// UDPClient_thdDlg.cpp : 구현 파일
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

CCriticalSection tx_cs;// tx_thread가 critical section에 접근할 때 lock, unlock할 수 있도록 한다.
CCriticalSection rx_cs;// rx_thread가 critical section에 접근할 때 lock, unlock할 수 있도록 한다.


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CUDPClient_thdDlg 대화 상자



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

UINT RXThread(LPVOID arg)// 버퍼에서 데이터를 꺼내오는 스레드
{
	ThreadArg *pArg = (ThreadArg *)arg;
	CStringList *plist = pArg->pList;
	CUDPClient_thdDlg *pDlg = (CUDPClient_thdDlg *)pArg->pDlg;
	while (pArg->Thread_run) {// Thread_run이 1이면 무한루프를 돌린다.
		POSITION pos = plist->GetHeadPosition();
		POSITION current_pos;
		while (pos != NULL) {
			current_pos = pos;

			rx_cs.Lock();// lock과 unlock을 통해 동기화 문제를 피한다.
			CString str = plist->GetNext(pos);// buffer에서 데이터를 꺼내온다.
			rx_cs.Unlock();
			//			AfxMessageBox(str);
			// 받은 메시지를 edit control에 출력한다.
			CString message;
			pDlg->m_rx_edit.GetWindowText(message);
			message += str;
			pDlg->m_rx_edit.SetWindowText(message);
			pDlg->m_rx_edit.LineScroll(pDlg->m_rx_edit.GetLineCount());
			//			pDlg->m_rx_edit.SetFocus();

			plist->RemoveAt(current_pos);// 받아온 메시지를 반복적으로 사용하지 않도록 삭제한다.
		}
		Sleep(10);
	}

	return 0;

}

UINT TXThread(LPVOID arg)// 보낼 데이터를 버퍼에서 꺼내서 전송하는 스레드
{
	ThreadArg *pArg = (ThreadArg *)arg;
	CStringList *plist = pArg->pList;
	CUDPClient_thdDlg *pDlg = (CUDPClient_thdDlg *)pArg->pDlg;

	while (pArg->Thread_run) {// Thread_run이 1이면 무한루프를 돌린다.
		POSITION pos = plist->GetHeadPosition();
		POSITION current_pos;
		while (pos != NULL) {
			current_pos = pos;

			tx_cs.Lock();// 동기화문제를 피한다.
			CString str = plist->GetNext(pos);// 버퍼에 들어있는 전송할 데이터를 꺼내온다.
			tx_cs.Unlock();

			CString message;
			message += str;// message에 내용을 담는다
			//AfxMessageBox(message);

			CString PeerAddr;
			UINT PeerPort;
			pDlg->m_pDataSocket->GetPeerNameEx(PeerAddr, PeerPort); 여기 고쳐야함
			pDlg->m_pDataSocket->SendToEx((LPCTSTR)message, (message.GetLength() + 1) * sizeof(TCHAR), PeerPort, PeerAddr);// DataSocket의 Send()를 통해 보낸다.

			plist->RemoveAt(current_pos);// message를 무한반복하며 보내지 않도록 삭제해준다.
		}
		Sleep(10);
	}

	return 0;
}


// CUDPClient_thdDlg 메시지 처리기

BOOL CUDPClient_thdDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
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
	m_ipaddr.SetWindowTextW(_T("127.0.0.1"));// IP Addr 설정
	
	m_pDataSocket = NULL;
	m_pDataSocket = new CDataSocket(this);
	if (m_pDataSocket->Create(8000, SOCK_DGRAM)) { 여기 확인해야함
		
		pThread1 = AfxBeginThread(TXThread, (LPVOID)&arg1);// TXThread 시작
		pThread2 = AfxBeginThread(RXThread, (LPVOID)&arg2);// RXThread 시작
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
		if (m_pDataSocket->Connect(addr, 8000)) {// 연결이 되면 다음 동작을 한다.
			MessageBox(_T("서버에 접속 성공!"), _T("알림"), MB_ICONINFORMATION);

			arg1.Thread_run = 1;// 연결되면 스레드를 켠다(두 번째 이상의 연결에서 해당 코드가 스레드 생성보다 상위에 존재해야 오류가 없었음)
			arg2.Thread_run = 1;// 연결되면 스레드를 켠다(두 번째 이상의 연결에서 해당 코드가 스레드 생성보다 상위에 존재해야 오류가 없었음)

			pThread1 = AfxBeginThread(TXThread, (LPVOID)&arg1);// 처음 연결될 때 스레드를 생성
			pThread2 = AfxBeginThread(RXThread, (LPVOID)&arg2);// 처음 연결될 때 스레드를 생성

			m_tx_edit_short.SetFocus();

			return TRUE;
		}
		else {// 연결이 안되었을때 메시지 출력하고 DataSocket 삭제하고 새로 연결할 준비를 한다.
			MessageBox(_T("서버에 접속 실패!"), _T("알림"), MB_ICONERROR);
			delete m_pDataSocket;
			m_pDataSocket = NULL;

			return FALSE;
		}
	}
	else {// 이미 연결되어있을때 알려준다.
		MessageBox(_T("서버에 이미 접속됨!"), _T("알림"), MB_ICONINFORMATION);
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CUDPClient_thdDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
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


	rx_cs.Lock();// 동기화 문제를 피한다.
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
