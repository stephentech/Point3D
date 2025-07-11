#include<stdlib.h>
#include <stdio.h>
#include<unistd.h>
#include<string.h>
#include <math.h>
#include<GL/gl.h>
#include<GL/glu.h>
#include<GL/glut.h>
#define AXIS 20
#define Co_Restitution 0.7

float vertices[31000][3];   	  // each vertice has three components..... ie   x,y,z
int nVertices = 0;		  	 //  number of vertices

int indices[50000][3];		//   used for connecting triangles in function View_Model
int nIndices = 0;			//   count of indices

int show_axis = 1;			//    Show axis is true for value 1 ... and false for value 0

float velocity[3],u[3],g[3] = {0,-9.8,0};	// velocity vector for simulation... u is initial velocity ... g is gravity

int KBflag = 0, 			//   KeyBoard mode set for choosing options ... set to 1 means use for rotating Mesh
	opt =0, 				//   variable used for option
	flag = 0, 				//   state whether Enter/Return key is pressed.....
	page = 1,			//   Set starting page to Front_Page
	ipMode = 0;			//   Set input mode to 1 when inputting characters(file names) ... 0 denotes normal choice
char ipBuffer[100]	= {'\0'};  //   input Buffer used to take input
//float scale = 0.3f;
int indx = 0;				// temp counting variable
int trans = -50;			//  set to translate -50 units from origin


