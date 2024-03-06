#ifndef BIT_FIRMWARE_TEST3D_H
#define BIT_FIRMWARE_TEST3D_H

#include "Devices/Display.h"
#include <vec3.hpp>
#include <gtc/quaternion.hpp>
#include "Util/Events.h"

#define Count(x) (sizeof(x)/sizeof(x[0]))

class Test3D {
public:
	Test3D(Display& display);

	void loop(float dt);

private:
	Display& display;
	Sprite& canvas;

	EventQueue evts;

	static constexpr glm::vec3 Up = { 0, 0, 1 };
	const glm::mat4 Proj;

	/**
	 * Spritesheet is 96x96 and contains 3x3 sprites, each 32x32 pixels
	 */
	static constexpr int Field[4][4] = {
			{ 0, 1, 1, 2 },
			{ 3, 4, 4, 5 },
			{ 3, 4, 5, 5 },
			{ 6, 6, 7, 8 }
	};

	struct Asset {
		const char* path;
		glm::vec<2, int> dim;
	};
	static constexpr Asset Assets[] = {
			{ "/spiffs/spritesheet.raw", { 96, 96 } },
			{ "/spiffs/bush.raw", { 28, 20 } },
			{ "/spiffs/light.raw", { 18, 88 } }
	};
	std::vector<uint8_t> assetData[Count(Assets)];

	struct BB {
		int index;
		glm::vec2 pos;
		float scale;
	};
	static constexpr BB Boards[] = {
			{ 1, { 0, 0 }, 0.1 },
			{ 2, { 0.5f, 1.0f }, 0.1 },
			{ 2, { 1.0f, -0.3f }, 0.1 }
	};

	glm::vec3 camPos = { 0, -0.5f, 2.2f };
	float rotUD = -80;
	float rotZ = 0;

	float spd = 0; // Forward/backward
	float spdUD = 0; // Up/down
	float spdZ = 0; // Left/right

	uint16_t getColor(float planeX, float planeY);

};


#endif //BIT_FIRMWARE_TEST3D_H
