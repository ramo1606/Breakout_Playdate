#include "resourcemanager.h"
#include "common.h"

#define DG_DYNARR_IMPLEMENTATION
#include "DG_dynarr.h"

#include <stdlib.h>
#include <string.h>

typedef struct
{
	void* res;		// Resources data like LCDbitmaps, AudioSample, Sound Effect
	char* name;		// Name of the resource to search by
} Resources;

DA_TYPEDEF(Resources, ResourcesArrayType);

static ResourcesArrayType images;			// Images
static ResourcesArrayType music;				// Music
static ResourcesArrayType sounds;			// Sounds

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
	Resources imageResource = {.name = name, .res = (void*)img };
	da_push(images, imageResource);
}

//static void RESOURCEMANAGER_loadSound(const char* filename, void* userdata)
//{
//	// Setup filename path to load
//	const char* outErr = NULL;
//	char* file = filename;
//	char* path = NULL;
//	file[strlen(filename) - 4] = '\0';
//	getPlaydateAPI()->system->formatString(&path, "sounds/%s", file);
//
//	// Load sounds
//	LCDBitmap* img = getPlaydateAPI()->graphics->loadBitmap(path, &outErr);
//	if (outErr != NULL) {
//		getPlaydateAPI()->system->logToConsole("Error loading image '%s': %s", filename, outErr);
//	}
//
//	pd_free(path);				// free path string
//
//	// Create a new image resource with image loaded and name, added to the dynamic arrray
//	char* name = strdup(file);
//	Resources imageResource = { .name = name, .res = (void*)img };
//	da_push(images, imageResource);
//}

/*  */
void RESOURCEMANAGER_load(void)
{
	// Load all images.
	da_init(images);
	int filesNum = getPlaydateAPI()->file->listfiles("images", &RESOURCEMANAGER_loadImage, NULL, 0);
	//int filesNum = getPlaydateAPI()->file->listfiles("sounds", &RESOURCEMANAGER_loadSound, NULL, 0);
	//RESOURCEMANAGER_loadMusic(resources);
}

/*  */
void RESOURCEMANAGER_unLoad(void)
{
	for(int i = 0; i < da_count(images); i++)
	{
		pd_free(da_get(images, i).name);
		pd_free(da_get(images, i).res);
	}
	da_clear(images);
	da_free(images);
}

/*  */
LCDBitmap* RESOURCEMANAGER_getImage(const char* image)
{
	LCDBitmap* return_image = NULL;
	for(int i = 0; i < da_count(images); i++)
	{
		if (strcmp(da_get(images, i).name, image) == 0)
		{
			return_image = da_get(images, i).res;
			break;
		}
	}

	return return_image;
}
