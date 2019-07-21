/**		Name: Michael Del Rosario and David Zweig
* 		CS6323, Final Project
* 		Section 001
*		Due Date: 12/04/2017
*		Functional description:
*
*		This project demonstrates a roller ball minigame
*       where the objective is to keep a ball on a path and avoid obstacles.
*/

#include<iostream>
#include<fstream>
#include<stdexcept>
#include<sstream>
#include<cmath>
#include <vector>
#include <array>
#include <stdlib.h>     /* For random number generator */
#include <time.h>       /* for time */
#include "GL/freeglut.h"
#include "GL/glu.h"
#include "AntTweakBar.h"
#include "vmath.h"

#define PI 3.1415926
#define BUFFER_OFFSET(offset) ((void*) (offset))

typedef enum { PTS, WIREFRAME, SOLID } primitive;
typedef enum { FLAT, SMOOTH } shademode;

// Diamond Object Global Variables
float DIAMOND_SPEED = .002f; // The speed at which a diamond moves towards the platform
float diamondFloat[5] = {0, 0, 0, 0, 0}; // Z-values for where diamonds are set to
int diamondCount = 1; // Number of diamonds that appear on screen

class animation // Supports the rotating cube
{
	public:
		int currenttime = 1;
		const int maxtime = 1200;
		const float fps = 30.0;

		// Diamond object variables
		GLfloat y_rotation = 0; // A variable that constantly increases every display
		GLfloat x_pos[5] = {0, 0, 0, 0, 0}; // X-positions of the diamonds
		bool diamondPlaced[5] = {false, false, false, false, false}; // Array that tells if each diamond is in play
	
		// Player variables
		GLfloat playerRotation; // The rotation angle of the platform, also used for the ball
		GLfloat pV; // Ball Velocity
		GLfloat pA; // Ball Distance

		// For game
		bool gameStart; // When true, game's being played. Initially false
		float score; // The current game score
		float highScore = 0; // Highest score achieved; updated if player gets a score greater than the current one
		int Speed_timer = 0; // A variable that increases over playtime and decreases once it reaches a certain threshold
		int seed; // RNG seed 

		void init() // Initialize variables
		{
			currenttime = 1;
			gameStart = false;
			playerRotation = 0;
			Speed_timer = 0;
			pV = 0;
			pA = 0;
			score = 0;
			srand((unsigned int) time(NULL)); // Set the seed for the rand() function
		}

		void reset()
		{
			y_rotation = 0; // Set rotation to 0 on Y
			DIAMOND_SPEED = 0.002f; // Reset diamond speed to where it's easy again at first
			init(); // Everything else is normal
		}

		void update() // Increment the WCS Y-rotation and LCS Z-rotation as necessary
		{
			if (gameStart) // As game plays out...
			{
				// If diamonds have not been placed, place them 
				for (int i = 0; i < 5; i++)
				{
					if (!diamondPlaced[i])
					{
						srand((unsigned int) time(NULL)); // Set the seed for the rand() function
						x_pos[i] = (float) (rand() % 16 - 8); // Randomly places the diamond
						diamondPlaced[i] = true; // The diamond is placed
					}
				}

				score += .001f; // Increment score
				Speed_timer++; // Increment speed_timer

				if (Speed_timer - 10000 >= 0) // If the speed timer reaches a threshold of 10000...
				{
					Speed_timer = 0; // New lap
					DIAMOND_SPEED += 0.0001f; // The rate of diamond translation increases
				}

				if (score > highScore) // Update high score if score surpasses high score
					highScore = score;

				// Check player's platform rotation and move the ball if platform has rotated.
				if (playerRotation != 0) // If the rotation angle is non-zero...
				{
					pV = playerRotation * 0.0001f; // Adjust the velocity
					pA -= pV; // Adjust distance from X-axis
				}

				else
					pV = 0; // Velocity is 0 on flat platform

				if (pA > 8 || pA < -8) // If distance is too high or too low, that indicates that the ball is going to fall off
					reset(); // Game over, man...

				for (int i = 0; i < 5; i++) // Check each diamond object's position and reset it if diamond moves too far
				{
					if (diamondFloat[i] > 40)
					{
						diamondFloat[i] = 0; // Diamonds restart their Z-paths
						diamondPlaced[i] = false; // Diamond is unplaced, so update it
					}
				}
			}

			else // Game has not started yet
			{
				playerRotation = 0; // Player rotation is reset to 0
				for(int i = 0; i < 5; i++) // No diamonds will be put in place
					diamondPlaced[i] = false;
				for (int j = 0; j < 5; j++) // Diamonds are not to get a head start on their paths
					diamondFloat[j] = 0;
			}
		}
};

