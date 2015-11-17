#include "Block.h"

Block::Block()
{
	visible = true;
}

Block::~Block()
{

}

bool Block::getVisible()
{
	return visible;
}

void Block::setVisible(bool isvisible)
{
	visible = isvisible;
}