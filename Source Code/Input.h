#pragma once
class Input
{
private:
	// Mouse structure, store mouse relevant information
	struct Mouse
	{
		int x, y;
		bool left, right;
	};

public:
	Input();

	//Keyboard Functions
	bool isKeyDown(int key);
	void setKeyUp(int key);
	void setKeyDown(int);

	//Mouse Functions
	void setMouseX(int lx);
	void setMouseY(int ly);
	void setMousePosition(int lx, int ly);
	int getMouseX();
	int getMouseY();
	bool isMouseLDown();
	void setMouseLDown(bool down);

private:

	// Mouse variable
	bool keys[256]{ false };

	Mouse mouse;
};

