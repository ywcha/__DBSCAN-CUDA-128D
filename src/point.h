
 #ifndef POINT_H_ 
 #define POINT_H_        

struct Point //128d point
{
	float a[128];	//array for the coordinates of a point
	int label;
	int hint;
};
// hint = -1 unvisited point; >= 1 visited point
// hint = 1 core, 2 border, 3 unknown
// label= -1 noise; 0,1,2,3 ...  - cluster label
 
 struct Point2D
 {
 	float x;    
 	float y;  
 	int label;
 	int hint;  
 };
  
 struct PointPolar2D  
 {
 	float r;       
 	float fi;
 	int label;
 };
 
 struct Point3D
 {
 	float x;
 	float y;
 	float z;
 	int label;
 };
 
   
 #endif /* POINT_H_ */
