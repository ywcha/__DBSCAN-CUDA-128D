 
#ifndef DBSCAN_H_
#define DBSCAN_H_ 
 
#include "point.h" 
 
  Point * cudaDbscanNR(Point * inputPoints, int pointNumber, float eps, int minPoint);
  
  
#endif /* DBSCAN_H_ */ 

