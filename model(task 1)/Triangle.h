#pragma once
#include "math3d.h"

using namespace math3d;

struct Triangle {
public:
	vec4 mas[3];
	int type;		//отвечает за расположение треугольника в пространстве

	vec4 a, b;		//координаты вектора

	Triangle(vec4 a, vec4 b, vec4 c, int type) :type(type) {
		mas[0] = a, mas[1] = b, mas[2] = c;

		//задаем координаты направл€ющей пары правой тройки векторов
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
	Triangle(double ax,double ay, double bx, double by, double cx, double cy) {
		vec4 a(ax, ay, 0), b(bx, by, 0), c(cx, cy, 0);
		mas[0] = a, mas[1] = b, mas[2] = c;				
	}

	Triangle() {}

	//функци€ проыер€ет, лежит ли точка в треугольнике
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

	//‘ункци€, возвращающа€ градиент
	PointF GetGradient() {
		//посчитаем векторнок произведение дл€ направл€ющих координат, получим вектор градиент
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