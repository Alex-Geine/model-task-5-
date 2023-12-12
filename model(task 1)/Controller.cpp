#include "pch.h"
#include "Controller.h"
#include <algorithm>
#include <string>

using namespace std;

#define _USE_MATH_DEFINES
#include  <math.h>

//поток, ждущий сообщений
DWORD WINAPI Controller::ModelFunk() {
	mod->Start();
	return 0;
}

//забирает данные из модели
void Controller::GetData() {
	F = mod->GetF();	
	X = mod->GetX();
	Y = mod->GetY();	
}

//запусткает вычисления
void Controller::StartSolve() {
	TREAD = CreateThread(NULL, NULL, StaticModelFunk, (void*)this, 0, NULL);
	MessageBoxW(NULL, L"START!", L"START!", NULL);
	//mod->Start();
}

//запускает отрисовку 2d графика
void Controller::DrawMainGr(LPDRAWITEMSTRUCT Item1) {
	PrepareData2d();
	MakeIsolines();

	double ItemWidth = Item1->rcItem.right - Item1->rcItem.left,
		ItemHeight = Item1->rcItem.bottom - Item1->rcItem.top;

	//ïàðàìåòðû ãðàôèêà â ìèðîâûõ êîîðèíàòàõ
	double top = 2 * R;
	double bottom = 0;
	double left = 0;
	double right = 2 * R;

	double
		steplenY = (top - bottom) / 10,
		steplenX = (right - left) / 10;

	double xScale = ItemWidth / (right + right / 15 - left);
	double yScale = -ItemHeight / (top + top / 5 - bottom);



	Bitmap Image(ItemWidth, ItemHeight);
	Graphics gr(&Image);
	gr.Clear(Color::White);
	//ñãëàæèâàíèå
	gr.SetSmoothingMode(SmoothingModeAntiAlias);

	//ïðåîáðàçîâàíèå ìèðà(ïåðåõîä ê êîîðäèíàòàì ñàìîãî ãðàôèêà
	Matrix matr;
	matr.Scale(xScale, (yScale + 1)); //ìàøòàáèðîâàíèå
	matr.Translate(right / 2, -top / 2); //ïåðåíîñ íà÷àëà êîîðäèíàò


	gr.SetTransform(&matr);


	Pen BackGroundPen(Color::DarkGray, 0.005);
	Pen pen(Color::BlueViolet, 0.006);
	Pen pen3(Color::BlueViolet, 0.002);
	Pen pen4(Color::White, 0.002);
	Pen pen2(Color::Green, 0.006);

	Pen StickPen(Color::CornflowerBlue, 0.02);
	SolidBrush brush(Color::Black);
	SolidBrush PlatformBrush(Color::Crimson);
	Gdiplus::Font font((L"Times New Roman"), 14, FontStyle::FontStyleRegular, UnitPixel);


	//Ðàçìåòêà
	for (int i = 0; i < 10; i++)
	{
		//ãîðèçîíòàëüíàÿ
		gr.DrawLine(&BackGroundPen, PointF(-right / 2, top / 2 - i * steplenY), PointF(right / 2, top / 2 - i * steplenY));

		//âåðòèêàëüíàÿ
		gr.DrawLine(&BackGroundPen, PointF(-right / 2 + i * steplenX, top / 2), PointF(-right / 2 + i * steplenX, -top / 2));
	}

	//Áîëüøèå îñè
	//ãîðèçîíòàëüíàÿ
	gr.DrawLine(&pen, PointF(-right / 2, -top / 2), PointF(right / 2, -top / 2));

	//âåðòèêàëüíàÿ
	gr.DrawLine(&pen, PointF(right / 2, top / 2), PointF(right / 2, -top / 2));

	gr.ResetTransform();

	ColorTable ct(MaxF);

	//ïîäïèñè ê îñÿì
	for (int i = 0; i < 11; i++)
	{
		CString str;

		//ïî Y
		str.Format(L"%.2f", top / 2 - i * steplenY);
		PointF strPoint(right / 2, top / 2 - i * steplenY);
		matr.TransformPoints(&strPoint);
		gr.DrawString(str, str.GetLength() + 1, &font, strPoint, &brush);

		//ïî X
		str.Format(L"%.2f", right / 2 - i * steplenX);
		strPoint.X = right / 2 - i * steplenX;
		strPoint.Y = -top / 2;
		matr.TransformPoints(&strPoint);
		gr.DrawString(str, str.GetLength() + 1, &font, strPoint, &brush);

		//äëÿ ïàëèòðû
		str.Format(L"%.2f", ct.mas[i]);
		strPoint.X = -right / 2 + i * steplenX;
		strPoint.Y = -(top) / 2 - steplenY / 1.7;
		matr.TransformPoints(&strPoint);
		gr.DrawString(str, str.GetLength() + 1, &font, strPoint, &brush);
	}

	gr.SetTransform(&matr);

	//Îòðèñîâêà ãðàôèêà


	Color color(Color::IndianRed);
	Pen StickPen1(color, 0.001);



	double
		leftkv = -right / 2,
		rightkv = leftkv + steplenX;

	double
		ykv1 = -(top + top / 5) / 2 - steplenY,
		ykv2 = ykv1 + steplenY;

	//îòðèñîâêà öâåòîâîé ïàëèòðû
	PointF* p = new PointF[4];
	for (int i = 0; i < ct.Size - 1; i++) {
		p[0] = PointF(leftkv, ykv1);
		p[1] = PointF(rightkv, ykv1);
		p[2] = PointF(rightkv, ykv2);
		p[3] = PointF(leftkv, ykv2);

		gr.FillPolygon(ct.brashes[i], p, 4);
		gr.DrawPolygon(&pen, p, 4);

		leftkv += steplenX;
		rightkv += steplenX;
	}


	//îòðèñîâêà ïàêåòà	
	for (int i = 0; i < polig.size(); i++) {
		double sum = (polig[i]->p1.z() + polig[i]->p2.z() + polig[i]->p3.z() + polig[i]->p4.z()) / 4;
		p[0] = PointF(polig[i]->p1.x(), polig[i]->p1.y());//(y1, Func(x1, y1));
		p[1] = PointF(polig[i]->p2.x(), polig[i]->p2.y());//(y2, Func(x2, y2));
		p[2] = PointF(polig[i]->p3.x(), polig[i]->p3.y());//(y3, Func(x3, y3));//
		p[3] = PointF(polig[i]->p4.x(), polig[i]->p4.y());//(y4, Func(x4, y4));//

		//	if (background)
		gr.FillPolygon(ct.GetBrush(sum), p, 4);
		//gr.DrawPolygon(&pen3, p, 4);
	}

	//îòðèñîâêà èçîëèíèé
	for (int i = 0; i < 11; i++)
		for (int j = 0; j < Isolines[i].size(); j++)
			gr.DrawLine(&pen4, Isolines[i][j].first, Isolines[i][j].second);


	Graphics grnew(Item1->hDC);
	grnew.DrawImage(&Image, 0, 0);
}

