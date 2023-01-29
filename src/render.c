/*
	render.c
	
	This includes function definitions for all of the rendering functions, from drawing pixels and primitives to 3d model rendering.
*/

//render functions
//reset depth buffer
void resetDepthBuffer(){
	int i;
	int j;
	
	for(i = 0; i < BUFFER_WIDTH; i++){
		for(j = 0; j < BUFFER_HEIGHT; j++){
			window.depthBuffer[i][j] = 9999999999;
		};
	};
};

//draw pixel
void drawPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue){
	//check that pixel is within bounds
	if(x >= 0 && x < BUFFER_WIDTH && y >= 0 && y < BUFFER_HEIGHT){
		//draw pixel
		Pixel * pixel = window.renderBuffer + y * BUFFER_WIDTH + x;
		pixel->red = red;
		pixel->green = green;
		pixel->blue = blue;
	};
};

//draw line
void drawLine(int x1, int y1, int x2, int y2, uint8_t red, uint8_t green, uint8_t blue){
	//get dx and dy
	int dx = x2 - x1;
	int dy = y2 - y1;
	
	int numberOfSteps;
	double xStep;
	double yStep;
	
	//check whether dy or dx is larger
	numberOfSteps = (abs(dy) > abs(dx)) ? abs(dy) : abs(dx);
	
	xStep = (double) dx / numberOfSteps;
	yStep = (double) dy / numberOfSteps;
	
	//iterate through steps
	double x = x1;
	double y = y1;
	
	int i;
	
	for(i = 0; i < numberOfSteps; i++){
		drawPixel(x, y, red, green, blue);
		x += xStep;
		y += yStep;
	};
};

//draw wireframe triangle
void drawWireframeTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint8_t red, uint8_t green, uint8_t blue){
	//draw lines
	drawLine(x1, y1, x2, y2, red, green, blue);
	drawLine(x2, y2, x3, y3, red, green, blue);
	drawLine(x3, y3, x1, y1, red, green, blue);
};

//draw iterpolated row
void drawInterpolatedRow(int x1, int x2, int y, double i1, double i2, double depth1, double depth2, uint8_t red, uint8_t green, uint8_t blue){
	//check that x-coordinates are not the same
	if(x1 != x2){
		int i;
		
		if(x2 < x1){
			int temp = x2;
			x2 = x1;
			x1 = temp;
			
			double temp2 = i2;
			i2 = i1;
			i1 = temp2;
			
			temp2 = depth2;
			depth2 = depth1;
			depth1 = temp2;
		};
		
		//caclculate change in intensity per unit x
		double di = (i2 - i1) / (x2 - x1); 
		double intensity = i1;
		
		//calculate change in depth per unit x
		double depthChange = (double) (1.f / depth2 - 1.f / depth1) / (x2 - x1);
		double depth = 1.f / depth1;
		
		for(i = x1; i <= x2; i++){
			/*
				Calculate depth of actual coordinates.
				
				Let A and B be the endpoints of the line.
				
				Let A' and B' be the projected coordinates of these endpoints.
				
				The projected x coordinate and the depth are not linearly related.
				
				P'x = Px / Pz 
				
				Therefore, the projected x coordinate is inversely proportional to the distance.
				
				Therefore, we can linearly interpolated for 1/Pz against Px
			*/
			
			//check if pixel is at front of depth buffer
			if(i >= 0 && i < BUFFER_WIDTH && y >= 0 && y < BUFFER_HEIGHT && 1.f / depth < window.depthBuffer[i][y]){
				drawPixel(i, y, (double) red * intensity, (double) green * intensity, (double) blue * intensity);
				window.depthBuffer[i][y] = 1.f / depth;
			};
			
			intensity += di;
			depth += depthChange;
		};
	};
};

