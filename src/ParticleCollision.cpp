#include <particle.h>
#include <ParticleCollision.h>

// Constructor for ParticleCollision class
ParticleCollision::ParticleCollision()
{
}

// Function to add contacts between particles in a brute-force manner
unsigned ParticleCollision::addContact(ParticleContact* contact, unsigned limit) const
{
    unsigned contactsUsed = 0;

    // Iterate through all pairs of particles
    for (int i = 0; i < blob_count - 1 && contactsUsed < limit; ++i)
    {
        // Iterate over the remaining particles to check for collisions
        for (int j = i + 1; j < blob_count && contactsUsed < limit; ++j)
        {
            Particle& currentParticle = bruteParticles[i];
            Particle& otherParticle = bruteParticles[j];

            // Check for collision between particleA and particleB
            if (checkCollision(currentParticle, otherParticle))
            {
                // Set contact properties for collision response
                contact->contactNormal = (currentParticle.getPosition() - otherParticle.getPosition()).unit();
                contact->restitution = 1.0f;
                contact->particle[0] = bruteParticles + i;
                contact->particle[1] = bruteParticles + j;
                contact->penetration = currentParticle.getRadius() - (currentParticle.getPosition() - otherParticle.getPosition()).magnitude();

                // Increment the number of contacts used
                ++contactsUsed;
                // Move to the next contact slot
                ++contact;
            }
        }
    }

    return contactsUsed;
}

// Function to check if two particles are colliding
bool ParticleCollision::checkCollision(Particle currentParticle, Particle otherParticle) const
{
    // Calculate the distance between the centers of two particles
    float distance = (currentParticle.getPosition() - otherParticle.getPosition()).magnitude();

    // Check if the distance is less than the sum of their radii, indicating a collision
    if (distance < currentParticle.getRadius() + otherParticle.getRadius())
        return true;
    else
        return false;
}

// Destructor for ParticleCollision class
ParticleCollision::~ParticleCollision()
{
    delete bruteParticles;
}
