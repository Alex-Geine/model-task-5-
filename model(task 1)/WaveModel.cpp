#include "pch.h"
#include "WaveModel.h"

//нахождение волнового пакета в следующий момент времени
void WaveModel::Start() {
	/*curE = GetError();
	Solve();
	CopyData();*/
	maxErr = GetError();

	while (err <= abs(curE - maxErr))
	{
		maxErr = GetError();
		Solve();
		CopyData();
		curE = GetError();
	}	
}

//начальная инициализация алгоритма
void WaveModel::InitData() {
	//рассчет координат для обкладок
	double
		y1l = -height / 2,			//левый нижний угол для левой обкладки		
		x1l = -D / 2 - width,
		y2l = y1l,					//правый нижний угол для левой обкладки
		x2l = -D / 2,
		y3l = height / 2,			//правый верхний угол для левой обкладки
		x3l = x2l,
		y4l = y3l,					//левый верхний угол для левой обкладки
		x4l = x1l;
		

	//преобразования координат

	//старые координаты
	double 
		y1r = y1l,					//левый нижний угол для правой бокладки
		x1r = D / 2 - D / 2 - width / 2,
		y2r = y1l,					//правый нижний угол для правой обкладки 
		x2r = D / 2 + width - D/2 - width / 2,
		y3r = y3l,					//правый верхний угол для правой обкладки
		x3r = x2r,
		y4r = y3l,					//левый верхний угол для правой обкладки
		x4r = x1r;

	tetta = tetta * 3.14 / 180;

	//новые координаты
	double 
		y1rn = x1r * sin(tetta) + y1r * cos(tetta),					//левый нижний угол для правой бокладки
		x1rn = x1r * cos(tetta) - y1r * sin(tetta) + D/2 + width / 2,
		y2rn = x2r * sin(tetta) + y2r * cos(tetta),					//правый нижний угол для правой обкладки 
		x2rn = x2r * cos(tetta) - y1r * sin(tetta) + D / 2 + width / 2,
		y3rn = x3r * sin(tetta) + y3r * cos(tetta),					//правый верхний угол для правой обкладки
		x3rn = x3r * cos(tetta) - y3r * sin(tetta) + D / 2 + width / 2,
		y4rn = x4r * sin(tetta) + y4r * cos(tetta),					//левый верхний угол для правой обкладки
		x4rn = x4r * cos(tetta) - y4r * sin(tetta) + D / 2 + width / 2;

	//разбиение прямоугольника на два треугольника
	Triangle t1(x1rn, y1rn, x2rn, y2rn, x3rn, y3rn), t2(x2rn, y2rn, x3rn, y3rn, x4rn, y4rn);

	//создание массивов
	X = new double[N];
	Y = new double[M];
	Fpast = new double* [N];
	Fnow = new double* [N];
	MapOfModel = new int* [N];

	//инициализация простраственной стеки
	stepX = 2 * R / N;
	stepY = 2 * R / M;
	double
		x0 = -R;

	for (int i = 0; i < N; i++)
		X[i] = x0 + i * stepX;

	for (int i = 0; i < M; i++)
		Y[i] = x0 + i * stepY;

	//начальные отсчеты распеределения потенциала
	for (int i = 0; i < N; i++) {
		Fpast[i] = new double[M];
		Fnow[i] = new double[M];
		MapOfModel[i] = new int[M];
		
		for (int j = 0; j < M; j++) {
			
			//для границы
			if ((i == 0) || (j == 0) || (i == N - 1) || (j == M - 1)) {
				MapOfModel[i][j] = BOARDS;
				Fpast[i][j] = 0;
				Fnow[i][j] = 0;
			}
			//для левой обкладки
			else if ((X[i] >= x1l) && (X[i] <= x2l) && (Y[j] >= y1l) && (Y[j] <= y3l)) {
				MapOfModel[i][j] = LFACING;
				Fpast[i][j] = -phi0;
				Fnow[i][j] = -phi0;
			}
				
			//для правой границы
			else if (CheckP(X[i],Y[j], x1rn, y1rn, x2rn, y2rn, x3rn, y3rn, x4rn, y4rn)) {
				MapOfModel[i][j] = RFACING;
				Fpast[i][j] = phi0;
				Fnow[i][j] = phi0;
			}
			//для остального пространства
			else {
				MapOfModel[i][j] = FIELD;
				Fpast[i][j] = 0;
				Fnow[i][j] = 0;
			}						
		}
	}

	//коэффициенты для аппроксимации уравнения Пуассона
	a = 1 / ( stepX * stepX);
	b = a;
	c = 1 / ( stepY * stepY);
	d = c;
	e = 2 / (stepX * stepX) + 2 / (stepY * stepY);
}

