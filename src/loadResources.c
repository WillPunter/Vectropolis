/*
	loadResources.c
	
	This contains definitions for all of the loading functions.
	Specifically, the code below allows the program to load 3d models (meshes, .obj), bitmap (.bmp) and sound (.wav) from their respective files or from an embedded resource
*/

//get raw data from resource
uint8_t * getRawDataFromResource(int resourceID){
	//get pointer to resource
	//first parameter is the module (use the module of the current executable - 0), second is a handle to the resource
	//a handle to the resource can be retrieved using the FindResource function, which takes the module, the resource name and the resource data type as parameters
	//a pointer to the raw data in memory is returned
	return LoadResource(GetModuleHandle(0), FindResource(GetModuleHandle(0), MAKEINTRESOURCE(resourceID), RT_RCDATA));
};

//load mesh from file
void loadMeshFromFile(char filePath[], Mesh * mesh){
	//open file
	FILE * file = fopen(filePath, "r");
	
	//check if open
	if(file){
		//read the number of vertices
		mesh->numberOfVertices = 0;
		mesh->numberOfTriangles = 0;
		
		int i;
		
		char line[128];
		while(fgets(line, 128, file) != NULL){
			if(line[0] == 'v' && line[1] == ' '){
				//increment vertices
				mesh->numberOfVertices += 1;
			} else if(line[0] == 'f' && line[1] == ' '){
				//increment triangles
				mesh->numberOfTriangles += 1;
			};
		};
		
		//allocate space for vertices
		mesh->vertices = (Vector4f *) malloc(mesh->numberOfVertices * sizeof(Vector4f));
		mesh->triangles = (Triangle *) malloc(mesh->numberOfTriangles * sizeof(Triangle));
		
		//rewind file
		rewind(file);
		
		//iterate
		int vertexIndex = 0;
		int triangleIndex = 0;
		while(fgets(line, 128, file) != NULL){
			if(line[0] == 'v' && line[1] == ' '){
				//create variables
				float x;
				float y;
				float z;
				
				//read vertices
				sscanf(line, "v %f %f %f\r\n", &x, &y, &z);
				mesh->vertices[vertexIndex].x = x;
				mesh->vertices[vertexIndex].y = y;
				mesh->vertices[vertexIndex].z = z;
				mesh->vertices[vertexIndex].w = 1;
				vertexIndex += 1;
			} else if(line[0] == 'f' && line[1] == ' '){
				//create vertices
				int v1;
				int v2;
				int v3;
				Triangle t;
				
				//read vertices
				sscanf(line, "f %d %d %d", &v1, &v2, &v3);
				t.vertices[0] = mesh->vertices[v1 - 1];
				t.vertices[1] = mesh->vertices[v2 - 1];
				t.vertices[2] = mesh->vertices[v3 - 1];
				mesh->triangles[triangleIndex] = t;
				triangleIndex += 1;
			};
		};
		
		//close file
		fclose(file);
	};
};

//load mesh from resource
void loadMeshFromResource(int resourceID, Mesh * mesh){
	//get pointer to resource data
	uint8_t * rawData = getRawDataFromResource(resourceID);
	
	//check if raw data loaded
	if(rawData){
		//create raw data 
		uint8_t * rawDataCounter = rawData;
		
		//read the number of vertices
		mesh->numberOfVertices = 0;
		mesh->numberOfTriangles = 0;
		
		//iterate through lines of obj file
		//obj file ends when there are two adjacent new line characters
		char prevChar = 0;
		char currentChar = 1;
		
		while(!(prevChar == 0 && currentChar == 0)){
			//get previous character
			prevChar = currentChar;
			currentChar = *rawDataCounter;
			
			//check if new line has been reached
			if(prevChar == '\n'){
				//check for vertex
				if(currentChar == 'v'){
					//add to vertices
					mesh->numberOfVertices += 1;
				} else if(currentChar == 'f'){
					//add to triangles
					mesh->numberOfTriangles += 1;
				};
			};
			
			//increment raw data counter
			rawDataCounter++;
		};
		
		//allocate space for vertices
		mesh->vertices = (Vector4f *) malloc(mesh->numberOfVertices * sizeof(Vector4f));
		mesh->triangles = (Triangle *) malloc(mesh->numberOfTriangles * sizeof(Triangle));
		
		//create vertex and triangle index variables
		int vertexIndex = 0;
		int triangleIndex = 0;
		
		//iterate through data and retrieve vertices and triangles
		prevChar = 0;
		currentChar = 1;
		
		while(!(prevChar == 0 && currentChar == 0)){
			//get previous character
			prevChar = currentChar;
			currentChar = *rawData;
			
			//check if new line
			if(prevChar == '\n'){
				//check for vertex
				if(currentChar == 'v'){
					//read vertices
					//create variables
					float x;
					float y;
					float z;
				
					//read vertices
					sscanf(rawData, "v %f %f %f\r\n", &x, &y, &z);
					mesh->vertices[vertexIndex].x = x;
					mesh->vertices[vertexIndex].y = y;
					mesh->vertices[vertexIndex].z = z;
					mesh->vertices[vertexIndex].w = 1;
					vertexIndex += 1;
				} else if(currentChar == 'f'){
					//read triangle
					//create vertices
					int v1;
					int v2;
					int v3;
					Triangle t;
					
					//read vertices
					sscanf(rawData, "f %d %d %d", &v1, &v2, &v3);
					t.vertices[0] = mesh->vertices[v1 - 1];
					t.vertices[1] = mesh->vertices[v2 - 1];
					t.vertices[2] = mesh->vertices[v3 - 1];
					mesh->triangles[triangleIndex] = t;
					triangleIndex += 1;
				};
			};
			
			//increment raw data pointer
			rawData++;
		};
	};
};