//подготавливает данные для отрисовки 2d
void Controller::PrepareData2d() {
	MaxF = 0;
	if (!poligReady) {
		Poligon* f;
		for (int i = 0; i < N - 1; i++) {
			for (int j = 0; j < M - 1; j++) {
				vec4
					p1((float)X[i], (float)Y[j], (float)(F[i][j])),
					p2((float)X[i + 1], (float)Y[j], (float)(F[i + 1][j])),
					p3((float)X[i + 1], (float)Y[j + 1], (float)(F[i + 1][j + 1])),
					p4((float)X[i], (float)Y[j + 1], (float)(F[i][j + 1]));

				//находим максимальное значение по оси z
				if (MaxF < p1.z())
					MaxF = p1.z();
				if (MaxF < p2.z())
					MaxF = p2.z();
				if (MaxF < p3.z())
					MaxF = p3.z();
				if (MaxF < p4.z())
					MaxF = p4.z();

				

				f = new Poligon{ p1, p2,p3,p4 };
				polig.push_back(f);
			}
		}
		poligReady = true;
	}
	else {
		int size = 0;
		for (int i = 0; i < N - 1; i++) {
			for (int j = 0; j < M - 1; j++) {
				polig[size]->p1.Setz((float)(F[i][j]));
				polig[size]->p2.Setz((float)(F[i + 1][j]));
				polig[size]->p3.Setz((float)(F[i + 1][j + 1]));
				polig[size]->p4.Setz((float)(F[i][j + 1]));

				polig[size]->p1.Setx((float)X[i]);
				polig[size]->p2.Setx((float)X[i + 1]);
				polig[size]->p3.Setx((float)X[i + 1]);
				polig[size]->p4.Setx((float)X[i]);

				polig[size]->p1.Sety((float)Y[j]);
				polig[size]->p2.Sety((float)Y[j]);
				polig[size]->p3.Sety((float)Y[j + 1]);
				polig[size]->p4.Sety((float)Y[j + 1]);

				//находим максимальное значение по оси z
				if (MaxF < polig[size]->p1.z())
					MaxF = polig[size]->p1.z();
				if (MaxF < polig[size]->p2.z())
					MaxF = polig[size]->p2.z();
				if (MaxF < polig[size]->p3.z())
					MaxF = polig[size]->p3.z();
				if (MaxF < polig[size]->p4.z())
					MaxF = polig[size]->p4.z();


				size++;
			}
		}
	}
}

