#include "StdAfx.h"
#include "Waterfalling.h"


CDrawer::CDrawer()
{
	m_wndParent = NULL;
	m_rcOccupy.SetRectEmpty();
}

HWND CDrawer::SetParent(HWND parent)
{
	HWND rtn = m_wndParent;

	m_wndParent = parent;

	return rtn;
}

void CDrawer::SetPos(int nLeft ,int nTop ,int nWidth,int nHeight)
{
	m_rcOccupy.SetRect(nLeft,nTop,nLeft+nWidth,nTop + nHeight);
	m_rcOccupy.NormalizeRect();
}

CMemoryDC::CMemoryDC(CDC* pDC,CRect rcOccupy)
{
	Init(pDC,rcOccupy);
}

void CMemoryDC::Init(CDC* pDC,CRect rcOccupy)
{
	m_memDC.CreateCompatibleDC(pDC);
	m_bitmap.CreateCompatibleBitmap(pDC,rcOccupy.Width(),rcOccupy.Height());
	m_memDC.SelectObject(&m_bitmap);
	m_innRect = rcOccupy;
	m_innRect.OffsetRect(-1*rcOccupy.left,-1*rcOccupy.top);
}

CMemoryDC::~CMemoryDC()
{
	Release();
}

void CMemoryDC::Release()
{
	if(m_memDC.m_hDC)
	{
		m_memDC.DeleteDC();
	}

	if(m_bitmap.m_hObject)
	{
		m_bitmap.DeleteObject();
	}

	m_innRect.SetRectEmpty();
}

void CMemoryDC::ReleaseInit(CDC* pDC,CRect rcOccupy)
{
	Release();
	Init(pDC,rcOccupy);
}

void CMemoryDC::RenderTo(CDC* pDstDC,CRect rcClient)
{
	if(pDstDC == NULL) return;
	pDstDC->BitBlt(rcClient.left,rcClient.top,rcClient.Width(),rcClient.Height(),
		&m_memDC,0,0,SRCCOPY);

}

void CLevelPoints::reserve(int nCapcity)
{
	if(m_pLevel == NULL)
	{
		m_pLevel   = (float*)calloc(nCapcity,sizeof(float));
		m_nCapcity = nCapcity;
	}else if(nCapcity > m_nCapcity)
	{
		m_pLevel = (float*)realloc(m_pLevel,sizeof(float)*nCapcity);
		m_nCapcity = nCapcity;
	}
}

CLevelPoints::~CLevelPoints()
{
	if(m_pLevel)
		free(m_pLevel);

	m_pLevel = NULL;
	m_nCapcity = m_nSize = 0;
}

int CLevelPoints::assign(float * data,int cnt)
{
	ASSERT(m_pLevel);
	if(cnt > m_nCapcity)
		return 0;
	
	memcpy(m_pLevel,data,sizeof(float)*cnt);
	m_nSize = cnt;
}

void CWaterfallingDrawer::Draw(CDC * pDC ,CFreqArray * pFeqArray)
{
	if(pDC == NULL||pFeqArray == NULL)
		return;
	
	const int ndx  = pFeqArray->GetIndex();
	CPoint ptLeftTop,ptRightBottom;
	ptLeftTop     = m_rcOccupy.TopLeft();
	ptRightBottom = m_rcOccupy.BottomRight();
	ptLeftTop.y += ndx * m_nLineHgt;
	ptRightBottom.y = ptLeftTop.y + m_nLineHgt;

	const CRect rcLine(ptLeftTop,ptRightBottom);
	
	if(!m_memDC.Valid())
	{
		m_memDC.Init(pDC,rcLine);
	}

	if(m_memDC.GetRect().Width() != rcLine.Width() || m_memDC.GetRect().Height()!= rcLine.Height())
	{
		m_memDC.ReleaseInit(pDC,rcLine);
	}

	m_memDC->FillSolidRect(&(m_memDC.GetRect()),RGB(56,45,78));
	
	pFeqArray->AggregateTo(rcLine.Width(),CFreqArray::eAGG_MAX,m_levelArray);		
	for(int i = 0 ; i < m_levelArray.size() ; i++)
	{
		m_memDC->FillSolidRect(i,0,1,rcLine.Height(),RGB(200,200,198));
	}
	
	m_memDC.RenderTo(pDC,rcLine);	
}


CFreqArray::CFreqArray(void)
{
	m_nDx = 0;
	m_startFreq = 0;
	m_endFreq   = 0;
	m_diff      = 0;
}

/*设置显示频率的宽度*/
BOOL CFreqArray::SetFreqWidth(long long start ,long long end, long long diff)
{
	if(start < 0  || end <= start || diff <=0 ) return FALSE;

	m_vecFreq.reserve((end - start)/diff + 1);

	m_startFreq = start;
	m_endFreq   = end;
	m_diff      = diff;

	//test code....
	for(int i = 0 ; i < (end - start)/diff + 1 ; i++)
	{
		m_vecFreq.push_back(sfreq());
	}
	return TRUE;

}

void CFreqArray::SetIndex(int nDx)
{
	m_nDx = nDx;
}

/*增加或更新频率数据*/
BOOL CFreqArray::AddFreq(sfreq freqbuffer[],int cnt)
{
	BOOL bRtn = FALSE;

	if(cnt < 1 || !SelfCheck())
		return FALSE;

	if(Check(freqbuffer[0]) && Check(freqbuffer[cnt-1]))
	{
		const int nStartPos = (freqbuffer[0].freq - m_startFreq)/m_diff;
		m_vecFreq.assign(freqbuffer,freqbuffer+cnt);		
	}

	return TRUE;
}


sfreq* CFreqArray::vec_min(sfreq *vec,int nCount)
{
	int iMin = 0;
	for(int i = 1 ; i < nCount ; i++)
	{
		if(vec[i].level < vec[iMin].level)
			iMin = i;
	}

	return &vec[iMin];
}

sfreq* CFreqArray::vec_max(sfreq *vec,int nCount)
{
	int iMax = 0;
	for(int i = 1 ; i < nCount ; i++)
	{
		if(vec[i].level > vec[iMax].level)
			iMax = i;
	}

	return &vec[iMax];
}

void CFreqArray::AggregateTo(int points,int mode,CLevelPoints&dstArray)
{
	int nThisPoints   = this->GetCount();
	double dfFact = nThisPoints*1.0/points;
	dstArray.clear();
	dstArray.reserve(points);	

	if(nThisPoints == 0) return ;

	if(dfFact >= 1.0)
	{
		double dfScale = points*1.0/nThisPoints;

		for(int i = 0 ; i < points ; i++)
			dstArray.push_back(m_vecFreq[i*dfScale].level);
	}else{
		double dfScale = nThisPoints*1.0/points;
		for(int i = 0 ; i < points ; i++)
		{
			int   nPrev  = (int)(i*dfScale);
			int   nNext  = (int)(i*dfScale)+1;	
			if(nNext < nThisPoints)
			{
				float fLevel = m_vecFreq[nPrev].level + (m_vecFreq[nNext].level - m_vecFreq[nPrev].level)*(i*dfScale-nPrev);
				dstArray.push_back(fLevel);
			}else{
				float fLevel = m_vecFreq[nPrev].level;
			}			
		}		
	}
}