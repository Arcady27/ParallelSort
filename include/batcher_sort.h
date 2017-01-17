#ifndef BATCHER_H    // если имя CPPSTUDIO_H ещё не определено
#define BATCHER_H 

#include <cassert>
#include <algorithm>
#include <vector>
#include <cstdio>
#include <set>
#include <iostream>
#include <cstdlib>
using namespace std;

#include "Point.h"

void merge_batcher(vector<pair< int,int> >&schedule, vector<int>&first_group, vector<int>&second_group);
void merge_batcher_parts(vector<pair<int, int> > &schedule, int first, int n0, int n1, int &comps);
void build_batcher_sort_net(vector<pair<int, int> > &schedule, int first, int last, int &comps);
int calc_batcher_tacts(vector<pair<int, int> > &schedule);

#endif