#include "omp_sort.h"
#include "Point.h"


int omp_sort(Point **a, int size, int axis, Point **tmp)
{
	int threads = omp_get_max_threads();
	omp_set_nested(1);
	DHsort(a, size, axis, threads, tmp);

	return 0;
}

void DHsort(Point **arr, int size, int axis, int threads, Point **tmp)
{
	Point *buf;
	int small_part = 50000;
	omp_set_num_threads(threads);

	#pragma omp parallel schedule(dynamic, 1000) for
	for(int i = 0; i < size; i += small_part)
	{
		int s = min(small_part, size - i);
		if (axis == 0)
		{
			make_heap(*arr + i, *arr + i + s, Comparator_x());
			sort_heap(*arr + i, *arr + i + s, Comparator_x());
		}
		else
		{
			make_heap(*arr + i, *arr + i + s, Comparator_y());
			sort_heap(*arr + i, *arr + i + s, Comparator_y());
		}
	}

	for(int i = small_part; i < size; i *= 2)
	{
		#pragma omp parallel schedule(dynamic, 1000) for 
		for(int j = 0; j < size; j = j + 2 * i)
		{
			int r = j + i;
			int n1 = max(0, min(i, size - j));
			int n2 = max(0, min(i, size - r));
			
			#pragma omp parallel schedule(dynamic, 1000) sections
			{
				#pragma omp section
				{
					int ind1 = 0, ind2 = 0, k = 0;
				
					for(; k < n1; k++)
					{
						if(ind1 >= n1)
							(*tmp)[j + k] = (*arr)[r + ind2++];
						
						else if (ind2 >= n2)
							(*tmp)[j + k] = (*arr)[j + ind1++];
						
						else if (axis == 0 && less_x((*arr)[j + ind1], (*arr)[r + ind2]) || axis == 1 && less_y((*arr)[j + ind1], (*arr)[r + ind2]) )
							(*tmp)[j + k] = (*arr)[j + ind1++];
						
						else
							(*tmp)[j + k] = (*arr)[r + ind2++];

					}
				}

				#pragma omp section
				{
					int ind1 = n1-1, ind2 = n2-1, k = n1+n2-1;
				
					for(; k >= n1; k--)
					{
						if(ind1 < 0)
							(*tmp)[j + k] = (*arr)[r + ind2--];
						
						else if (ind2 < 0)
							(*tmp)[j + k] = (*arr)[j + ind1--];
						
						else if (axis == 0 && greater_x((*arr)[j + ind1], (*arr)[r + ind2]) || axis == 1 && greater_y((*arr)[j + ind1], (*arr)[r + ind2]) )
							(*tmp)[j + k] = (*arr)[j + ind1--];
						
						else
							(*tmp)[j + k] = (*arr)[r + ind2--];

					}
				}

			}
		}
		Point *buf = *arr;
		*arr = *tmp;
		*tmp = buf;
	}
} 
