#include "dbscan.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cuda_runtime.h>
#include <cuda.h>
#include <cublas_v2.h>
#include <stack>
#include <time.h>
#include <ctime>

__constant__ Point sp[2000];
 
#define CUDA_CHECK_RETURN(value) {											\
	cudaError_t _m_cudaStat = value;										\
	if (_m_cudaStat != cudaSuccess) {										\
		fprintf(stderr, "Error %s at line %d in file %s\n",					\
				cudaGetErrorString(_m_cudaStat), __LINE__, __FILE__);		\
		exit(1);															\
	} }


//####################################################################################

float __device__ cudaManhattanDistance(const Point &source, const Point &destination)	//the Manhattan distance for 128D points
{
	float dist = 0.0;

	for (unsigned i=0; i < 128; i++)
		dist += fabs(source.a[i] - destination.a[i]);
	
	return dist;
}

float __device__ cudaEuclideanDistance(const Point &source, const Point &destination)	//the Euclidean distance for 128D points
{
 	float dist = 0.0;

 	for (unsigned i=0; i < 128; i++)
		dist += pow(source.a[i] - destination.a[i], 2);

	return sqrt(dist);
}


float __device__ cudaManhattanDistance2D(const Point2D &source, const Point2D &destination)
{
	return (fabs(source.x - destination.x) + fabs(source.y - destination.y)	);
}

float __device__ cudaEuclideanDistance2D(const Point2D &source, const Point2D &destination)
{
 	return sqrt(	pow(source.x - destination.x, 2) + pow(source.y - destination.y, 2)	);
}

void __global__ cudaCreateNeighborsTableShared(Point * inputPoints, int pointNumber,
		float eps, int * cudaNeighborhoodMatrix, int minPoint)					//the version with shared memory
{
	unsigned int id;
	unsigned int source;
	unsigned int destination;
	unsigned int symValue;
	float dist;
	int sum;
	unsigned int tid = blockIdx.x * blockDim.x + threadIdx.x;
	id = tid;
	
	while(id < pointNumber * pointNumber) // neighborhood matrix
	{
		source = id / pointNumber;		// device x
		destination = id % pointNumber; // device y

		if(source < destination)
		{
			symValue = destination * pointNumber + source;
			
			dist = 0.0;

 			for (unsigned i = 0; i < 128; i++)
				dist += pow(sp[source].a[i] - sp[destination].a[i], 2);  //euclidean distance (faster than calling the function)

			//dist = sqrt(	pow(sp[source].x - sp[destination].x, 2) + pow(sp[source].y - sp[destination].y, 2)	);
			if(dist < eps) //&& cudaDistTable[id] != 0)
			{
				cudaNeighborhoodMatrix[id] = 1;
				cudaNeighborhoodMatrix[symValue] = 1;
			}
			else
			{
				cudaNeighborhoodMatrix[id] = 0;
				cudaNeighborhoodMatrix[symValue] = 0;
			}
		}

		id += blockDim.x * gridDim.x;
	}

	__syncthreads();
	
	id = tid;
	while(id < pointNumber) // tag core
	{
		sum = 0;
		source = id * pointNumber;
		for (int i = 0; i < pointNumber;i++)
		{
			if(id != i) // not itself
			{
				if(cudaNeighborhoodMatrix[source+i])
				{
					cudaNeighborhoodMatrix[sum] = i;
					sum++;
				}
			}
		}
		
		if(sum >= minPoint)
		{
			inputPoints[id].hint = 1; //hint = 1 - core;
		}
		id += blockDim.x * gridDim.x;
	}
}


