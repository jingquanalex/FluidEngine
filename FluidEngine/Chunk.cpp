#include "Chunk.h"

Chunk::Chunk()
{
	blocks = nullptr;
}

Chunk::~Chunk()
{
	if (blocks != nullptr)
	{
		for (int i = 0; i < blockSize; i++)
		{
			for (int j = 0; j < blockSize; j++)
			{
				delete[] blocks[i][j];
			}
			delete[] blocks[i];
		}
		delete[] blocks;
	}
}

// Build the 3D array of blocks of size in all dimension
void Chunk::load(int size)
{
	blockSize = size;
	blocks = new Block**[size];
	for (int i = 0; i < size; i++)
	{
		blocks[i] = new Block*[size];
		for (int j = 0; j < size; j++)
		{
			blocks[i][j] = new Block[size];
		}
	}
}