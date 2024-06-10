#include "FredGame.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Rendering/MultiRC.h"
#include "GameEngine/Rendering/AnimRC.h"
#include "GameEngine/Collision/RectCC.h"

using namespace Fred;

FredGame::FredGame(Sprite& canvas) : Game(canvas, Games::Fred, "/Games/Fred", {
		{ "/border.raw", {}, true },
		{ "/floor1.raw", {}, true },
		{ "/floor2.raw", {}, true },
		{ "/wall1.raw",  {}, true },
		{ "/wall2.raw",  {}, true },
		{ "/wall3.raw",  {}, true },
		{ "/wall4.raw",  {}, true },
		{ "/wall5.raw",  {}, true },
		{ "/wall6.raw",  {}, true },
		{ "/fred.gif",   {}, false }}){

}

void FredGame::onStart(){
	Game::onStart();


}

void FredGame::onStop(){
	Game::onStop();
}

void FredGame::onLoad(){
	border = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/border.raw"), PixelDim({ 128, 128 })),
			nullptr
	);
	addObject(border);
	border->getRenderComponent()->setLayer(0);

	setupOriginalTiles();
	buildTilemap();

	auto playerObj = std::make_shared<GameObject>(
			std::make_unique<AnimRC>(getFile("/fred.gif")),
			std::make_unique<RectCC>(glm::vec2{ 10, 10 })
	);
	auto anim = std::static_pointer_cast<AnimRC>(playerObj->getRenderComponent());
	anim->setLoopMode(GIF::Infinite);
	anim->start();
	playerObj->setPos(5 * TileSize.x + OutlineSize.x, 8 * TileSize.y + OutlineSize.y);

	addObject(playerObj);
	player.setObj(playerObj);
}

void FredGame::onLoop(float deltaTime){
	player.move(deltaTime);
}

void FredGame::handleInput(const Input::Data& data){
	if(!started) started = true;

	if(data.action != Input::Data::Press) return;
	switch(data.btn){
		case Input::Up:
			player.inputDirection(Direction::Up, started);
			break;
		case Input::Down:
			player.inputDirection(Direction::Down, started);
			break;
		case Input::Left:
			player.inputDirection(Direction::Left, started);
			break;
		case Input::Right:
			player.inputDirection(Direction::Right, started);
			break;
		default:
			break;
	}
}

uint32_t FredGame::getXP() const{
	return 0;
}

uint32_t FredGame::getScore() const{
	return Game::getScore();
}

void FredGame::setupOriginalTiles(){
	for(uint8_t i = 0; i < FloorTypes; ++i){
		floorObjectsOriginal[i] = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(getFile(FloorPaths[i]), TileSize),
				nullptr
		);
		floorObjectsOriginal[i]->setPos(-50, -50);
		addObject(floorObjectsOriginal[i]);
	}

	for(uint8_t i = 0; i < WallTypes; ++i){
		wallObjectsOriginal[i] = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(getFile(WallPaths[i]), TileSize),
				nullptr
		);
		wallObjectsOriginal[i]->setPos(-50, -50);
		addObject(wallObjectsOriginal[i]);
	}
}

void FredGame::buildTilemap(){
	for(uint8_t y = 0; y < GridSize.y; ++y){
		for(uint8_t x = 0; x < GridSize.x; ++x){
			const auto type = TileMap[y][x];

			GameObjPtr tile;

			switch(type){
				case TileType::Wall:
					tile = std::make_shared<GameObject>(std::make_unique<MultiRC>(wallObjectsOriginal[rand() % WallTypes]->getRenderComponent()), nullptr);
					break;
				case TileType::Floor:
					tile = std::make_shared<GameObject>(std::make_unique<MultiRC>(floorObjectsOriginal[rand() % FloorTypes]->getRenderComponent()), nullptr);
					break;
				case TileType::LeftWarp:
					tile = std::make_shared<GameObject>(std::make_unique<MultiRC>(floorObjectsOriginal[rand() % FloorTypes]->getRenderComponent()), nullptr);
					break;
				case TileType::RightWarp:
					tile = std::make_shared<GameObject>(std::make_unique<MultiRC>(floorObjectsOriginal[rand() % FloorTypes]->getRenderComponent()), nullptr);
					break;
				default:
					break;
			}

			tile->setPos(OutlineSize.x + x * TileSize.x, OutlineSize.y + y * TileSize.y);
			addObject(tile);
			tiles[y * GridSize.x + x] = tile;
		}
	}
}
