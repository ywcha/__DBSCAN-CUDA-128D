
#include "generator.h"
#include <stdio.h>
#include <iostream>

Point *generatePointsNormal(unsigned pNumber, float sigmaS)
{
	//srand((unsigned int)rdtsc());

	Point * genPoints = new Point[pNumber];
	
	for (int i = 0; i < pNumber; i++)
	{
		for (int j = 0; j < 128; j++)
		{
			genPoints[i].a[j] = ((2 * 3 * sqrt(sigmaS)) *	((float)rand() / (float)RAND_MAX)) - 3 * sqrt(sigmaS); //random values for the point coordintes
				
			genPoints[i].label = -1;	//-1 is for not visited
		}
	}
	return genPoints;
}

Point * readFromFile128(char fileName[], unsigned &maxNum) {

	FILE *f = fopen( fileName, "r+b"); 
	 
 	if (f == NULL) { //if such file doesn't exist, create random dataset with maxNum keypoints

 		std::cout << "The binary file '" << fileName << "' does not exist. Creating random keypoints data..." << std::endl;
 		return generatePointsNormal(maxNum);
		 
 		//return Init( maxNum);	 
 	}

	std::cout << "Reading keypoints data from '" << fileName << "' ..." << std::endl;
	Point * genPoints = new Point[maxNum];  // allocate memory for the maxNum points

	unsigned counter = 0;

	while (!feof(f)) {

		int j;
		for (j = 0; (!feof(f)) && (j < 128); j++)
		{
			fread(&genPoints[counter].a[j], sizeof(float), 1, f);
		}

		genPoints[counter].label = -1;
		genPoints[counter].hint = -1;

		if (j >= 128)
		{
			counter++;  //only if a complete 128D point is retrieved, increase the counter
			
			if (counter >= maxNum) break; //don't exceed the maximum number of points
		}

	}

	std::cout << "The total of " << counter << " points retrieved from the binary file." << std::endl;
	
	maxNum = counter; //set the number of points to the number of retrieved points

	fclose(f);
	//system("pause");
	std::cout << std::endl;

	return genPoints;
}


Point2D * generatePointsNormal2D(float x1, float y1, unsigned pNumber, float sigmaS)
{
//	srand((unsigned int)rdtsc());

	Point2D * genPoints = new Point2D[pNumber];
	float x,y,u,v,w;

	bool flag;
	for(int i = 0; i < pNumber; i++)
	{
		x = x1;
		y = y1;
		flag = true;
		while(flag)
		{
			u = ( (2* 3 *sqrt(sigmaS) ) *	(	(float)rand()/(float)RAND_MAX	)	) - 3 *sqrt(sigmaS);
			v = ( (2* 3 *sqrt(sigmaS) ) *	(	(float)rand()/(float)RAND_MAX	)	) - 3 *sqrt(sigmaS);

 			w = (	(float)rand()/(float)RAND_MAX	);
  
	 		// w < 1 / (2*myPI*sigmaS) e ^ -1/2[ (u^2 + v^2) / sigmaS ]
	 		if(w < (1.0 / (2.0 *myPI*sigmaS) ) * exp( -0.5 * (	(	pow(u,2) + pow(v,2)	) / sigmaS ) )  )
			{
	 			flag = false;
	 			genPoints[i].x = x + u;
	 			genPoints[i].y = y + v;
	 			genPoints[i].label = -1;
 			}
 		}
 	}
 	return genPoints;
 }

Point2D * Init(unsigned number) {

	std::cout << "Data set with " << number << " points" << std::endl;
//	srand((unsigned int)rdtsc());

	Point2D * genPoints = new Point2D[number];

	for (unsigned j = 0; j < number; j++) {

		genPoints[j].x = 2.0f*rand() / RAND_MAX - 1;
		genPoints[j].y = 2.0f*rand() / RAND_MAX - 1;
		genPoints[j].label = -1;
		
		std::cout << "(" << genPoints[j].x << " , " << genPoints[j].y << ')' << std::endl;
	}
	//system("pause");
	//std::cout << std::endl;

	return genPoints;
}