//draw shaded triangle
void drawShadedTriangle(int x1, int y1, int x2, int y2, int x3, int y3, double i1, double i2, double i3, double d1, double d2, double d3, uint8_t red, uint8_t green, uint8_t blue){
	/*
		A shaded triangle should be drawn in two halves:
		The first goes from the bottom of the triangle to the height of the first vertex
		The second goes from the first vertex to the top of the triangle.
		The triangle should be drawn row-by-row where the length of each row is linearly interpolated.
	*/
	
	//check that triangle is valid
	if(!(y1 == y2 && y2 == y3)){
		//sort vertices in ascending order of height
		if(y2 < y1){
			int temp = y2;
			y2 = y1;
			y1 = temp;
			
			temp = x2;
			x2 = x1;
			x1 = temp;
			
			double temp2 = i2;
			i2 = i1;
			i1 = temp2;
			
			temp2 = d2;
			d2 = d1;
			d1 = temp2;
		};
		
		if(y3 < y2){
			int temp = y3;
			y3 = y2;
			y2 = temp;
			
			temp = x3;
			x3 = x2;
			x2 = temp;
			
			double temp2 = i3;
			i3 = i2;
			i2 = temp2;
			
			temp2 = d3;
			d3 = d2;
			d2 = temp2;
		};
		
		if(y2 < y1){
			int temp = y2;
			y2 = y1;
			y1 = temp;
			
			temp = x2;
			x2 = x1;
			x1 = temp;
			
			double temp2 = i2;
			i2 = i1;
			i1 = temp2;
			
			temp2 = d2;
			d2 = d1;
			d1 = temp2;
		};
		
		//draw first half of triangle
		//calculate change in x per unit y from first vertex to other two vertices
		double v12XStep;
		if(y2 - y1 == 0){
			v12XStep = (double) (x2 - x1) / (y2 + 0.000000001 - y1);
		} else {
			v12XStep = (double) (x2 - x1) / (y2 - y1);
		};
		
		double v13XStep;
		if(y3 - y1 == 0){
			v13XStep = (double) (x3 - x1) / (y3 + 0.000000001 - y1);
		} else {
			v13XStep = (double) (x3 - x1) / (y3 - y1);
		};
		
		//calculate change in intensity per unit y from first vertex to other two vertices
		double v12IStep;
		if(y2 - y1 == 0){
			v12IStep = (double) (i2 - i1) / (y2 + 0.000000001 - y1);
		} else {
			v12IStep = (double) (i2 - i1) / (y2 - y1);
		};
		
		double v13IStep;
		if(y3 - y1 == 0){
			v13IStep = (double) (i3 - i1) / (y3 + 0.000000001 - y1);
		} else {
			v13IStep = (double) (i3 - i1) / (y3 - y1);
		};
		
		//calculate change in depth per unit y from first vertex to other two vertices
		double v12DStep;
		if(y2 - y1 == 0){
			v12DStep = (double) (1.f / d2 - 1.f / d1) / (y2 + 0.000000001 - y1);
		} else {
			v12DStep = (double) (1.f / d2 -  1.f / d1) / (y2 - y1);
		};
		
		double v13DStep;
		if(y3 - y1 == 0){
			v13DStep = (double) (1.f / d3 - 1.f / d1) / (y3 + 0.000000001 - y1);
		} else {
			v13DStep = (double) (1.f / d3 - 1.f / d1) / (y3 - y1);
		};
		
		//iterate through rows
		int i;
		double v12SideX = x1;
		double v13SideX = x1;
		double v12SideI = i1;
		double v13SideI = i1;
		double v12SideD = 1.f / d1;
		double v13SideD = 1.f / d1;
		
		for(i = y1; i < y2; i++){
			//draw row of pixels between v12 and v13 side x coordinates
			drawInterpolatedRow(v12SideX, v13SideX, i, v12SideI, v13SideI, 1.f / v12SideD, 1.f / v13SideD, red, green, blue);
			
			//increment v12 and v13 x coordinates
			v12SideX += v12XStep;
			v13SideX += v13XStep;
			
			//increment v12 and v13 intensities
			v12SideI += v12IStep;
			v13SideI += v13IStep;
			
			//increment v12 and v13 depths
			v12SideD += v12DStep;
			v13SideD += v13DStep;
		};
		
		//draw second half of triangle
		//calculate change in x per unit y from v2 to v3
		double v23XStep;
		if((y3 - y2) == 0){
			v23XStep = (double) (x3 - x2) / (y3 + 0.000000001 - y2);
		} else {
			v23XStep = (double) (x3 - x2) / (y3 - y2);
		};
		
		//calculate change in intensity per unit y from v2 to v3
		double v23IStep;
		if((y3 - y2) == 0){
			v23IStep = (double) (i3 - i2) / (y3 + 0.000000001 - y2);
		} else {
			v23IStep = (double) (i3 - i2) / (y3 - y2);
		};
		
		//calculate change in intensity per unit y from v2 to v3
		double v23DStep;
		if((y3 - y2) == 0){
			v23DStep = (double) (1.f / d3 - 1.f / d2) / (y3 + 0.000000001 - y2);
		} else {
			v23DStep = (double) (1.f / d3 - 1.f / d2) / (y3 - y2);
		};
		
		//iterate through rows
		double v23SideX = x2;
		double v23SideI = i2;
		double v23SideD = 1.f / d2;
		
		for(i = y2; i < y3; i++){
			//draw row of pixels between v12 and v13 side x coordinates
			drawInterpolatedRow(v23SideX, v13SideX, i, v23SideI, v13SideI, 1.f / v23SideD, 1.f / v13SideD, red, green, blue);
			
			//increment v23 and v13 x coordinates
			v23SideX += v23XStep;
			v13SideX += v13XStep;
			
			//increment v23 and v13 intensities
			v23SideI += v23IStep;
			v13SideI += v13IStep;
			
			//increment v23 and v13 depths
			v23SideD += v23DStep;
			v13SideD += v13DStep;
		};
	};
};

