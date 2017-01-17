#ifndef POINT_H 
#define POINT_H 

#include <iostream>
using namespace std;

struct Point
{
	float coord[2];
	int index;

	float x (int i, int j);
	float y (int i, int j);

	void init (int i, int j, int n1, int n2, bool fake_index);

	Point& operator = (const Point &a);
}; 

ostream& operator << (ostream& os, const Point &a);

bool less_x (const Point &a, const Point &b);
bool less_equal_x (const Point &a, const Point &b);
bool less_y (const Point &a, const Point &b);
bool less_equal_y (const Point &a, const Point &b);
bool greater_x (const Point &a, const Point &b);
bool greater_y (const Point &a, const Point &b);

struct Comparator_x
{
	bool operator()(const Point &a, const Point &b)
	{
		return less_x(a, b);
	}
};

struct Comparator_y
{
	bool operator()(const Point &a, const Point &b) 
	{
		return less_y(a, b);
	}
};

#endif