void __global__ cudaCreateNeighborsTable(Point * inputPoints, int pointNumber,
		float eps, int * cudaNeighborhoodMatrix, int minPoint)
{
	unsigned int id;
	unsigned int source;
	unsigned int destination;
	unsigned int symValue;
	int sum;
	unsigned int tid = blockIdx.x * blockDim.x + threadIdx.x;
	id = tid;
	
	while(id < pointNumber * pointNumber) // neighborhood matrix
	{
		source = id / pointNumber;		// device x
		destination = id % pointNumber; // device y
		
		if(source < destination)
		{
			symValue = destination * pointNumber + source;

			if(cudaEuclideanDistance(inputPoints[source], inputPoints[destination]) < eps) //&& cudaDistTable[id] != 0)
			{
				cudaNeighborhoodMatrix[id] = 1;
				cudaNeighborhoodMatrix[symValue] = 1;
			}
			else
			{
				cudaNeighborhoodMatrix[id] = 0;
				cudaNeighborhoodMatrix[symValue] = 0;
			}
		}
		id += blockDim.x * gridDim.x;
	}
	
	__syncthreads();
	
	id = tid;
	while(id < pointNumber) // tag core
	{
		sum = 0;
		source = id * pointNumber;
		for(int i = 0; i < pointNumber; i++)
		{
			if(id != i) // not itself
			{
				if(cudaNeighborhoodMatrix[source+i])
				{
					cudaNeighborhoodMatrix[sum] = i;
					sum++;
				}
			}
		}
		if(sum>=minPoint)
		{
			inputPoints[id].hint = 1; //hint = 1 - core;
		}
		id += blockDim.x * gridDim.x;
	}
}

void __global__ cudaLabeling(Point * inputPoints, int pointNumber, float * cudaPowerMatrix)
{
	unsigned int id;
	unsigned int source;
	unsigned int tid = blockIdx.x * blockDim.x + threadIdx.x;
	id = tid;
	
	while(id < pointNumber)
	{
		source = id * pointNumber;
		for(int i = 0; i < pointNumber; i++)
		{
			if(cudaPowerMatrix[source + i] > 0)
			{
				inputPoints[id].label = i;
				cudaPowerMatrix[source + i] = 0;
				break;
			}
		}
		id += blockDim.x * gridDim.x;
	}
}

void hostLabeling(Point *inputPoints, int pointNumber, int *hostNeighborhoodMatrix)
{
	std::stack<int> stack;
	int label = 1;

	for (int i = 0; i < pointNumber; i++)
	{
		if(inputPoints[i].hint == 1) // if the point is a core point
		{
			inputPoints[i].label = label;
			int offset = pointNumber*i;
			
			while(hostNeighborhoodMatrix[offset] != 0)
			{
 				inputPoints[hostNeighborhoodMatrix[offset]].label = label;
 				stack.push(hostNeighborhoodMatrix[offset]);
				offset++;
			}

			while(!stack.empty())
			{
				if(inputPoints[stack.top()].hint == 1)
				{
					offset = stack.top() * pointNumber;
					while(hostNeighborhoodMatrix[offset] != 0)
					{
						if(inputPoints[hostNeighborhoodMatrix[offset]].label < 1)
						{
							inputPoints[hostNeighborhoodMatrix[offset]].label = label;
							stack.push(hostNeighborhoodMatrix[offset]);
						}
						offset++;
					}
				}
				stack.pop();
			}

			label++;
		}
	}
}

