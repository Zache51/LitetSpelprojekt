#include "Map.h"

Map::Map()
{
	arrOfTiles = nullptr;
	// Get seed for noise
	setRandom(1);
	// Initiate point-map
	this->chunkSize = pow(2, 5) + 1;// +1 gives the map a mid-point
	seed = 60.0f;
	ds = new float*[chunkSize];// 33x33, 17x17, etc
	for (int i = 0; i < chunkSize; i++)
	{
		ds[i] = new float[chunkSize];
	}
	chunkSize--;
	// Initiate map
	tiles = new TileClass*[chunkSize];// 32x32, 16x16, etc
	baseTiles = new BaseTile*[chunkSize];
	for (int i = 0; i < chunkSize; i++)
	{
		tiles[i] = new TileClass[chunkSize];
		baseTiles[i] = new BaseTile[chunkSize];
	}

	// Create noise - algorithm usage
	DiamondSquare(30.0f, 0.76789f);

	// Create Tile-based map
	CreateTiles();
}
Map::Map(int randSeed, int exponent, float startValue)
{
	arrOfTiles = nullptr;
	// Get seed for noise
	setRandom(randSeed);
	// Initiate point-map
	this->chunkSize = pow(2, exponent) + 1;// +1 gives the map a mid-point
	seed = startValue;
	ds = new float*[chunkSize];// 33x33, 17x17, etc
	for (int i = 0; i < chunkSize; i++)
	{
		ds[i] = new float[chunkSize];
	}
	chunkSize--;
	// Initiate map
	tiles = new TileClass*[chunkSize];// 32x32, 16x16, etc
	baseTiles = new BaseTile*[chunkSize];
	for (int i = 0; i < chunkSize; i++)
	{
		tiles[i] = new TileClass[chunkSize];
		baseTiles[i] = new BaseTile[chunkSize];
	}

	// Create noise - algorithm usage
	DiamondSquare((startValue * 0.5f), 0.76789f);

	// Create Tile-based map
	CreateTiles();
}
Map::~Map()
{
	for (int h = chunkSize - 1; h > 0; h--)
	{
		delete[] ds[h];// If fatal crash happened, check this one.
		delete[] tiles[h];
		delete[] baseTiles[h];
	}
	delete ds[0];
	delete[] tiles;
	delete[] ds;
	delete[] baseTiles;
	delete[] arrOfTiles;
}

void Map::setSeed(float seed)
{
	this->seed = seed;
}
float Map::getSeed() const
{
	return this->seed;
}
void Map::setChunkSize(int size)
{
	this->chunkSize = size;
}
int Map::getChunkSize() const
{
	return this->chunkSize;
}
void Map::setOffset(float offset)
{
	this->offset = offset;
}
float Map::getOffset() const
{
	return this->offset;
}
void Map::setRandom(int value)
{
	srand(value);
}
float Map::getRandom()
{
	random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);// Random value between 0.0f -> 1.0f
	return random;
}
int Map::getWater() const
{
	return this->water;
}
int Map::getPeak() const
{
	return this->peak;
}
int Map::getObstacles() const
{
	return (getWater() + getPeak());
}

int Map::pow(int base, int exponent)
{
	int b = 1;
	if (exponent > 0)
	{
		b = 2;
		for (int i = 1; i < exponent; i++)
		{
			b *= base;
		}
	}
	else
	{
		b = 1;
	}
	return b;
}

