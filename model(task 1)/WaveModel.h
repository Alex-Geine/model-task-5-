#pragma once
#include <iostream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>


#define BOARDS		400
#define LFACING		401
#define RFACING		402
#define FIELD		403

using namespace std;

class WaveModel {
private:
	//параметры модели

	int N;		//количество точек по оси x
	int M;		//количество точек по оси y

	double
		D,			//расстояние межу обкладками
		height,		//высота обкладок
		width,		//ширина обкладок
		phi0,		//потенциал на обкладках
		stepX,		//шаг по пространству оси X
		stepY,		//шаг по пространству оси Y
		R,			//размер поля для моделирования
		err;			//допустимая погрешность

	//DATA
	
	double** Fnow = NULL;		//текущее распеределение потенциала
	double** Fpast = NULL;		//распределение потенциала на предыдущем шаге
	double* X = NULL;			//значения по X
	double* Y = NULL;			//значения по Y
	int **MapOfModel = NULL;			//в массиве будут идентификаторы каждой клетки поля

	//KOEF
	
	//коеффициенты для дискретного уравнения Пуассона
	double
		a,
		b,
		c,
		d,
		e;


	

	//находит невязку
	double GetR(int i, int j) {
		return -(a * Fpast[i - 1][j] + b * Fpast[i + 1][j] + c * Fpast[i][j - 1] + d * Fpast[i][j + 1] + e * Fpast[i][j]);
	}

	//находит среднюю ошибку
	double GetError() {
		double res = 0;
		for (int i = 1; i < N - 1; i++)
			for (int j = 1; j < M - 1; j++)
				if(MapOfModel[i][j] == FIELD)
					res += GetR(i, j) * GetR(i,j);

		return sqrt(res);
	}

	//находит новое приближение
	void Solve();

	//копирует отсчеты функции на текущем шаге в буфер для предыдущего шага
	void CopyData();	
public:
	//начальная инициализация алгоритма
	void InitData();

	//текущая погрешность
	double curE = 10;

	//Отдает указатель на F()
	double** GetF();

	//сбрасывает настройки
	void Reset();

	//отдает указатель на X
	double* GetX();

	//отдает указатель на Y
	double* GetY();

	//апдейтит параметры модели
	void Update(int N, int M, double D, double height, double width, double phi0, double R, double err);

	//запускает вычисления
	void Start();
};