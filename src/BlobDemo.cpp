/*
 * The Blob.
 *
 */
#include <gl/glut.h>
#include "app.h"
#include "coreMath.h"
#include "pcontacts.h"
#include "pworld.h"
#include <stdio.h>
#include <vector>
#include <cassert>
#include <math.h>
#include <iostream>
#include <time.h>
#include <list>
#include <ParticleCollision.h>
#include <ctime>
#include <chrono>

#define BLOB_COUNT 6
#define PLATFORM_COUNT 11

using namespace std;
using namespace std::chrono;

const Vector2 Vector2::GRAVITY = Vector2(0, -9.81);

class Platform : public ParticleContactGenerator
{
	public:
		Vector2 start;
		Vector2 end;
		/**
		* Holds a pointer to the particles we're checking
		 * for collisions with.
		 */
		Particle *particles;

		float restitution; /**< Coefficient of restitution for collisions. */

		virtual unsigned addContact(
			ParticleContact *contact,
			unsigned limit) const;
};

unsigned Platform::addContact(ParticleContact *contact,
							  unsigned limit) const
{
	unsigned used = 0;

	// Iterate through each particle to check for collisions
	for (unsigned i = 0; i < BLOB_COUNT; i++)
	{
		// Check if the maximum number of contacts has been reached
		if (used >= limit)
			return used;

		Vector2 toParticle = particles[i].getPosition() - start; // Calculate the vector from the platform start to the particle
		Vector2 lineDirection = end - start; // Calculate the direction of the platform line

		float projected = toParticle * lineDirection; 
		float platformSqLength = lineDirection.squareMagnitude();
		float squareRadius = particles[i].getRadius() * particles[i].getRadius();

		// Check if the particle is on the side of the platform start
		if (projected <= 0)
		{

			// The blob is nearest to the start point
			if (toParticle.squareMagnitude() < squareRadius)
			{
				// collision
				contact->contactNormal = toParticle.unit();
				contact->restitution = restitution;
				contact->particle[0] = particles + i;
				contact->particle[1] = nullptr; // No contact with another particle
				contact->penetration = particles[i].getRadius() - toParticle.magnitude();
				used++;
				contact++;
			}
		}
		else if (projected >= platformSqLength) // Check if the particle is on the side of the platform end
		{
			// The blob is nearest to the end point
			toParticle = particles[i].getPosition() - end;
			if (toParticle.squareMagnitude() < squareRadius)
			{
				// collision
				contact->contactNormal = toParticle.unit();
				contact->restitution = restitution;
				contact->particle[0] = particles + i;
				contact->particle[1] = nullptr; // No contact with another particle
				contact->penetration = particles[i].getRadius() - toParticle.magnitude();
				used++;
				contact++;
			}
		}
		else // Particle is between the start and end points of the platform
		{
			// Calculate the distance from the particle to the platform
			float distanceToPlatform = toParticle.squareMagnitude() - projected * projected / platformSqLength;
			if (distanceToPlatform < squareRadius)
			{
				// collision
				// Calculate the closest point on the platform line to the particle
				Vector2 closestPoint = start + lineDirection * (projected / platformSqLength);
				contact->contactNormal = (particles[i].getPosition() - closestPoint).unit();
				contact->restitution = restitution;
				contact->particle[0] = particles + i;
				contact->particle[1] = nullptr; // No contact with another particle
				contact->penetration = particles[i].getRadius() - sqrt(distanceToPlatform);
				used++;
				contact++;
			}
		}
	}
	return used;
}

class BlobDemo : public Application
{
	Particle *blobs;
	Platform *platforms;
	ParticleCollision *blobCollision;
	ParticleWorld world;

public:
	/** Creates a new demo object. */
	BlobDemo();
	virtual ~BlobDemo();
	/** Returns the window title for the demo. */
	virtual const char *getTitle();
	/** Display the particles. */
	virtual void display();
	/** Update the particle positions. */
	virtual void update();
};

void convexPolygons(Platform* platforms) {
	// Define platform data
	struct PlatformData {
		Vector2 start;
		Vector2 end;
		float restitution;
	};

	// Array of platform data
	PlatformData platformData[] = {
		{Vector2(0.0, -20.0), Vector2(40.0, -45.0), 1.0f},
		{Vector2(0.0, -20.0), Vector2(-40.0, -45.0), 1.0f},
		{Vector2(-40.0, -45.0), Vector2(-25.0, -80.0), 1.0f},
		{Vector2(40.0, -45.0), Vector2(25.0, -80.0), 1.0f},
		{Vector2(-25.0, -80.0), Vector2(25.0, -80.0), 1.0f}
	};

	// Initialize platforms using the data
	for (int i = 6; i <= 10; ++i) {
		platforms[i].start = platformData[i - 6].start;
		platforms[i].end = platformData[i - 6].end;
		platforms[i].restitution = platformData[i - 6].restitution;
	}
}

