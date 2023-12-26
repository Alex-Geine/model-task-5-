#pragma once

#include <gdiplus.h>
#include <vector>
#include "WaveModel.h"

//#include "Triangle.h"

using namespace std;
using namespace Gdiplus;
using namespace math3d;

#define BOARDS		400
#define LFACING		401
#define RFACING		402
#define FIELD		403
#define OTHER		404


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

	double	R;					//ширина ямы
	int N;						//количество точек по X
	int M;						//количество точек по Y
	int QuantCount = 100;		//количество пробных частиц, запускаемых с одной стороны

	double MaxF;				//максимальное значение на графике 	
	double MinF;				//минимальное значение на графике

	double** F = NULL;			//массив значений распределения потенциалов
	double* X = NULL;			//вектор значений по X
	double* Y = NULL;			//вектор значений по Y
	int** MapOfModel = NULL;	//массив с классификацией узлов на поле
	int* MapOfPoligon = NULL;	//массив с классификацией полигонов на поле

	vector<Poligon*> polig;		//вектор со всеми полигонами				

	vector <pair<PointF, PointF>> Isolines[11];					//массив с изолиниями
	vector <pair<PointF, PointF>> QuantTraces[10000];		//траектории движения пробных частиц

	//флаг, отвечающий за создание вектора с полигонами
	bool poligReady = false;

	struct Triangle {
	public:
		vec4 mas[3];
		int type;		//отвечает за расположение треугольника в пространстве

		vec4 a, b;		//координаты вектора

		Triangle(vec4 a, vec4 b, vec4 c, int type) :type(type) {
			mas[0] = a, mas[1] = b, mas[2] = c;

			//задаем координаты направляющей пары правой тройки векторов
			if (type == 0) {
				this->a.Setx(b.x() - a.x());
				this->a.Sety(b.y() - a.y());
				this->a.Setz(b.z() - a.z());

				this->b.Setx(c.x() - a.x());
				this->b.Sety(c.y() - a.y());
				this->b.Setz(c.z() - a.z());
			}
			else {
				this->a.Setx(c.x() - b.x());
				this->a.Sety(c.y() - b.y());
				this->a.Setz(c.z() - b.z());

				this->b.Setx(a.x() - b.x());
				this->b.Sety(a.y() - b.y());
				this->b.Setz(a.z() - b.z());
			}
		}

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

		//Функция, возвращающая градиент
		PointF GetGradient() {
			//посчитаем векторнок произведение для направляющих координат, получим вектор градиент
			double x = a.y() * b.z() - a.z() * b.y();
			double y = -a.x() * b.z() - a.z() * b.x();
			PointF p;

			if (type == 1) {
				p.X = -x / sqrt(x * x + y * y),
					p.Y = -y / sqrt(x * x + y * y);
			}

			else {
				p.X = -x / sqrt(x * x + y * y),
					p.Y = -y / sqrt(x * x + y * y);
			}

			return p;
		}
	};
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
		SolidBrush* GetBrush(int type) {
			if (type == LFACING)
				return brashes[0];
			else if (type == RFACING)
				return brashes[Size - 2];
			else
				return new SolidBrush(Color::Black);
		}
	};

	

	

	//подготавливает данные для отрисовки 2d
	void PrepareData2d();	

	//находит изолинии
	void MakeIsolines();
	 
	//находит силовые силии
	void MakeForceLines();

	//запускает пробную частицу
	void StartQuantum(double x, double y, int count);

	//движение частицы по треугольнику
	void MoveTriangle(Triangle t, PointF & p);

	//движение частицы по полигону
	PointF MovePoligon(int countPolig, int countTrace, double x, double y);

	//находит полигон в начальный момент времени
	int FindStartPoligon(double x, double y);

	//находит рядом стоящий полигон
	int FindNearPoligon(int cur, double x, double y);

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
		double err,			//допустимая погрешность
		double tetta		//угол поворота обкладки
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

	//проверяет, отработала ли модель
	bool CheckTread() {
		if (WaitForSingleObject(TREAD, 10) == WAIT_OBJECT_0)
			return 1;
		else
			return 0;

	}

	//создает новую модель
	void InitModel() {
		mod = new WaveModel();
		//QuantTraces = new vector<pair<PointF, PointF>>[QuantCount];
	}
};