#include "particles.h"

#include "common.h"
#include "utils.h"
#include "raymath.h"
#include "rmem.h"
#include "resourcemanager.h"

#include "DG_dynarr.h"

#define PARTICLES_POOL_MAX 500

typedef struct 
{
	float x;
	float y;
	float dx;
	float dy;
	EParticleType type;
	float maxAge;
	float age;
	LCDColor color;
	LCDColor colors[10];
	int colorsCount;
	int rotation;
	float rotationTimer;
	float s;
	float os;
}Particle;

DA_TYPEDEF(Particle*, ParticlesArrayType);

ObjPool particlesPool;
ParticlesArrayType particles;

void PARTICLES_init(void)
{
	particlesPool = CreateObjPool(sizeof(Particle), PARTICLES_POOL_MAX);
	da_init(particles);
}

void PARTICLES_update(void)
{
	for (size_t i = da_count(particles); i > 0; i--)
	{
		Particle* particle = da_get(particles, i - 1);
		particle->age += 1;
		if ((particle->age > particle->maxAge) ||
			(particle->x < -20.0f || particle->x > pd->display->getWidth() + 20.0f) ||
			(particle->y < -20.0f || particle->y > pd->display->getHeight() + 20.0f))
		{
			PARTICLES_removeParticle(i - 1);
		}
		else 
		{
			//Change color
			if (particle->colorsCount == 1) 
			{
				particle->color = particle->colors[0];
			}
			else 
			{
				float colorIndex = particle->age / particle->maxAge;
				colorIndex = 1 + floorf(colorIndex * particle->colorsCount);
				particle->color = particle->colors[(int)colorIndex];
			}


			// Apply gravity
			if (particle->type == GRAVITY_PIX || particle->type == ROTATING_SPRITE)
			{
				particle->dy += 0.05f;
			}

			// Apply low gravity
			if (particle->type == GRAVITY_SMOKE)
			{
				if (abs(particle->dy) < 1.f) 
				{
					particle->dy += 0.01f;
				}
			}

			// Rotate
			if (particle->type == ROTATING_SPRITE)
			{
				particle->rotationTimer += 1.f;
				if (particle->rotationTimer > 5.f)
				{
					particle->rotation += 1;
					if (particle->rotation > 4)
					{
						particle->rotation = 0;
					}
				}
			}

			// Shrink
			if (particle->type == SMOKE_BALL || particle->type == GRAVITY_SMOKE || particle->type == SPEED_LINE)
			{
				float ci = 1.f - (particle->age / particle->maxAge);
				particle->s = ci * particle->os;
			}

			// Friction
			if (particle->type == SMOKE_BALL || particle->type == SPEED_LINE)
			{
				particle->dx = particle->dx / 1.2f;
				particle->dy = particle->dy / 1.2f;
			}

			// Move Particle
			particle->x += particle->dx;
			particle->y += particle->dy;
		}
	}
}

void PARTICLES_draw(void)
{
	for (int i = 0; i < da_count(particles); i++) 
	{
		Particle* particle = da_get(particles, i);

		if (particle->type == STATIC_PIX || particle->type == GRAVITY_PIX)
		{
			pd->graphics->fillRect(particle->x, particle->y, 1, 1, particle->color);
		}
		else if (particle->type == SMOKE_BALL || particle->type == GRAVITY_SMOKE)
		{
			pd->graphics->fillEllipse(particle->x - particle->s * 0.5f, particle->y - particle->s * 0.5f, particle->s, particle->s, 0.f, 0.f, particle->color);
		}
		else if (particle->type == ROTATING_SPRITE)
		{
			LCDBitmapFlip flipped = kBitmapUnflipped;
			if (particle->type == ROTATING_SPRITE)
			{
				if (FloatEquals(particle->rotation, 2.f))
				{
					flipped = kBitmapFlippedY;
					
				}
				else if (FloatEquals(particle->rotation, 3.f))
				{
					flipped = kBitmapFlippedXY;
					
				}
				else if (FloatEquals(particle->rotation, 4.f))
				{
					flipped = kBitmapFlippedX;
					
				}
			}
			pd->graphics->drawBitmap(RESOURCEMANAGER_getImage("particle"), particle->x, particle->y, flipped);
		}
		else if (particle->type == SPEED_LINE)
		{
			if (particle->dx < 0.f) 
			{
				pd->graphics->drawLine(particle->x, particle->y, particle->x + particle->s, particle->y, 2.f, particle->color);
			}
			else 
			{
				pd->graphics->drawLine(particle->x - particle->s, particle->y, particle->x, particle->y, 2.f, particle->color);
			}
		}
	}
}

void PARTICLES_destroy(void)
{
	PARTICLES_removeAllParticles();
	da_free(particles);
	DestroyObjPool(&particlesPool);
}

void PARTICLES_addParticle(float x, float y, float dx, float dy, EParticleType type, float maxage, LCDColor colors[], int colorsCount, float s)
{
	Particle* newPart = ObjPoolAlloc(&particlesPool);
	if (newPart)
	{
		newPart->x = x;
		newPart->y = y;
		newPart->dx = dx;
		newPart->dy = dy;
		newPart->type = type;
		newPart->age = 0.f;
		newPart->maxAge = maxage;
		newPart->color = colors[0];
		newPart->colorsCount = colorsCount;
		newPart->s = s;
		newPart->rotation = 0;
		newPart->rotationTimer = 0;
		newPart->os = s;

		for (int i = 0; i < colorsCount; i++) 
		{
			newPart->colors[i] = colors[i];
		}

		da_push(particles, newPart);
	}
}

void PARTICLES_removeParticle(int index)
{
	Particle* particle = da_get(particles, index);
	ObjPoolCleanUp(&particlesPool, (void**)&particle);
	da_delete(particles, index);
}

void PARTICLES_removeAllParticles(void)
{
	for (int i = 0; i < da_count(particles); i++)
	{
		PARTICLES_removeParticle(i);
	}
	da_clear(particles);
}