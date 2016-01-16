
#include "algorithms.h"

float manhattanDistance(Point & sample, Point & centroid) //& is for faster passing of parameters
{
	float dist = 0.0;

	for (unsigned i = 0; i < 128; i++)
		dist += fabs(sample.a[i] - centroid.a[i]); 
	 
	return dist;
}

float euclideanDistance(Point & sample, Point & centroid)
{
	float dist = 0.0; 

	for (unsigned i = 0; i < 128; i++)
		dist += pow(sample.a[i] - centroid.a[i], 2);
	 
	return sqrt(dist);
} 

float manhattanDistance2D(Point2D & sample, Point2D & centroid)   
{ 
	return (fabs(sample.x - centroid.x) + fabs(sample.y - centroid.y));   
}
 
float manhattanDistance3D(Point3D & sample, Point3D & centroid)         
{
	return (fabs(sample.x - centroid.x) + fabs(sample.y - centroid.y) + fabs(sample.z - centroid.z));           
}

float euclideanDistance2D(Point2D & sample, Point2D & centroid)                                          
{ 
	 	return sqrt(	pow(sample.x - centroid.x, 2) + pow(sample.y - centroid.y, 2)	);                            
}  
   
float euclideanDistance3D(Point3D & sample, Point3D & centroid)                                                                                        
{ 
	    return sqrt(	pow(sample.x - centroid.x, 2) + pow(sample.y - centroid.y, 2) + pow(sample.z - centroid.z, 2)	);                                               
}
 
void resetLabelAndHint(Point * inputPoints, int pointNumber, int newLabel , int newHint )                                
{                                            
 	for(int i=0; i < pointNumber; i++)                                 
 	{
 		inputPoints[i].label = newLabel;                                                                                                                
 		inputPoints[i].hint = newHint;                            
 	}
}

float * createDistTable(Point * inputPoints, int pointNumber)                                                          
{
	float * distTable = new float [pointNumber * pointNumber];                 

 	for(int i = 0; i < pointNumber; i++)                
	{   
 		for(int j = 0; j < pointNumber; j++)                                                                         
		{  
			distTable[i*pointNumber + j] = euclideanDistance(inputPoints[i], inputPoints[j]);                
		}
	}
	return distTable;         
}

//unsigned __int64 rdtsc(void)            
//{   
//  return __rdtsc();                                             
//}   
 
Point2D * genPINumber(int pNUmber, float *myPI)  //function creating different myPI for random point coordinates                                                                                  
{
	int S = 0;      

	Point2D * genP = new Point2D [pNUmber];                                            
	for(int i = 0; i < pNUmber; i++)                                                                     
	{
			genP[i].x = (	((float)rand()/(float)RAND_MAX*2) -1	);                                           
 			genP[i].y = (	((float)rand()/(float)RAND_MAX*2) -1	);                                                                                    
 			genP[i].label = 0;     
 			if(pow(genP[i].x, 2) + pow(genP[i].y, 2) <= 1)                                                                                                                                   
			{
				genP[i].label = 1;                                                                                                                
				S++;                                                                                                                            
			}                                                                      
	}    
	(*myPI) = (4.0f * S) / pNUmber;         
	
	return genP;                                                                                       
}   

void normalizeLabels(Point *inputPoints, int pointNumber)                                                                                                                                                                                          
{   
 	std::vector<int> collect;                                                                                                                                          
  	bool flag;   

 	for(int i = 0; i < pointNumber; i++)                                                                                                                               
 	{
 		flag = true;       
 		for(std::vector<int>::iterator it = collect.begin(); it < collect.end() ; it++)                                                                          
 		{                                                                                 
 			if((*it) == inputPoints[i].label)                                                                              
 			{                                                                                  
  				flag = false;                                                                                                                                 
	 			break;                                                                                                                       
	 		}                                                           
	 	}
	 	if(flag)                                                                                                                                                            
	 	{                                                
	 		collect.push_back(inputPoints[i].label);                                        
	 	}                                                        
	}
	/*for(unsigned int i = 0 ; i < collect.size(); i++)    
	{
		std::cout<<collect[i]<<std::endl;
	}*/  
 	for(unsigned int i = 0; i < collect.size(); i++)        
 	{
 		collect[i] += collect.size();          
 	}
 	for(int i = 0; i < pointNumber; i++)                                                        
 	{     
 		inputPoints[i].label += collect.size();                                                               
 	}     
	//std::cout<<"[size] "<<collect.size()<<std::endl;                                                                                            
 	
	int counter;                                                    
 	for(unsigned int i = 0 ; i < collect.size(); i++)                                                                                                                     
 	{                
 		counter = 0;   
 		for(int j = 0; j < pointNumber; j++)                                                                                                        
 		{
 			if(inputPoints[j].label == collect[i])                                                                                   
 			{                                                  
 				inputPoints[j].label = i;                                                                                          
 				counter++;                                                                                                   
 			}          
 		}   
 		collect[i] = counter;                                                            
 	}  
 
 	std::cout << "######################################" << std::endl;                                                        
 	
	for(unsigned int i = 0; i < collect.size(); i++)                                                   
	{                  
		std::cout << "label [" << i << "] ; size = " << collect[i] << std::endl;                                     
	}                   
}