Point *cudaDbscanNR(Point *inputPoints, int pointNumber, float eps, int minPoint)
{
	cudaEvent_t start, stop, allocate, memcopy, neighbor,  labeling, mainCopy, sstop;
	cudaEventCreate(&start);
	cudaEventCreate(&allocate);
	cudaEventCreate(&memcopy);
	cudaEventCreate(&neighbor);
	cudaEventCreate(&labeling);
	cudaEventCreate(&mainCopy);
	cudaEventCreate(&stop);
	cudaEventCreate(&sstop);
	cudaEventRecord( start, 0 );

	int hostLabelingTime;
	int *hostNeighborhoodMatrix = (int*) malloc(pointNumber*pointNumber*sizeof(int));

	Point *cudaPoints;
	int *cudaNeighborhoodMatrix;

	int allNum = pointNumber * pointNumber;
	int threadsNum = 128;

//############################################################
	CUDA_CHECK_RETURN(cudaMalloc((void**)&cudaPoints, pointNumber * sizeof(Point)));

	CUDA_CHECK_RETURN(cudaMalloc((int**)&cudaNeighborhoodMatrix, pointNumber * pointNumber * sizeof(int)));

	cudaEventRecord( allocate, 0 );

//############################################################
	CUDA_CHECK_RETURN(cudaMemcpy(cudaPoints, inputPoints, pointNumber*sizeof(Point), cudaMemcpyHostToDevice));

	CUDA_CHECK_RETURN(cudaMemcpyToSymbol(sp, inputPoints, 2000 * sizeof(Point)));

	cudaEventRecord( memcopy, 0 );

//###############################################################

	dim3 threads; 
	threads.x = threadsNum;

    dim3 blocks; 
	blocks.x = ( ( allNum + threads.x - 1 ) / threads.x );

	printf ("thread = %d ; blocks %d \n", threads.x, blocks.x);

	//cudaCreateNeighborsTable<<<blocks, threads>>>(cudaPoints, pointNumber, eps, cudaNeighborhoodMatrix, minPoint);
	cudaCreateNeighborsTableShared<<<blocks, threads>>>(cudaPoints, pointNumber, eps, cudaNeighborhoodMatrix, minPoint);

	cudaEventRecord( neighbor, 0 );

//##############################################################

	CUDA_CHECK_RETURN(cudaMemcpy(hostNeighborhoodMatrix, cudaNeighborhoodMatrix, pointNumber*pointNumber*sizeof(int), cudaMemcpyDeviceToHost));

	CUDA_CHECK_RETURN(cudaMemcpy(inputPoints, cudaPoints, pointNumber*sizeof(Point), cudaMemcpyDeviceToHost));

	cudaEventRecord( mainCopy, 0 );

	/*for(int i = 0; i < pointNumber; i++)
	{
		for(int j =0;j<pointNumber;j++)
		{
			printf("%7.2f",hostDistTable[i*pointNumber + j]);
		}
		printf ("\n");
	}

	for(int i=0;i<pointNumber;i++)
	{
		printf("hint = %d\t", inputPoints[i].hint);
		for(int j =0;j<pointNumber;j++)
		{
			printf("%7.3f",hostNeighborhoodMatrix[i*pointNumber + j]);
		}
		printf ("\n");
	}*/

//###############################################

	clock_t hostRun = clock();
	hostLabeling(inputPoints, pointNumber, hostNeighborhoodMatrix);
    hostLabelingTime = clock() - hostRun;

//	cudaLabeling<<<blocks, threads>>>(cudaPoints, pointNumber, cudaNeighborhoodMatrixPower);
//
//	cudaEventRecord( labeling, 0 );
//
//	CUDA_CHECK_RETURN(cudaMemcpy(inputPoints, cudaPoints,
//			 pointNumber*sizeof(Point2D), cudaMemcpyDeviceToHost));



//##############################################

	cudaFree(cudaPoints);
	cudaFree(cudaNeighborhoodMatrix);

	cudaEventRecord( stop, 0 );
	cudaEventSynchronize( stop );

	cudaEventRecord( sstop, 0 );
	cudaEventSynchronize( sstop );

	float elapsedTime;
	cudaEventElapsedTime( &elapsedTime, start, stop );
	printf( "Time of all (stop):%3.2f ms\n", elapsedTime );

	cudaEventElapsedTime( &elapsedTime, start, sstop );
	printf( "Time of all (sstop):%3.2f ms\n", elapsedTime );

	cudaEventElapsedTime( &elapsedTime, start, allocate );
	printf( "Time of allocate:%3.2f ms\n", elapsedTime );

	cudaEventElapsedTime( &elapsedTime, allocate, memcopy );
	printf( "Time of memcopy:%3.2f ms\n", elapsedTime );

	cudaEventElapsedTime( &elapsedTime, memcopy, neighbor );
	printf( "Time of neighbor matrix:%3.2f ms\n", elapsedTime );

	printf("Host labeling: %d clicks (%f seconds).\n", hostLabelingTime, ((float) hostLabelingTime)/CLOCKS_PER_SEC);
//	cudaEventElapsedTime( &elapsedTime, labeling , mainCopy);
//	printf( "Time of mainCopy:%3.2f ms\n", elapsedTime );

	cudaEventElapsedTime( &elapsedTime, mainCopy , stop);
	printf( "Time of free:%3.2f ms\n", elapsedTime );

	cudaEventDestroy(start);
	cudaEventDestroy(allocate);
	cudaEventDestroy(memcopy);
	cudaEventDestroy(labeling);
	cudaEventDestroy(mainCopy);
	cudaEventDestroy(stop);
	cudaEventDestroy(sstop);

	return NULL;
}

