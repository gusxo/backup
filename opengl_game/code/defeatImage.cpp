#include "defeatImage.h"
void defeatImage::init() {
	waitInit = 0;
	model_init({ "defeat" });
	model_scale[0].y = 0.5f;
}