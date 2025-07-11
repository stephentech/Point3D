# Point 3D

Built with: C/C++, opengl, gl utility toolkit (GLUT)
Built as a hobby project during my college in the year 2005. Won awards in several competetions.

This was my first step into 3D and eventually gaming.  
Point 3D is a tool to load up models(defined in .vtx files) and apply the transformation (rotation, scaling, position) over the loaded model.  

The entire code is written into one file, and the application consists of Four UI Pages (input based).  

FirstPage: displays options  
	- 1 for Load predefined model (Second page)  
	- 2 for defining your own model. (Third Page)  
 	- 3 for Particle simulation. (Fourth Page)  
	- 4 for Help (Help Page)  

Second Pages:  
    - asks for the name of the model you wish to load.
	- loads up the requested model(.vtx)  
	- shifts to view controls  
 	- translation using Arrow Keys  
  	- rotation using Control and Arrow Keys  

Third Page (manually build model):  
	- enter values as floats (3D vertex form: x,y,z)  
	- shifts to view controls  
 	- translation using Arrow Keys  
  	- rotation using Control and Arrow Keys  
	
Fourth Page:  
	- particle simulation with a velocity (direction + magnitude) and acting under gravity  
 	- draws a trajectory of a particle motion  
