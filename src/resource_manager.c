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
	LCDBitmap* normal_block;
	LCDBitmap* hard_block;
	LCDBitmap* explosive_block;
	LCDBitmap* unbreakable_block;
	LCDBitmap* powerup_block;
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
	//else if (strcmp(image, "paddle_large") == 0)
	//{
	//	return_image = resources->big_paddle;
	//}

	return return_image;
}