//draw rectangle
void drawRectangle(int x1, int y1, int x2, int y2, uint8_t red, uint8_t green, uint8_t blue){
	//iterate through rows and columns
	int i;
	int j;
	
	for(i = x1; i < x2; i++){
		for(j = y1; j < y2; j++){
			drawPixel(i, j, red, green, blue);
		};
	};
};

//draw bitmap 
void drawBitmap(Bitmap * bitmap, int x, int y){
	//iterate through pixels
	int i;
	int j;
	Pixel * pixel;
	
	for(i = 0; i < bitmap->infoHeader.biWidth; i++){
		for(j = 0; j < bitmap->infoHeader.biHeight; j++){
			pixel = bitmap->pixels + j * bitmap->infoHeader.biWidth + i;
			
			//check that not transparent
			if(!(pixel->red == window.transparentRed && pixel->green == window.transparentGreen && pixel->blue == window.transparentBlue)){
				//draw
				drawPixel(x + i, y + j, pixel->red, pixel->green, pixel->blue);
			};
		};
	};
};

//draw sprite
void drawSprite(Sprite * sprite, int x, int y){
	//check if source coordinates are in the wrong order
	if(sprite->srcX1 > sprite->srcX2){
		//swap
		int temp = sprite->srcX1;
		sprite->srcX1 = sprite->srcX2;
		sprite->srcX2 = temp;
	};
	
	if(sprite->srcY1 > sprite->srcY2){
		//swap
		int temp = sprite->srcY1;
		sprite->srcY1 = sprite->srcY2;
		sprite->srcY2 = temp;
	};
	
	//calculate new corners
	int srcWidth = sprite->srcX2 - sprite->srcX1;
	int srcHeight = sprite->srcY2 - sprite->srcY1;

	//create list of vertices
	double xCoord[4];
	double yCoord[4];
	
	//add coordinates
	xCoord[0] = -sprite->originX;
	yCoord[0] = -sprite->originY;
	xCoord[1] = -sprite->originX;
	yCoord[1] = -sprite->originY + srcHeight;
	xCoord[2] = -sprite->originX + srcWidth;
	yCoord[2] = -sprite->originY + srcHeight;
	xCoord[3] = -sprite->originX + srcWidth;
	yCoord[3] = -sprite->originY;
	
	//calculate sin and cos of the angle
	double cosAngle = cos(sprite->angle);
	double sinAngle = sin(sprite->angle);
	
	//store minX, maxX, minY and maxY
	double maxX = -100000;
	double minX = 100000;
	double maxY = -100000;
	double minY = 100000;
	
	//create loop counter
	int i;
	
	for(i = 0; i < 4; i++){
		//multiply coordinate pair by scale factor
		xCoord[i] *= sprite->xScale;
		yCoord[i] *= sprite->yScale;
		
		/*
			Rotate coordinates
			x = lcos(a)
			y = lsin(a)
			
			new x = lcos(a + b)
			new y = lsin(a + b)
			
			new x = lcos(a)cos(b) - lsin(a)sin(b)
			new y = lsin(a)cos(b) + lsin(b)cos(a)
		
			new x = xcos(b) - ysin(b)
			new y = ycos(b) + xsin(b)
		*/
		
		double newX = xCoord[i] * cosAngle - yCoord[i] * sinAngle;
		double newY = xCoord[i] * sinAngle + yCoord[i] * cosAngle;
		
		xCoord[i] = newX + x;
		yCoord[i] = newY + y;
			
		//check if coordinates exceed min and max values
		if(xCoord[i] < minX){
			minX = xCoord[i];
		};
		
		if(xCoord[i] > maxX){
			maxX = xCoord[i];
		};
		
		if(yCoord[i] < minY){
			minY = yCoord[i];
		};
		
		if(yCoord[i] > maxY){
			maxY = yCoord[i];
		};
	};
	
	//iterate through pixels in bounding box
	int j;
	
	for(i = minX; i <= maxX; i++){
		for(j = minY; j < maxY; j++){
			//reverse transformation
			double pixelX = i - x;
			double pixelY = j - y;
			
			//reverse rotation
			//cos(-a) = cos(a)
			//sin(-a) = -sin(a)
			//new x = xcos(a) - ysin(a)
			//new y = xsin(a) + ycos(a) 
			double newX = pixelX * cosAngle + pixelY * sinAngle;
			double newY = pixelX * -sinAngle + pixelY * cosAngle;
			
			pixelX = newX;
			pixelY = newY;
			
			//reverse scaling
			pixelX /= sprite->xScale;
			pixelY /= sprite->yScale;
			
			//add origin x and y
			pixelX += sprite->originX;
			pixelY += sprite->originY;
			
			//add on the source coordinates
			pixelX += sprite->srcX1;
			pixelY += sprite->srcY1;
			
			//set colour pointer
			if(pixelX >= sprite->srcX1 && pixelX <= sprite->srcX2 && pixelY >= sprite->srcY1 && pixelY <= sprite->srcY2){
				Pixel * colour = sprite->bitmap->pixels + ((int) pixelY * sprite->bitmap->infoHeader.biWidth + (int) pixelX);
				
				//check if colour is transparent
				if(!(colour->red == window.transparentRed && colour->green == window.transparentGreen && colour->blue == window.transparentBlue)){
					//check if solid colour
					if(sprite->solidColour == 1){
						*colour = sprite->colour;
					};
					
					//draw pixel
					drawPixel(i, j, colour->red, colour->green, colour->blue);
				};
			};
		};
	};
};

