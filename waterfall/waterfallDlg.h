
// waterfallDlg.h : 头文件
//

#pragma once
#include <list>
using namespace std;

#include "Waterfalling.h"
#define FREQ_START (80000000)
#define FREQ_END   (108000000)
#define FREQ_DIFF  (100000)
#define FREQ_HIS_NUM (100)
// CwaterfallDlg 对话框
class CwaterfallDlg : public CDialogEx
{
// 构造
public:
	CwaterfallDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_WATERFALL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