int hf=0;					//   global angle counter used only in function BackGround()
	void BackGround()
	{
		glColor3f(0,1,0);
		float x=0,y=0,an=60,uu=10;
		glBegin(GL_POINTS);
		x=0;hf++;hf%=360;
		
		// provide a background sine wave animation.
		for(an=hf;an<1492+hf;an++)
		{
			x+=.2;
			y=uu*sin(an*3.14/180);
				glVertex3f(x-300,y,0);
				glVertex3f(x-297,y,0);
				
				glVertex3f(300-x,y,0);
				glVertex3f(297-x,y,0);
			
			y=uu*sin((an+180)*3.14/180);
							
				glVertex3f(x-300,y,0);
				glVertex3f(x-297,y,0);
				
				glVertex3f(300-x,y,0);
				glVertex3f(297-x,y,0);
		}
		glEnd();
		glColor3f(1,1,1);
	}
	
	void setupGL()			//    Setting up the window and preparing for drawing
	{
		glClearColor(0,0,0,1);		// sets color to black
		glMatrixMode(GL_PROJECTION);
		gluPerspective(60,1.0,3,1000);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();			// Sets matrix to identity
		glTranslatef(0,0,trans);   	// Translates along Z-axis by trans units...
	}
	
	void printing(char *text,float x, float y)	//    Function created for printing within window
	{
		glRasterPos2d(x,y);				// 	position cursor within window...
		for (int i = 0; *(text+i) != '\0' ; i++)
		{
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,text[i]);
		}
	}
	
	void show_Axis()		// drawing x,y,z -- axis
	{
		show_axis = show_axis%2;
		if(show_axis == 1)
		{
			glBegin(GL_LINES);
			glColor3f(.7,.7,0);		glVertex3f(0,0,0); 	glVertex3f(AXIS,0,0);	// 	red colour
			glColor3f(0,.7,.7);		glVertex3f(0,0,0);  	glVertex3f(0,AXIS,0);	// 	yellow colour
			glColor3f(.7,.2,.2);		glVertex3f(0,0,0);	glVertex3f(0,0,AXIS);	//	blue colour
			glEnd();
		}
	}
	
	// read the vtx file format.
	// vtx file contains the vertex information and the Indices to for triangle of the vertices.
	int Read_File()
	{
		FILE *fp;
		fp = fopen(ipBuffer,"r");
		if(!fp)
		{
			fprintf( stderr, "\nFile read error ..." );
			return -1;
		}
		else
			fprintf( stderr, "\nFile opened successfully ..." );
		char* text = (char*)malloc(100);
		int number;
		int i, indx = 0;
		size_t len = 100;
			getline((char**)&text,&len,fp);
			number = atoi(text);
			fprintf( stderr, "\nNumber : %d", number );
			for( i=0; i<number; i++ )
			{
				getline( (char**)&text, &len, fp );			// reading co-ordinates from file
				indx = 0;
				vertices[i][0] = atof(text);				// seperating vertices
				while(text[indx] != ';' ) indx++;
				vertices[i][1] = atof(text+indx+1);
				indx++;
				while(text[indx] != ';' ) indx++;
				vertices[i][2] = atof(text+indx+1);
			}
			nVertices = i;
			
			// read indices
			getline((char**)&text,&len,fp);				//  reading number of indices from file
			number = atoi(text);
			fprintf( stderr, "\nIndices  : %d", number );
			for( i=0; i<number; i++ )
			{
				getline( (char**)&text, &len, fp );			// reading indices from file
				indx = 0;
				while(text[indx] != ';' ) indx++;			// seperating vertices
				indices[i][0] = atoi(text+indx+1);
				indx++;
				while(text[indx] != ',' ) indx++;
				indices[i][1] = atoi(text+indx+1);
				indx++;
				while(text[indx] != ',' ) indx++;
				indices[i][2] = atoi(text+indx+1);
			}
			nIndices = i;
		free(text);
		text = NULL;
		return 1;
	}
	
	void View_Model()										
	{
		KBflag = 1;										// set to use direction keys for rotating
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(0.0f,1.0f,0.0f);
		int k;
		
		glBegin(GL_TRIANGLES);							// initiate start of triangles co-ordinates
		for(k=0; k<nIndices; k++ )						// loading vertices of mesh
		{
			//glScalef(scale,scale,scale);	
			glVertex3fv(vertices[indices[k][0]]);
			glVertex3fv(vertices[indices[k][1]]);
			glVertex3fv(vertices[indices[k][2]]);
		}
		glEnd();											//  end of triangle co-ordinates
	}
	
	void Create_Model()									// creating self plotted models
	{
		KBflag = 1;
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(0.0f,1.0f,0.0f);
		if(nVertices < 2 ) return;
		int k;
		
		glBegin(GL_LINE_STRIP);							// start of Line co-ordinates
		for(k=0; k<nVertices; k++ )
		{
			glVertex3fv(vertices[k]);
			glVertex3fv(vertices[k]);
			glVertex3fv(vertices[k]);
		}
		glEnd();											// end of Line Co-ordinates
	}
	
	void Simulate()										// Simulation of point in gravity
	{	
		// The particle bounces as the starting point and velocity vectors are changed 
		//  as the particle hits ground(y=0 or x-z plane)... (the simulation continues till time = limit)
		KBflag = 1;
		int i;
		float t,limit = 100;
		float S[3] = {0,0,0};
		u[0] = velocity[0];
		u[1] = velocity[1];
		u[2] = velocity[2];
		float temp_S[3] = { 0,0,0};
		glColor3f(0,1,0);
		glPointSize(2);						// increasing point size to be visible
		glBegin(GL_POINTS);
		for(t = 0; t<limit;t+=.01)
		{
			for(i = 0; i<3;i++)
				temp_S[i] = S[i] + u[i] * t + .5 * g[i] * t *t;
			if(temp_S[1] < 0 ) 				// condition for particle bouncing...
			{
				u[1] = u[1] * Co_Restitution;	//  Co_restitution is co-effecient of restitution ...
				S[0] = temp_S[0];				//  resetting starting point
				S[1] = 0.0f;
				S[2] = temp_S[2];
				limit-=t;
				t= 0;
			}
			glVertex3f(temp_S[0],temp_S[1],temp_S[2]);
		}
		glEnd();
		glPointSize(1);						// resetting point size
	}
	
	// the first screen as you start the app.
	void FrontPage()
	{
		glColor3f(1,0,0);						
		printing("3D Simulater",-7,20);
		printing("Project Designed by : ",-9,-12);
		printing("Nikhil Stephen Joseph",-7,-17);
		printing("Press F1 Key for Help anytime .....",-7,-23);		
		///    option section
		char text[4][30] = {"View Models","Create Model","View Simulation"};
		if(opt < 0 ) opt = 2;					//	option selection
		if(opt > 2) opt = 0;
		for(int j = 0; j<3; j++)
		{
			if(opt == j) glColor3f(0,1,0);
			else glColor3f(1,1,1);
			printing(text[j],-6,10-j*5);
		}
		if(flag == 1)							// Enter key Pressed
		{
			
			switch(opt)
			{
				case 0:  page = 2;  flag = 0;    break;
				case 1:  page = 3;  flag = 0;    break;
				case 2:  page = 4;  flag = 0;    break;
				case 3:  page = 8;  flag = 0;    break;
			}
			glutPostRedisplay();
		}
	}
	
	// second page, asks for the model's name which need to be loaded.
	void SecondPage()
	{
		glColor3f(1,0,0);
		printing("View Models",-5,20);
		printing("Enter Name of model : ",-6,0);
		
		if(ipMode == 0) ipMode =1;				// input
		printing(ipBuffer,0,-2);
		
		if(ipMode < 2) return;
		strcat(ipBuffer,".vtx"); 
		if(Read_File() == -1 ) 
		{
			ipMode = 1;
			ipBuffer[0] = '\0';
			char err[] = {"Error opening file ...."};
			printing(err,0,-8);
			return;
		}
		ipMode = 0;								// resetting values for further use
		ipBuffer[0] = '\0';
		page = 5;
		glutPostRedisplay();
	}	
	
	// third screen, used to make custom models. One needs patience for this.
	void ThirdPage()
	{
		
		char text[2][50] = {"Create Models","Enter co-ordinates(x,y,z).....(0,0,0) to exit : "};
		glColor3f(1,0,0);
		printing(text[0],-5,20);
		printing(text[1],-6,0);
		
		if(ipMode == 0) 							// input
		{
			nVertices = 0;
			ipMode =1;
		}
		printing(ipBuffer,0,-2);
		
		if(ipMode < 2) 
			return;
			
		// input complete
		int i=0;		
		vertices[nVertices][0] = atof(ipBuffer);		// seperating x,y,z
		while(ipBuffer[i] != ',' ) i++;
		vertices[nVertices][1] = atof(ipBuffer+i+1);
		i++;
		while(ipBuffer[i] != ',' ) i++;
		vertices[nVertices][2] = atof(ipBuffer+i+1);
		
		if(fabs(vertices[nVertices][0]) <  0.0001 && 				// condition for exit
				 fabs(vertices[nVertices][1]) < 0.0001 && 
				 fabs(vertices[nVertices][2]) < 0.0001)
		{
			ipMode = 0;							// resetting values for further use
			page = 6;
			ipBuffer[0] = '\0';
			return;
		}
		nVertices++;								// resetting values to continue with function
		ipMode = 1;
		ipBuffer[0] = '\0';
		glutPostRedisplay();
	}
	
	// fourth screen, Particle simulation under gravity.
	void FourthPage()
	{
		glColor3f(1,0,0);
		printing("Simulation",-5,20);
		printing("Here we simulate a gravitation field Effect on a particle : ",-20,0);
		printing("Please Enter the velocity vector (ai+bj+ck)..... only values of a,b,c : ",-20,-3);
	
		if(ipMode == 0) 						// input
			ipMode =1;
		printing(ipBuffer,0,-8);
		
		if(ipMode < 2) return;
		// input complete
		int i=0;		
		velocity[0] = atof(ipBuffer);				// seperating velocity vectors
		while(ipBuffer[i] != ',' ) i++;
		velocity[1] = atof(ipBuffer+i+1);
		i++;
		while(ipBuffer[i] != ',' ) i++;
		velocity[2] = atof(ipBuffer+i+1);
		
		ipMode = 0;							// resetting values for further use
		page = 7;
		ipBuffer[0] = '\0';
		return;
	}
	
	// help screen.
	void HELP()
	{
		glColor3f(1,0,0);
		printing("Directions to use this Software",-20,21);
		glColor3f(1,1,1);
		printing("Use Up and Down Direction Keys to Rotate Mesh ",-22,18);
		printing("Up/Down  --> Along X-axis",-15,15);
		printing("Right/Left --> Along y-axis",-15,12);
		printing("Use 1 and 2 to zoom in and zoom out along X-axis",-22,9);
		printing("1 --> zoom  in  along X-axis",-15,6);
		printing("2 --> zoom out along X-axis",-15,3);
		printing("Press Escape to go to the previous menu",-22,0);
		printing("Use F5 key to toggle Co-ordinate axis ON/OFF",-22,-3);
		printing("Colour of Co-ordinate Axis :",-22,-6);
		printing("X-Axis -->  Red",-15,-9);
		printing("Y-Axis -->  Blue",-15,-12);
		printing("Z-Axis -->  Yellow",-15,-15);
		printing("Models provided in View Model :",-22,-18);
		printing("cuboid,  pyramid,  sphere,  hedra,  cylinder,  cone",-15,-21);
		printing("male,  female,  torous,  torous_knot,  teapot,  dobby",-15,-24);		
		printing("Name of Models are case Sensitive",-22,-27);		
	}
	
	void render()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
															// calling function according to page value
		switch(page)
		{
			case 1: 
				BackGround();   
				FrontPage();      
			break;
			
			case 2: 
				BackGround();   
				SecondPage();   
			break;
			
			case 3: 
				BackGround();   
				ThirdPage();      
			break;
			
			case 4: 
				BackGround();   
				FourthPage();    
			break;
			
			case 5: 
				show_Axis();     
				View_Model();  
			break;
			
			case 6: 
				show_Axis();     
				Create_Model();  
			break;
			
			case 7: 
				show_Axis();     
				Simulate();  
			break;
			
			case 8: 
				BackGround();   
				HELP();  
			break;
		}
		glutSwapBuffers();
	}
	
	void SPKboard(int key,int x,int y)					// Special keyboard Function
	{
		if(key == GLUT_KEY_F1)    
			page = 8 ; 		// Help page
			
		if(KBflag == 0)
		{
			switch(key)
			{
				case GLUT_KEY_UP:
					opt--; 
					if(opt < 0 ) 
						opt =2;
					break; 

				case GLUT_KEY_DOWN:  
					opt++; 
					opt=(opt%3);
				break; 
			}
		}
		
		if(KBflag == 1)
		{
			switch(key)
			{
				case GLUT_KEY_UP:         glRotatef(-5,1,0,0);       break; 
				case GLUT_KEY_RIGHT:  glRotatef(-5,0,1,0);       break; 
				case GLUT_KEY_LEFT:     glRotatef(5,0,1,0);     break; 
				case GLUT_KEY_DOWN:  glRotatef(5,1,0,0);     break; 
				case GLUT_KEY_F5:          show_axis++;     break; 
			}
		}
		glutPostRedisplay();
	}
	
	void Timer( int time)						// Timer set for function BackGround()
	{
		glutPostRedisplay();
		glutTimerFunc(0,Timer,0);
	}
	
	void KBHandler(unsigned char key, int x, int y)			// Keyboard key handler
	{
		if(key  == 27) 						// Escape key code
		{
			KBflag = 0;
			glLoadIdentity();
			glTranslatef(0,0,-50);
			switch(page)						// setting to previous page
			{
				case 1: exit(0);
				case 2: page =1;    break;
				case 3: page =1;    break;
				case 4: page =1;    break;
				case 5: page =2;	break;
				case 6: page =3;	break;
				case 7: page =4;	break;
				case 8: page =1;	break;
			}
			
			if(page == 1) 
				ipMode =0;
		}
		
		if(ipMode == 0)
		{
			switch(key)						// Zoom in --- Zoom out condition
			{
				case 13:   flag = 1; break;
				case '1':   trans =5;   glTranslatef(0,0,trans);     break;
				case '2':   trans=-5;   glTranslatef(0,0,trans);     break;
			}
		}
		else if(ipMode == 1)					// input mode
		{
			switch(key)
			{
				case 13: ipMode = 2; break;
				case 8: 
					if(strlen(ipBuffer) > 0) 
						ipBuffer[strlen(ipBuffer)-1] = '\0';
					break;
				default:
					ipBuffer[strlen(ipBuffer)+1] = '\0';
					ipBuffer[strlen(ipBuffer)] = key;       
					break;			
			}
		}
		glutPostRedisplay();
	}
	
	int main()
	{
		glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);		// initiating Display
		int win = glutCreateWindow("nik");					// Creating window
		glutReshapeWindow(800,600);						// resizing to 800 by 600
		setupGL();										//  Setting initial settings for drawing
		glutDisplayFunc(render);							// registering functions
		glutKeyboardFunc(KBHandler);
		glutSpecialFunc(SPKboard);
		glutTimerFunc(0,Timer,0);
		glutMainLoop();									// infinite loop on registered functions in main
	}