class color
{
	public:
		vmath::vec4 ambient;
		vmath::vec4 diffuse;
		vmath::vec4 specular;
		float shine;

		color()
		{
			ambient = vmath::vec4(0, 0, 0, 1);
			diffuse = vmath::vec4(0, 0.5, 0, 1);
			specular = vmath::vec4(0, 0, 0.5, 1);
			shine = 1.0;
		}
};

class model
{
public:
	const int trinum_ = 12;

	GLfloat platform_[108] = {-8.0f,-0.5f,-1.0f, // triangle 1 : begin
								-8.0f, -0.5f, 1.0f,
								-8.0f, 0.5f, 1.0f, // triangle 1 : end
								8.0f, 0.5f, -1.0f, // triangle 2 : begin
								-8.0f, -0.5f, -1.0f,
								-8.0f, 0.5f, -1.0f, // triangle 2 : end
								8.0f, -0.5f, 1.0f, -8.0f, -0.5f, -1.0f, 8.0f, -0.5f, -1.0f,
								8.0f, 0.5f, -1.0f, 8.0f, -0.5f, -1.0f, -8.0f, -0.5f, -1.0f,
								-8.0f, -0.5f, -1.0f, -8.0f, 0.5f, 1.0f, -8.0f, 0.5f, -1.0f,
								8.0f, -0.5f, 1.0f, -8.0f, -0.5f, 1.0f, -8.0f, -0.5f, -1.0f,
								-8.0f, 0.5f, 1.0f, -8.0f, -0.5f, 1.0f, 8.0f, -0.5f, 1.0f,
								8.0f, 0.5f, 1.0f, 8.0f, -0.5f, -1.0f, 8.0f, 0.5f, -1.0f,
								8.0f, -0.5f, -1.0f, 8.0f, 0.5f, 1.0f, 8.0f, -0.5f, 1.0f,
								8.0f, 0.5f, 1.0f, 8.0f, 0.5f, -1.0f, -8.0f, 0.5f, -1.0f,
								8.0f, 0.5f, 1.0f, -8.0f, 0.5f, -1.0f, -8.0f, 0.5f, 1.0f,
								8.0f, 0.5f, 1.0f, -8.0f, 0.5f, 1.0f, 8.0f, -0.5f, 1.0f };