//draw character
void drawCharacter(char c, int x, int y, int size, uint8_t red, uint8_t green, uint8_t blue){
	//create sprite
	Sprite charSprite = {0};
	charSprite.bitmap = &bmp_font;
	charSprite.xScale = (double) size / FONT_SIZE;
	charSprite.yScale = (double) size / FONT_SIZE;
	
	//convert to upper case
	if(c >= 'a' && c <= 'z'){
		c -= ('a' - 'A');
	};
	
	//get source coordiates
	if(c >= 'A' && c <= 'Z'){
		charSprite.srcX1 = (c - 'A') * FONT_SIZE; 
		charSprite.srcY1 = charSprite.bitmap->infoHeader.biHeight - FONT_SIZE;
	} else if(c >= '0' && c <= '9'){
		charSprite.srcX1 = (c - '0') * FONT_SIZE; 
		charSprite.srcY1 = charSprite.bitmap->infoHeader.biHeight - 2 * FONT_SIZE;
	} else {
		charSprite.srcY1 = charSprite.bitmap->infoHeader.biHeight - 2 * FONT_SIZE;
		
		switch(c){
			case '.':{
				charSprite.srcX1 = 10 * FONT_SIZE;
				break;
			};
			
			case ',':{
				charSprite.srcX1 = 11 * FONT_SIZE;
				break;
			};
			
			case ':':{
				charSprite.srcX1 = 12 * FONT_SIZE;
				break;
			};
			
			case '!':{
				charSprite.srcX1 = 13 * FONT_SIZE;
				break;
			};
			
			case '?':{
				charSprite.srcX1 = 14 * FONT_SIZE;
				break;
			};
			
			case '-':{
				charSprite.srcX1 = 15 * FONT_SIZE;
				break;
			};
			
			case '+':{
				charSprite.srcX1 = 16 * FONT_SIZE;
				break;
			};
			
			case '*':{
				charSprite.srcX1 = 17 * FONT_SIZE;
				break;
			};
			
			case '/':{
				charSprite.srcX1 = 18 * FONT_SIZE;
				break;
			};
			
			case '_':{
				charSprite.srcX1 = 19 * FONT_SIZE;
				break;
			};
			
			case '(':{
				charSprite.srcX1 = 20 * FONT_SIZE;
				break;
			};
			
			case ')':{
				charSprite.srcX1 = 21 * FONT_SIZE;
				break;
			};
			
			default:{
				charSprite.srcX1 = 22 * FONT_SIZE;
				break;
			};
		};
	};
	
	//add source dimensions
	charSprite.srcX2 = charSprite.srcX1 + FONT_SIZE;
	charSprite.srcY2 = charSprite.srcY1 + FONT_SIZE;
	
	//set solid colour
	charSprite.solidColour = 1;
	charSprite.colour.red = red;
	charSprite.colour.green = green;
	charSprite.colour.blue = blue;
	
	//draw sprite
	drawSprite(&charSprite, x, y);
};

//draw string
void drawString(char str[STR_LENGTH], int x, int y, int size, uint8_t red, uint8_t green, uint8_t blue){
	//iterate through characters
	int i ;
	int length = strlen(str);
	
	for(i = 0; i < length; i++){
		drawCharacter(str[i], x + size * i, y, size, red, green, blue);
	};
};

//draw model
void drawModel(Model * model, Vector4f position, Matrix4x4f cameraTransform, Vector4f cameraPosition){
	//create world transformation matrix
	Matrix4x4f transform = createTransformationMatrix(model->scale, model->rotation, position);
	
	//apply the translation before the remaining camera transformations
	transform.data[0][3] -= cameraPosition.x;
	transform.data[1][3] -= cameraPosition.y;
	transform.data[2][3] -= cameraPosition.z;
	
	//apply camera transform to world transform
	transform = matrixProduct(cameraTransform, transform);
	
	//iterate through triangles
	int i;
	int j;
	for(i = 0; i < model->mesh->numberOfTriangles; i++){		
		//transform triangle
		Triangle transformedTriangle = transformTriangle(model->mesh->triangles[i], transform);
			
		//get normal vector
		Vector4f normal = crossProduct(subtractVectors(transformedTriangle.vertices[1], transformedTriangle.vertices[0]), subtractVectors(transformedTriangle.vertices[2], transformedTriangle.vertices[0]));
	
		//apply lighting to all vertices
		for(j = 0; j < 3; j++){
			transformedTriangle.intensities[j] = lightVertex(transformedTriangle.vertices[j], normal);
		};
		
		//printf("(%f %f %f)\n", transformedTriangle.intensities[0], transformedTriangle.intensities[1], transformedTriangle.intensities[2]);
		
		//cull back faces
		cullBackFaces(transformedTriangle, model->colour);
	};
};