Point2D * readFromFile(char fileName[], unsigned &maxNum) {

	FILE *f = fopen(fileName, "r+b");
	
	if (f == NULL)  { //if such file doesn't exist, create random dataset with maxNum keypoints

 		std::cout << "The binary file '" << fileName << "' does not exist. Creating random keypoints data..." << std::endl;
		return generatePointsNormal2D(0, 0, maxNum); 
		
		//return Init( maxNum);
 
 	}
 
 	std::cout << "Reading keypoints data from '" << fileName << "' ..." << std::endl;
	Point2D * genPoints = new Point2D[maxNum];  //TODO: provide some comments
 
 	unsigned br = 0;

	while (!feof(f)) {

		fread(&genPoints[br].x, sizeof(float), 1, f); //TODO: change the keypoint reading to 128d keypoints
		fread(&genPoints[br].y, sizeof(float), 1, f);
		
		genPoints[br].label = -1;
		genPoints[br].hint = -1;
		
		br++;
		if (br >= maxNum) break;

	}

	std::cout << "The total of " << br << " points retrieved from the binary file." << std::endl;
	maxNum = br;

	fclose(f);
	//system("pause");
	std::cout << std::endl;

	return genPoints;
}

 Point3D * generatePointsNormal3D(float x1, float y1, float z1, unsigned pNumber, float sigmaS)
 {
// 	srand((unsigned int)rdtsc());
 
 	Point3D * genPoints = new Point3D[pNumber];
 	float x,y,z,u,v,t,w;
 
 	bool flag;
 	for(int i=0; i< pNumber; i++)
 	{
 		x = x1;
 		y = y1;
 		z = z1;
 		flag = true;
 		while(flag)
 		{
 			u = ( (2* 3 *sqrt(sigmaS) ) *	(	(float)rand()/(float)RAND_MAX	)	) - 3 *sqrt(sigmaS);
 			v = ( (2* 3 *sqrt(sigmaS) ) *	(	(float)rand()/(float)RAND_MAX	)	) - 3 *sqrt(sigmaS);
 			t = ( (2* 3 *sqrt(sigmaS) ) *	(	(float)rand()/(float)RAND_MAX	)	) - 3 *sqrt(sigmaS);
 
 			w = (	(float)rand()/(float)RAND_MAX	);
 
 			// w < 1 / (2*myPI*sigmaS) e ^ -1/2[ (u^2 + v^2) / sigmaS ]
 			if(w < (1.0 / (2.0 *myPI*sigmaS) ) * exp( -0.5 * (	(	pow(u,2) + pow(v,2) + pow(t,2)	) / sigmaS ) )  )
 			{
 				flag = false;
 				genPoints[i].x = x + u; 
				genPoints[i].y = y + v; 
		 		genPoints[i].z = z + t; 
				genPoints[i].label = -1; 
			} 
		} 
 	}
 	return genPoints;
}

 int savePoints(Point *inputPoints, unsigned pointNumber, std::string fileName)
 {
	 std::ofstream outputFile;
	 outputFile.open(fileName.c_str());

	 if (outputFile.good())
	 {
		 std::cout << "Files successfully open" << std::endl;
	 }
	 else
	 {
		 std::cout << "Error (file opening) " << fileName << std::endl;
		 return 0;
	 }

	 for (int i = 0; i < pointNumber; i++)
	 {
		 for (int j = 0; j < 128; j++)
			 outputFile << inputPoints[i].a[j] << "\t";
		 
		 outputFile << inputPoints[i].label << "\t" << inputPoints[i].hint << "\t" << std::endl;
	 }

	 outputFile.close();
	 return 1;
 }


int savePoints2D(Point2D *inputPoints, unsigned pointNumber, std::string fileName)
{ 
 	std::ofstream outputFile;
 	outputFile.open(fileName.c_str());
 
  	if (outputFile.good())
  	{
  		std::cout << "Files successfully open" << std::endl;
  	}
  	else
  	{
  		std::cout << "Error (file opening) "<< fileName << std::endl;
   		return 0;
  	}
 
  	for(int i = 0; i < pointNumber; i++)
 	{
  		outputFile << inputPoints[i].x << "\t" << inputPoints[i].y << "\t" << inputPoints[i].label << "\t" << inputPoints[i].hint << "\t" << std::endl;
 	}
 
 	outputFile.close();
 	return 1;
}


