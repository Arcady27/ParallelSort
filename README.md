# ParallelSort
Hybrid parallel sort for system with distributed memory: it uses OpenMP for local sort (heap sort + merge sort) on each node and than MPI for Batcher merge sort between nodes.
Used data structure: point. It has x,y coordinates (float) and index (int).
Task formulation: sort array in distributed system. Array is 1D array but it represents the vertices of n1*n2 grid (indices are calculated accordingly to position in grid). Sorting in ditributed system is understood as following:
  1. Elements on all nodes are sorted along the chosen axis
  2. First element on each node is not smaller along the chosen axis than all elements on nodes with smaller node-ids
  3. Last element on each node is not larger along the chosen axis than all elements on nodes with greater node-ids

*Input (command line arguments)*:
  1. n1
  2. n2
  3. Axis along which the array is sorted. 0 is to sort array along x, 1 is to sort array along y.
  Grid elements are equally distibuted between nodes.
  
*Ouput*:
  Sorted array is placed on nodes

**Hybrid sort algorithm features**:
  1. Batcher sort requires the equal number of elements on all nodes. That's why fake elements with index = -1 are added if necessary.
  2. Local sort on nodes has following algorithm: heap sort of array parts of size 50000, then merge sorton these parts. OPenMP is used for both stages of local sort.
  3. Calculation of parallel algorithm running time, speedup and effectiveness is performed. 