//loading functions
//load bitmap from file
void loadBitmapFromFile(char filePath[], Bitmap * bitmap){
	//attempt to open file
	FILE * file = fopen(filePath, "rb");
	
	//check if file opened
	if(file){
		//attempt to read file header
		fread(&bitmap->fileHeader, sizeof(BITMAPFILEHEADER), 1, file);
		
		//attempt to read the info header
		fread(&bitmap->infoHeader, sizeof(BITMAPINFOHEADER), 1, file);
		
		//allocate memory for pixels
		bitmap->pixels = malloc(bitmap->infoHeader.biWidth * bitmap->infoHeader.biHeight * sizeof(Pixel));
		
		//read pixels
		fread(bitmap->pixels, sizeof(Pixel), bitmap->infoHeader.biWidth * bitmap->infoHeader.biHeight, file);
		
		//close file
		fclose(file);
	};
};

//load bitmap from resource
void loadBitmapFromResource(int resourceID, Bitmap * bitmap){
	//get pointer to resource data
	uint8_t * rawData = getRawDataFromResource(resourceID);
	
	//check if raw data loaded
	if(rawData){
		//read bitmap info header
		memcpy(&bitmap->fileHeader, rawData, sizeof(BITMAPFILEHEADER));
		
		//move raw data pointer
		rawData += sizeof(BITMAPFILEHEADER);
		
		//read bitmap file header
		memcpy(&bitmap->infoHeader, rawData, sizeof(BITMAPINFOHEADER));
		
		//move raw data pointer
		rawData += sizeof(BITMAPINFOHEADER);
		
		//get bitmap dimensions
		printf("Image size: %d by %d\n", bitmap->infoHeader.biWidth, bitmap->infoHeader.biHeight);
		
		//initialise pixel buffer
		bitmap->pixels = malloc(bitmap->infoHeader.biWidth * bitmap->infoHeader.biHeight * sizeof(Pixel));
		
		//copy data from resource
		memcpy(bitmap->pixels, rawData, bitmap->infoHeader.biWidth * bitmap->infoHeader.biHeight * sizeof(Pixel));
	};
};

