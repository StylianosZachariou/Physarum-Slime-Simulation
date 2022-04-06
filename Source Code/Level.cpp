#include "Level.h"

Level::Level(sf::RenderWindow* hwnd, Input* in)
{
	window = hwnd;
	input = in;

	//Initialize Random Number Generator
	srand(time(0));

	//Create the Background
	background.setSize(sf::Vector2f(WIDTH, HEIGHT));
	background.setFillColor(sf::Color::Black);
	background.setPosition(0, 0);

	//Create The Trail Map
	trailMapImage.create(WIDTH, HEIGHT, sf::Color::Black);

	for (int j = 0; j < WIDTH*HEIGHT; j++)
	{
		trailMap.push_back(0x00000000);
	}
	
	//Initialize Particles
	for (int i = 0; i < NUM_OF_PARTICLES; i++)
	{
		Particle newParticle;
		//Random Positions
		newParticle.position.x = rand() % WIDTH/ 2 + WIDTH / 4;
		newParticle.position.y = rand() % HEIGHT/ 2 + HEIGHT / 4;
		
		//Random Angles
		newParticle.angle = (Calc::DegToRad(rand() % 360)) ;
		particles.push_back(newParticle);
		//Fill Random Number Array
		randNum.push_back(Calc::DegToRad((rand() %360)));
	};
	
}

// Handle user input
void Level::handleInput()
{
	//Reset Simulation
	if (input->isKeyDown(sf::Keyboard::Num1))
	{
		Reset(1);	//Particles start in Centre
	}

	if (input->isKeyDown(sf::Keyboard::Num2))
	{
		Reset(2);	//Particles spread throughout the whole image
	}


	if (input->isKeyDown(sf::Keyboard::Num3))
	{
		Reset(3);	//Firework
	}
}

//Update
void Level::update()
{
	//Create Array views
	array_view<Particle> av(NUM_OF_PARTICLES, particles);
	array_view<uint32_t, 2> av2(WIDTH,HEIGHT, trailMap);
	const array_view<float,1> av3(NUM_OF_PARTICLES, randNum);

	//Re-radomize
	int randomize = rand() % NUM_OF_PARTICLES/2;
	
	//Slime Simulation

	//Sense and Rotate Particles
	Rotate(av2, av, av3, randomize);

	//Move Particles
	Move(av2,av,av3,randomize);

	//Deffuse Trail Map
	Defuse(av2);
	  
	//Decay Trail Map
	Decay(av2);
	
	//Print Everything on the texture
	for (int j = 0; j < WIDTH; j++)
	{
		for (int i = 0; i < HEIGHT; i++)
		{
			int pos = i + (j * HEIGHT);
			trailMapImage.setPixel(j,i, sf::Color(trailMap[pos]));
		}
	}
	
}

// Render level
void Level::render()
{
	//Convert the Trail Map texture to a sprite
	sf::Texture texture;
	texture.loadFromImage(trailMapImage);
	sf::Sprite sprite;
	sprite.setTexture(texture);

	//Display background and sprite
	beginDraw();
		window->draw(background);
		window->draw(sprite);
	endDraw();
}

// Begins rendering to the back buffer
void Level::beginDraw()
{
	window->clear();
}

// Ends rendering to the back buffer.
void Level::endDraw()
{
	window->display();
}

//Sense Where to Rotate
float Particle::Sense(float sensorAngle, array_view<uint32_t,2> trailMap) restrict (amp)
{
	//Set sensor offset
	float sensorOffset = SENSOR_OFFSET;
	Complexf offsetCentre;
	
	//Find sensor Angle Depending on particle angle
	sensorAngle += angle;
		
	//Calculate offcentre X and Y
	offsetCentre.x = precise_math::sinf(sensorAngle) * sensorOffset;
	offsetCentre.y = precise_math::cosf(sensorAngle) * sensorOffset;
	
	float sum = 0;

	//The array the particles are able to look at
	int sensorWidth = (SENSOR_WIDTH / 2 - 0.5);

	//Iterate SensorWidthxSensorWidth
	for (int j = -sensorWidth; j < sensorWidth+1; j++)
	{
		int posY = (int(position.y + offsetCentre.y) + j);

		for (int i = -sensorWidth; i < sensorWidth+1; i++)
		{
			int posX = (int(position.x + offsetCentre.x)+i);

			//If particle in trailmap range
			if ((posX > 0) && (posY > 0) && (posX < WIDTH) && (posY < HEIGHT))
			{
				//Add RGB values to sum
				sum += (trailMap[posX][posY] & 0xFF000000) >> 24;
				sum += (trailMap[posX][posY] & 0x00FF0000) >> 16;
				sum += (trailMap[posX][posY] & 0x0000FF00) >> 8;
			}
		}
	}
	
	return sum;
}

