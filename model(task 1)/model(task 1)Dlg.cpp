
// model(task 1)Dlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "model(task 1).h"
#include "model(task 1)Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Диалоговое окно CAboutDlg используется для описания сведений о приложении

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

// Реализация
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// Диалоговое окно Cmodeltask1Dlg



Cmodeltask1Dlg::Cmodeltask1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MODELTASK_1_DIALOG, pParent)
	, M(100)
	, n(100)
	, D(1)
	, width(0.5)
	, height(1)
	, R(2)
	, err(0.01)
	, U0(2)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cmodeltask1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_MAINGRAPH, MainGraph);
	DDX_Text(pDX, IDC_EDIT13, M);
	DDX_Text(pDX, IDC_EDIT1, n);
	DDX_Text(pDX, IDC_EDIT2, D);
	DDX_Text(pDX, IDC_EDIT3, width);
	DDX_Text(pDX, IDC_EDIT4, height);
	DDX_Text(pDX, IDC_EDIT5, R);
	DDX_Text(pDX, IDC_EDIT6, U0);
	DDX_Text(pDX, IDC_EDIT7, err);
}

BEGIN_MESSAGE_MAP(Cmodeltask1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON4, &Cmodeltask1Dlg::OnBnClickedButton4)	
END_MESSAGE_MAP()


// Обработчики сообщений Cmodeltask1Dlg

BOOL Cmodeltask1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Добавление пункта "О программе..." в системное меню.

	// IDM_ABOUTBOX должен быть в пределах системной команды.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию
	control = new Controller();

	//график с 2д картинкой
	MainGraph.GetContr(control);
	MainGraph.drawerID = 1;
	
	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

void Cmodeltask1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void Cmodeltask1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR Cmodeltask1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void Cmodeltask1Dlg::OnTimer(UINT_PTR nIDEvent)
{	
	//control->StartSolve();
	control->GetData();
	MainGraph.draw = 1;
	MainGraph.Invalidate(false);	

	while (PeekMessage(&msg, 0, WM_PAINT, WM_PAINT, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}		

	CDialogEx::OnTimer(nIDEvent);
}

void CAboutDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

//Посчитать значения
void Cmodeltask1Dlg::OnBnClickedButton4()
{
	UpdateData();
		
	control->Clear();
	control->InitModel();
	control->UpdateModel(n, M, D, height, width, U0, R, err);
	control->StartSolve();

	timer = SetTimer(1, 10, 0);
}