//проверяет, находится ли точка в прямоугольнике
bool WaveModel::CheckP(double x, double y, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) {
	//перенос прямоугольника в начало координат
	x1 = x1 - D / 2 - width / 2;
	x2 = x2 - D / 2 - width / 2;
	x3 = x3 - D / 2 - width / 2;
	x4 = x4 - D / 2 - width / 2;

	//перенос координат для точки
	x = x - D / 2 - width / 2;

	//обратный поворот координат
	//новые координаты для прямоугольника
	double
		y1n = x1 * sin(-tetta) + y1 * cos(-tetta),					//левый нижний угол для правой бокладки
		x1n = x1 * cos(-tetta) - y1 * sin(-tetta),
		y2n = x2 * sin(-tetta) + y2 * cos(-tetta),					//правый нижний угол для правой обкладки 
		x2n = x2 * cos(-tetta) - y1 * sin(-tetta),
		y3n = x3 * sin(-tetta) + y3 * cos(-tetta),					//правый верхний угол для правой обкладки
		x3n = x3 * cos(-tetta) - y3 * sin(-tetta),
		y4n = x4 * sin(-tetta) + y4 * cos(-tetta),					//левый верхний угол для правой обкладки
		x4n = x4 * cos(-tetta) - y4 * sin(-tetta);

	//новые координаты для точки
	double
		xn = x * cos(-tetta) - y * sin(-tetta),
		yn = x * sin(-tetta) + y * cos(-tetta);

	//сортировка координат
	double masx[4] = { x1n, x2n, x3n, x4n };
	double masy[4] = { y1n, y2n, y3n, y4n };
	double maxy = masy[0], maxx = masx[0], miny = maxy, minx = maxx;

	for (int i = 0; i < 4; i++) {
		if (maxx < masx[i])
			maxx = masx[i];
		if (minx > masx[i])
			minx = masx[i];
		if (maxy < masy[i])
			maxy = masy[i];
		if (miny > masy[i])
			miny = masy[i];
	}

	if ((minx <= xn) && (maxx >= xn) && (miny <= yn) && (maxy >= yn))
		return true;
	else
		return false;

}

//копирует отсчеты функции на текущем шаге в буфер для предыдущего шага
void WaveModel::CopyData() {
	for (int i = 0; i < N; i++)
		for (int j = 0; j < M; j++)
			Fpast[i][j] = Fnow[i][j];
}

//находит новое приближение
void WaveModel::Solve() {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			if (MapOfModel[i][j] == FIELD)
				Fnow[i][j] = (Fnow[i+1][j] + Fnow[i-1][j] + Fnow[i][j+1] + Fnow[i][j-1]) / 4.0;				
		}			
	}			
}

//апдейтит параметры модели
void WaveModel::Update(int N, int M, double D, double height, double width, double phi0, double R, double err, double tetta) {
	this->N = N;
	this->M = M;	
	this->R = R;
	this->D = D;
	this->height = height;
	this->width = width;
	this->phi0 = phi0;
	this->err = err;
	this->tetta = tetta;
}

//Отдает указатель на F
double** WaveModel::GetF() {
	return Fpast;
}

//отдает указатель на X
double* WaveModel::GetX() {
	return X;
}

//отдает указатель на Y
double* WaveModel::GetY() {
	return Y;
}

//отдает указатель на мап
int** WaveModel::GetMap() {
	return MapOfModel;
}

//сбрасывает настройки
void WaveModel::Reset()
{	
	if (X != NULL) {
		for (int i = 0; i < N; i++) {
			delete[] Fpast[i];
			delete[] Fnow[i];
			delete[] MapOfModel[i];
		}

		delete[] X;
		delete[] Y;
		delete[] Fpast;
		delete[] Fnow;
		delete[] MapOfModel;

		X = NULL;
		Y = NULL;
		Fpast = NULL;
		Fnow = NULL;
		MapOfModel = NULL;
	}	
}
