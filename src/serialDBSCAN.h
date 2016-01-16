#pragma once
#ifndef __DBSCAN
#define __DBSCAN

#include "point.h"
#include <algorithm>
#include "algorithms.h" //euclideanDist
#include <vector>
#include <list>
#include <cmath>
#include <stack>

#define PI 3.14159265

//float * createDistTable(Point2D * inputPoints, int pointNumber)
//{
//	float * distTable = new float [pointNumber * pointNumber];
//	for(int i = 0;i<pointNumber;i++)
//	{
//		for(int j = 0;j<pointNumber;j++)
//		{
//			distTable[i*pointNumber + j] = euclideanDistance(inputPoints[i],inputPoints[j]);
//		}
//	}
//	return distTable;
//}

//void resetLabelAndHint(Point2D * inputPoints, int pointNumber, int newLabel = 0, int newHint =  0)
//{
//	for(int i=0;i<pointNumber;i++)
//	{
//		inputPoints[i].label = newLabel;
//		inputPoints[i].hint = newHint;
//	}
//}

int searchNeighbors(float * distTable, int pointNumber, int pos, float eps, int * neighbors); 
//returns the number of neighbors of the i-th point, and changes neighbors array

int *createNeighborhoodMatrix(float * distTable, int pointNumber, float eps)
{
	int *neighborhoodMatrix = new int [pointNumber * pointNumber];
	for(int i = 0; i < pointNumber * pointNumber; i++)
	{
		neighborhoodMatrix[i]=0;
	}

	int neighbourNumber;
	int *neighbors = new int [pointNumber];
	
	for(int i = 0; i < pointNumber; i++)
	{
		neighbourNumber = searchNeighbors(distTable, pointNumber, i, eps, neighbors);
		for(int j = 0; j < neighbourNumber; j++)
		{
			neighborhoodMatrix[i*pointNumber + neighbors[j]] = 1;
		}
	}
	delete [] neighbors;
	return neighborhoodMatrix;
}

//bool isReachable(int source, int destiny, int * neighborhoodMatrix, int pointNumber) 
//{
//	int * visited = new int [pointNumber];
//	for(int i=0;i<pointNumber;i++)
//	{
//		visited = 0;
//	}
//	visited[source] = 1;
//	for(int i=0;i<pointNumber;i++)
//	{
//
//	}
//	return true;
//}

int searchNeighbors(float * distTable, int pointNumber, int pos, float eps, int *neighbors)
{
	int counter = 0;
	int offset = pos*pointNumber;
	
	for(int i = 0; i < pointNumber; i++)
	{
		if(distTable[offset+i] < eps && i != pos )
		{
			neighbors[counter] = i;
			counter++;
		}
	}
	return counter;
}

int searchNeighborsNumber(float * distTable, int pointNumber, int pos, float eps)  //only counts the number of neighbors of the i-th point
{
	int counter = 0;
	int offset = pos*pointNumber;
	for(int i=0;i<pointNumber;i++)
	{
		if(distTable[offset+i] < eps && i != pos )
		{
			counter++;
		}
	}
	return counter;
}

int * copyNeighbors(int * neighbours, int neighborsNumber)
{
	int * newNeighborsTable = new int [neighborsNumber];
	for(int i=0;i<neighborsNumber;i++)
	{
		newNeighborsTable[i] = neighbours[i];
	}
	return newNeighborsTable;
}

int expandCluster(Point * inputPoints, int pointNumber, float * distTable, int pos, float eps,
	int * neighbors, int neighborsNumberLocal, int * classLabel, int minPoint, bool * newCluster)
{
 	static int debugI=0;
 	debugI++;
	//std::cout<<"start expand Cluster (recurent) "<<debugI<<" ; "<<neighborsNumberLocal<<std::endl;
	
	int * neighborsLocal =  copyNeighbors(neighbors, neighborsNumberLocal);
	int neighborsNumber;
	
	for(int i = 0; i < neighborsNumberLocal; i++)
	{
		if(inputPoints[neighborsLocal[i]].hint < 1)
		{
			//std::cout<<"neighbour , hint "<<neighborsLocal[i]<<" ; "<<inputPoints[neighborsLocal[i]].hint<<std::endl;
			neighborsNumber = searchNeighbors(distTable, pointNumber, neighborsLocal[i], eps, neighbors);	//neighbors are changed after this

			if(neighborsNumber >= minPoint)
			{
				*newCluster = true;
				if(inputPoints[pos].hint != 1)
				{
					inputPoints[pos].hint = 2;		//2 - border;
					inputPoints[pos].label = *classLabel;
				}

				inputPoints[neighborsLocal[i]].label = *classLabel;
				inputPoints[neighborsLocal[i]].hint = 1; // 1 - core point

				expandCluster(inputPoints, pointNumber, distTable, neighborsLocal[i], eps, neighbors, neighborsNumber, classLabel, minPoint, newCluster);
				
				if(inputPoints[pos].hint != 1)	//if not a core point
				{
					break;
				}
			}
			else
			{
				if(inputPoints[pos].hint == 1) //1 - core
				{
					inputPoints[neighborsLocal[i]].label = *classLabel;
					inputPoints[neighborsLocal[i]].hint = 2;
				}
				inputPoints[neighborsLocal[i]].label = -1;
				inputPoints[neighborsLocal[i]].hint = 3;	//set it to unknown
			}
		}
	}
	delete[] neighborsLocal;
	return 0;
}
 
