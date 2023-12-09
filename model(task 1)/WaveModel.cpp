#include "pch.h"
#include "WaveModel.h"

//���������� ��������� ������ � ��������� ������ �������
void WaveModel::FindWave() {
	InitData();
	
	
}

//��������� ������������� ���������
void WaveModel::InitData() {

	//�������� ��������
	X = new double[N];
	Y = new double[M];
	Fpast = new double*[N];
	Fnow = new double* [N];

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

		for (int j = 0; j < M; j++) {		
			Fpast[i][j] = 1;
			Fnow[i][j] = 0;
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
	for(int i = 1; i < N - 1; i++)
		for (int j = 1; j < M - 1; j++) {

		}
}

//�������� ��������� ������
void WaveModel::Update(int N, int M,  double dt, double R, double a, double b, double U0, double f0, double asrx, double asry, double gammax, double gammay) {
	this->N = N;
	this->M = M;
	this->dt = dt;
	this->R = R;
	this->a = a;
	this->b = b;
	this->U0 = U0;
	this->f0 = f0;
	this->aSrx = asrx;
	this->aSry = asry;
	this->gammax = gammax;
	this->gammay = gammay;
	//this->SFId = N / 2;
}



//������ ��������� �� F
complex<double>** WaveModel::GetF() {
	return Fpast;
}

//������ ��������� �� FFur
complex<double>*** WaveModel::GetFFur() {
	return FBuf;
}

//������ ��������� �� X
double* WaveModel::GetX() {
	return X;
}

//������ ��������� �� Y
double* WaveModel::GetY() {
	return Y;
}

//������ ��������� �� f
double* WaveModel::Getf() {
	return f;
}

//���������� ���������
void WaveModel::Reset()
{
	if ((Fpast != NULL)) {
		delete [] X;
		delete [] A;
		delete [] B;
		delete [] C;
		delete [] D;
		delete [] alpha;
		delete [] betta;

		/*for (int i = 0; i < IdMax; i++)
			delete [] F[i];
		for (int i = 0; i < N; i++)
			delete [] FFur[i];

		delete [] F;
		delete [] FFur;
		Id = 1;
		Energes.clear();*/
	}	
}
