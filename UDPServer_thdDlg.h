
// UDPServer_thdDlg.h : 헤더 파일
//

#pragma once
#include "afxcoll.h"
#include "afxwin.h"

struct ThreadArg// thread에 제공할 구조체이다.
{
	CStringList* pList;// 주고받을 메시지에 접근하도록 하는 변수
	CDialogEx* pDlg;// 대화상자에 접근할 수 있도록 한다
	int Thread_run;// thread를 무한루프를 돌릴때 1로, 쉴때에는 0으로 꺼둘 수 있도록 한다.
};

class CDataSocket;

// CUDPServer_thdDlg 대화 상자
class CUDPServer_thdDlg : public CDialogEx
{
// 생성입니다.
public:
	CUDPServer_thdDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UDPSERVER_THD_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_tx_edit_short;// 대화상자에서 메시지를 적을 edit control의 변수이다.
	CEdit m_tx_edit;// 보낸 내용의 로그를 저장할 edit control의 변수이다.
	CEdit m_rx_edit;// 받은 메시지를 표시할 edit control의 변수이다.
	CWinThread *pThread1, *pThread2; // 스레드 객체 주소
	ThreadArg arg1, arg2; // 스레드 전달 인자
	CDataSocket *m_pDataSocket;// 통신을 위한 DataSocket의 포인터


	void ProcessReceive(CDataSocket* pSocket, int nErrorCode);
	afx_msg void OnBnClickedSend();
};