//cull back faces
void cullBackFaces(Triangle t, Pixel colour){
	//get normal vector
	Vector4f sideVector1 = subtractVectors(t.vertices[1], t.vertices[0]);
	Vector4f sideVector2 = subtractVectors(t.vertices[2], t.vertices[0]);
	Vector4f normalVector = crossProduct(sideVector1, sideVector2);
	
	//get line of sight vector
	Vector4f lineOfSight = t.vertices[0];
	
	//check if the component of the normal vector along the camera vector is negative - i.e. that the normal vector is pointing towards the camera
	if(dotProduct(normalVector, lineOfSight) <= 0){
		//clip screen plane
		clipScreenPlane(t, colour);
	};
};

/*
	Clip lines of triangles against plane vector
	Plane is given in format (P - P0) . n = 0
	Therefore:
		(P - P0) . n = 0
		P . n - P0 . n = 0
		P . n = P0 . n = constant
	
	For each triangle, get the number of inside and outside vertices
*/
int clipPlane(Triangle t, Vector4f p0, Vector4f n, int greater, int compareAxis, Triangle * t1, Triangle * t2){
	//this returns the scalar that d must be multiplied with to get the point on the line
	/*
		Plane is in format r . n = p0 . n = k
		
		This can be derived from
		(r - p0) . n = 0
		This is because any vector within the plane (r - p0) should be perpendicular to the normal vector to the plane.
		
		r . n = k
		
		This can be converted to cartesian form, using:
		r . n = k
		rx * nx + ry * ny + rz * nz = k
		nx * x + ny * y + nz * z = k
		ax + by + cz = k	
	*/
	
	//get plane constant
	double k = dotProduct(p0, n);
	
	//iterate through triangles
	int i;
	Vector4f * insideVertices[3];
	Vector4f * outsideVertices[3];
	double * insideIntensities[3];
	double * outsideIntensities[3];
	int insideIndex = 0;
	int outsideIndex = 0;
				
	for(i = 0; i < 3; i++){
		/*
			We now have an equation for our plane in the form ax + by + cz = k
			We can then compare it with the vertex, using our given coordinate axis.
			
			
			For example, if we want to see if the vertex is greater than the plane in the x-axis,
			we would calculate the x coordiate of the plane for the corresponding y and z coordinate of the vertex
		*/
		
		int isInside = 0;
		
		Vector4f targetCoord;
		
		switch(compareAxis){
			//compare x axis
			case X_AXIS:{
				/*
					Plane is given by: nx * x + ny * y + nz * z = k
					We want to find the x coordinate:
					x = (k - (nz * z + ny * y)) / nx
				*/
				targetCoord.x = (k - (n.y * t.vertices[i].y + n.z * t.vertices[i].z)) / n.x;
				isInside = (t.vertices[i].x * greater > targetCoord.x * greater);
				break;
			};
			
			//compare y axis
			case Y_AXIS:{
				/*
					Plane is given by: nx * x + ny * y + nz * z = k
					We want to find the x coordinate:
					y = (k - (nz * z + nx * x)) / ny
				*/
				targetCoord.y = (k - (n.x * t.vertices[i].x + n.z * t.vertices[i].z)) / n.y;
				isInside = (t.vertices[i].y * greater > targetCoord.y * greater);
				break;
			};
			
			//compare z axis
			case Z_AXIS:{
				/*
					Plane is given by: nx * x + ny * y + nz * z = k
					We want to find the z coordinate:
					z = (k - (nx * x + ny * y)) / nz
				*/
				targetCoord.z = (k - (n.y * t.vertices[i].y + n.x * t.vertices[i].x)) / n.z;
				isInside = (t.vertices[i].z * greater > targetCoord.z * greater);
				break;
			};
		};
		
		//check if triangle is inside the plane
		if(isInside){
			//add to inside vertices
			insideVertices[insideIndex] = &t.vertices[i];
			insideIntensities[insideIndex] = &t.intensities[i];
			insideIndex += 1;
		} else {
			//add to outside vertices
			outsideVertices[outsideIndex] = &t.vertices[i];
			outsideIntensities[outsideIndex] = &t.intensities[i];
			outsideIndex += 1;
		};
	};
	
	//check how many vertices are on screen
	if(insideIndex == 3){
		//done - keep triangle the same
		*t1 = t;
		return 1;
	} else if(insideIndex == 2){
		/*
			Two vertices are inside. One vertex is outside
		*/
		
		//calculate the first line vector (outside vertex - inside vertex 1)
		Vector4f line1 = subtractVectors(*outsideVertices[0], *insideVertices[0]);
	
		//calculate intersection scalar (the scalar that line1 should be multiplied by to get the intersect)
		double intersect1 = getLinePlaneIntersection(*insideVertices[0], line1, p0, n);
		
		//calculate the second line vector (outside vertex - inside vertex 2)
		Vector4f line2 = subtractVectors(*outsideVertices[0], *insideVertices[1]);
		
		//calculate intersection scalar (the scalar that line2 should be multiplied by to get the intersect)
		double intersect2 = getLinePlaneIntersection(*insideVertices[1], line2, p0, n);
		
		//set the outside vertex to the first intersection
		double startDistance = vectorMagnitude(line1);
		*outsideVertices[0] = addVectors(*insideVertices[0], scalarProduct(line1, intersect1));
		double endDistance = vectorMagnitude(subtractVectors(*outsideVertices[0], *insideVertices[0]));
		*outsideIntensities[0] = *insideIntensities[0] + (*outsideIntensities[0] - *insideIntensities[0]) * endDistance / startDistance;
		
		//draw second triangle
		*t1 = t;
		
		//set the first inside vertex to the second intersection
		startDistance = vectorMagnitude(line2);
		*insideVertices[0] = addVectors(*insideVertices[1], scalarProduct(line2, intersect2));
		endDistance = vectorMagnitude(subtractVectors(*insideVertices[0], *insideVertices[1]));
		*insideIntensities[0] = *insideIntensities[1] + (*insideIntensities[0] - *insideIntensities[1]) * endDistance / startDistance;
		
		//swap inside vertices tp avoid normals being disrupted (lighting is dependent on the cross product, and thus on all triangles being arranged in the same way - anticlockwise)
		Vector4f temp = *insideVertices[0];
		double tempIntensity = *insideIntensities[0];
		*insideVertices[0] = *insideVertices[1];
		*insideVertices[1] = temp;
		*insideIntensities[0] = *insideIntensities[1];
		*insideIntensities[1] = tempIntensity;
		
		//draw second triangle
		*t2 = t;
		
		return 2;
	} else if(insideIndex == 1){
		/*
			One vertex is inside - find the intersection of the lines with the plane
			The lines to be checked are from the inside index to both outside vertices
		*/
		
		//calculate the first line vector (outside vertex 1 - inside vertex)
		Vector4f line1 = subtractVectors(*outsideVertices[0], *insideVertices[0]);
	
		//calculate intersection scalar (the scalar that line1 should be multiplied by to get the intersect)
		double intersect1 = getLinePlaneIntersection(*insideVertices[0], line1, p0, n);
		
		//calculate the second line vector (outside vertex 2 - inside vertex)
		Vector4f line2 = subtractVectors(*outsideVertices[1], *insideVertices[0]);
		
		//calculate intersection scalar (the scalar that line2 should be multiplied by to get the intersect)
		double intersect2 = getLinePlaneIntersection(*insideVertices[0], line2, p0, n);
		
		//set the outside vertices to their respective intersections
		double startDistance = vectorMagnitude(line1);
		*outsideVertices[0] = addVectors(*insideVertices[0], scalarProduct(line1, intersect1));
		double endDistance = vectorMagnitude(subtractVectors(*outsideVertices[0], *insideVertices[0]));
		*outsideIntensities[0] = *insideIntensities[0] + (*outsideIntensities[0] - *insideIntensities[0]) * endDistance / startDistance; 
		
		//set the second outside vertex to the intersection
		startDistance = vectorMagnitude(line2);
		*outsideVertices[1] = addVectors(*insideVertices[0], scalarProduct(line2, intersect2));
		endDistance = vectorMagnitude(subtractVectors(*outsideVertices[1], *insideVertices[0]));
		*outsideIntensities[1] = *insideIntensities[0] + (*outsideIntensities[1] - *insideIntensities[0]) * endDistance / startDistance; 
		
		//project and draw triangle
		*t1 = t;
		return 1;
	};
	return 0;
};

