#pragma once

#include <vector>
#include <set>
using namespace std;

class CFreqArray;

struct sfreq{
	long long freq;//单位hz
	float     level;//dbuv
	float     bw;//KHz
	sfreq(){freq = 0;level = 0;bw=0;}
};

class CDrawer{
public:
	CDrawer();
	HWND SetParent(HWND parent);
	void SetPos(int nLeft ,int nTop ,int nWidth,int nHeight);
	virtual void Draw(CDC * pDC ,CFreqArray * pFeqArray){;}
protected:
	HWND  m_wndParent;
	CRect m_rcOccupy;
private:
	CPen  m_penDot;
};

class CMemoryDC{
public:
	CMemoryDC(){;}
	CMemoryDC(CDC* pDC,CRect rcOccupy);
	~CMemoryDC();
	void Init(CDC* pDC,CRect rcOccupy);
	void Release();
	void ReleaseInit(CDC* pDC,CRect rcOccupy);
	CDC* operator ->(){return &m_memDC;}
	void RenderTo(CDC* pDstDC,CRect rcClient);
	CRect GetRect(){return m_innRect;}
	const BOOL Valid(){return (m_bitmap.m_hObject != NULL && m_memDC.m_hDC !=NULL);}
protected:
private:
	CBitmap    m_bitmap;
	CDC        m_memDC;
	CRect      m_innRect;
};

typedef	vector<sfreq> CFreqPoints; 

class CLevelPoints{
public:	
	CLevelPoints(){m_pLevel = NULL;m_nSize = 0;m_nCapcity = 0;}
	~CLevelPoints();
	void   reserve(int nCapcity);
	inline int    size(){return m_nSize;}	
	inline int    capcity()const {return m_nCapcity;}
	inline float *data()const {return m_pLevel;}
	inline void   clear(){m_nSize = 0;}
	inline void   push_back(float elem){ASSERT(m_pLevel);m_pLevel[m_nSize++] = elem;}
	inline void   set_size(int size){m_nSize = size;}
	inline int    assign(float * data,int cnt);
private:
	float * m_pLevel;
	int m_nSize;
	int m_nCapcity;
};

class CWaterfallingDrawer : public CDrawer
{
public:
	CWaterfallingDrawer(){m_nLineHgt = 1;}
	void SetLineHeight(int nLineHgt){m_nLineHgt = nLineHgt;}
	virtual void Draw(CDC * pDC ,CFreqArray * pFeqArray);
private:
	int  m_nLineHgt;
	CLevelPoints m_levelArray;
	CMemoryDC  m_memDC;
};

class CFreqArray
{
public:
	enum{eAGG_MAX,eAGG_MIN,eAGG_AVG};
	CFreqArray();
	/*设置显示频率的宽度start,end,diff:单位Hz*/
	BOOL SetFreqWidth(long long start ,long long end, long long diff);
	inline long long GetStartFreq()const {return m_startFreq;}
	inline long long GetEndFreq()const {return m_endFreq;}
	inline long long GetFreqDiff()const {return m_diff;}
	/*增加或更新频率数据*/
	BOOL AddFreq(sfreq freqbuffer[],int cnt);
	/*绘制*/
	void Draw(CDrawer * pDrawer);
	void SetIndex(int nDx);
	int  GetIndex()const{return m_nDx;}
	int  GetCount(){return m_vecFreq.size();}
	void AggregateTo(int points,int mode,CLevelPoints& dstArray);
protected:
	sfreq* vec_min(sfreq *vec,int nCount);
	sfreq* vec_max(sfreq *vec,int nCount);
	inline BOOL SelfCheck()const{return (m_startFreq >=0 &&  m_endFreq >m_startFreq && m_diff > 0) ;}
	inline BOOL Check(const sfreq&freq)const {return (freq.freq >= m_startFreq && freq.freq <= m_endFreq);}
private:
	vector<sfreq>  m_vecFreq;
	long long      m_startFreq;//单位hz
	long long      m_endFreq;//单位hz
	long long      m_diff;//单位hz
	int            m_nDx;
};