	GLfloat diamond_[144] = {0.0f, 8.0f, 0.0f, // triangle begin 1
								0.5f, 0.0f, 0.5f,
								0.5f, 0.0f, -0.5f, // triangle end
								0.0f, 8.0f, 0.0f, // triangle begin 2
								0.5f, 0.0f, 0.5f,
								-0.5f, 0.0f, 0.5f, // triangle end
								0.0f, 8.0f, 0.0f, // triangle begin 3
								-0.5f, 0.0f,-0.5f,
								0.5f, 0.0f, -0.5f, // triangle end
								0.0f, 8.0f, 0.0f, // triangle begin 4
								-0.5f, 0.0f, -0.5f,
								-0.5f, 0.0f, 0.5f, // triangle end

								0.0f, 8.0f, 0.0f, // triangle begin 5
								-0.5f, 0.0f, 0.5f,
								0.5f, 0.0f, 0.5f, // triangle end
								0.0f, 8.0f, 0.0f, // triangle begin 6
								-0.5f, 0.0f, 0.5f,
								-0.5f, 0.0f, -0.5f, // triangle end
								0.0f, 8.0f, 0.0f, // triangle begin 7
								0.5f, 0.0f,-0.5f,
								0.5f, 0.0f, 0.5f, // triangle end
								0.0f, 8.0f, 0.0f, // triangle begin 8
								0.5f, 0.0f, -0.5f,
								-0.5f, 0.0f, -0.5f, // triangle end

								0.0f, -8.0f, 0.0f, // triangle begin 9
								0.5f, 0.0f, 0.5f,
								0.5f, 0.0f, -0.5f, // triangle end
								0.0f, -8.0f, 0.0f, // triangle begin 10
								0.5f, 0.0f, 0.5f,
								-0.5f, 0.0f, 0.5f, // triangle end
								0.0f, -8.0f, 0.0f, // triangle begin 11
								-0.5f, 0.0f,-0.5f,
								0.5f, 0.0f, -0.5f, // triangle end
								0.0f, -8.0f, 0.0f, // triangle begin 12
								-0.5f, 0.0f, -0.5f,
								-0.5f, 0.0f, 0.5f, // triangle end

								0.0f, -8.0f, 0.0f, // triangle begin 13
								-0.5f, 0.0f, 0.5f,
								0.5f, 0.0f, 0.5f, // triangle end
								0.0f, -8.0f, 0.0f, // triangle begin 14
								-0.5f, 0.0f, 0.5f,
								-0.5f, 0.0f, -0.5f, // triangle end
								0.0f, -8.0f, 0.0f, // triangle begin 15
								0.5f, 0.0f,-0.5f,
								0.5f, 0.0f, 0.5f, // triangle end
								0.0f, -8.0f, 0.0f, // triangle begin 16
								0.5f, 0.0f, -0.5f,
								-0.5f, 0.0f, -0.5f // triangle end
							};

	GLfloat dnormal_[144] = {0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f
	};

	GLfloat normal_[108] = { -1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f };
};

class camera
{
	public:
		vmath::vec3 rot; // This is rotation for camera
		vmath::vec3 trans; // This is translation for camera
		float near_;
		float far_;
		vmath::vec3 lastrot;
		vmath::vec3 lasttrans;

		// Transformation
		vmath::vec3 eye;
		vmath::vec3 u;
		vmath::vec3 v;
		vmath::vec3 w;
		vmath::mat4 M;

		void init()
		{
			near_ = (float) std::min(0.1, eye[2] + 0.1);
			far_ = (float) std::max(50.0, eye[2] + 40.0);
			reset();
		}

		void reset()
		{
			rot = vmath::vec3(-20, 0, 0); // Adjust camera rotation on X
			trans = vmath::vec3(0, 8, -1); // Adjust camera translation on Y and Z; this will give a better view of obstacles ahead
			u = vmath::vec3(1, 0, 0);
			v = vmath::vec3(0, 1, 0);
			w = vmath::vec3(0, 0, 1);
			eye = vmath::vec3(0, 0, 15);
			lastrot = vmath::vec3(0.0, 0.0, 0.0);
			lasttrans = vmath::vec3(0.0, 0.0, 0.0);
			M = vmath::mat4::identity();
		}

		void update()
		{
			if (rot[0] != lastrot[0] || rot[1] != lastrot[1] || rot[2] != lastrot[2])
			{
				vmath::mat4 rt = vmath::mat4::identity();
				if (rot[0] != lastrot[0])
					rt = vmath::rotate(-rot[0] + lastrot[0], vmath::vec3(1, 0, 0));
				else if (rot[1] != lastrot[1])
					rt = vmath::rotate(-rot[1] + lastrot[1], vmath::vec3(0, 1, 0));
				else if (rot[2] != lastrot[2])
					rt = vmath::rotate(-rot[2] + lastrot[2], vmath::vec3(0, 0, 1));
				M = rt * M;
				lastrot = rot;
			}

			if (trans[0] != lasttrans[0] || trans[1] != lasttrans[1] || trans[2] != lasttrans[2])
			{
				vmath::vec3 tt = vmath::vec3(0, 0, 0);
				if (trans[0] != lasttrans[0])
					tt = -(trans[0] - lasttrans[0]) * u;
				else if (trans[1] != lasttrans[1])
					tt = -(trans[1] - lasttrans[1]) * v;
				else if (trans[2] != lasttrans[2])
					tt = -(trans[2] - lasttrans[2]) * w;
				vmath::mat4 mt = vmath::translate(tt);
				M = mt * M;
				lasttrans = trans;
			}
		}
};