//clip screen plane
void clipScreenPlane(Triangle t, Pixel colour){
	/*
		Clip against plane Z = screen distance
		
		This plane needs to be written in the form r . n = p0 . n 
		P0 = (0, 0, screenDistance)
		n = (1, 0, 0)
		
	*/
	
	Vector4f p0;
	p0.x = 0;
	p0.y = 0;
	p0.z = window.screenDistance;
	
	Vector4f n;
	n.x = 0;
	n.y = 0;
	n.z = 1;
	
	Triangle t1;
	Triangle t2;
	
	int numberOfTriangles = clipPlane(t, p0, n, 1, Z_AXIS, &t1, &t2);
	
	//check if 1 or 2 triangles
	if(numberOfTriangles == 1){
		clipLeftPlane(t1, colour);
	} else if(numberOfTriangles == 2){
		clipLeftPlane(t1, colour);
		clipLeftPlane(t2, colour);
	};
};

//clip left plane
void clipLeftPlane(Triangle t, Pixel colour){
	/*
		Clip against left plane
		The plane vector iteself is given by x = -1, z = screenDistance
		The normal vector is this rotated by 90 degrees along the x-z plane
		Rotation =  [cos(a) -sin(a)]
					[sin(a) cos(a) ]
		a = 90 degrees
		[0 -1]
		[1 0 ]
	*/
	
	Vector4f p0;
	p0.x = 0;
	p0.y = 0;
	p0.z = 0;
	
	Vector4f n;
	n.x = -window.screenDistance;
	n.y = 0;
	n.z = -1;
	
	Triangle t1;
	Triangle t2;
	
	int numberOfTriangles = clipPlane(t, p0, n, 1, X_AXIS, &t1, &t2);
	
	//check if 1 or 2 triangles
	if(numberOfTriangles == 1){
		clipRightPlane(t1, colour);
	} else if(numberOfTriangles == 2){
		clipRightPlane(t1, colour);
		clipRightPlane(t2, colour);
	};
};

