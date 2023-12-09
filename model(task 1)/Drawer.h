#pragma once
#include <afxwin.h>
#include<gdiplus.h>
#include "Controller.h"

using namespace Gdiplus;

// Drawer

class Drawer : public CStatic
{
	DECLARE_DYNAMIC(Drawer)
	friend class Controller;
public:
	Drawer();
	virtual ~Drawer();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	
	int drawerID;

	Controller* control;
	bool draw = false;
	void GetContr(Controller* control);

	double
		f,
		df,
		top,
		bottom,
		left,
		right,
		xPadding,
		yPadding,
		steplenY,
		steplenX,
		ItemWidth,
		ItemHeight,
		xScale,
		yScale;
	double
		l, //������ ��������
		a, //��������� �������
		w, //������� �������
		f0, //��������� ����������
		df0, //��������� ������� ��������
		t, //�����
		h = 0.01, //��� ��������������
		T0, //����������� �������
		Y0; //����������� ����������
//	afx_msg void OnPaint();
};
