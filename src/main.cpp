#include <gl/glut.h>
#include "app.h"
#include <iostream>

using namespace std;

// Global application object
extern Application* getApplication();
Application* app;

// Function to handle rendering
void display(void)
	{
	app->display();
	}

// Function to create the window
void createWindow(const char* title, int h, int w)
{
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(w, h);
    glutCreateWindow(title);
	
}

// Timer function for periodic updates
void TimerFunc(int value)
{
    app->update(); // Update the application state
	float  timeinterval = app->getTimeinterval(); // Set up a timer to repeatedly call itself with a specified time interval
	glutTimerFunc(timeinterval, TimerFunc, 1);
}

// Function to handle keyboard input
void keyboard(unsigned char key, int x, int y)
{
	app->keyboard(key, x, y);
}

// Function to handle window resizing
void resize(int width, int height)
{
    app->resize(width, height);
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    app = getApplication(); // Get the application object

	// Set the time interval for updates
	float  timeinterval = 10;
	app->setTimeinterval(timeinterval);

	// Create the window
	createWindow("Assignment 1", app->getheight(), app->getwidth());

	// Set up callback functions
	glutReshapeFunc(resize);
	glutDisplayFunc(display); 
	glutTimerFunc(timeinterval, TimerFunc, 1);
	app->initGraphics();
	glutKeyboardFunc(keyboard);

	// Enter the GLUT main loop
	glutMainLoop();

	// Clean up and exit
	delete app; 
	return 0;
}
