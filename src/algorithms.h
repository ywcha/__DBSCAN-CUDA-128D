
#ifndef ALGORITHMS_H_
#define ALGORITHMS_H_      

#include "point.h"  

  
#include <iostream>   
#include <algorithm>
#include <vector>   
#include <list>
#include <cmath> 
#include <stdint.h>
  
//#define myPI 3.14159265   
  
/*static __inline__ unsigned long rdtsc(void){                       
  unsigned long long int x; 
  __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));                             
  return x; 
};*/ 
 
float manhattanDistance(Point & sample, Point & centroid);

float euclideanDistance(Point & sample, Point & centroid);

float manhattanDistance2D(Point2D & sample, Point2D & centroid);                                                        
  
float manhattanDistance3D(Point3D & sample, Point3D & centroid);                                               
  
float euclideanDistance2D(Point2D & sample, Point2D & centroid);                                        

float euclideanDistance3D(Point3D & sample, Point3D & centroid);                
 
void resetLabelAndHint(Point * inputPoints, int pointNumber, int newLabel = 0, int newHint =  0);                 

float * createDistTable(Point * inputPoints, int pointNumber);          

Point2D * genPINumber(int pNUmber, float * myPI); 

void normalizeLabels(Point * inputPoints, int pointNumber); 


#endif /* ALGORITHMS_H_ */




