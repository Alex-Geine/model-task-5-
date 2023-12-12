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
		SolidBrush* GetBrush(double val) {
			for (int i = 0; i < Size - 1; i++) {
				if ((val >= mas[i]) && (val <= mas[i + 1]))
					return brashes[i];				
			}
			return new SolidBrush(Color::Black);
		}
	};

	

	double	R;				//������ ���
	int N;					//���������� ����� �� X
	int M;					//���������� ����� �� Y

	double MaxF;			//������������ �������� �� ������� 	
	double MinF;			//����������� �������� �� �������

	double** F = NULL;		//������ �������� ������������� �����������
	double* X = NULL;		//������ �������� �� X
	double* Y = NULL;		//������ �������� �� Y

	//������ �� ����� ����������
	vector<Poligon*> polig;

	vector <pair<PointF, PointF>> Isolines[11];		//������ � ����������

	//����, ���������� �� �������� ������� � ����������
	bool poligReady = false;

	//�������������� ������ ��� ��������� 2d
	void PrepareData2d();	

	//������� ��������
	void MakeIsolines();

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
		double err			//���������� �����������
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

	//������� ����� ������
	void InitModel() {
		mod = new WaveModel();
	}
};