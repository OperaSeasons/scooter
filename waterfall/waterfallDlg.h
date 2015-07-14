
// waterfallDlg.h : ͷ�ļ�
//

#pragma once
#include <list>
using namespace std;

#include "Waterfalling.h"
#define FREQ_START (80000000)
#define FREQ_END   (108000000)
#define FREQ_DIFF  (100000)
#define FREQ_HIS_NUM (100)
// CwaterfallDlg �Ի���
class CwaterfallDlg : public CDialogEx
{
// ����
public:
	CwaterfallDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_WATERFALL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
private:
	CWaterfallingDrawer m_waterfallingDrawer;
	CFreqArray		    m_freqData;
	list<CFreqArray *>       m_listHistFreq;	           
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
