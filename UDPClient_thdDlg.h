
// UDPClient_thdDlg.h : ��� ����
//

#pragma once
#include "afxcoll.h"
#include "afxwin.h"
#include "afxcmn.h"

struct ThreadArg// thread�� ������ ����ü�̴�.
{
	CStringList* pList;// �ְ����� �޽����� �����ϵ��� �ϴ� ����
	CDialogEx* pDlg;// ��ȭ���ڿ� ������ �� �ֵ��� �Ѵ�
	int Thread_run;// thread�� ���ѷ����� ������ 1��, �������� 0���� ���� �� �ֵ��� �Ѵ�.
};

class CDataSocket;

// CUDPClient_thdDlg ��ȭ ����
class CUDPClient_thdDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CUDPClient_thdDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UDPCLIENT_THD_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_tx_edit_short;// ��ȭ���ڿ��� �޽����� ���� edit control�� �����̴�.
	CEdit m_tx_edit;// ���� ������ �α׸� ������ edit control�� �����̴�.
	CEdit m_rx_edit;// ���� �޽����� ǥ���� edit control�� �����̴�.
	CWinThread *pThread1, *pThread2; // ������ ��ü �ּ�
	ThreadArg arg1, arg2; // ������ ���� ����
	CDataSocket *m_pDataSocket;// ����� ���� DataSocket�� ������

	CIPAddressCtrl m_ipaddr;
	void ProcessReceive(CDataSocket* pSocket, int nErrorCode);
	afx_msg void OnBnClickedSend();
};