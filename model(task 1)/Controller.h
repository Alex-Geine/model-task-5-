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

	double	R;					//������ ���
	int N;						//���������� ����� �� X
	int M;						//���������� ����� �� Y
	int QuantCount = 100;		//���������� ������� ������, ����������� � ����� �������

	double MaxF;				//������������ �������� �� ������� 	
	double MinF;				//����������� �������� �� �������

	double** F = NULL;			//������ �������� ������������� �����������
	double* X = NULL;			//������ �������� �� X
	double* Y = NULL;			//������ �������� �� Y
	int** MapOfModel = NULL;	//������ � �������������� ����� �� ����
	int* MapOfPoligon = NULL;	//������ � �������������� ��������� �� ����

	vector<Poligon*> polig;		//������ �� ����� ����������				

	vector <pair<PointF, PointF>> Isolines[11];					//������ � ����������
	vector <pair<PointF, PointF>> QuantTraces[10000];		//���������� �������� ������� ������

	//����, ���������� �� �������� ������� � ����������
	bool poligReady = false;

	struct Triangle {
	public:
		vec4 mas[3];
		int type;		//�������� �� ������������ ������������ � ������������

		vec4 a, b;		//���������� �������

		Triangle(vec4 a, vec4 b, vec4 c, int type) :type(type) {
			mas[0] = a, mas[1] = b, mas[2] = c;

			//������ ���������� ������������ ���� ������ ������ ��������
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

		//�����������, ����������� ���������� � ������
		Triangle(double ax, double ay, double bx, double by, double cx, double cy) {
			vec4 a(ax, ay, 0), b(bx, by, 0), c(cx, cy, 0);
			mas[0] = a, mas[1] = b, mas[2] = c;
		}

		Triangle() {}

		//������� ���������, ����� �� ����� � ������������
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

		//�������, ������������ ��������
		PointF GetGradient() {
			//��������� ��������� ������������ ��� ������������ ���������, ������� ������ ��������
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
		//������ ��������
		SolidBrush** brashes;
		
		//������ ����� ������
		int Size = 11;

		//������ � ���������� ����������
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

		//���������� ��������� �� ����� �� ������� ���������
		SolidBrush* GetBrush(int type) {
			if (type == LFACING)
				return brashes[0];
			else if (type == RFACING)
				return brashes[Size - 2];
			else
				return new SolidBrush(Color::Black);
		}
	};

	

	

	//�������������� ������ ��� ��������� 2d
	void PrepareData2d();	

	//������� ��������
	void MakeIsolines();
	 
	//������� ������� �����
	void MakeForceLines();

	//��������� ������� �������
	void StartQuantum(double x, double y, int count);

	//�������� ������� �� ������������
	void MoveTriangle(Triangle t, PointF & p);

	//�������� ������� �� ��������
	PointF MovePoligon(int countPolig, int countTrace, double x, double y);

	//������� ������� � ��������� ������ �������
	int FindStartPoligon(double x, double y);

	//������� ����� ������� �������
	int FindNearPoligon(int cur, double x, double y);

	//��������� ������� ��� ������ ��������
	bool CheckPoligon(double T, pair<PointF, PointF>& buf, Poligon pol);

	//�������, ������� �������� � ������ � �������
	DWORD WINAPI ModelFunk();

	static DWORD WINAPI StaticModelFunk(PVOID param) {
		Controller* This = (Controller*)param;
		return This->ModelFunk();
	}	
	
public:	
	void UpdateModel(
		int N,				//���������� ����� �� ��� x
		int M,				//���������� ����� �� ��� y
		double	D,			//���������� ���� ����������
		double height,		//������ ��������
		double width,		//������ ��������
		double phi0,		//��������� �� ���������		
		double R,			//������ ���� ��� �������������
		double err,			//���������� �����������
		double tetta		//���� �������� ��������
	);
		
	//������� ������
	void Clear();
	
	//��������� ��������� 2d �������
	void DrawMainGr(LPDRAWITEMSTRUCT Item1);	
	
	//���������� ����������
	void StartSolve();	

	Controller() {
		GdiplusStartupInput si;
		GdiplusStartup(&token, &si, NULL);	
	}

	//����������
	~Controller() {
		GdiplusShutdown(token);
	}	
	
	//�������� ������ �� ������
	void GetData();	

	//�������� �������� ������ �� �����
	double GetError() {
		return mod->curE;
	}

	//���������, ���������� �� ������
	bool CheckTread() {
		if (WaitForSingleObject(TREAD, 10) == WAIT_OBJECT_0)
			return 1;
		else
			return 0;

	}

	//������� ����� ������
	void InitModel() {
		mod = new WaveModel();
		//QuantTraces = new vector<pair<PointF, PointF>>[QuantCount];
	}
};