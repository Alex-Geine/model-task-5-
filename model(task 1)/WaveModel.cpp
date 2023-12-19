#include "pch.h"
#include "WaveModel.h"

//нахождение волнового пакета в следующий момент времени
void WaveModel::Start() {
	/*curE = GetError();
	Solve();
	CopyData();*/
	while (err <= curE)
	{
		curE = GetError();
		Solve();
		CopyData();
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
		x4l = x1l,
		y1r = y1l,					//левый нижний угол для правой бокладки
		x1r = D / 2,
		y2r = y1l,					//правый нижний угол для правой обкладки 
		x2r = D / 2 + width,
		y3r = y3l,					//правый верхний угол для правой обкладки
		x3r = x2r,
		y4r = y3l,					//левый верхний угол для правой обкладки
		x4r = x1r;


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
			else if ((X[i] >= x1r) && (X[i] <= x2r) && (Y[j] >= y1r) && (Y[j] <= y3r)){
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
void WaveModel::Update(int N, int M, double D, double height, double width, double phi0, double R, double err) {
	this->N = N;
	this->M = M;	
	this->R = R;
	this->D = D;
	this->height = height;
	this->width = width;
	this->phi0 = phi0;
	this->err = err;
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