//load sound from file
void loadSoundFromFile(char filePath[], Sound * sound){
	//attempt to open file
	FILE * file = fopen(filePath, "rb");
	
	//check if file opened
	if(file){
		//read file size
		fseek(file, 0, SEEK_END);
		int fileSize = ftell(file);
		rewind(file);
		
		//read entire file into byte array
		char * rawBuffer = malloc(fileSize);
		fread(rawBuffer, 1, fileSize, file);
		char * filePosition = rawBuffer;
		
		//create data processing variables
		uint16_t dataChunkOffset = 0;
		DWORD dataChunkSearcher = 0;
		int dataChunkFound = 0;
		DWORD dataChunkSize = 0;
		
		//.wav is part of Microsoft's RIFF file specification
		//this means that the first four bytes should be the letters RIFF
		//RIFF stands for resource interchange file format
		//check if RIFF value is equal to RIFF ascii characters (in little endian arrangement)
		if(rawBuffer[0] == 'R' && rawBuffer[1] == 'I' && rawBuffer[2] == 'F' && rawBuffer[3] == 'F'){
			//riff is correct - read metadata
			//set file pointer to 20th byte
			filePosition = rawBuffer + 20;
			
			//read wave format (.wav metadata)
			memcpy(&sound->waveFormat, filePosition, sizeof(WAVEFORMATEX));
			
			//check that wave format has been correctly read - wave file is malformed otherwise
			if(sound->waveFormat.wFormatTag == WAVE_FORMAT_PCM && sound->waveFormat.nBlockAlign == (sound->waveFormat.nChannels * sound->waveFormat.wBitsPerSample) / 8 && sound->waveFormat.wFormatTag == WAVE_FORMAT_PCM){
				//search for data chunk
				while(!dataChunkFound && dataChunkOffset < 1024){
					//read chunk of data
					memcpy(&dataChunkSearcher, filePosition, 4);
					
					//check if new chunk is equal to data chunk value
					//this spells data in big  endian (i.e. backwards)
					if(dataChunkSearcher == 0x61746164){
						//this data chunk should usually be located between bytes 36 and 40
						dataChunkFound = 1;
						break;
					} else {
						//increment data chunk offset
						filePosition++;
						dataChunkOffset += 1;
					};
				};
			
				//check that data chunk was found
				if(dataChunkFound){
					//read data chunk size
					//the chunk size is usually stored between bytes 30 and 44
					filePosition += 4;
					memcpy(&dataChunkSize, filePosition, 4);
					filePosition += 4;
					
					//allocate size for xaudio2 buffer
					sound->buffer.pAudioData = malloc(dataChunkSize);
					
					//read data chunk of specified size
					memcpy((void *) sound->buffer.pAudioData, filePosition, dataChunkSize);
					
					//set sound buffer metadata
					sound->buffer.Flags = XAUDIO2_END_OF_STREAM;
					sound->buffer.AudioBytes = dataChunkSize;
				};
			};
		};
		
		//free raw buffer
		free(rawBuffer);
		
		//close file
		fclose(file);
	};
};

//load sound from resource
void loadSoundFromResource(int resourceID, Sound * sound){
	//get raw data 
	//get pointer to resource data
	uint8_t * rawData = getRawDataFromResource(resourceID);
	
	//check if raw data loaded
	if(rawData){
		//read entire file into byte array
		char * dataPosition = rawData;
		
		//create data processing variables
		uint16_t dataChunkOffset = 0;
		DWORD dataChunkSearcher = 0;
		int dataChunkFound = 0;
		DWORD dataChunkSize = 0;
		
		//.wav is part of Microsoft's RIFF file specification
		//this means that the first four bytes should be the letters RIFF
		//RIFF stands for resource interchange file format
		//check if RIFF value is equal to RIFF ascii characters (in little endian arrangement)
		if(rawData[0] == 'R' && rawData[1] == 'I' && rawData[2] == 'F' && rawData[3] == 'F'){
			//riff is correct - read metadata
			//set file pointer to 20th byte
			dataPosition = rawData + 20;
			
			//read wave format (.wav metadata)
			memcpy(&sound->waveFormat, dataPosition, sizeof(WAVEFORMATEX));
			
			//check that wave format has been correctly read - wave file is malformed otherwise
			if(sound->waveFormat.wFormatTag == WAVE_FORMAT_PCM && sound->waveFormat.nBlockAlign == (sound->waveFormat.nChannels * sound->waveFormat.wBitsPerSample) / 8 && sound->waveFormat.wFormatTag == WAVE_FORMAT_PCM){
				//search for data chunk
				while(!dataChunkFound && dataChunkOffset < 1024){
					//read chunk of data
					memcpy(&dataChunkSearcher, dataPosition, 4);
					
					//check if new chunk is equal to data chunk value
					//this spells data in big  endian (i.e. backwards)
					if(dataChunkSearcher == 0x61746164){
						//this data chunk should usually be located between bytes 36 and 40
						dataChunkFound = 1;
						break;
					} else {
						//increment data chunk offset
						dataPosition++;
						dataChunkOffset += 1;
					};
				};
			
				//check that data chunk was found
				if(dataChunkFound){
					//read data chunk size
					//the chunk size is usually stored between bytes 30 and 44
					dataPosition += 4;
					memcpy(&dataChunkSize, dataPosition, 4);
					dataPosition += 4;
					
					//allocate size for xaudio2 buffer
					sound->buffer.pAudioData = malloc(dataChunkSize);
					
					//read data chunk of specified size
					memcpy((void *) sound->buffer.pAudioData, dataPosition, dataChunkSize);
					
					//set sound buffer metadata
					sound->buffer.Flags = XAUDIO2_END_OF_STREAM;
					sound->buffer.AudioBytes = dataChunkSize;
				};
			};
		};
	};
};