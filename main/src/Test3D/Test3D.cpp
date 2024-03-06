#include "Test3D.h"
#include "Devices/Input.h"
#include "gtx/rotate_vector.hpp"

Test3D::Test3D(Display& display) : display(display), canvas(display.getCanvas()), evts(12), Proj(glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 20.0f)){
	for(int i = 0; i < Count(Assets); i++){
		const auto asset = Assets[i];
		auto& data = assetData[i];

		data.resize(96*96*2, 0);
		auto file = fopen(asset.path, "r");
		fread(data.data(), 1, asset.dim.x*asset.dim.y*2, file);
		fclose(file);
	}

	canvas.setTextColor(TFT_GREEN);
	canvas.setTextFont(0);
	canvas.setTextSize(1);

	Events::listen(Facility::Input, &evts);
}

void Test3D::loop(float dt){
	if(dt == 0) return;

	Event evt {};
	if(evts.get(evt, 0)){
		const auto data = (Input::Data*) evt.data;

		if(data->btn == Input::A){
			spd += (data->action == Input::Data::Press) ? 1 : -1;
		}else if(data->btn == Input::B){
			spd += (data->action == Input::Data::Press) ? -1 : 1;
		}else if(data->btn == Input::Left){
			spdZ += (data->action == Input::Data::Press) ? 1 : -1;
		}else if(data->btn == Input::Right){
			spdZ += (data->action == Input::Data::Press) ? -1 : 1;
		}else if(data->btn == Input::Up){
			spdUD += (data->action == Input::Data::Press) ? 1 : -1;
		}else if(data->btn == Input::Down){
			spdUD += (data->action == Input::Data::Press) ? -1 : 1;
		}
	}

	rotUD += spdUD * dt * 140.0f;
	rotZ += spdZ * dt * 140.0f;

	while(rotZ >= 360){
		rotZ -= 360.0f;
	}
	while(rotZ <= 360){
		rotZ += 360.0f;
	}
	if(rotUD < -89){
		rotUD = -89;
	}else if(rotUD > 89){
		rotUD = 89;
	}

	glm::vec3 forward(1, 0, 0);
	forward = glm::rotateY(forward, glm::radians(-rotUD)); // Rotation is limited to [-89, 89] deg (above) - disregard gimbal lock
	forward = glm::rotateZ(forward, glm::radians(rotZ));
	// After rotations, the vector should remain normalized (disregarding fp rounding errors)

	camPos += forward * spd * dt;

	const auto view = glm::lookAt(camPos, camPos + forward, Up);
	const glm::mat4 vpMat = Proj * view;
	const glm::mat4 vpInv = glm::inverse(vpMat);

	for(int x = 0; x < 128; x++){
		for(int y = 0; y < 128; y++){
			if(camPos.z < 0.01f){
				canvas.drawPixel(x, y, TFT_BLACK);
				continue;
			}

			// The magic number is 1/127 -- multiplication is faster than division. This optimisation alone yields ~10% faster rendering
			const float posX = ((float) x * 0.007874016f) * 2.0f - 1.0f;
			const float posY = ((float) y * 0.007874016f) * 2.0f - 1.0f;

			const glm::vec4 screenPos = glm::vec4(posX, -posY, 1.0f, 1.0f);
			const glm::vec4 worldPos = vpInv * screenPos;

			const glm::vec3 ray = glm::normalize(glm::vec3(worldPos));

			const float t = - camPos.z / ray.z;
			if(t < 0){
				canvas.drawPixel(x, y, TFT_BLACK);
				continue;
			}

			// x/y coords on the z=0 plane (world space)
			const float planeX = t * ray.x + camPos.x;
			const float planeY = t * ray.y + camPos.y;

			const auto c = getColor(planeX, planeY);
			canvas.drawPixel(x, y, c);
		}
	}

	// Billboards
	for(int i = 0; i < Count(Boards); i++){
		const auto board = Boards[i];
		const auto asset = Assets[board.index];

		const glm::vec4 originH = glm::vec4(board.pos, 0, 1.0f);
		const auto screenPos = vpMat * originH;
		const auto screenCoords = glm::vec3(screenPos / screenPos.w) * glm::vec3(64, 64, 1.0f) + glm::vec3(64, 64, 0.0f);

		canvas.pushImage(screenCoords.x, -screenCoords.y, asset.dim.x, asset.dim.y, assetData[board.index].data());
	}

	// Framerate display
	const float fps = 1.0f / dt;
	char fpstext[32];
	sprintf(fpstext, "%.1f", fps);

	canvas.setCursor(2, 2);
	canvas.write(fpstext);

	// Push to display
	canvas.pushSprite(0, 0);
}

uint16_t Test3D::getColor(float planeX, float planeY){
	/**
	 * The ground plane is from x/y [-2, -2] to [2, 2] and is divided into a 4x4 grid. It
	 * uses sprites from the spritemap according to the Field structure. This function gets
	 * called for every screen pixel that intersects the z=0 plane.
	 * Pixels that are outside [-2, 2] on either x or y are black. Other pixels sample the spritemap.
	 */

	if(planeX > 2 || planeX < -2 || planeY > 2 || planeY < -2) return TFT_BLACK;

	planeX = planeX + 2.0f;
	planeY = planeY + 2.0f;
	if(planeX >= 4) planeX = 3.99f;
	if(planeY >= 4) planeY = 3.99f;

	const int spriteIndex = Field[(int) std::floor(planeY)][(int) std::floor(planeX)];

	// Location of sprite inside sprite-sheet [3x3]
	const int spriteY = spriteIndex / 3;
	const int spriteX = spriteIndex - spriteY * 3;

	// Start pixel coords of needed sprite inside sprite-sheet [96x96]
	const int spriteStartX = spriteX * 32;
	const int spriteStartY = spriteY * 32;

	// Pixel pos inside needed sprite [0x1]
	const float pixelX = planeX - std::floor(planeX);
	const float pixelY = planeY - std::floor(planeY);
	// Pixel pos inside needed sprite [32x32]
	const int spritePixelX = std::floor(pixelX * 32.0f);
	const int spritePixelY = std::floor(pixelY * 32.0f);

	const int index = (spriteStartX + spritePixelX) + (spriteStartY + spritePixelY) * 96;

	return ((uint16_t*) assetData[0].data())[index];
}
