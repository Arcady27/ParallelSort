#include <fstream>
#include <cassert>
#include <cmath>
using namespace std;

#include <mpi.h>

#include "omp_sort.h"
#include "batcher_sort.h"

float sort_benchmark(int rank, int nproc, int n1, int n2, int size_for_proc, int axis, const MPI_Datatype &point_datatype);
int calc_size_for_proc (int nproc, int n1, int n2);
Point* allocate_points(int rank, int nproc, int size_for_proc, int n1, int n2);
void sort_by_schedule(Point **arr, Point *other_arr, Point **buf_arr, int size, int axis, vector<pair<int, int> >&schedule, int rank, const MPI_Datatype &point_datatype);
void print_array(Point *arr, int size, int rank);
void validate_sort(Point *arr, int rank, int nproc, int axis, int size, const MPI_Datatype &point_datatype);
int main (int argc, char *argv[]);

float sort_benchmark(int rank, int nproc, int n1, int n2, int size_for_proc, int axis, const MPI_Datatype &point_datatype)
{
	if (rank != 0)
		return 0;
	int size = nproc * size_for_proc;
	cout<<size<<" "<<sizeof(Point)<<endl;
	Point *arr, *tmp;
	
	arr = allocate_points(0, 1, size, n1, n2);
	tmp = new Point[size];

	float start_time_serial = MPI_Wtime();
	
	if (rank == 0)
	{
		DHsort(&arr, size, axis, 1, &tmp);
	}
	
	float end_time_serial = MPI_Wtime();
	
	for (int i = 1; i < size; ++i)
	{
		if (axis == 0)
			assert(less_equal_x(arr[i-1], arr[i]));
		else if (axis == 1)
			assert(less_equal_y(arr[i-1], arr[i]));
	}

	delete[] arr;
	delete[] tmp;
	return end_time_serial - start_time_serial;
}

int calc_size_for_proc (int nproc, int n1, int n2)
{
	int size_for_proc, full_size;
	if ((n1 * n2) % nproc == 0)
		full_size = n1 * n2;
	else
		full_size = ((n1 * n2) / nproc + 1) * nproc;
	
	size_for_proc = full_size / nproc;
	return size_for_proc;
}

Point* allocate_points(int rank, int nproc, int size_for_proc, int n1, int n2)
{
	Point* arr;
	
	arr = new Point[size_for_proc];
	for (int i = 0; i < size_for_proc; ++i)
	{
		bool fake_index = false;
		if (rank * size_for_proc + i >= n1 * n2)
			fake_index = true;

		int global_ind = rank * size_for_proc + i; 
		arr[i].init(global_ind / n1, global_ind - n1 * (global_ind / n1), n1, n2, fake_index);
		
		//cout<<rank<<" "<<i<<" "<<" "<<n1<<" "<<n2<<" | "<<arr[i];
	}
	return arr;
}

