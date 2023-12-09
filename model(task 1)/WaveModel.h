#pragma once
#include <iostream>
#include <complex>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

#define Pi 3.1415926535
#define  NUMBER_IS_2_POW_K(x)   ((!((x)&((x)-1)))&&((x)>1))  // x is pow(2, k), k=1,2, ...
#define  FT_DIRECT        -1    // Direct transform.
#define  FT_INVERSE        1    // Inverse transform.

#define FIRSTHALF		1
#define SECONDHALF		0


using namespace std;

class WaveModel {
private:
	//параметры модели

	int N;		//количество точек по оси x
	int M;		//количество точек по оси y

	double
		d,			//расстояние межу обкладками
		height,		//высота обкладок
		width,		//ширина обкладок
		phi0,		//потенциал на обкладках
		stepX,		//шаг по пространству оси X
		stepY,		//шаг по пространству оси Y
		R,			//размер поля для моделирования
		e;			//допустимая погрешность

	//DATA
	
	double** Fnow = NULL;		//текущее распеределение потенциала
	double** Fpast = NULL;		//распределение потенциала на предыдущем шаге
	double* X = NULL;			//значения по X
	double* Y = NULL;			//значения по Y

	//KOEF
	
	//коеффициенты для дискретного уравнения Пуассона
	double
		a,
		b,
		c,
		d,
		e;


	//начальная инициализация алгоритма
	void InitData();

	//находит невязку
	double GetR(int i, int j) {
		return -(a * Fpast[i - 1][j] + b * Fpast[i + 1][j] + c * Fpast[i][j - 1] + d * Fpast[i][j + 1] + e * Fpast[i][j]);
	}

	//находит новое приближение
	void Solve();

	//копирует отсчеты функции на текущем шаге в буфер для предыдущего шага
	void CopyData();
public:
	//текущая погрешность
	double curE = 10;

	//флаг, отвечающий за готовность произвести фурье
	bool DataReady = false;

	//флаг, отвечающий за начало рассчета фурье
	bool FurStart = false;

	//функция, которая добавляет в буфер новый отсчет времени
	bool PutData();

	//Отдает указатель на F()
	complex<double>** GetF();

	//Отдает указатель на FFur()
	complex<double>*** GetFFur();

	//Отдает указатель на вектор с энергиями
	vector<pair<double, int>> GetEnerges();

	//сбрасывает настройки
	void Reset();

	//отдает указатель на X
	double* GetX();

	//отдает указатель на Y
	double* GetY();

	//отдает указатель на f
	double* Getf();

	//нахождение волнового пакета в следующий момент времени
	void FindWave();

	//нахождение собственных функций частицы
	void FindFunc();

	//апдейтит параметры модели
	void Update(int N, int M, double dt, double R, double a, double b, double U0, double f0, double asrx, double asry, double gammax, double gammay);

	//находит собственные функции в конкретном 
	void FindSF(int id);
};