Point * dbscan(Point *inputPoints, int pointNumber, float eps, int minPoint) 
{
	//std::cout<<"start dbscan (recurent)"<<std::endl;
	float * distTable = createDistTable(inputPoints, pointNumber);
	resetLabelAndHint(inputPoints, pointNumber, -1, -1);		// hint = -1 -> unvisited point; >= 1 -> visited point
																// hint = 1 -> core, 2 -> border, 3 -> unknown
																// label = -1 -> noise; 0,1,2,3 ...  -> class label
	int *neighbors = new int [pointNumber];
	int neighborsNumber;
	int classLabel=0;
	bool newCluster = false;

	for(int i = 0; i < pointNumber; i++)
	{
		if(inputPoints[i].hint < 1)	//if not visited
		{
			inputPoints[i].hint = 3;
			neighborsNumber = searchNeighbors(distTable, pointNumber, i, eps, neighbors);	//neighbors changed after this

			if(neighborsNumber < minPoint)
			{
				inputPoints[i].label = -1;	//mark it as noise
			}
			else
			{
				inputPoints[i].label = classLabel;	//map it to the current cluster
				inputPoints[i].hint = 1;	//mark it as core point
				newCluster = true;
			}
		}

		if(neighborsNumber > 0)
		{
			expandCluster(inputPoints, pointNumber, distTable, i, eps, neighbors, neighborsNumber, &classLabel, minPoint, &newCluster);
			if(newCluster) classLabel++;
			newCluster = false;
		}

	}
	delete[] neighbors;
	delete[] distTable;

	return inputPoints;
}


Point * dbscanNR(Point * inputPoints, int pointNumber, float eps, int minPoint)
{
	//std::cout<<"start dbscan (step based)"<<std::endl;
	float * distTable = createDistTable(inputPoints, pointNumber);
	resetLabelAndHint(inputPoints, pointNumber, -1, -1); // hint < 2 unvisited point; >= 2 visited point
														// hint = 1 or 2 core, 3 border
														// label -1 noise; 0,1,2,3 ...  - class label

	int neighbourNumber;
	for(int i=0;i<pointNumber;i++)
	{
		neighbourNumber = searchNeighborsNumber(distTable, pointNumber, i, eps);
		if(neighbourNumber >= minPoint)
		{
			inputPoints[i].hint = 1; // 1 - core
		}
	}

	int * neighbors = new int [pointNumber];
	int neighborsNumber;
	int classLabel=-1;
	for(int i=0;i<pointNumber;i++)
	{
		if(inputPoints[i].hint == 1 && inputPoints[i].label == -1)
		{
			classLabel++;
			inputPoints[i].label = classLabel;
			inputPoints[i].hint = 2; // visited
			neighborsNumber = searchNeighbors(distTable, pointNumber, i, eps, neighbors);
			for(int j=0;j<neighborsNumber;j++)
			{
				inputPoints[neighbors[j]].label = classLabel;
			}

			bool flag = true;

			while(flag)
			{
				flag = false;
				for(int k = 0; k < pointNumber; k++)
				{
					if(inputPoints[k].label == classLabel && inputPoints[k].hint < 2)
					{
						flag = true;
						inputPoints[k].hint = 2;
						if(inputPoints[k].hint < 1)
						{
							inputPoints[k].hint = 3;
							inputPoints[k].label = classLabel;
						}
						else
						{
							neighborsNumber = searchNeighbors(distTable, pointNumber, k, eps, neighbors);
							for(int j = 0; j < neighborsNumber; j++)
							{
								if(inputPoints[neighbors[j]].label == -1)
								inputPoints[neighbors[j]].label = classLabel;
							}
						}

					}//if
				}//for
			}//while

		}//if

	}//for

	return inputPoints;
}














#endif