void sort_by_schedule(Point **arr, Point *other_arr, Point **buf_arr, int size, int axis, vector<pair<int, int> >&schedule, int rank, const MPI_Datatype &point_datatype)
{
	MPI_Status status[schedule.size() - 1][2];
	MPI_Request req[schedule.size() - 1][2];
	
	for (int t = 0; t < schedule.size(); ++t)
	{
		int first_ind = schedule[t].first, second_ind = schedule[t].second;
		if (rank == first_ind || rank == second_ind)
		{
			int other_rank;
			if (rank == first_ind)
				other_rank = second_ind;
			else
				other_rank = first_ind;

			// MPI_Sendrecv(*arr, size, point_datatype, other_rank, 0,
			// 	other_arr, size, point_datatype, other_rank, 0, 
			// 	MPI_COMM_WORLD, &status[0][0]);

			MPI_Irecv(other_arr, size, point_datatype, other_rank, 0, MPI_COMM_WORLD, &req[t][1]);  				
			MPI_Isend(*arr, size, point_datatype, other_rank, 0, MPI_COMM_WORLD, &req[t][0]); 

			MPI_Wait(&req[t][1], &status[t][1]);

			if (rank == first_ind)
			{
				int i = 0, j = 0, ind = 0;
				while (ind < size)
				{
					if (i == size)
						(*buf_arr)[ind] = other_arr[j++];
					
					else if (j == size)
						(*buf_arr)[ind] = (*arr)[i++];

					else 
					{
						if ( (axis == 0 && less_x((*arr)[i], other_arr[j])) || (axis == 1 && less_y((*arr)[i], other_arr[j])))
							(*buf_arr)[ind] = (*arr)[i++];
						else
							(*buf_arr)[ind] = other_arr[j++];
					}
					//cout<<*buf_arr[ind]; 
				
					ind++; 
				}

			}
			else
			{
				int i = size - 1, j = size - 1, ind = size - 1;
				while (ind >= 0)
				{
					if (i == -1)
						(*buf_arr)[ind] = other_arr[j--];
					
					else if (j == -1)
						(*buf_arr)[ind] = (*arr)[i--];

					else 
					{
						if ( (axis == 0 && greater_x((*arr)[i], other_arr[j])) || (axis == 1 && greater_y((*arr)[i], other_arr[j])))
							(*buf_arr)[ind] = (*arr)[i--];
						else
							(*buf_arr)[ind] = other_arr[j--];
					}			
					ind--; 
				}
			}

			MPI_Wait(&req[t][0], &status[t][0]);
			Point *tmp = *arr;
			*arr = *buf_arr;
			*buf_arr = tmp;
			//for (int i=0;i<size;++i)
			// 	cout<<rank<<" "<<(*arr)[i];
		}
	}
}

void print_array(Point *arr, int size, int rank)
{
	char filename[50];
	sprintf (filename, "outputs/proc%d", rank);
	fstream out;
	out.open(filename, ofstream::out);
	out<<"Rank: "<<rank<<endl;
	for (int i = 0; i < size; ++i)
		out<<i<<": "<<arr[i];
	out.close();
}

void validate_sort(Point *arr, int rank, int nproc, int axis, int size, const MPI_Datatype &point_datatype)
{
	for (int i = 1; i < size; ++i)
	{
		if (axis == 0)
			assert(less_equal_x(arr[i-1], arr[i]));
		else if (axis == 0)
			assert(less_equal_y(arr[i-1], arr[i]));
	}

	MPI_Status status[4]; 
	MPI_Request req[4]; 
	Point left, right;
	
	if (rank >= 1) 
	{ 
		MPI_Isend(&arr[0], 1, point_datatype, rank - 1, 0, MPI_COMM_WORLD, &req[0]); 
		MPI_Irecv(&left, 1, point_datatype, rank - 1, 0, MPI_COMM_WORLD, &req[1]);       
	} 

	if (rank <= nproc-2) 
	{ 
		MPI_Isend(&arr[size - 1], 1, point_datatype, rank + 1, 0, MPI_COMM_WORLD, &req[2]); 
		MPI_Irecv(&right, 1, point_datatype, rank + 1, 0, MPI_COMM_WORLD, &req[3]);       
	} 
		
	if (rank == nproc-1 && nproc >= 2) 
	{
		MPI_Waitall(2, req, status); 
		if (axis == 0)
			assert(less_equal_x(left, arr[0]));
		else if (axis == 1)
			assert(less_equal_y(left, arr[0]));
	}
	if (rank == 0 && nproc >= 2) 
	{
		MPI_Waitall(2, req+2, status+2); 
		if (axis == 0)
			assert(less_equal_x(arr[size - 1], right));
		else if (axis == 1)
			assert(less_equal_y(arr[size - 1], right));
	}
	
	if (0 < rank && rank < nproc-1 && nproc >= 3)
	{
		MPI_Waitall(4, req, status); 
		if (axis == 0)
		{
			//cout<<left <<" l| "<<arr[size-1]<<" |r "<<right;
			assert(less_equal_x(left, arr[0]));
			assert(less_equal_x(arr[size - 1], right));
		}
		else if (axis == 1)
		{
			assert(less_equal_y(left, arr[0]));
			assert(less_equal_y(arr[size - 1], right));
		}
	}


	printf("PROC %d SORT SUCCESFULLY VALIDATED\n", rank);
}	