class scene
{
	public:
		bool cw;
		primitive prim;
		shademode shading;
		bool reset_;
		camera camera_;
		animation animate_;

		// Light parameters
		bool lightson;
		color light1;
		color light2;

		scene()
		{
			init();
		}

		void init()
		{
			cw = false; // Using GL_CW
			prim = SOLID;
			shading = FLAT;
			reset_ = false;
			camera_.init();
			animate_.init();
			reset();
		}

		void reset()
		{
			camera_.reset();
			animate_.reset();
			lightson = true;
			light1.ambient = vmath::vec4(0.5, 0.5, 0.5, 1.0);
			light1.diffuse = vmath::vec4(0.5, 0.5, 0.5, 1.0);
			light1.specular = vmath::vec4((float) 0.1, (float) 0.1, (float) 0.1, (float) 1.0);
			light2.ambient = vmath::vec4(0.5, 0.5, 0.5, 1.0);
			light2.diffuse = vmath::vec4(0.5, 0.5, 0.5, 1.0);
			light2.specular = vmath::vec4((float) 0.1, (float) 0.1, (float) 0.1, (float) 1.0);
		}
};

model gmodel_;
scene g_scene_;

void TW_CALL CopyStdStringToClient(std::string & dst, const std::string & src)
{
	dst = src;
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	TwWindowSize(width, height);
}

void setmaterial_old(std::string whichone)
{
	vmath::vec4 ambient, diffuse, specular;
	GLfloat shine = 0.5;

	if (whichone == "cube")
	{
		ambient = vmath::vec4((float) 0.4, (float) 0.0, (float) 0.0, (float) 1.0);
		diffuse = vmath::vec4((float) 0.6, (float) 0.0, (float) 0.0, (float) 1.0);
		specular = vmath::vec4(0.0, 0.0, 0.0, 1.0);
	}

	else
	{
		ambient = vmath::vec4(0.5, 0.5, 0.5, 1.0);
		diffuse = vmath::vec4(0.5, 0.5, 0.5, 1.0);
		specular = vmath::vec4(0.0, 0.0, 0.0, 1.0);
	}

	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shine);
}

void lightsInitialize()
{
	GLfloat light0_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light0_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light0_specular[] = { (GLfloat) 0.1, (GLfloat) 0.1, (GLfloat) 0.1, (GLfloat) 1.0 };
	GLfloat light1_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light1_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light1_specular[] = { (GLfloat) 0.1, (GLfloat) 0.1, (GLfloat) 0.1, (GLfloat) 1.0 };
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
}