//Rotate All Particles
void Level::Rotate(array_view<uint32_t,2> &trailMap, array_view<Particle> &particles,const array_view<float,1> randNum,int randomize)
{
	concurrency::parallel_for_each(particles.extent, [particles, trailMap, randNum,randomize](index<1> idx)
		restrict(amp) {
			//Angle that particle can sense
			float sensorAngle = Calc::DegToRad(SENSOR_ANGLE);
			//Angle that particle can steer
			float steerAngle = Calc::DegToRad(STEER_ANGLE);

			//Sense in three direction
			float forward = particles[idx].Sense(0, trailMap);
			float right = particles[idx].Sense(sensorAngle, trailMap);
			float left = particles[idx].Sense(-sensorAngle, trailMap);

			//Compare best direction to rotate
			if (forward > right&& forward > left)	//If forward is best
			{
				//Remains on the same path
			}
			else
			{
				if (forward < right && forward < left) //If forward is worst
				{
					float random;
					//Get Random Number
					if (idx[0] + randomize > NUM_OF_PARTICLES)	
					{
						random = randNum[idx[0] + randomize];
					}
					else
					{
						random = randNum[idx[0] - randomize];
					}

					//Rotate randomly left or right
					if (random > M_PI)
					{
						particles[idx].angle += steerAngle;
					}
					else
					{
						particles[idx].angle -= steerAngle;
					}
				}
				else
				{
					if (right > left) //Go right
					{
						particles[idx].angle += steerAngle;
					}
					else
					{
						if (left > right) //Go left
						{
							particles[idx].angle -= steerAngle;
						}
						
					}
				}
		    }
		});
	particles.synchronize();
}

//Move All Particles
void Level::Move(array_view<uint32_t,2> &trailMap, array_view<Particle> &particles,const array_view<float,1> randNum,int randomize)
{
	//For all Particles
	concurrency::parallel_for_each(particles.extent, [particles,trailMap,randNum,randomize](index<1> idx)
		restrict(amp) {
			
			//Particle Attributes
			Complexf direction;
			Complexf newPos;
			float speed = SPEED;

			//Calculate Direction
			direction.x = precise_math::sinf(particles[idx].angle );
			direction.y = precise_math::cosf(particles[idx].angle );
			
			//Calculate Position
			newPos.x = (direction.x *speed + particles[idx].position.x);
			newPos.y = (direction.y *speed + particles[idx].position.y);

			float random=0;

			//Get Random Number
			if (idx[0] + randomize > NUM_OF_PARTICLES)
			{
				random =randNum[idx[0] + randomize];
			}
			else
			{
				random = randNum[idx[0] - randomize];
			}

			//If Particle is on boundaries change angle Randomly
			if (newPos.x <= 0)
			{
				newPos.x = 0;
				particles[idx].angle = random;
			}
			
			if (newPos.y <= 0)
			{
				newPos.y = 0;
				particles[idx].angle = random;
			}

			if (newPos.x >= WIDTH-1)
			{
				newPos.x = WIDTH-1;
				particles[idx].angle = random;
			}

			if (newPos.y >= HEIGHT-1)
			{
				newPos.y = HEIGHT-1; 
				particles[idx].angle = random;
			}
			
			//Change Particles Position and Display it on trail map
			particles[idx].position.x = newPos.x;
			particles[idx].position.y = newPos.y;
			trailMap[newPos.x][newPos.y] = COLOR;
		});
	particles.synchronize();
	trailMap.synchronize();
}

//Decay Trail Map
void Level::Decay(array_view<uint32_t,2> &trailMap)
{
	//For all pixels in trail map
	concurrency::parallel_for_each(trailMap.extent, [trailMap](index<2> idx)
		restrict(amp) {
		
			//Get current RGB values
			uint32_t r = 0,g=0,b=0;
			r += (trailMap[idx] & 0xFF000000) >> 24;
			g += (trailMap[idx] & 0x00FF0000) >> 16;
			b += (trailMap[idx] & 0x0000FF00) >> 8;

			//If values are more than zero
			//Subtract aa small amount
			if (((trailMap[idx] & 0xFF000000) >> 24) > 0)
			{
				r -= 2.5;
			}

			if (((trailMap[idx] & 0x00FF0000) >> 16 ) > 0)
			{
				g -= 2.5;
			}

			if (((trailMap[idx] & 0x0000FF00) >> 8 ) > 0)
			{
				b -= 2.5;
			}

			//Save on trail Map 
			trailMap[idx] = (r << 24) | (g << 16) | (b << 8) | (255);
		});
	trailMap.synchronize();
}

