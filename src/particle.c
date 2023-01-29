/*
	particle.c
	
	This file includs code for initialising, updating and rendering particles.
*/


//particle functions
void initialiseParticle(double timeToLive, double x, double y, double z, double vx, double vy, double vz, uint8_t red, uint8_t green, uint8_t blue){
	//search for empty slot
	int i;
	
	for(i = 0; i < NUMBER_OF_PARTICLES; i++){
		//check if slot is empty
		if(particles[i].alive == 0){
			//set properties
			particles[i].alive = 1;
			particles[i].timeToLive = timeToLive;
			particles[i].position.x = x;
			particles[i].position.y = y;
			particles[i].position.z = z;
			particles[i].velocity.x = vx;
			particles[i].velocity.y = vy;
			particles[i].velocity.z = vz;
			particles[i].colour.red = red;
			particles[i].colour.green = green;
			particles[i].colour.blue = blue;
			break;
		};
	};
};

//update particle
void updateParticle(Particle * particle){
	//move
	particle->position = addVectors(particle->position, scalarProduct(particle->velocity, gameClock.deltaTime));
	
	//decrease time to live
	particle->timeToLive -= gameClock.deltaTime;
	
	//check if out of time
	if(particle->timeToLive <= 0){
		particle->alive = 0;
	};
};

//render particle
void renderParticle(Vector4f position, Pixel colour, Matrix4x4f cameraTransform){
	//transform star
	Vector4f newPosition = position;
	newPosition.x -= camera.position.x;
	newPosition.y -= camera.position.y;
	newPosition.z -= camera.position.z;
	newPosition = matrixVectorProduct(cameraTransform, newPosition);
	
	//check that not off screen
	if(newPosition.z > window.screenDistance){
		/*
			Original coordinate P = (x, y)
			Projected coordinate P'
			Right angle triangle formed from origin (O), P, P'
			screenDistance / Pz = P'y / Py
			P'y = screenDistance / (Pz * Py)
			Same applies for x
		*/
		
		//project coordinates
		double screenX = newPosition.x * window.screenDistance / (newPosition.z);
		double screenY = newPosition.y * window.screenDistance / (newPosition.z);
		
		//convert to screen coordinates
		screenX = (BUFFER_WIDTH * (screenX + 1.f)) / 2.f;
		screenY = (BUFFER_HEIGHT * (screenY + 1.f)) / 2.f;
		
		//get distance to position
		double distance = vectorMagnitude(subtractVectors(position, camera.position));
		
		//check if space in depth buffer is not already occupied
		if(screenX >= 0 &&  screenX < BUFFER_WIDTH && screenY >= 0 && screenY <= BUFFER_HEIGHT && distance < window.depthBuffer[(int) screenX][(int) screenY]){
			window.depthBuffer[(int) screenX][(int) screenY] = distance;
			drawPixel(screenX, screenY, colour.red, colour.green, colour.blue);
		};
	};
};