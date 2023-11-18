#pragma once
#include <pcontacts.h>
#include <list>
#include <iostream>
#include <vector>
#include <gl/glut.h>

using namespace std;

// ParticleCollision class inherits from ParticleContactGenerator
class ParticleCollision : public ParticleContactGenerator
{
	public:
		Particle* bruteParticles;
		int blob_count; // Number of blob particles in the scene.

		// Constructor
		ParticleCollision();

		// Function to add contacts between particles (implementation of ParticleContactGenerator)
		virtual unsigned addContact(
			ParticleContact* contact,
			unsigned limit
		) const;

		// Function to check collision between two particles
		bool checkCollision(Particle, Particle) const;

		// Destructor
		~ParticleCollision();
};
