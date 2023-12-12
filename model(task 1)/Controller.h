#pragma once

#include <gdiplus.h>
#include <vector>
#include "WaveModel.h"
#include "math3d.h"

using namespace std;
using namespace Gdiplus;
using namespace math3d;

struct Poligon
{
	vec4 p1;
	vec4 p2;
	vec4 p3;
	vec4 p4;
};

class Controller {	
private:	
	ULONG_PTR token;
	WaveModel* mod = nullptr;
	HANDLE TREAD;

	struct ColorTable {
	public:
		//массив кисточек
		SolidBrush** brashes;
		
		//размер сетки цветов
		int Size = 11;

		//массив с пороговыми значениями
		double *mas;

		ColorTable(double max) {
			double step = (2 * max) / (Size - 1);

			mas = new double[Size];
			brashes = new SolidBrush*[Size - 1];
			
			for (int i = 0; i < Size; i++) 
				mas[i] = -max + i * step;	

			for (int i = 0; i < Size - 1; i++) {
				Color col(255 *  i / (Size - 1), 0, 255 - 255 * i / (Size - 1));
				brashes[i] = new SolidBrush(col);
			}
		}

		//возвращает указатель на кисть из нужного диапазона
		SolidBrush* GetBrush(double val) {
			for (int i = 0; i < Size - 1; i++) {
				if ((val >= mas[i]) && (val <= mas[i + 1]))
					return brashes[i];				
			}
			return new SolidBrush(Color::Black);
		}
	};

	

	double	R;				//ширина ямы
	int N;					//количество точек по X
	int M;					//количество точек по Y

	double MaxF;			//максимальное значение на графике 	
	double MinF;			//минимальное значение на графике

	double** F = NULL;		//массив значений распределения потенциалов
	double* X = NULL;		//вектор значений по X
	double* Y = NULL;		//вектор значений по Y

	//вектор со всеми полигонами
	vector<Poligon*> polig;

	vector <pair<PointF, PointF>> Isolines[11];		//массив с изолиниями

	//флаг, отвечающий за создание вектора с полигонами
	bool poligReady = false;

	//подготавливает данные для отрисовки 2d
	void PrepareData2d();	

	//находит изолинии
	void MakeIsolines();

	//проверяет полигон для поиска изолинии
	bool CheckPoligon(double T, pair<PointF, PointF>& buf, Poligon pol);

	//функция, которая работает в потоке с моделью
	DWORD WINAPI ModelFunk();

	static DWORD WINAPI StaticModelFunk(PVOID param) {
		Controller* This = (Controller*)param;
		return This->ModelFunk();
	}	
	
public:	
	void UpdateModel(
		int N,				//количество точек по оси x
		int M,				//количество точек по оси y
		double	D,			//расстояние межу обкладками
		double height,		//высота обкладок
		double width,		//ширина обкладок
		double phi0,		//потенциал на обкладках		
		double R,			//размер поля для моделирования
		double err			//допустимая погрешность
	);
		
	//очищает данные
	void Clear();
	
	//запускает отрисовку 2d графика
	void DrawMainGr(LPDRAWITEMSTRUCT Item1);	
	
	//запусткает вычисления
	void StartSolve();	

	Controller() {
		GdiplusStartupInput si;
		GdiplusStartup(&token, &si, NULL);	
	}

	//деструктор
	~Controller() {
		GdiplusShutdown(token);
	}	
	
	//забирает данные из модели
	void GetData();	

	//получает значение ошибки из модли
	double GetError() {
		return mod->curE;
	}

	//создает новую модель
	void InitModel() {
		mod = new WaveModel();
	}
};