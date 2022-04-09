# Physarum Slime Simulation


### Background:

<img src="https://user-images.githubusercontent.com/59959821/162450520-e2ba8dd9-6489-4ae1-bc03-f5d7c323786c.jpg" width="300">
Physarum Polysephalum is a type of slime mold that specializes in growth, movement and area reduction. Due to these characteristics, this slime mold is efficient at finding the shortest path between any amount of given points.

### Slime Simulation

The above described behaviour can be replicated by simulating a large number of particles which follow the same movement principles as Physarum Polycephalum. To efficiently simulate over 500000 slime particles, GPGPU (AMP in C++) was used.

<img src="https://user-images.githubusercontent.com/59959821/162447996-9e8953b0-ac9d-444f-b3ec-f1b502d936bb.png" width="400">


A description of the techniques and kernels used can be found in a presentation format [here](https://github.com/StylianosZachariou/Physarum-Slime-Simulation/files/8452681/Slime.pdf).

### The Application
A zip file containing the executable can be downloaded in the Release page or [here](https://github.com/StylianosZachariou/Physarum-Slime-Simulation/releases/download/1.0/executable.zip). As soon as the application is launched, the slime simulation begins. The user has some control when reseting the simulation:

1- Place particles in a centered square.

<img src="https://media.giphy.com/media/zufgNQkdMt17etRdBm/giphy.gif" width="200">

2- Spread the particles throught the scene.

<img src="https://media.giphy.com/media/5BNMA9DBgwlLxXlE0H/giphy-downsized.gif" width="400">

3- Place particles in the center pixel assigning them with random rotations.

<img src="https://media.giphy.com/media/Mz2Sq4bhHFYtoi5K3e/giphy-downsized.gif" width="400">

Each of these can be triggered using the respective number keys on the keyboard.

**Warning: The application is computationally heavy and a good graphics card is recomended for it to function as expected.**
