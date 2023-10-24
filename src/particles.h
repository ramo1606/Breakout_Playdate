#ifndef PARTICLES_H
#define PARTICLES_H

#define RMEM_IMPLEMENTATION

#include "pd_api.h"

//particles
// type 0 - static pixel
// type 1 - gravity pixel
// type 2 - ball of smoke
// type 3 - rotating sprite
// type 4 - blue rotating sprite
// type 5 - gravity smoke
// type 6 - speedline
typedef enum
{
	STATIC_PIX,
	GRAVITY_PIX,
	SMOKE_BALL,
	ROTATING_SPRITE,
	BLUE_ROTATING_SPRITE,
	GRAVITY_SMOKE,
	SPEED_LINE
}EParticleType;

typedef struct Particle Particle;

void PARTICLES_init(void);
void PARTICLES_update(void);
void PARTICLES_draw(void);
void PARTICLES_destroy(void);

void PARTICLES_addParticle(float x, float y, float dx, float dy, EParticleType type, float maxage, LCDColor color, float s);
void PARTICLES_removeParticle(int index);
void PARTICLES_removeAllParticles(void);

void PARTICLES_spawnTrail(float x, float y, float radius);
void PARTICLES_spawnPuft(float x, float y);

#endif // !PARTICLES_H