void Map::CreateTiles()
{
	float avg = 0.0f;
	float posX = 0.0f;
	float posZ = 0.0f;
	DirectX::XMFLOAT3 worldpos = { posX, 0.0f, posZ };

	for (int h = 0; h < chunkSize; h++)
	{
		for (int w = 0; w < chunkSize; w++)
		{
			avg = ds[h][w] +
				ds[h + 1][w] +
				ds[h][w + 1] +
				ds[h + 1][w + 1];
			avg /= 4;

			// Set world position
			worldpos.x = (h * TILESIZE);
			worldpos.z = (w * TILESIZE);
			tiles[h][w] = TileClass(avg, worldpos);

			EvaluateTile(tiles[h][w]);
		}
	}
}
int Map::getNrOfTiles() const
{
	return (chunkSize * chunkSize);
}
DirectX::XMMATRIX* Map::getTileMatrices()
{
	int count = 0;
	//TODO http://stackoverflow.com/questions/20104815/warning-c4316-object-allocated-on-the-heap-may-not-be-aligned-16
	arrOfTiles = new DirectX::XMMATRIX[getNrOfTiles()];
	for (int h = 0; h < chunkSize; h++)
	{
		for (int w = 0; w < chunkSize; w++)
		{
			arrOfTiles[count] = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&tiles[h][w].getWorldPos()));
			count++;
		}
	}
	return arrOfTiles;
}
DirectX::XMMATRIX Map::setMapPlane()
{
	int center = (int)(chunkSize * 0.5f * TILESIZE);
	float scale = TILESIZE * chunkSize * 0.25f;
	return (DirectX::XMMatrixTranslationFromVector(DirectX::XMVectorSet(0.f, 0.f, 0.f, 1.f)) * DirectX::XMMatrixScaling(scale, 1.f, scale));
}
//TODO Make sure average is okey once visual graphics is at hand
void Map::DiamondSquare(float range, float decrease)
{
	// Starting value for corners
	float startOffset = 1.2f;
	ds[0][0] = seed + (seed * getRandom() * startOffset);// Top left
	ds[0][chunkSize] = seed + (seed * getRandom() * startOffset);// Top right
	ds[chunkSize][0] = seed + (seed * getRandom() * startOffset);// Bot left
	ds[chunkSize][chunkSize] = seed + (seed * getRandom() * startOffset);// Bot right

	offset = range;// the range (-off -> +off) for the average offset
	float avg;
	for (int i = chunkSize; i >= 2; i /= 2, offset *= decrease)// decrease the variation of the offset
	{
		int halfI = i / 2;
		// generate new square values
		for (int h = 0; h < chunkSize; h += i)
		{
			for (int w = 0; w < chunkSize; w += i)
			{
				// calculate average of existing corners
				avg = ds[h][w] +// top left
					ds[h + i][w] +// top right
					ds[h][w + i] +// bot left
					ds[h + i][w + i];// bot right
				avg /= 4.0;

				// center is average plus random offset
				ds[h + halfI][w + halfI] = avg + (getRandom() * 2 * offset) - offset;
			}
		}//__SQUARE_END__//

		// generate the diamond values
		for (int h = 0; h < (chunkSize + 1); h += halfI)
		{
			for (int w = (h + halfI) % i; w < (chunkSize + 1); w += i)
			{
				avg = ds[(h - halfI + chunkSize) % (chunkSize)][w] +// left of center
					ds[(h + halfI) % (chunkSize)][w] +// right of center
					ds[h][(w + halfI) % (chunkSize)] +// below center
					ds[h][(w - halfI + chunkSize) % (chunkSize)];// above center
				avg /= 4.0;

				// new value = average plus random offset
				avg = avg + (getRandom() * 2 * offset) - offset;
				// update value
				ds[h][w] = avg;

				// remove this below and adjust loop condition above
				if (h == 0)
				{
					ds[chunkSize][w] = avg;
				}
				if (w == 0) // Due to modulus-usage
				{
					ds[h][chunkSize] = avg;
				}
			}
		}//__DIAMOND_END__//
	}//__HEIGHT_MAP_END__//
}
//TODO Set tile type
void Map::EvaluateTile(TileClass& tile)
{
	if (tile.getHeight() < 50)
	{
		tile.setObstacle(true);
		tile.setType(1);
		water++;
	}
	else if (tile.getHeight() > 82 && tile.getHeight() < 84)
	{
		tile.setObstacle(true);
		tile.setType(2);
		peak++;
	}
	else if (tile.getHeight() > 75 && tile.getHeight() < 77)
	{
		tile.setObstacle(true);
		tile.setType(2);
		peak++;
	}
	else if (tile.getHeight() > 125)
	{
		tile.setObstacle(true);
		tile.setType(2);
		peak++;
	}
	else
	{
		tile.setType(0);
	}
}
bool Map::EvaluateMap()
{
	bool redo = false;
	int mapSize = chunkSize * chunkSize;
	float calc = (float)mapSize;
	calc = water / calc;
	if (calc < 0.03 || calc > 0.18)
	{
		redo = true;
		water = 0;
	}
	return redo;
}

BaseTile** Map::getBaseTiles()
{
	baseTiles = new BaseTile*[chunkSize];
	for (int h = 0; h < chunkSize; h++)
	{
		baseTiles[h] = new BaseTile[chunkSize];
		for (int w = 0; w < chunkSize; w++)
		{
			baseTiles[h][w] = tiles[h][w].getBase();
		}
	}
	return baseTiles;
}