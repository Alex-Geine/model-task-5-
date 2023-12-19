#include "pch.h"
#include "WaveModel.h"

//���������� ��������� ������ � ��������� ������ �������
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

//��������� ������������� ���������
void WaveModel::InitData() {
	//������� ��������� ��� ��������
	double
		y1l = -height / 2,			//����� ������ ���� ��� ����� ��������		
		x1l = -D / 2 - width,
		y2l = y1l,					//������ ������ ���� ��� ����� ��������
		x2l = -D / 2,
		y3l = height / 2,			//������ ������� ���� ��� ����� ��������
		x3l = x2l,
		y4l = y3l,					//����� ������� ���� ��� ����� ��������
		x4l = x1l,
		y1r = y1l,					//����� ������ ���� ��� ������ ��������
		x1r = D / 2,
		y2r = y1l,					//������ ������ ���� ��� ������ �������� 
		x2r = D / 2 + width,
		y3r = y3l,					//������ ������� ���� ��� ������ ��������
		x3r = x2r,
		y4r = y3l,					//����� ������� ���� ��� ������ ��������
		x4r = x1r;


	//�������� ��������
	X = new double[N];
	Y = new double[M];
	Fpast = new double* [N];
	Fnow = new double* [N];
	MapOfModel = new int* [N];

	//������������� ��������������� �����
	stepX = 2 * R / N;
	stepY = 2 * R / M;
	double
		x0 = -R;

	for (int i = 0; i < N; i++)
		X[i] = x0 + i * stepX;

	for (int i = 0; i < M; i++)
		Y[i] = x0 + i * stepY;

	//��������� ������� �������������� ����������
	for (int i = 0; i < N; i++) {
		Fpast[i] = new double[M];
		Fnow[i] = new double[M];
		MapOfModel[i] = new int[M];
		
		for (int j = 0; j < M; j++) {
			
			//��� �������
			if ((i == 0) || (j == 0) || (i == N - 1) || (j == M - 1)) {
				MapOfModel[i][j] = BOARDS;
				Fpast[i][j] = 0;
				Fnow[i][j] = 0;
			}
			//��� ����� ��������
			else if ((X[i] >= x1l) && (X[i] <= x2l) && (Y[j] >= y1l) && (Y[j] <= y3l)) {
				MapOfModel[i][j] = LFACING;
				Fpast[i][j] = -phi0;
				Fnow[i][j] = -phi0;
			}
				
			//��� ������ �������
			else if ((X[i] >= x1r) && (X[i] <= x2r) && (Y[j] >= y1r) && (Y[j] <= y3r)){
				MapOfModel[i][j] = RFACING;
				Fpast[i][j] = phi0;
				Fnow[i][j] = phi0;
			}
			//��� ���������� ������������
			else {
				MapOfModel[i][j] = FIELD;
				Fpast[i][j] = 0;
				Fnow[i][j] = 0;
			}						
		}
	}

	//������������ ��� ������������� ��������� ��������
	a = 1 / ( stepX * stepX);
	b = a;
	c = 1 / ( stepY * stepY);
	d = c;
	e = 2 / (stepX * stepX) + 2 / (stepY * stepY);
}

//�������� ������� ������� �� ������� ���� � ����� ��� ����������� ����
void WaveModel::CopyData() {
	for (int i = 0; i < N; i++)
		for (int j = 0; j < M; j++)
			Fpast[i][j] = Fnow[i][j];
}

//������� ����� �����������
void WaveModel::Solve() {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			if (MapOfModel[i][j] == FIELD)
				Fnow[i][j] = (Fnow[i+1][j] + Fnow[i-1][j] + Fnow[i][j+1] + Fnow[i][j-1]) / 4.0;				
		}			
	}			
}

//�������� ��������� ������
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

//������ ��������� �� F
double** WaveModel::GetF() {
	return Fpast;
}

//������ ��������� �� X
double* WaveModel::GetX() {
	return X;
}

//������ ��������� �� Y
double* WaveModel::GetY() {
	return Y;
}

//������ ��������� �� ���
int** WaveModel::GetMap() {
	return MapOfModel;
}

//���������� ���������
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