void lightsPosition()
{
	GLfloat light0_position[] = { 0.0, 10.0, 3.0, 1.0 };
	GLfloat light1_position[] = { -5.0, -10.0, 2.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
}

void display()
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);
	glDepthFunc(GL_LEQUAL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glFrontFace(GL_CCW);

	if (g_scene_.reset_)
	{
		g_scene_.reset();
		g_scene_.reset_ = false;
	}

	glShadeModel(GL_FLAT);
	lightsInitialize();
	lightsPosition();

	// Projection matrix
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	vmath::mat4 projectionmat = vmath::perspective(60, (float) width / (float) height, g_scene_.camera_.near_, g_scene_.camera_.far_);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (double) width / (double) height, g_scene_.camera_.near_, g_scene_.camera_.far_);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	g_scene_.camera_.update();
	glLoadIdentity();
	vmath::mat4 Mlookat = vmath::lookat(g_scene_.camera_.eye, g_scene_.camera_.eye - vmath::vec3(0, 0, 1), vmath::vec3(0, 1, 0));
	Mlookat = g_scene_.camera_.M * Mlookat;
	GLfloat m[16];
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			m[4 * i + j] = Mlookat[i][j];
	glMultMatrixf(m);

	// Platform
	glPushMatrix(); // Push matrix for platform
	g_scene_.animate_.update();
	glRotatef(g_scene_.animate_.playerRotation, 0, 0, 1.0f); // Rotate Player Platform
	setmaterial_old("cube"); // The platform will be red, just like the ball
	glBegin(GL_TRIANGLES); // Begin drawing phase

	for (int i = 0; i < gmodel_.trinum_ * 3; ++i) // Draw the platform
	{
		glVertex3f(gmodel_.platform_[3 * i], gmodel_.platform_[3 * i + 1], gmodel_.platform_[3 * i + 2]);
		glNormal3f(gmodel_.normal_[3 * i], gmodel_.normal_[3 * i + 1], gmodel_.normal_[3 * i + 2]);
	}

	glEnd(); // End drawing phase

	// Sphere           
	glPushMatrix(); // Push matrix for sphere
	glTranslatef(g_scene_.animate_.pA, 1.0, 0.0); // Translate sphere on the top of the platform
	glRotatef(-g_scene_.animate_.pA * 100, 0.0f, 0.0f, 1.0f); // Rotate sphere on the platform
	glutSolidSphere(1.0, 20, 50); // Draw Sphere
	glPopMatrix(); // Pop Sphere and Platform matrices

	if (g_scene_.animate_.gameStart) // Deal with diamonds if game is playing
	{
		int currentScore = (int) g_scene_.animate_.score; // Get the score in integer form
		diamondCount = (currentScore / 100) + 1; // Number of diamonds should be porportional to the current player score

		if (diamondCount > 5) // No more than 5 diamonds
			diamondCount = 5;

		switch (diamondCount) // What happens next depends on the number of diamonds that should appear. More diamonds are drawn with high count
		{
			case 5:
				glPushMatrix(); // Push matrix for diamond
				g_scene_.animate_.update(); // Call update to adjust player and obstacle variables
				glTranslatef(0.0f, 0.0f, -20); // (0, 0, -20) for starting position, before determining X-position and path
				
				if (g_scene_.animate_.gameStart) // If the game is going...
				{
					glTranslatef(g_scene_.animate_.x_pos[4], 0.0f, diamondFloat[4]); // Diamond X-position is set, but Z-position increases
					diamondFloat[4] += DIAMOND_SPEED; // The appropriate diamond in the array gets new Z-position by the diamond speed
				}

				glRotatef(g_scene_.animate_.y_rotation, 0, 1.0, 0); // Diamond rotates on local Y axis
				glMaterialfv(GL_FRONT, GL_AMBIENT, vmath::vec4(1.0f, 0.0f, 1.0f, 1.0f)); // Set the ambient color to magenta
				glMaterialfv(GL_FRONT, GL_DIFFUSE, vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f)); // Set the diffuse color to white
				glBegin(GL_TRIANGLES); // Begin drawing phase for diamond

				for (int i = 0; i < 16 * 3; i++) // Draw the diamond
				{
					glVertex3f(gmodel_.diamond_[3 * i], gmodel_.diamond_[3 * i + 1], gmodel_.diamond_[3 * i + 2]);
					glNormal3f(gmodel_.dnormal_[3 * i], gmodel_.dnormal_[3 * i + 1], gmodel_.dnormal_[3 * i + 2]);
				}

				glEnd(); // End drawing phase for diamond
				glPopMatrix(); // Pop matrix for diamond
				
			case 4:
				glPushMatrix();
				g_scene_.animate_.update();
				glTranslatef(0.0f, 0.0f, -20);
				
				if (g_scene_.animate_.gameStart)
				{
					glTranslatef(g_scene_.animate_.x_pos[3], 0.0f, diamondFloat[3]);
					diamondFloat[3] += DIAMOND_SPEED;
				}

				glRotatef(g_scene_.animate_.y_rotation, 0, 1.0, 0);
				glMaterialfv(GL_FRONT, GL_AMBIENT, vmath::vec4(1.0f, 0.0f, 1.0f, 1.0f)); // Set the ambient color to magenta
				glMaterialfv(GL_FRONT, GL_DIFFUSE, vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f)); // Set diffuse color to white
				glBegin(GL_TRIANGLES);

				for (int i = 0; i < 16 * 3; i++)
				{
					glVertex3f(gmodel_.diamond_[3 * i], gmodel_.diamond_[3 * i + 1], gmodel_.diamond_[3 * i + 2]);
					glNormal3f(gmodel_.dnormal_[3 * i], gmodel_.dnormal_[3 * i + 1], gmodel_.dnormal_[3 * i + 2]);
				}

				glEnd();
				glPopMatrix();

			case 3:
				glPushMatrix();
				g_scene_.animate_.update();
				glTranslatef(0.0f, 0.0f, -20);
				
				if (g_scene_.animate_.gameStart)
				{
					glTranslatef(g_scene_.animate_.x_pos[2], 0.0f, diamondFloat[2]);
					diamondFloat[2] += DIAMOND_SPEED;
				}

				glRotatef(g_scene_.animate_.y_rotation, 0, 1.0, 0);
				glMaterialfv(GL_FRONT, GL_AMBIENT, vmath::vec4(1.0f, 0.0f, 1.0f, 1.0f)); // Set the ambient color to magenta
				glMaterialfv(GL_FRONT, GL_DIFFUSE, vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f)); // Set the diffuse color to white
				glBegin(GL_TRIANGLES);

				for (int i = 0; i < 16 * 3; i++)
				{
					glVertex3f(gmodel_.diamond_[3 * i], gmodel_.diamond_[3 * i + 1], gmodel_.diamond_[3 * i + 2]);
					glNormal3f(gmodel_.dnormal_[3 * i], gmodel_.dnormal_[3 * i + 1], gmodel_.dnormal_[3 * i + 2]);
				}

				glEnd();
				glPopMatrix();

			case 2:
				glPushMatrix();
				g_scene_.animate_.update();
				glTranslatef(0.0f, 0.0f, -20);
				
				if (g_scene_.animate_.gameStart)
				{
					glTranslatef(g_scene_.animate_.x_pos[1], 0.0f, diamondFloat[1]);
					diamondFloat[1] += DIAMOND_SPEED;
				}

				glRotatef(g_scene_.animate_.y_rotation, 0, 1.0, 0);
				glMaterialfv(GL_FRONT, GL_AMBIENT, vmath::vec4(1.0f, 0.0f, 1.0f, 1.0f)); // Set the ambient color to magenta
				glMaterialfv(GL_FRONT, GL_DIFFUSE, vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f)); // Set the diffuse color to white
				glBegin(GL_TRIANGLES);

				for (int i = 0; i < 16 * 3; i++)
				{
					glVertex3f(gmodel_.diamond_[3 * i], gmodel_.diamond_[3 * i + 1], gmodel_.diamond_[3 * i + 2]);
					glNormal3f(gmodel_.dnormal_[3 * i], gmodel_.dnormal_[3 * i + 1], gmodel_.dnormal_[3 * i + 2]);
				}

				glEnd();
				glPopMatrix();

			case 1:
				glPushMatrix();
				g_scene_.animate_.update();
				glTranslatef(0.0f, 0.0f, -20);
				
				if (g_scene_.animate_.gameStart)
				{
					glTranslatef(g_scene_.animate_.x_pos[0], 0.0f, diamondFloat[0]);
					diamondFloat[0] += DIAMOND_SPEED;
				}

				glRotatef(g_scene_.animate_.y_rotation, 0, 1.0, 0);
				glMaterialfv(GL_FRONT, GL_AMBIENT, vmath::vec4(1.0f, 0.0f, 1.0f, 1.0f)); // Set the ambient color to magenta
				glMaterialfv(GL_FRONT, GL_DIFFUSE, vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f)); // Set the diffuse color to white
				glBegin(GL_TRIANGLES);

				for (int i = 0; i < 16 * 3; i++)
				{
					glVertex3f(gmodel_.diamond_[3 * i], gmodel_.diamond_[3 * i + 1], gmodel_.diamond_[3 * i + 2]);
					glNormal3f(gmodel_.dnormal_[3 * i], gmodel_.dnormal_[3 * i + 1], gmodel_.dnormal_[3 * i + 2]);
				}

				glEnd();
				glPopMatrix();
				
			default:
				g_scene_.animate_.y_rotation += 0.05f; // Update rotation for diamonds so that they look like they're spinning
				break;
		}

		for (int i = 0; i < 5; i++) // Collision check for each diamond
		{
			// If the diamond's Z-position is within platform bounds
			// and the distance between the ball and diamond is less than the sum of their radii... Game over, man.
			if ((abs(g_scene_.animate_.pA - g_scene_.animate_.x_pos[i]) <= 1) && (diamondFloat[i] > 20 && diamondFloat[i] < 21))
				g_scene_.reset();
		}
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(!g_scene_.animate_.gameStart)
		TwDraw();
	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}