//Deffuse Trail Map
void Level::Defuse(array_view<uint32_t,2> &trailMap)
{

	// Horizontal Tiled Diffusion
	concurrency::parallel_for_each(trailMap.extent.tile<WIDTH/2, 1>(), [trailMap](tiled_index<WIDTH/2, 1> t_idx)
		restrict(amp) {

			uint32_t r = 0, g = 0, b = 0;
			
			//Tile Static Memory
			tile_static uint32_t t[WIDTH/2];
			t[t_idx.local[0]] = trailMap[t_idx.global];
			t_idx.barrier.wait();

			//For adjacent pixels
			for (int i = -1; i < 2 ; i++)
			{
				if (t_idx.local[0] + i > 0 && t_idx.local[0] + i < WIDTH/2-1) //Use tile static memory
				{	
					//Add rgb values
					r += (t[t_idx.local[0] + i] & 0xFF000000) >> 24;
					g += (t[t_idx.local[0] + i] & 0x00FF0000) >> 16;
					b += (t[t_idx.local[0] + i] & 0x0000FF00) >> 8;	
				}
				else //If pixel is out of the tile use global memory
				{
					
				//Add RGB values
					r += (trailMap[t_idx.global[0] + i][t_idx.global[1]] & 0xFF000000) >> 24;
					g += (trailMap[t_idx.global[0] + i][t_idx.global[1]] & 0x00FF0000) >> 16;
					b += (trailMap[t_idx.global[0] + i][t_idx.global[1]] & 0x0000FF00) >> 8;
					
				}

			}

			//Find Mean of RGB
			r /= 3;
			g /= 3;
			b /= 3;

			t_idx.barrier.wait();//Wait For all threads to finish

			//Sve results on trail Map
			trailMap[t_idx.global] = (r << 24) | ((g << 16) | b << 8) | (255);
		
		});
	trailMap.synchronize();


	// Vertical Tiled Diffusion
	concurrency::parallel_for_each(trailMap.extent.tile<1, HEIGHT/2>(), [trailMap](tiled_index<1, HEIGHT / 2> t_idx)
	restrict(amp) {

		uint32_t r = 0, g = 0, b = 0;

		//Tile Static Memory
		tile_static uint32_t t[HEIGHT / 2];
		t[t_idx.local[1]] = trailMap[t_idx.global];
		t_idx.barrier.wait();
		
		//For adjacent pixels
		for (int i = -1; i < 2; i++)
		{

			if (t_idx.local[1] + i > 0 && t_idx.local[1] + i < HEIGHT / 2 -1)//Use Tile static Memory
			{
				//Add RGB Values
				r += (t[t_idx.local[1] + i] & 0xFF000000) >> 24;
				g += (t[t_idx.local[1] + i] & 0x00FF0000) >> 16;
				b += (t[t_idx.local[1] + i] & 0x0000FF00) >> 8;
				
			}
			else  //If pixel is out of the tile use global memory
			{
				//Add RGB Values
				r += (trailMap[t_idx.global[0]][t_idx.global[1] + i] & 0xFF000000) >> 24;
				g += (trailMap[t_idx.global[0]][t_idx.global[1] + i] & 0x00FF0000) >> 16;
				b += (trailMap[t_idx.global[0]][t_idx.global[1] + i] & 0x0000FF00) >> 8;

			}
		}
		//Find Mean of RGB
		r /= 3;
		g /= 3;
		b /= 3;

		t_idx.barrier.wait();//Wait For all threads to finish

		//Sve results on trail Map
		trailMap[t_idx.global] = (r << 24) | ((g << 16) | b << 8) | (255);

	});
	trailMap.synchronize();
}

//Reset Simulation
void Level::Reset(int mode)
{
	//Reset Trail Map
	trailMap.clear();
	for (int j = 0; j < WIDTH * HEIGHT; j++)
	{
		trailMap.push_back(0x00000000);
	}

	//Reset Particles
	particles.clear();
	for (int i = 0; i < NUM_OF_PARTICLES; i++)
	{
		Particle newParticle;

		//Set Position Depending on mode
		if (mode == 1)
		{
			newParticle.position.x = rand() % WIDTH / 2 + WIDTH / 4;
			newParticle.position.y = rand() % HEIGHT / 2 + HEIGHT / 4;
		}
		else
		{
			if (mode == 2)
			{
				newParticle.position.x = rand() % WIDTH ;
				newParticle.position.y = rand() % HEIGHT;
			}
			else
			{
				newParticle.position.x = WIDTH/2;
				newParticle.position.y = HEIGHT / 2;
			}
		}
		//Set Angle to random
		newParticle.angle = randNum[i];
		particles.push_back(newParticle);
	}
}

//Degrees To Radians
float Calc::DegToRad(float num) restrict (amp,cpu)
{
	return num * M_PI / 180;
}


