# KMEAN-CLUSTERING-using-parrallel-algorithms

The K-Means algorithm is implemented to cluster a matrix into K clusters. 

The input image used is grayscale image of dimension 800x800 stored in *input.raw* file. Two different cluster sizes are used.
The convergence is attained after certain number of iterations. Instead of random initial mean values, predefined values are set.
The cluster size is 6 with initial mean values {0, 65, 100, 125, 190, 255}. 
The convergence is set at the iteration is 50 and then terminated to produce output image *output.raw*.

## Serial Version
`kmeanserial.c` is a serial version of kmean clustering.
K-Means algorithm has the following steps:
1.  Initialize a mean value for each cluster.
2.  For each data element, compute its'distance' to the mean value of each cluster. Assign it to the closest cluster.
3.  After each data element is assigned to a cluster, recompute the mean value of each cluster.
4.  Check convergence; if the algorithm converges, replace the value of each data with the mean value of the cluster which 
    the data belongs to, then terminate the algorithm; otherwise, go to Step 2.

## Parallel version using Pthreads
`kmeanphread.c` is the parallel version of kmean clustering. The algorithm is as follows: 
1.  Initialize a mean value for each cluster.
2.  The data elements is partitioned and distributed  among the threads. Each thread is responsible for a subset of data elements 
    and assigns its data elements to the corresponding cluster. 
3.  Each thread also keeps track of the number of data element assigned to each cluster in current iteration and the corresponding sum value. 
4.  A variable r  is used to denote the number of threads which have completed the work for the current iteration. At the beginning of each iteration, r = 0.
    When a thread finishes its work for the current iteration, it checks the value of r. *`Since r is a shared variable, a mutex is needed whenever any thread tries to read/write it`*
    - If r < #No_of_Threads-1, r is incremented, the thread goes to sleep.
    - If r = p-1, r is updated to 0, the thread recomputes the mean value of each cluster based on the local intermediate data of all 
      the threads, then reinitializes the intermediate data of each thread. If the algorithm does not converge after the current iteration,
      this thread sends a broadcast single to wake up all the threads. Go to Step 3 to start a new iteration.
