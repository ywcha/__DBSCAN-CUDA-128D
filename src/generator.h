
#ifndef GENERATOR_H_
#define GENERATOR_H_


#include <time.h>
#include <ctime>   
#include <iostream>    
#include "point.h"            
#include <fstream> 
#include <string> 
#include "algorithms.h"   
 
  
const float myPI = 3.141592f;  

Point * generatePointsNormal(unsigned pNumber, float sigmaS = 0.03); //128d versions

Point * readFromFile128(char fileName[], unsigned &maxNum);

int savePoints(Point * inputPoints, unsigned pointNumber, std::string fileName); //return 1 - succes , 0 - fail

Point * generate(unsigned & pNumber);

Point * clonePoints(Point * source, unsigned pNumber);

Point2D * generatePointsNormal2D(float x1, float y1, unsigned pNumber, float sigmaS = 0.03);

Point2D * Init(unsigned number);

Point2D * readFromFile(char fileName[], unsigned &maxNum);
   
Point3D * generatePointsNormal3D(float x1, float y1, float z1, unsigned pNumber, float sigmaS = 0.03);
 
Point2D * generate2D(unsigned & pNumber); 

Point3D * generate3D(unsigned & pNumber); 
 
int savePoints2D(Point2D * inputPoints, unsigned pointNumber, std::string fileName); //return 1 - succes , 0 - fail

Point2D * loadPoints2D(unsigned & pointNumber, std::string fileName);
 
Point2D * clonePoints2D(Point2D * source, unsigned pNumber);
 
#endif /* GENERATOR_H_ */