Point2D * loadPoints2D(unsigned & pointNumber, std::string fileName) 
{ 
 	Point2D * inputPoints;
 	std::ifstream inputFile;
 	inputFile.open(fileName.c_str());
 
 	if (inputFile.is_open())
 	{
 		std::cout << "Files successfully open"<<std::endl;
 	}
 	else
 	{
 		std::cout << "Error"<<std::endl;
 		return 0; 
 	}
 
 	int counter=0;
 	std::string str;
 	while(inputFile.eof() == false)
 	{
 		getline(inputFile, str); 
		if(str.empty()) break; 
		counter++; 
	} 
 
 	inputFile.close();
	 
 	pointNumber = counter;
 	inputPoints = new Point2D [pointNumber];
 
 	inputFile.open(fileName.c_str());
	 
 	for(int i=0; i < pointNumber; i++)
 	{
 		inputFile>>inputPoints[i].x; 
 		inputFile>>inputPoints[i].y;
 		inputFile>>inputPoints[i].label;
 		inputFile>>inputPoints[i].hint;
 	}
  
 	inputFile.close();
 	return inputPoints;
 }
 
Point * clonePoints(Point * source, unsigned pNumber)
{
	Point * genPoints = new Point[pNumber];
	for (int i = 0; i < pNumber; i++)
	{
		genPoints[i] = source[i];
	}
	return genPoints;
}

 Point2D * clonePoints2D(Point2D * source, unsigned pNumber)
 {
 	Point2D * genPoints = new Point2D[pNumber];
 	for(int i = 0; i < pNumber; i++)
 	{ 
 		genPoints[i] = source[i]; 
 	} 
 	return genPoints; 
 } 
  


 Point * generate(unsigned & pNumber)
 {
	 int value;
	 char numInput[256];
	 
	 Point *genPoint = NULL;
	 std::string fileName;

	 printf("Enter number of points: ");
	 fgets(numInput, 256, stdin);
	 pNumber = atoi(numInput);

	 std::cout << "Choose data set to generate 2D points: " << pNumber << std::endl;
	 std::cout << "1 - Normal" << std::endl;
	 std::cout << "2 - Read from the binary file 'episode-3'" << std::endl;

	 fgets(numInput, 256, stdin);
	 value = atoi(numInput);

	 switch (value)
	 {
	 case 1:
		 genPoint = generatePointsNormal(pNumber);
		 break;

	 case 2:
		 genPoint = readFromFile128("../data/episode-3", pNumber);
		 break;

	 default:
		 break;
	 }
	 return genPoint;
 }

  
 Point2D * generate2D(unsigned & pNumber)
 {
 	int value;
 	char numInput [256];   
 	Point2D * genPoint2D = NULL;          
 	std::string fileName;  
   
 	printf ("Enter number of points: ");                 
 	fgets ( numInput, 256, stdin );         
 	pNumber = atoi (numInput);   
 
 	std::cout << "Choose data set to generate 2D points: " << pNumber << std::endl;                
 	std::cout << "1 - Normal" << std::endl;         
	std::cout << "2 - Load from file" << std::endl;                        
	std::cout << "3 - Read from the binary file 'episode-3'" << std::endl;

 	fgets ( numInput, 256, stdin );                  
  	value = atoi (numInput);       
 
 	switch(value)            
 	{
 		case 1:   
 			genPoint2D = generatePointsNormal2D(0,0, pNumber);                 
 			break;   
  
 		case 2:     
			std::cout<<"put file name"<<std::endl;                   
 			std::cin>>fileName;
 			genPoint2D = loadPoints2D(pNumber,"data/" + fileName);      
 			std::cout<<"data loaded"<<std::endl;     
 			break; 

		case 3:
			genPoint2D = readFromFile("../data/episode-3", pNumber);
			break;

 		default:
 			break;
 	}
 	return genPoint2D;     
 }

 Point3D * generate3D(unsigned & pNumber)      
 {
 	char value;         
 	char numInput [256];     
  	Point3D * genPoint3D;       
 
  	printf ("Enter number of points: ");       
  	fgets ( numInput, 256, stdin );         
  	pNumber = atoi (numInput);         
  
 	std::cout<<"Choose data set (to generate 3D) points number: "<<pNumber<<std::endl;                          
 	std::cout<<"1 - Normal"<<std::endl;    
 	
 	std::cin>>value;        
 	switch(value)                          
	{
		case 1:    
			genPoint3D = generatePointsNormal3D(0,0,0, pNumber);      
			break;       
			  
		default:
			break;       
	}
	return genPoint3D;
}

