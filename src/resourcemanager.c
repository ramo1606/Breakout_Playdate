#include "resourcemanager.h"
#include "common.h"
#include "array.h"

#include <stdlib.h>
#include <string.h>

typedef struct
{
	void* res;		// Resources data like LCDbitmaps, AudioSample, Sound Effect
	char* name;		// Name of the resource to search by
} Resources;

static Resources* images = NULL;			// Images
static Resources* music = NULL;				// Music
static Resources* sounds = NULL;			// Sounds

/*  */
static void RESOURCEMANAGER_loadImage(const char* filename, void* userdata)
{
	// Setup filename path to load
	const char* outErr = NULL;
	char* file = filename;
	char* path = NULL;
	file[strlen(filename) - 4] = '\0';
	getPlaydateAPI()->system->formatString(&path, "images/%s", file);
	
	// Load bitmap
	LCDBitmap* img = getPlaydateAPI()->graphics->loadBitmap(path, &outErr);
	if (outErr != NULL) {
		getPlaydateAPI()->system->logToConsole("Error loading image '%s': %s", filename, outErr);
	}

	pd_free(path);				// free path string

	// Create a new image resource with image loaded and name, added to the dynamic arrray
	char* name = strdup(file);
	Resources imageResource = { (void*)img, name };
	array_push(images, imageResource);
}

/*  */
void RESOURCEMANAGER_load(void)
{
	// Load all images.
	int filesNum = getPlaydateAPI()->file->listfiles("images", &RESOURCEMANAGER_loadImage, NULL, 0);
	//RESOURCEMANAGER_loadSounds(resources);
	//RESOURCEMANAGER_loadMusic(resources);
}

/*  */
void RESOURCEMANAGER_unLoad(void)
{
	for(int i = 0; i < array_length(images); i++)
	{
		pd_free(images[i].name);
		pd_free(images[i].res);
	}
	array_free(images);
}

/*  */
LCDBitmap* RESOURCEMANAGER_getImage(const char* image)
{
	LCDBitmap* return_image = NULL;
	for(int i = 0; i < array_length(images); i++)
	{
		if (strcmp(images[i].name, image) == 0)
		{
			return_image = images[i].res;
			break;
		}
	}

	return return_image;
}
