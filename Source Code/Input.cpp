#include "Input.h"

Input::Input()
{
	// set default values
	mouse.left = false;
	mouse.right = false;
	mouse.x = 0;
	mouse.y = 0;
}

void Input::setKeyUp(int key)//Set Key Up
{
	if (key >= 0)
	{
		keys[key] = false;
	}
}

void Input::setMouseLDown(bool down)// Set Mouse Left Down
{
	mouse.left = down;
}

bool Input::isKeyDown(int key) //Check if key is down
{
	if (key >= 0)
	{
		return keys[key];
	}
	return false;
}

int Input::getMouseX()// Get mouse X coordinate
{
	return mouse.x;
}

int Input::getMouseY() // Get mouse Y coordinate
{
	return mouse.y;
}


bool Input::isMouseLDown() // Check if mouse left is down
{

	return mouse.left;
}

void Input::setKeyDown(int key)//Set Key Down
{
	if (key >= 0)
	{
		keys[key] = true;
	}
}

void Input::setMouseX(int lx)// Set mouse X position
{
	mouse.x = lx;
}

void Input::setMouseY(int ly) //set mouse Y position
{
	mouse.y = ly;
}

void Input::setMousePosition(int lx, int ly)// set mouse Position
{
	setMouseX(lx);
	setMouseY(ly);
}