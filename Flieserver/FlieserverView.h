﻿
// FlieserverView.h: CFlieserverView 类的接口
//

#pragma once
#include "FlieserverDoc.h"


class CFlieserverView : public CView
{
protected: // 仅从序列化创建
	CFlieserverView() noexcept;
	DECLARE_DYNCREATE(CFlieserverView)

// 特性
public:
	CFlieserverDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 实现
public:
	virtual ~CFlieserverView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // FlieserverView.cpp 中的调试版本
inline CFlieserverDoc* CFlieserverView::GetDocument() const
   { return reinterpret_cast<CFlieserverDoc*>(m_pDocument); }
#endif
