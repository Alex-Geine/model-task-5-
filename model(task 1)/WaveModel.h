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
	//��������� ������

	int N;		//���������� ����� �� ��� x
	int M;		//���������� ����� �� ��� y

	double
		d,			//���������� ���� ����������
		height,		//������ ��������
		width,		//������ ��������
		phi0,		//��������� �� ���������
		stepX,		//��� �� ������������ ��� X
		stepY,		//��� �� ������������ ��� Y
		R,			//������ ���� ��� �������������
		e;			//���������� �����������

	//DATA
	
	double** Fnow = NULL;		//������� �������������� ����������
	double** Fpast = NULL;		//������������� ���������� �� ���������� ����
	double* X = NULL;			//�������� �� X
	double* Y = NULL;			//�������� �� Y

	//KOEF
	
	//������������ ��� ����������� ��������� ��������
	double
		a,
		b,
		c,
		d,
		e;


	//��������� ������������� ���������
	void InitData();

	//������� �������
	double GetR(int i, int j) {
		return -(a * Fpast[i - 1][j] + b * Fpast[i + 1][j] + c * Fpast[i][j - 1] + d * Fpast[i][j + 1] + e * Fpast[i][j]);
	}

	//������� ����� �����������
	void Solve();

	//�������� ������� ������� �� ������� ���� � ����� ��� ����������� ����
	void CopyData();
public:
	//������� �����������
	double curE = 10;

	//����, ���������� �� ���������� ���������� �����
	bool DataReady = false;

	//����, ���������� �� ������ �������� �����
	bool FurStart = false;

	//�������, ������� ��������� � ����� ����� ������ �������
	bool PutData();

	//������ ��������� �� F()
	complex<double>** GetF();

	//������ ��������� �� FFur()
	complex<double>*** GetFFur();

	//������ ��������� �� ������ � ���������
	vector<pair<double, int>> GetEnerges();

	//���������� ���������
	void Reset();

	//������ ��������� �� X
	double* GetX();

	//������ ��������� �� Y
	double* GetY();

	//������ ��������� �� f
	double* Getf();

	//���������� ��������� ������ � ��������� ������ �������
	void FindWave();

	//���������� ����������� ������� �������
	void FindFunc();

	//�������� ��������� ������
	void Update(int N, int M, double dt, double R, double a, double b, double U0, double f0, double asrx, double asry, double gammax, double gammay);

	//������� ����������� ������� � ���������� 
	void FindSF(int id);
};