/*
	terrain.c
	
	This file includes code for initialising and updating terrain and buildings.
*/

//terrain functions
void initialiseTerrain(double x, double y, double z, double width, double height, double length, int type){
	//search for empty slot
	int i;
	for(i = 0; i < NUMBER_OF_ENTITIES; i++){
		if(terrain[i].alive == 0){
			terrain[i].alive = 1;
			terrain[i].position.x = x;
			terrain[i].position.y = y;
			terrain[i].position.z = z;
			terrain[i].size.x = width;
			terrain[i].size.y = height;
			terrain[i].size.z = length;
			terrain[i].model.mesh = &mesh_cube;
			terrain[i].model.scale = scalarProduct(terrain[i].size, 0.5);
			terrain[i].model.colour.red = 50 + rand() % 156;
			terrain[i].model.colour.green = 50 + rand() % 156;
			terrain[i].model.colour.blue = 50 + rand() % 156;
			terrain[i].type = type;
			terrain[i].updateProc = &updateTerrain;
			terrain[i].hp = 1;
			break;
		};
	};
};

//update terrain
void updateTerrain(Entity * terrain){
	//kill self if hp <= 0
	if(terrain->hp <= 0){
		//play explosion sound
		playSound(&sound_buildingDestroy);
		
		//set alive to zero
		terrain->alive = 0;
		numberOfBuildings -= 1;
		
		//generate debris
		int i;
		for(i = 0; i < 10; i++){
			initialiseDebris(terrain->position.x - terrain->size.x / 2 + rand() % (int) terrain->size.x, terrain->position.y - terrain->size.y / 2 + rand() % (int) terrain->size.y, terrain->position.z  - terrain->size.z / 2 + rand() % (int) terrain->size.z, terrain->model.colour.red, terrain->model.colour.green, terrain->model.colour.blue);
		};
		
		//generate particles
		for(i = 0; i < 150; i++){
			initialiseParticle(5, terrain->position.x - terrain->size.x / 2 + rand() % (int) terrain->size.x, terrain->position.y - terrain->size.y / 2 + rand() % (int) terrain->size.y, terrain->position.z  - terrain->size.z / 2 + rand() % (int) terrain->size.z, 25 - (rand() % 50), 25 - (rand() % 50), 25 - (rand() % 50), terrain->model.colour.red, terrain->model.colour.green, terrain->model.colour.blue);
		};
	};
};

//generate buildings
void generateBuildings(){
	//erase existing buildings
	memset(terrain, 0, NUMBER_OF_ENTITIES * sizeof(Entity));
	
	//initialise terrain blocks
	initialiseTerrain(0, -0.5, 0, 400, 1, 400, FLOOR);
	
	//initialise variables for buildings
	double x;
	double z;
	double width;
	double height;
	
	//create buildings
	int i;
	numberOfBuildings = 50;
	
	for(i = 0; i < numberOfBuildings; i++){
		//randomise dimenions
		int validDimensions = 0;
		
		do {
			width = 4.f + (double) (rand() % 200) / 25.f;
			height = 15.f + (double) (rand() %200) / 5.f;
			x = 180 - rand() % 360;
			z = 180 - rand() % 360;
			
			int j;
			validDimensions = 1;
			
			for(j = 0; j < NUMBER_OF_ENTITIES; j++){
				//check if new building overlaps building j
				if(terrain[j].type == BUILDING && terrain[j].alive == 1 && fabs(x - terrain[j].position.x) < (terrain[j].size.x + width) / 2 && fabs(z - terrain[j].position.z) < (terrain[j].size.z + width) / 2){
					//overlap - so dimensions are invalid
					validDimensions = 0;
					printf("%d %d\n", i, j);
				};
			};
		} while(!validDimensions);
		
		//create terrain
		initialiseTerrain(x, height / 2, z, width, height, width, BUILDING);
	};
};