//clip right plane
void clipRightPlane(Triangle t, Pixel colour){
	/*
		Clip against right plane
		The plane vector iteself is given by x = 1, z = screenDistance
		The normal vector is this rotated by 90 degrees along the x-z plane
		Rotation =  [cos(a) -sin(a)]
					[sin(a) cos(a) ]
		a = 90 degrees
		[0 -1]
		[1 0 ]
		
		normal:
			x = -screenDistance
			z = 1
	*/
	
	Vector4f p0;
	p0.x = 0;
	p0.y = 0;
	p0.z = 0;
	
	Vector4f n;
	n.x = -window.screenDistance;
	n.y = 0;
	n.z = 1;
	
	Triangle t1;
	Triangle t2;
	
	int numberOfTriangles = clipPlane(t, p0, n, -1, X_AXIS, &t1, &t2);
	
	//check if 1 or 2 triangles
	if(numberOfTriangles == 1){
		clipTopPlane(t1, colour);
	} else if(numberOfTriangles == 2){
		clipTopPlane(t1, colour);
		clipTopPlane(t2, colour);
	};
};

//clip top plane
void clipTopPlane(Triangle t, Pixel colour){
	/*
		Clip against bottom plane
		The plane vector iteself is given by y = 1, z = screenDistance
		The normal vector is this rotated by 90 degrees along the x-z plane
		Rotation =  [cos(a) -sin(a)]
					[sin(a) cos(a) ]
		a = 90 degrees
		[0 -1]
		[1 0 ]
		
		normal:
			y = -screenDistance
			z = 1
	*/
	
	Vector4f p0;
	p0.x = 0;
	p0.y = 0;
	p0.z = 0;
	
	Vector4f n;
	n.x = 0;
	n.y = -window.screenDistance;
	n.z = 1;
	
	Triangle t1;
	Triangle t2;
	
	int numberOfTriangles = clipPlane(t, p0, n, -1, Y_AXIS, &t1, &t2);
	
	//check if 1 or 2 triangles
	if(numberOfTriangles == 1){
		clipBottomPlane(t1, colour);
	} else if(numberOfTriangles == 2){
		clipBottomPlane(t1, colour);
		clipBottomPlane(t2, colour);
	};
};

//clip bottom plane
void clipBottomPlane(Triangle t, Pixel colour){
	/*
		Clip against bottom plane
		The plane vector iteself is given by y = -1, z = screenDistance
		The normal vector is this rotated by 90 degrees along the y-z plane
		Rotation =  [cos(a) -sin(a)]
					[sin(a) cos(a) ]
		a = 90 degrees
		[0 -1]
		[1 0 ]
	*/
	
	Vector4f p0;
	p0.x = 0;
	p0.y = 0;
	p0.z = 0;
	
	Vector4f n;
	n.x = 0;
	n.y = -window.screenDistance;
	n.z = -1;
	
	Triangle t1;
	Triangle t2;
	
	int numberOfTriangles = clipPlane(t, p0, n, 1, Y_AXIS, &t1, &t2);
	
	//check if 1 or 2 triangles
	if(numberOfTriangles == 1){
		projectTriangle(t1, colour);
	} else if(numberOfTriangles == 2){
		projectTriangle(t1, colour);
		projectTriangle(t2, colour);
	};
};

