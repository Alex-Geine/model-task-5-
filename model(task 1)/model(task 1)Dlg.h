
// model(task 1)Dlg.h: файл заголовка
//

#pragma once
#include "Drawer.h"
#include "Controller.h"

// Диалоговое окно Cmodeltask1Dlg
class Cmodeltask1Dlg : public CDialogEx
{
// Создание
public:
	Cmodeltask1Dlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MODELTASK_1_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:	
	Drawer MainGraph;
	UINT_PTR timer;
	MSG msg;
	Controller* control;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButton4();

	int n;			//количество точек по оси X
	int M;			//количество точек по оси Y
	double D;		//расстояние между обкладками
	double width;	//ширина обкладки
	double height;	//высота обкладки
	double R;		//граница области
	double err;		//допустимая ошибка
	double U0;		//напряжение на обкладках
	double tetta;
	afx_msg void OnEnChangeEdit7();
};
