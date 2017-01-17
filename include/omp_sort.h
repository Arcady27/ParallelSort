#ifndef OMPSORT_H 
#define OMPSORT_H 

#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <omp.h>
#include <ctime>
using namespace std;

#include "Point.h"

int omp_sort(Point **arr, int size, int axis, Point **tmp);
void DHsort(Point **arr, int size, int axis, int threads, Point **tmp);

#endif 