//project triangle
void projectTriangle(Triangle t, Pixel colour){
	/*
		Imagine we are staring down the x-axis, so that only the y-axis and z-axis are visible
		The projected screen is a fixed distance away.
		For a point on the grid somewhere past the screen, we can draw a right-angled triangle from the origin to the point.
		This triangle passes through the screen, such that a smaller, similar triangle can be createed by finding the intersections with the screen.
		As these triangles are similar, screen y / screen z = real y / real z. We know screen z - this is the screen distance.
		Therefore screen y = screen z * real y / real z
		Therefore, for a point, P, the coordinates of the projected point P' can be found with the following:
			P'z = screen z = screen distance
			P'y = screen distance * Py / Pz
			P'x = screen distance * Px / Pz
	*/
	Triangle projectedTriangle;
	
	int i;
	
	for(i = 0; i < 3; i++){
		//project coordinate using projection equation
		if(t.vertices[i].z > 0){
			projectedTriangle.vertices[i].x = window.screenDistance * t.vertices[i].x / t.vertices[i].z;
			projectedTriangle.vertices[i].y = window.screenDistance * t.vertices[i].y / t.vertices[i].z;
			projectedTriangle.vertices[i].z = 0;
		
			//convert screen coordinates (-1 to 1) to pixel coordiates (0 to BUFFER_WIDTH) (0 to BUFFER_HEIGHT)
			projectedTriangle.vertices[i].x = (projectedTriangle.vertices[i].x + 1.f) * BUFFER_WIDTH / 2;
			projectedTriangle.vertices[i].y = (projectedTriangle.vertices[i].y + 1.f) * BUFFER_HEIGHT / 2;
		};
	};
	
	//draw triangle
	shadeAndDrawTriangle(projectedTriangle, t, colour);
};

//light vertex
double lightVertex(Vector4f v, Vector4f normal){
	//iterate through lights
	int i;
	double intensity = 0;
	Vector4f direction;
	
	for(i = 0; i < NUMBER_OF_LIGHTS; i++){
		//check if light is active
		if(transformedLights[i].alive == 1){
			//check type of light
			if(transformedLights[i].type == AMBIENT_LIGHT){
				//add intensity to vertex
				intensity += transformedLights[i].intensity;
			} else {
				//check if point or directional
				if(transformedLights[i].type == POINT_LIGHT){
					direction = subtractVectors(transformedLights[i].position, v);
				} else {
					direction = transformedLights[i].direction;
				};
				
				//calculate the intensity of the light from this angle
				double directionalIntensity = dotProduct(direction, normal) / ((vectorMagnitude(direction) * vectorMagnitude(normal)));
				
				//check that directional intensity is larger than zero
				if(directionalIntensity > 0){
					//add intensity . normal (as we want the component of the light along the normal to be visible)
					intensity += directionalIntensity;
				};
			};
		};
	};
	
	if(intensity > 1){
		return 1;
	} else if(intensity < 0){
		return 0;
	};
	
	return intensity;
};

//shade and draw triangle
void shadeAndDrawTriangle(Triangle projectedTriangle, Triangle original, Pixel colour){
	//get normal vector for triangle
	Vector4f normal = crossProduct(subtractVectors(original.vertices[1], original.vertices[0]), subtractVectors(original.vertices[2], original.vertices[0]));
	
	//calculate intensities
	int i;
	double intensities[3] = {1, 1, 1};
	
	//draw shaded triangle
	drawShadedTriangle(projectedTriangle.vertices[0].x, projectedTriangle.vertices[0].y, projectedTriangle.vertices[1].x, projectedTriangle.vertices[1].y, projectedTriangle.vertices[2].x, projectedTriangle.vertices[2].y, original.intensities[0], original.intensities[1], original.intensities[2], original.vertices[0].z, original.vertices[1].z, original.vertices[2].z, colour.red, colour.green, colour.blue);
};

//draw menu BG
void drawMenuBG(){
	//reset depth buffer
	resetDepthBuffer();
	
	//transform camera
	//apply the camera transformation matrix to the model transformation matrix
	Matrix4x4f cameraTransform = createCameraRotationMatrix(scalarProduct(menuCamera.rotation, -1));
	
	//draw stars
	int i;
	for(i = 0; i < NUMBER_OF_STARS; i++){
		//draw star as particle
		renderParticle(stars[i], starColours[i], cameraTransform);
	};
	
	//transform lights
	transformLights(cameraTransform, menuCamera.position);
	
	//draw buildings
	//render all terrain
	for(i = 0; i < NUMBER_OF_ENTITIES; i++){
		if(terrain[i].alive == 1){
			//render
			drawModel(&terrain[i].model, terrain[i].position, cameraTransform, menuCamera.position);
		};
	};
	
	//point menu camera towards origin
	Vector4f origin = {0};
	Vector4f targetDir = subtractVectors(origin, menuCamera.position);
	menuCamera.rotation = directionToAngle(targetDir);
	
	//move perpendicular to target vector and upwards
	Vector4f upwardsDir = {0};
	upwardsDir.y = 1;
	Vector4f velocityDir = normaliseVector(crossProduct(targetDir, upwardsDir));
	menuCamera.position = addVectors(menuCamera.position, scalarProduct(velocityDir, gameClock.deltaTime * 100));
};

//draw mouse
void drawMouse(){
	//draw mouse 
	drawBitmap(&bmp_cursor, mouseX, mouseY - 16);
};