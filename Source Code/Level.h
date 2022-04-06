#pragma once
#define _USE_MATH_DEFINES

#include <SFML/Graphics.hpp>
#include "Input.h"
#include <stdio.h>
#include <time.h>   
#include <math.h>
#include <vector>
#include <amp.h>
#include <amp_math.h>
#include <amp_short_vectors.h>

using namespace concurrency;

//SIMULATION ATTRIBUTES
#define WIDTH 1500
#define HEIGHT 1500
#define NUM_OF_PARTICLES 1000000

//SLIME ATTRIBUTES
#define SENSOR_ANGLE 50
#define STEER_ANGLE	 30
#define SENSOR_OFFSET 30
#define SENSOR_WIDTH 3
#define DIFFUSION 3
#define SPEED 5
#define COLOR 0x0FC8FFFF

struct Complexf
{
	float x;
	float y;
};

struct Complexi
{
	int x;
	int y;
};

//Particle Structure
struct Particle
{
	Complexf position;
	float angle;
	float Sense(float sensorAngle, array_view<uint32_t,2> trailMap) restrict(amp);
};

//Calculator
struct Calc
{
	//Maths
	static float DegToRad(float num)  restrict(amp, cpu);
};

//Level Class Responsible 
class Level
{ 
public:

	Level(sf::RenderWindow* hwnd, Input* in);

	//Functions for "Game Loop"
	void handleInput();
	void update();
	void render();

private:
	// Default functions for rendering to the screen.
	void beginDraw();
	void endDraw();

	// Default variables for level class.
	sf::RenderWindow* window;
	Input* input;

	//Texture
	sf::Image trailMapImage;
	
	//Background
	sf::RectangleShape background;

	//Particles
	std::vector<Particle> particles;
	
	//Trail Map
	std::vector<uint32_t> trailMap;
	
	//Simulation Function
	void Rotate(array_view<uint32_t,2> &trailMap, array_view<Particle> &particles,const array_view<float> randNum,int randomize);
	void Move(array_view<uint32_t,2> &trailMap, array_view<Particle> &particles,const array_view<float> randNum,int randomize);
	void Decay(array_view<uint32_t,2> &trailMap);
	void Defuse(array_view<uint32_t,2> &trailMap);

	//Reset
	void Reset(int mode);
	//Randomness
	std::vector<float> randNum;
};