//находит изолинии (работает только после PrepareData)
void Controller::MakeIsolines() {
	//буфер для возвращаемого значения
	pair<PointF, PointF> buf;

	//уровни для изотерм
	ColorTable ct(MaxF);

	for (int i = 0; i < 11; i++) {
		//отчистка предыдущих значений
		Isolines[i].clear();

		//перебор всех полигонов
		for (int j = 0; j < polig.size(); j++)
			if (CheckPoligon(ct.mas[i], buf, *polig[j]))
				Isolines[i].push_back(buf);
	}
}

//проверяет полигон для поиска изолинии
bool Controller::CheckPoligon(double T, pair<PointF, PointF>& buf, Poligon pol) {
	bool res = false;

	double maxX, maxY, minX, minY, maxZ, minZ;

	double x[4] = { pol.p1.x(), pol.p2.x(), pol.p3.x(), pol.p4.x() };
	double y[4] = { pol.p1.y(), pol.p2.y(), pol.p3.y(), pol.p4.y() };
	double z[4] = { pol.p1.z(), pol.p2.z(), pol.p3.z(), pol.p4.z() };

	PointF p;

	//проверка 4х граней полигона
	for (int i = 0; i < 4; i++) {
		int idnext = i + 1;
		if (i == 3)
			idnext = 0;

		if (((z[i] < T) && (z[idnext] > T) || ((z[i] > T) && (z[idnext] < T)))) {

			//находим максимальное и минимальное значение по z
			if (z[i] > z[idnext]) {
				maxX = x[i];
				maxY = y[i];
				maxZ = z[i];
				minX = x[idnext];
				minY = y[idnext];
				minZ = z[idnext];
			}
			else {
				minX = x[i];
				minY = y[i];
				minZ = z[i];
				maxX = x[idnext];
				maxY = y[idnext];
				maxZ = z[idnext];
			}

			//находим значения пересечения по x и по y
			p.X = maxX - (maxX - minX) * (maxZ - T) / (maxZ - minZ);
			p.Y = maxY - (maxY - minY) * (maxZ - T) / (maxZ - minZ);

			if (!res)
				buf.first = p;
			else
				buf.second = p;

			res = true;
		}
	}


	return res;
}


//очищает данные
void Controller::Clear() {
	if (mod != nullptr) {
		TerminateThread(TREAD, 0);
		TREAD == NULL;

		X = nullptr;
		Y = nullptr;
		F = nullptr;
		poligReady = false;
		polig.clear();
		mod->Reset();
		delete mod;
		mod = nullptr;
	}	
}

void Controller::UpdateModel(
	int N,				//количество точек по оси x
	int M,				//количество точек по оси y
	double	D,			//расстояние межу обкладками
	double height,		//высота обкладок
	double width,		//ширина обкладок
	double phi0,		//потенциал на обкладках		
	double R,			//размер поля для моделирования
	double err			//допустимая погрешность
	) 
{
	mod->Update(N, M, D, height, width, phi0, R, err);
	mod->InitData();
	this->R = R;
	this->N = N;
	this->M = M;	
}