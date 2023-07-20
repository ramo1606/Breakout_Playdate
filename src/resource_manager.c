#include "resource_manager.h"
#include "common.h"
#include "memory.h" 

#include <stdlib.h>

struct Resources
{
	LCDBitmap* paddle;
	LCDBitmap* big_paddle;
	LCDBitmap* sticky_paddle;
	LCDBitmap* ball;
	LCDBitmap* special_ball;
	LCDBitmap* normal_brick;
	LCDBitmap* hard_brick;
	LCDBitmap* explosive_brick;
	LCDBitmap* unbreakable_brick;
	LCDBitmap* powerup_brick;
	LCDBitmap* pill;
};

static LCDBitmap* load_Image_at_path(const char* path)
{
	const char* outErr = NULL;
	LCDBitmap* img = get_playdate_API()->graphics->loadBitmap(path, &outErr);
	if (outErr != NULL) {
		get_playdate_API()->system->logToConsole("Error loading image at path '%s': %s", path, outErr);
	}
	return img;
}

Resources* load_resources()
{
	Resources* resources = pd_calloc((size_t)1, sizeof(Resources));
	resources->ball = load_Image_at_path("images/ball");
	resources->paddle = load_Image_at_path("images/paddle");
	resources->normal_brick = load_Image_at_path("images/brick");
	//resources->big_paddle = load_Image_at_path(playdate, "images/paddle_large");

	return resources;
}

LCDBitmap* get_image(Resources* resources, const char* image)
{
	LCDBitmap* return_image = NULL;
	if (strcmp(image, "paddle") == 0)
	{
		return_image = resources->paddle;
	}
	else if (strcmp(image, "ball") == 0)
	{
		return_image = resources->ball;
	}
	else if (strcmp(image, "brick") == 0)
	{
		return_image = resources->normal_brick;
	}

	return return_image;
}