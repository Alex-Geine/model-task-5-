#pragma once
#include <iostream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
//#include "Triangle.h"
#include "math3d.h"



#define BOARDS		400
#define LFACING		401
#define RFACING		402
#define FIELD		403

using namespace std;
using namespace math3d;

class WaveModel {
private:
	struct Triangle {
	public:
		vec4 mas[3];
		int type;		//отвечает за расположение треугольника в пространстве

		
		//конструктор, принимающий координаты в думере
		Triangle(double ax, double ay, double bx, double by, double cx, double cy) {
			vec4 a(ax, ay, 0), b(bx, by, 0), c(cx, cy, 0);
			mas[0] = a, mas[1] = b, mas[2] = c;
		}

		Triangle() {}

		//функция проыеряет, лежит ли точка в треугольнике
		bool Check(double x, double y) {
			//(x1 - x0)* (y2 - y1) - (x2 - x1) * (y1 - y0)
			//(x2 - x0) * (y3 - y2) - (x3 - x2) * (y2 - y0)
			//(x3 - x0) * (y1 - y3) - (x1 - x3) * (y3 - y0)
			double a = (mas[0].x() - x) * (mas[1].y() - mas[0].y()) - (mas[1].x() - mas[0].x()) * (mas[0].y() - y);
			double b = (mas[1].x() - x) * (mas[2].y() - mas[1].y()) - (mas[2].x() - mas[1].x()) * (mas[1].y() - y);
			double c = (mas[2].x() - x) * (mas[0].y() - mas[2].y()) - (mas[0].x() - mas[2].x()) * (mas[2].y() - y);

			if ((a >= 0 && b >= 0 && c >= 0) || (a <= 0 && b <= 0 && c <= 0))
				return 1;
			else return 0;
		}

		
	};
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
		err,		//допустимая погрешность
		tetta;		//угол отклонения обкладки 

	double maxErr;
	bool first = false;

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
		return (Fpast[i - 1][j] + Fpast[i + 1][j] + Fpast[i][j - 1] + Fpast[i][j + 1] + Fpast[i][j]) / 5;
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

	//проверяет, находится ли точка в прямоугольнике
	bool CheckP(double x, double y, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4);
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

	//отдает указатель на мап
	int** GetMap();

	//апдейтит параметры модели
	void Update(int N, int M, double D, double height, double width, double phi0, double R, double err, double tetta);

	//запускает вычисления
	void Start();
};