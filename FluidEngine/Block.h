#pragma once

class Block
{

private:

	bool visible;

public:

	Block();
	~Block();

	bool getVisible();
	void setVisible(bool isvisible);

};