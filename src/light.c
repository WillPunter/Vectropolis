/*
	light.c
	
	This file provides code 
*/

//light functions
//initialise light
void initialiseLight(double x, double y, double z, double dirX, double dirY, double dirZ, double intensity, int type){
	//search for empty lights
	int i;
	
	for(i = 0; i < NUMBER_OF_LIGHTS; i++){
		//check if empty
		if(lights[i].alive == 0){
			//set light properties
			lights[i].alive = 1;
			lights[i].position.x = x;
			lights[i].position.y = y;
			lights[i].position.z = z;
			lights[i].direction.x = dirX;
			lights[i].direction.y = dirY;
			lights[i].direction.z = dirZ;
			lights[i].intensity = intensity;
			lights[i].type = type;
			break;
		};
	};
};

//generate lights
void generateLights(){
	//create lights
	initialiseLight(0, 0, 0, 0, 0, 0, 0.1f, AMBIENT_LIGHT);
	initialiseLight(0, 0, 0, -3, 1.5, -4.5, 0.4f, DIRECTIONAL_LIGHT);
	initialiseLight(0, 0, 0, 3, 1.5, 4.5, 0.4f, DIRECTIONAL_LIGHT);
};

//transform lights
void transformLights(Matrix4x4f cameraTransform, Vector4f cameraPosition){
	//transform all lights
	//reset transformed lights
	memset(transformedLights, 0, NUMBER_OF_LIGHTS * sizeof(Light));
	
	//iterate through lights and transform them
	int i = 0;
	for(i = 0; i < NUMBER_OF_LIGHTS; i++){
		if(lights[i].alive == 1){
			transformedLights[i] = lights[i];
			transformedLights[i].position = subtractVectors(transformedLights[i].position, cameraPosition);
			transformedLights[i].position = matrixVectorProduct(cameraTransform, transformedLights[i].position);
			transformedLights[i].direction = matrixVectorProduct(cameraTransform, transformedLights[i].direction);
		};
	};
};