void full_sort(int argc, char *argv[])
{
	srand(0);
	MPI_Init(&argc, &argv);

	int n1 = atoi(argv[1]);
	int n2 = atoi(argv[2]);
	int axis = atoi(argv[3]); //0 - x, 1 - y
	
	const int n_fields = 2;
	int block_lengths[2] = {2, 1};
	MPI_Datatype types[2] = {MPI_FLOAT, MPI_INT};
	MPI_Datatype mpi_point_type;
	MPI_Aint offsets[2];

	offsets[0] = 0;
	offsets[1] = 2 * sizeof(float);
	//cout<<offsets[0]<<" "<<offsets[1]<<endl;
	MPI_Type_create_struct(n_fields, block_lengths, offsets, types, &mpi_point_type);
	MPI_Type_commit(&mpi_point_type);

	int nproc, rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	int size_cur_proc;

	int size_for_proc = calc_size_for_proc(nproc, n1, n2);
	Point *arr = allocate_points(rank, nproc, size_for_proc, n1 ,n2);
	Point *another_arr = new Point[size_for_proc], *buf_arr;
	if (nproc > 1)
		buf_arr = new Point[size_for_proc];

	MPI_Barrier(MPI_COMM_WORLD);
	float start_time = MPI_Wtime();
	
	omp_sort(&arr, size_for_proc, axis, &another_arr);
	//std::sort(arr, arr + size_for_proc, Comparator_x()); 
	cout << rank << " -- " << MPI_Wtime() - start_time<<endl;
	float one_proc_time = MPI_Wtime() - start_time;

	int comps = 0;
	vector<pair<int, int> > schedule;
	build_batcher_sort_net(schedule, 0, nproc - 1, comps);
	int tacts = calc_batcher_tacts(schedule);
	sort_by_schedule (&arr, another_arr, &buf_arr, size_for_proc, axis, schedule, rank, mpi_point_type);
	MPI_Barrier(MPI_COMM_WORLD);
	float end_time = MPI_Wtime();
	float elapsed_time_parallel = end_time - start_time;
	
	//for (int i=0;i<size_for_proc;++i)
	//	cout<<rank<<" "<<arr[i];

	//print_array(arr, size_for_proc, rank);
	validate_sort(arr, rank, nproc, axis, size_for_proc, mpi_point_type);
	MPI_Barrier(MPI_COMM_WORLD);
	delete[] another_arr;
	delete[] buf_arr;
	delete[] arr;
	float elapsed_time_serial = 0;
	if (rank == 0)
		elapsed_time_serial = sort_benchmark(rank, nproc, n1, n2, size_for_proc, axis, mpi_point_type);

	if (rank == 0)
	{
		fstream out;
		char filename[100];
		sprintf(filename,"time-%d.txt",nproc);
		out.open(filename, ofstream::out);
		char buffer [100];
		sprintf(buffer, "N1:%d N2:%d\n", n1, n2);
		out<<string(buffer);
		sprintf(buffer, "TOTAL SERIAL SORT TIME: %.3f\n", elapsed_time_serial);
		out<<string(buffer);
		sprintf(buffer, "ONE PROCESS SORT TIME: %.3f\n", one_proc_time);
		out<<string(buffer);
		sprintf(buffer, "TOTAL PARALLEL SORT TIME: %.3f | PROCESSES: %d | TACTS: %d\n", elapsed_time_parallel, nproc, tacts);
		out<<string(buffer);
		
		float s = elapsed_time_serial / elapsed_time_parallel;
		int n = size_for_proc * nproc;
		float e_max = log2(n) / (log2(n) + tacts - log2(nproc));
		float e = s / nproc;
		float s_max = e_max * nproc;
		
		sprintf(buffer, "S: %.3f | S_max: %.3f | S/S_max: %.3f\n", s, s_max, s / s_max);
		out<<string(buffer);
		sprintf(buffer, "E: %.3f | E_max: %.3f | E/E_max: %.3f\n", e, e_max, e / e_max);
		out<<string(buffer);
		
		out.close();
	}
	

	MPI_Type_free(&mpi_point_type);
	
	MPI_Finalize();
}

int main (int argc, char *argv[])
{
	full_sort(argc, argv);
	return 0;
}