void Terminate(void)
{
	TwTerminate();
}

void reset()
{
	g_scene_.reset();
}

void init_gui()
{
	TwBar *operbar = TwNewBar("Roller Ball");

	// Main GUI
	TwAddVarRW(operbar, "Game Start", TW_TYPE_BOOL8, &g_scene_.animate_.gameStart, "group='Main Menu'");
	TwAddVarRO(operbar, "Score", TW_TYPE_FLOAT, &g_scene_.animate_.score, "group='Main Menu' min=0 step=1");
	TwAddVarRO(operbar, "High Score", TW_TYPE_FLOAT, &g_scene_.animate_.highScore, "group='Main Menu' min=0 step=1");
	TwDefine(
		" GLOBAL help='How To Play:\nUse < / > arrow keys to rotate the platform and move the ball. Avoid the diamonds for as long as possible and do not roll off.'");
}

GLvoid Timer(int value)
{
	++g_scene_.animate_.currenttime;
	g_scene_.animate_.currenttime %= g_scene_.animate_.maxtime;
	if (value)
		glutPostRedisplay();
	glutTimerFunc((unsigned int) (1200 / g_scene_.animate_.fps), Timer, value);
}

// Based on the keyboard input, be able to rotate the platform and ball with left and right arrow keys
void processKeys(int Key, int x, int y)
{
	if (g_scene_.animate_.gameStart && Key != 0) // While the game is playing...
	{
		if (Key == 100 && g_scene_.animate_.playerRotation < 45) // When right arrow key is pressed...
			g_scene_.animate_.playerRotation += 2.5f; // Rotate right while rotation is under 45 degrees
		else if (Key == 102 && g_scene_.animate_.playerRotation > -45) // When left arrow key is pressed...
			g_scene_.animate_.playerRotation -= 2.5f; // Rotate left while rotation is over -45 degrees
	}
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(800, 600);
	glutCreateWindow("CS 6323.001 Del Rosario Zweig Final Project");
	glutCreateMenu(NULL);
	TwInit(TW_OPENGL, NULL);
	glutMouseFunc((GLUTmousebuttonfun) TwEventMouseButtonGLUT);
	glutMotionFunc((GLUTmousemotionfun) TwEventMouseMotionGLUT);
	glutPassiveMotionFunc((GLUTmousemotionfun) TwEventMouseMotionGLUT);
	glutKeyboardFunc((GLUTkeyboardfun) TwEventKeyboardGLUT);
	glutTimerFunc((unsigned int) (1200 / g_scene_.animate_.fps), Timer, 0);
	glutSpecialFunc((GLUTspecialfun) TwEventSpecialGLUT);
	TwGLUTModifiersFunc(glutGetModifiers);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(processKeys); // Set functionality for keys
	init_gui();
	glutMainLoop();
	atexit(Terminate);
	return 0;
}