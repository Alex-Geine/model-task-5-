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
	//��������� ������

	int N;		//���������� ����� �� ��� x
	int M;		//���������� ����� �� ��� y

	double
		D,			//���������� ���� ����������
		height,		//������ ��������
		width,		//������ ��������
		phi0,		//��������� �� ���������
		stepX,		//��� �� ������������ ��� X
		stepY,		//��� �� ������������ ��� Y
		R,			//������ ���� ��� �������������
		err;			//���������� �����������

	//DATA
	
	double** Fnow = NULL;		//������� �������������� ����������
	double** Fpast = NULL;		//������������� ���������� �� ���������� ����
	double* X = NULL;			//�������� �� X
	double* Y = NULL;			//�������� �� Y
	int **MapOfModel = NULL;			//� ������� ����� �������������� ������ ������ ����

	//KOEF
	
	//������������ ��� ����������� ��������� ��������
	double
		a,
		b,
		c,
		d,
		e;


	

	//������� �������
	double GetR(int i, int j) {
		return -(a * Fpast[i - 1][j] + b * Fpast[i + 1][j] + c * Fpast[i][j - 1] + d * Fpast[i][j + 1] + e * Fpast[i][j]);
	}

	//������� ������� ������
	double GetError() {
		double res = 0;
		for (int i = 1; i < N - 1; i++)
			for (int j = 1; j < M - 1; j++)
				if(MapOfModel[i][j] == FIELD)
					res += GetR(i, j) * GetR(i,j);

		return sqrt(res);
	}

	//������� ����� �����������
	void Solve();

	//�������� ������� ������� �� ������� ���� � ����� ��� ����������� ����
	void CopyData();	
public:
	//��������� ������������� ���������
	void InitData();

	//������� �����������
	double curE = 10;

	//������ ��������� �� F()
	double** GetF();

	//���������� ���������
	void Reset();

	//������ ��������� �� X
	double* GetX();

	//������ ��������� �� Y
	double* GetY();

	//�������� ��������� ������
	void Update(int N, int M, double D, double height, double width, double phi0, double R, double err);

	//��������� ����������
	void Start();
};