#pragma once

#include "Block.h"

class Chunk
{

private:

	int blockSize;
	Block*** blocks;

public:

	Chunk();
	~Chunk();

	void load(int size);
	void update(float dt);
	void draw();

};