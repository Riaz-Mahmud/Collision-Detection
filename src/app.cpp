
#include <gl/glut.h>
#include "app.h"

// Getter for the width of the application window
int Application::getwidth()
{
    return width;
}

// Getter for the height of the application window
int Application::getheight()
{
    return height;
}

// Getter for the time interval
float Application::getTimeinterval()
{
    return timeinterval;
}

// Setter for the time interval
void Application::setTimeinterval(float timeinterval)
{
    Application::timeinterval = timeinterval;
}

// Keyboard input handler
void Application::keyboard(unsigned char key, int x, int y)
{
}

// Initialize graphics settings, including the clear color
void Application::initGraphics()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f );	
}


void Application::display()
{
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
}

// Update function to trigger a redisplay
void Application::update()
{
    glutPostRedisplay();
}

// Resize function to handle window resizing
void Application::resize(int width, int height)
{
    //nRange = 100.0f;
    GLfloat aspectRatio = (GLfloat)width / (GLfloat)height;

    // Prevent a divide by zero
    if (height == 0)
        height = 1;

    // Set Viewport to window dimensions
    glViewport(0, 0, width, height);

	// Reset coordinate system
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

     //Establish clipping volume (left, right, bottom, top, near, far)
    if (width <= height) 
	{
	    Application::width = nRange;
        Application::height = nRange/aspectRatio;
		glOrtho (-nRange, nRange, -nRange/aspectRatio, nRange/aspectRatio, -nRange*2.0f, nRange*2.0f);
	}
    else 
	{
	    Application::width = nRange*aspectRatio;
        Application::height = nRange;
        glOrtho (-nRange*aspectRatio, nRange*aspectRatio, -nRange, nRange, -nRange*2.0f, nRange*2.0f);
	}

	// Reset the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}