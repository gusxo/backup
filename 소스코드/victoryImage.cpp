#include "victoryImage.h"
void victoryImage::init() {
	waitInit = 0;
	model_init({ "victory" });
	model_scale[0].y = 0.5f;
}