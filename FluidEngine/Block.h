#pragma once

class Block
{

private:

	bool enabled;

public:

	Block();
	~Block();

	bool getEnabled();
	void setEnabled(bool value);

};