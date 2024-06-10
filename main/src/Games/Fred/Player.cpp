#include "Player.h"
#include "FredGame.h"
#include "GameEngine/Rendering/AnimRC.h"

using namespace Fred;

void Player::setObj(GameObjPtr player){
	this->obj = player;
}

void Player::move(float deltaT){
	if(!moving) return;

	const glm::vec2 gridPosition = Player::gridPosition(obj->getPos(), FredGame::TileSize);
	if(obj->getPos() == snapToGrid(gridPosition, dir)){
		obj->setPos(snappedMovement(deltaT));
	}else{
		obj->setPos(unsnappedMovement(gridPosition, deltaT));
	}

}

void Player::inputDirection(Fred::Direction input, bool start){

	desiredDir = input;

	if(!moving && start){
		moving = true;
		std::static_pointer_cast<AnimRC>(obj->getRenderComponent())->start();
	}
}

glm::vec2 Player::snapToGrid(glm::vec2 gridpos, Direction dir){

	auto newPosition = gridpos;
	std::function<int16_t(float)> func = roundingFunction(dir);


	if(dir == Direction::Up || dir == Direction::Down){
		newPosition.y = func(newPosition.y);
	}else{
		newPosition.x = func(newPosition.x);
	}

	return { (newPosition.x) * FredGame::TileSize.x + FredGame::OutlineSize.x,
			 (newPosition.y) * FredGame::TileSize.y + FredGame::OutlineSize.y };
}

glm::vec2 Player::snappedMovement(float deltaT){
	auto desired = determineNewPositions(obj->getPos(), Speed, desiredDir, deltaT);
	auto alternate = determineNewPositions(obj->getPos(), Speed, dir, deltaT);

	if(wallCollisionCheck(desired.gridPosition, desiredDir)){
		if(wallCollisionCheck(alternate.gridPosition, dir)){
			moving = false;
			std::static_pointer_cast<AnimRC>(obj->getRenderComponent())->stop();
			return obj->getPos();
		}
		return alternate.position;
	}

	if(desiredDir == Direction::Left){
		obj->getRenderComponent()->setFlipY(true);
	}else if(desiredDir == Direction::Right){
		obj->getRenderComponent()->setFlipY(false);
	}

	dir = desiredDir;
	return desired.position;
}

Player::PositionSet Player::determineNewPositions(glm::vec2 pos, float speed, Direction dir, float deltaT){
	auto newPos = pos;
	switch(dir){
		case Direction::Up:
			newPos.y -= Speed * deltaT;
			break;
		case Direction::Down:
			newPos.y += Speed * deltaT;
			break;
		case Direction::Left:
			newPos.x -= Speed * deltaT;
			break;
		case Direction::Right:
			newPos.x += Speed * deltaT;
			break;
	}

	auto newGridPos = gridPosition(newPos, FredGame::TileSize);

	return { newPos, newGridPos };
}

bool Player::wallCollisionCheck(glm::vec2 gridPos, Direction dir){
	const auto rf = roundingFunction(dir);

	const auto x = rf(gridPos.x);
	const auto y = rf(gridPos.y);

	if(x < 0 || x >= FredGame::GridSize.x || y < 0 || y >= FredGame::GridSize.y){
		return true;
	}

	return FredGame::TileMap[y][x] == FredGame::TileType::Wall;
}

glm::vec2 Player::unsnappedMovement(glm::vec2 gridpos, float deltaT){
	auto desired = determineNewPositions(obj->getPos(), Speed, desiredDir, deltaT);
	auto alternate = determineNewPositions(obj->getPos(), Speed, dir, deltaT);

	if(turning180(dir, desiredDir)){
		if(desiredDir == Direction::Left){
			obj->getRenderComponent()->setFlipY(true);
		}else if(desiredDir == Direction::Right){
			obj->getRenderComponent()->setFlipY(false);
		}
		dir = desiredDir;
		return desired.position;
	}

	if(changingGridPosition(gridpos, alternate.gridPosition)){
		return snapToGrid(gridpos, dir);
	}

	return alternate.position;
}

glm::vec2 Player::gridPosition(glm::vec2 position, PixelDim tileSize){
	return { ((position.x - FredGame::OutlineSize.x) / tileSize.x), ((position.y - FredGame::OutlineSize.y) / tileSize.y) };
}
