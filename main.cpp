
#include <iostream> 
#include "point.h"     
#include "generator.h"     
#include "dbscan.h"     
#include "serialDBSCAN.h"           
#include <time.h>          
     
using namespace std;         

int main() 
{  
	long int seqDBSCAN = 0;    //serial DBSCAN                               
	long int parDBSCAN = 0;     //parallel DBSCAN                                    
	  
	unsigned pointsNumber = 100;                                     
	        
	std::cout <<" Serial and parallel DBSCAN v3" << std::endl;                   
	std::cout <<" Point size: "<< sizeof(Point) << std::endl;                                                                                                                     
 	 
 	int sharedSize;                                                                                                
	std::cout << " 64kb = " << (sharedSize = 64 * 1024) << std::endl;                
	std::cout << " Max points in shared memory: " << sharedSize / sizeof(Point) << std::endl;                                                                                            
	                           
 	//Point2D * generatedPoints2D;                                      
	//Point2D * cloneGeneratedPoints2D;                            
	Point * generatedPoints;
	Point * cloneGeneratedPoints;
	 
	generatedPoints = generate(pointsNumber);                                                                        
	std::cout << "Points generated" << std::endl;                                                         
	      
	cloneGeneratedPoints = clonePoints(generatedPoints, pointsNumber);                                                                                                                  
	std::cout<<"Points cloned" << std::endl;                                                  
         	                        
	savePoints(generatedPoints, pointsNumber, "../data/everyrun.data");                                                                                                                                                              
	std::cout<< "Points saved to ../data/everyrun.data" << std::endl;                                                            
	std::cout<< "Point number: " << pointsNumber << std::endl;                                                           
	                                         
 	clock_t start = clock();                       
	cudaDbscanNR(generatedPoints, pointsNumber, 0.1f, 6);  //eps=0.1    minPts=6                                                                                                                          
 	parDBSCAN = clock() - start;           
	
	start = clock();                                        
	dbscan( cloneGeneratedPoints, pointsNumber, 0.1f, 6);                                                                                    
 	seqDBSCAN = clock() - start;                  
	  
	/*for(int i=0;i<pointsNumber;i++)                                
	{
		std::cout<<"["<<i<<"]  x = "<<generatedPoints2D[i].x                            
			<<" ; y = "<<generatedPoints2D[i].y                       
			<<" ; label = "<<generatedPoints2D[i].label                          
			<<" ; hint = "<<generatedPoints2D[i].hint                                                                                    
		<<std::endl;
	}*/
	     
	cout << "Time required for serial DBSCAN: " << seqDBSCAN << endl;                                                                      
	cout << "Time required for parallel DBSCAN: " << parDBSCAN << endl;                    

	savePoints(generatedPoints, pointsNumber, "../data/clusters.data");                            
	std::cout<<"points saved to ../data/clusters.data"<<std::endl;                                                                

	return 0;                                                             
}   
  
