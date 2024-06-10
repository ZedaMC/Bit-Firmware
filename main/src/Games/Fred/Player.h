#ifndef BIT_FIRMWARE_FREDPLAYER_H
#define BIT_FIRMWARE_FREDPLAYER_H

#include "GameEngine/GameObject.h"

namespace Fred {

enum class Direction : uint8_t {
	Up, Down, Left, Right
};

class Player {
public:
	void setObj(GameObjPtr player);

	void inputDirection(Direction input, bool start = false);

	void move(float deltaT);

private:
	Direction dir = Direction::Right;
	Direction desiredDir = Direction::Right;

	GameObjPtr obj;

	static constexpr float Speed = 30;

	bool animFlip = false;

	bool moving = false;


	static glm::vec2 gridPosition(glm::vec2 position, PixelDim tileSize);

	static std::function<int16_t(float)> roundingFunction(Direction dir){
		if(dir == Direction::Up || dir == Direction::Left){
			return [](float f){ return std::floor(f); };
		}else{
			return [](float f){ return std::ceil(f); };
		}
	}

	struct PositionSet {
		glm::vec2 position;
		glm::vec2 gridPosition;
	};

	static PositionSet determineNewPositions(glm::vec2 pos, float speed, Direction dir, float deltaT);

	glm::vec2 snapToGrid(glm::vec2 gridpos, Direction dir);
	glm::vec2 snappedMovement(float deltaT);
	glm::vec2 unsnappedMovement(glm::vec2 gridpos, float deltaT);

	static bool wallCollisionCheck(glm::vec2 gridPos, Direction dir);

	static constexpr bool turning180(Direction dir, Direction desiredDir){
		return (dir == Direction::Left && desiredDir == Direction::Right) || (dir == Direction::Right && desiredDir == Direction::Left) ||
			   (dir == Direction::Up && desiredDir == Direction::Down) || (dir == Direction::Down && desiredDir == Direction::Up);
	}

	static constexpr bool changingGridPosition(glm::vec2 oldPos, glm::vec2 newPos){
		return (std::floor(oldPos.x) != std::floor(newPos.x) || std::floor(oldPos.y) != std::floor(newPos.y)
		);
	}
};

}


#endif //BIT_FIRMWARE_FREDPLAYER_H
