/*
	debris.c
	
	This file includes functions for initialising and updating debris.
*/

//debris functions
void initialiseDebris(double x, double y, double z, uint8_t red, uint8_t green, uint8_t blue){
	//search for empty slot
	int i;
	for(i = 0; i < NUMBER_OF_ENTITIES; i++){
		if(debris[i].alive == 0){
			debris[i].alive = 1;
			debris[i].position.x = x;
			debris[i].position.y = y;
			debris[i].position.z = z;
			debris[i].size.x = 0.25 + (rand() % 8) / 8.f;
			debris[i].size.y = 0.25 + (rand() % 8) / 8.f;
			debris[i].size.z = 0.25 + (rand() % 8) / 8.f;
			debris[i].rotation.x = 2.f * PI * (rand() % 360) / 360.f;
			debris[i].rotation.y = 2.f * PI * (rand() % 360) / 360.f;
			debris[i].rotation.z = 2.f * PI * (rand() % 360) / 360.f;
			debris[i].model.mesh = &mesh_debris;
			debris[i].model.scale = debris[i].size;
			debris[i].model.colour.red = red;
			debris[i].model.colour.green = green;
			debris[i].model.colour.blue = blue;
			debris[i].updateProc = &updateDebris;
			break;
		};
	};
};

//update debris
void updateDebris(Entity * debris){
	//move
	Matrix4x4f rotation = createRotationMatrix(debris->rotation);
	Vector4f moveDir = {0};
	moveDir.z = 1;
	moveDir = matrixVectorProduct(rotation, moveDir);
	
	debris->velocity = scalarProduct(moveDir, 50);
	debris->position = addVectors(debris->position, scalarProduct(debris->velocity, gameClock.deltaTime));
	
	if(vectorMagnitude(subtractVectors(debris->position, playerInstance.position)) > 300){
		debris->alive = 0;
	};
	
	debris->model.rotation = debris->rotation;
};