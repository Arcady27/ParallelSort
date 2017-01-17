#include "Point.h"
#include <iostream>
#include <iomanip>

float Point::x(int i, int j)
{
	return j;
}

float Point::y(int i, int j)
{
	return i;
}	

void Point::init(int i, int j, int n1, int n2, bool fake_index = false)
{
	if (!fake_index)
		index = n1 * i + j;
	else
		index = -1;
	//cout<<index<<" "<<i<<" "<<j<<endl;
	
	coord[0] = x(i, j);
	coord[1] = y(i, j);	
}

Point& Point::operator = (const Point &a)
{
	index = a.index;
	coord[0] = a.coord[0];
	coord[1] = a.coord[1];

	return *this;
}

ostream& operator << (ostream& os, const Point &a)
{
    os << fixed << setprecision(3) << a.index << ' ' << a.coord[0] << ' ' << a.coord[1] << endl;
    return os;
}

bool less_x (const Point &a, const Point &b)
{
	if (a.index == -1 && b.index != -1)
		return true;
	if (b.index == -1)
		return false;

	return (a.coord[0] < b.coord[0]);
}

bool less_equal_x (const Point &a, const Point &b)
{
	if (a.index == -1)
		return true;
	if (b.index == -1)
		return false;

	return (a.coord[0] <= b.coord[0]);
}

bool less_y (const Point &a, const Point &b)
{
	if (a.index == -1 && b.index != -1)
		return true;
	if (b.index == -1)
		return false;

	return (a.coord[1] < b.coord[1]);
}

bool less_equal_y (const Point &a, const Point &b)
{
	if (a.index == -1)
		return true;
	if (b.index == -1)
		return false;

	return (a.coord[1] <= b.coord[1]);
}

bool greater_x (const Point &a, const Point &b)
{
	if (b.index == -1 && a.index != -1)
		return true;
	if (a.index == -1)
		return false;

	return (a.coord[0] > b.coord[0]);
}

bool greater_y (const Point &a, const Point &b)
{
	if (b.index == -1 && a.index != -1)
		return true;
	if (a.index == -1)
		return false;

	return (a.coord[1] > b.coord[1]);
}