void setBorder(Platform* platforms, float nRange, float margin) {
	// Define platform data for border platforms
	struct BorderPlatformData {
		Vector2 start;
		Vector2 end;
		float restitution;
	};

	// Array of border platform data
	BorderPlatformData borderPlatformData[] = {
		{Vector2(-nRange * margin, -nRange * margin), Vector2(nRange * margin, -nRange * margin), 1.0f},
		{Vector2(-nRange * margin, nRange * margin), Vector2(nRange * margin, nRange * margin), 1.0f},
		{Vector2(-nRange * margin, -nRange * margin), Vector2(-nRange * margin, nRange * margin), 1.0f},
		{Vector2(nRange * margin, -nRange * margin), Vector2(nRange * margin, nRange * margin), 1.0f}
	};

	// Initialize border platforms using the data
	for (int i = 0; i < 4; ++i) {
		platforms[i].start = borderPlatformData[i].start;
		platforms[i].end = borderPlatformData[i].end;
		platforms[i].restitution = borderPlatformData[i].restitution;
	}

}

// Method definitions
BlobDemo::BlobDemo() : world(PLATFORM_COUNT + BLOB_COUNT, BLOB_COUNT)
{
	// Set the window dimensions and range
	width = 600;
	height = 600;
	nRange = 100.0;
	float margin = 0.95;

	// Create an array of particles representing blobs
	blobs = new Particle[BLOB_COUNT];

	// Create an array of platforms
	platforms = new Platform[PLATFORM_COUNT];

	// Create an instance of ParticleCollision for blob collisions
	blobCollision = new ParticleCollision;

	//Set the window border
	setBorder(platforms, nRange, margin);

	// Set up the middle top left line platform
	platforms[4].start = Vector2(-nRange * margin, 40.0);
	platforms[4].end = Vector2(-30, 10.0);
	platforms[4].restitution = 1.0f;

	// Set up the middle top right line platform
	platforms[5].start = Vector2(nRange * margin, 40.0);
	platforms[5].end = Vector2(30, 10.0);
	platforms[5].restitution = 1.0f;

	// Draw convex polygon by initializing additional platforms
	convexPolygons(platforms);

	// Seed the random number generator
	srand(time(NULL));

	// Create blob particles with random positions
	for (unsigned i = 0; i < BLOB_COUNT; i++)
	{

		float x = (float)(rand() % 180) - 90.0f; // any random value in posivite or negative
		float y = (float)(rand() % 90); //only positive ranodm value

		// Set blob properties
		blobs[i].setPosition(x, y);
		blobs[i].setRadius(5);
		blobs[i].setVelocity(0, 0);
		blobs[i].setDamping(1);
		blobs[i].setAcceleration(Vector2::GRAVITY * 20.0f);
		blobs[i].setMass(30.0f);
		blobs[i].clearAccumulator();

		// Add blob particles to the world
		world.getParticles().push_back(blobs + i);
	}

	// Set up particle array for each platform and add them to contact generators
	blobCollision->blob_count = BLOB_COUNT;
	for (int i = 0; i < PLATFORM_COUNT; i++)
	{
		platforms[i].particles = blobs;
		world.getContactGenerators().push_back(platforms + i);
	}

	// add the blob particle array to the bobCollision bruteParticles vector
	blobCollision->bruteParticles = blobs;

	// Set up blobCollision for blob-to-blob collisions and add it to contact generators
	world.getContactGenerators().push_back(blobCollision);
}

BlobDemo::~BlobDemo()
{
	delete blobs;
}

// a function to set up the brute force algorithm

void BlobDemo::display()
{
	Application::display();
	glBegin(GL_LINES);
	glColor3f(0, 1, 1);
	for (int i = 0; i < PLATFORM_COUNT; i++)
	{
		const Vector2 &p0 = platforms[i].start;
		const Vector2 &p1 = platforms[i].end;
		glVertex2f(p0.x, p0.y);
		glVertex2f(p1.x, p1.y);
	}
	glEnd();
	for (int i = 0; i < BLOB_COUNT; i++)
	{
		glColor3f(255, 0, 0);
		const Vector2 &p = blobs[i].getPosition();
		glPushMatrix();
		glTranslatef(p.x, p.y, 0);
		glutSolidSphere(blobs[i].getRadius(), 12, 12);
		glPopMatrix();
	}

	glutSwapBuffers();
}

void BlobDemo::update()
{
	// Recenter the axes
	float duration = timeinterval / 1000;
	// Run the simulation
	world.runPhysics(duration);

	Application::update();
}

const char *BlobDemo::getTitle()
{
	return "Blob Demo";
}

/**
 * Called by the common demo framework to create an application
 * object (with new) and return a pointer.
 */
Application *getApplication()
{
	return new BlobDemo();
}
