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
	MapOfModel = mod->GetMap();
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
	MakeForceLines();

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
	Pen pen5(Color::Orange, 0.002);
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

	//отрисовка силовых линий
	int count = 0;
	for (int i = 0; i < 10000; i++) {
		int size = QuantTraces[i].size();
		//for (int j = 0; j < QuantCount; j++) {
			for (int k = 0; k < size; k++) {
				gr.DrawLine(&pen5, QuantTraces[i][k].first, QuantTraces[i][k].second);
			}
			count++;
		//}			
	}

	Graphics grnew(Item1->hDC);
	grnew.DrawImage(&Image, 0, 0);
}

//подготавливает данные для отрисовки 2d
void Controller::PrepareData2d() {
	MaxF = 0;
	int size = 0;

	if (!poligReady) {
		Poligon* f;
		MapOfPoligon = new int[(N - 1) * (M - 1)];
		
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

				//заполняем карту полигонов
				if (MapOfModel[i][j] == FIELD && MapOfModel[i + 1][j] == FIELD && MapOfModel[i][j + 1] == FIELD && MapOfModel[i + 1][j + 1] == FIELD)
					MapOfPoligon[size] = FIELD;
				else
					MapOfPoligon[size] = OTHER;				

				f = new Poligon{ p1, p2,p3,p4 };
				polig.push_back(f);
				size++;
			}
		}
		poligReady = true;
	}
	else {		
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

//находит силовые силии
void Controller::MakeForceLines() {
	double Step = abs(X[1] - X[0]);
	double masx[4] = { X[2], X[M], X[M - 3], X[N - 3] };
	double masy[4] = { Y[2], Y[M - 3], Y[2], Y[2] };
	double x, y;

	int count = 0;
	//for (int i = 0; i < 4; i++) {
	//	for (int j = 0; j < QuantCount; j++) {
	//		QuantTraces[count].clear();
	//		
	//		if (i == 0) {	//Нижняя и верхняя стенки
	//			x = masx[i] + j * Step;
	//			y = masy[i];				
	//		}
	//		else if (i == 1) {
	//			x = masx[i];
	//			y = masy[i] - j * Step;
	//		}
	//		else if (i == 2) {
	//			x = masx[i];
	//			y = masy[i] + j * Step;
	//		}

	//		else {			//Правая и левая стенки
	//			x = masx[i];
	//			y = masy[i] - j * Step;
	//		}
	//		StartQuantum(x, y, count);
	//		count++;
	//	}
	//}
	for (int i = 2; i < N - 2; i++) {
		for (int j = 2; j < M - 2; j++){
			QuantTraces[count].clear();

			x = X[i];
			y = Y[j];

			StartQuantum(x, y, count);
			count++;
		}
	}
	
}

//запускает пробную частицу
void Controller::StartQuantum(double x, double y, int count) {
	//находим полигон, в котором точка находится изначально
	int curPolig = FindStartPoligon(x, y);

	if (curPolig == -1)
		return;

	//если попали на границу или обкладку
	if (MapOfPoligon[curPolig] == OTHER)
		return;
	
	//ходим по этому полигону и получаем точку выхода из него
	PointF p = MovePoligon(curPolig, count, x, y);

	curPolig = FindStartPoligon(p.X, p.Y);
	if (curPolig == -1)
		return;

	//ходим по полигонам, пока не попадем на границу или обкладку
	while (MapOfPoligon[curPolig] == FIELD) {
		//ходим по этому полигону и получаем точку выхода из него
		p = MovePoligon(curPolig, count, p.X, p.Y);
		curPolig = FindStartPoligon(p.X, p.Y);
		if (curPolig == -1)
			return;
	}
}

//движение частицы по полигону
PointF Controller::MovePoligon(int countPolig, int countTrace, double x, double y) {
	PointF
		p1(x, y),
		p2;
	//делим на два треугольника
	Triangle t1(polig[countPolig]->p1, polig[countPolig]->p2, polig[countPolig]->p4, 0), t2(polig[countPolig]->p2, polig[countPolig]->p3, polig[countPolig]->p4, 1), curt;

	//находим, в каком частица
	if (t1.Check(p1.X, p1.Y))
		curt = t1;
	else
		curt = t2;

	//двигаемся по градиенту, пока не выйдем за границы трегольника
	p2.X = p1.X;
	p2.Y = p1.Y;

	MoveTriangle(curt, p2);

	QuantTraces[countTrace].push_back({ p1,p2 });	//загоняем две точки в траеткорию

	p1.X = p2.X;
	p1.Y = p2.Y;

	while (t1.Check(p1.X, p1.Y) || t2.Check(p1.X, p1.Y)) {
		if (t1.Check(p1.X, p1.Y)) {
			curt = t1;
			MoveTriangle(curt, p2);

			QuantTraces[countTrace].push_back({ p1,p2 });	//загоняем две точки в траеткорию
		}
		else if (t2.Check(p1.X, p1.Y)) {
			curt = t2;
			MoveTriangle(curt, p2);

			QuantTraces[countTrace].push_back({ p1,p2 });	//загоняем две точки в траеткорию
		}

		p1.X = p2.X;
		p1.Y = p2.Y;
	}

	////проверяем, не ушла ти точка в соседний треугольник
	//if (t1.Check(p1.X, p1.Y))
	//	MessageBox(NULL, L"Рекурсия! Частица в том же треугольнике", L"", NULL);
	//else if (t2.Check(p1.X, p1.Y)) {
	//	curt = t2;
	//	MoveTriangle(curt, p2);

	//	QuantTraces[countTrace].push_back({ p1,p2 });	//загоняем две точки в траеткорию
	//}
	
	return p2;
}

//движение частицы по треугольнику
void Controller::MoveTriangle(Triangle t, PointF& p) {
	PointF grad = t.GetGradient();	//нужно реализовать
	double step = sqrt((abs(polig[0]->p1.x() - polig[0]->p1.x()) + abs(polig[0]->p1.y() - polig[0]->p4.y()))) / 20;

	while (t.Check(p.X, p.Y)) {
		p.X += grad.X * step;
		p.Y += grad.Y * step;
	}
}

//находит полигон в начальный момент времени
int Controller::FindStartPoligon(double x, double y) {
	/*Triangle t1, t2;*/
	for (int i = 0; i < polig.size(); i++) {
		////разбили треугольник на прямоугольники
		//t1.mas[0] = polig[i]->p1;
		//t1.mas[1] = polig[i]->p2;
		//t1.mas[2] = polig[i]->p3;

		//t2.mas[0] = polig[i]->p2;
		//t2.mas[1] = polig[i]->p3;
		//t2.mas[2] = polig[i]->p4;

		////проверяем, есть ли в треугольниках частица
		//if (t1.Check(x, y))
		//	return i;
		//else if (t2.Check(x, y))
		//	return i;
		if (x >= polig[i]->p1.x() && x <= polig[i]->p2.x() && y >= polig[i]->p1.y() && y <= polig[i]->p4.y())
			return i;
	}

	return -1;
}

//находит рядом стоящий полигон
int Controller::FindNearPoligon(int cur, double x, double y) {
	//смотрим размерность матрицы полигонов
	int rate = M - 1;
	//while (!(polig.size() % (rate * rate)))
		//rate++;

	int curi = 0, curj = 0;
	//находим положение нашего полигона
	for (int i = 0; i < rate; i++)
		for (int j = 0; j < rate; j++)
			if (cur == i * rate + j)
				curi = i, curj = j;

	pair<int, int> poligIds[8] = {
		{curi - 1, curj - 1}, {curi - 1, curj}, {curi - 1, curj + 1}, {curi, curj + 1},
		{curi, curj - 1}, {curi + 1, curj - 1}, {curi + 1, curj}, {curi + 1, curj + 1}
	};

	Triangle t1, t2;
	//проверяем эти 8 полигонов
	for (int i = 0; i < 8; i++) {
		//разбили треугольник на прямоугольники
		t1.mas[0] = polig[poligIds[i].first * rate + poligIds[i].second]->p1;
		t1.mas[1] = polig[poligIds[i].first * rate + poligIds[i].second]->p2;
		t1.mas[2] = polig[poligIds[i].first * rate + poligIds[i].second]->p3;

		t2.mas[0] = polig[poligIds[i].first * rate + poligIds[i].second]->p2;
		t2.mas[1] = polig[poligIds[i].first * rate + poligIds[i].second]->p3;
		t2.mas[2] = polig[poligIds[i].first * rate + poligIds[i].second]->p4;

		//проверяем, есть ли в треугольниках частица
		if (t1.Check(x, y))
			return poligIds[i].first * rate + poligIds[i].second;
		else if (t2.Check(x, y))
			return poligIds[i].first * rate + poligIds[i].second;
	}


	return -1;
}

//очищает данные
void Controller::Clear() {
	if (mod != nullptr) {
		TerminateThread(TREAD, 0);
		TREAD == NULL;

		X = nullptr;
		Y = nullptr;
		F = nullptr;

		MapOfModel = nullptr;
		
		delete[] MapOfPoligon;

		MapOfPoligon = nullptr;
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