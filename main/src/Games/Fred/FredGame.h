#ifndef BIT_FIRMWARE_FREDGAME_H
#define BIT_FIRMWARE_FREDGAME_H

#include "GameEngine/Game.h"
#include "Games/Fred/Player.h"

namespace Fred {

class FredGame : public Game {
	friend Player;
public:
	FredGame(Sprite& canvas);

protected:
	void onStart() override;
	void onStop() override;
	void onLoad() override;
	void onLoop(float deltaTime) override;
	void handleInput(const Input::Data& data) override;
	uint32_t getXP() const override;
	uint32_t getScore() const override;

private:
	bool started = false;

	//----------Background grid----------
	static constexpr PixelDim GridSize = { 12, 12 }; //grid is 12x12 tiles
	static constexpr PixelDim TileSize = { 10, 10 }; //tiles are 10x10 px
	static constexpr PixelDim OutlineSize = { 4, 4 }; //4px-wide border around grid

	GameObjPtr border;

	enum class TileType : uint8_t {
		Wall, Floor, LeftWarp, RightWarp, Door
	};
	static constexpr const TileType TileMap[GridSize.y][GridSize.x] = {
			{ TileType::Floor,    TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall,  TileType::Wall,  TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor },
			{ TileType::Floor,    TileType::Wall,  TileType::Wall,  TileType::Wall,  TileType::Floor, TileType::Wall,  TileType::Wall,  TileType::Floor, TileType::Wall,  TileType::Wall,  TileType::Wall,  TileType::Floor },
			{ TileType::Floor,    TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor },
			{ TileType::Floor,    TileType::Wall,  TileType::Wall,  TileType::Wall,  TileType::Floor, TileType::Wall,  TileType::Wall,  TileType::Floor, TileType::Wall,  TileType::Wall,  TileType::Wall,  TileType::Floor },
			{ TileType::Floor,    TileType::Wall,  TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall,  TileType::Floor },
			{ TileType::Floor,    TileType::Wall,  TileType::Floor, TileType::Wall,  TileType::Wall,  TileType::Door,  TileType::Door,  TileType::Wall,  TileType::Wall,  TileType::Floor, TileType::Wall,  TileType::Floor },
			{ TileType::LeftWarp, TileType::Floor, TileType::Floor, TileType::Wall,  TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall,  TileType::Floor, TileType::Floor, TileType::RightWarp },
			{ TileType::Wall,     TileType::Wall,  TileType::Floor, TileType::Wall,  TileType::Wall,  TileType::Wall,  TileType::Wall,  TileType::Wall,  TileType::Wall,  TileType::Floor, TileType::Wall,  TileType::Wall },
			{ TileType::Wall,     TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Wall },
			{ TileType::Floor,    TileType::Floor, TileType::Wall,  TileType::Wall,  TileType::Floor, TileType::Wall,  TileType::Wall,  TileType::Floor, TileType::Wall,  TileType::Wall,  TileType::Floor, TileType::Floor },
			{ TileType::Floor,    TileType::Wall,  TileType::Wall,  TileType::Wall,  TileType::Floor, TileType::Wall,  TileType::Wall,  TileType::Floor, TileType::Wall,  TileType::Wall,  TileType::Wall,  TileType::Floor },
			{ TileType::Floor,    TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor, TileType::Floor }
	};
	GameObjPtr tiles[GridSize.x * GridSize.y];

	static constexpr uint8_t WallTypes = 6;
	static constexpr uint8_t FloorTypes = 2;
	static constexpr const char* WallPaths[WallTypes] = { "/wall1.raw", "/wall2.raw", "/wall3.raw", "/wall4.raw", "/wall5.raw", "/wall6.raw" };
	static constexpr const char* FloorPaths[FloorTypes] = { "/floor1.raw", "/floor2.raw" };

	GameObjPtr floorObjectsOriginal[FloorTypes];
	GameObjPtr wallObjectsOriginal[WallTypes];

	void setupOriginalTiles();
	void buildTilemap();

	//-------------Player--------------
	Player player;
};

}

#endif //BIT_FIRMWARE_FREDGAME_H
