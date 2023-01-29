/*
	mathematics.c
	
	This file includes functions for vector and matrix operations, as well as collision detection and various other bits of mathematical code.
*/

//mathematical functions
//vector magnitude
double vectorMagnitude(Vector4f v){
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
};

//normalise vector
Vector4f normaliseVector(Vector4f v){
	double magnitude = vectorMagnitude(v);
	v.x /= magnitude;
	v.y /= magnitude;
	v.z /= magnitude;
	return v;
};

//add vectors
Vector4f addVectors(Vector4f a, Vector4f b){
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
};

//subtract vectors
Vector4f subtractVectors(Vector4f a, Vector4f b){
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	return a;
};

//scalar product
Vector4f scalarProduct(Vector4f a, double k){
	a.x *= k;
	a.y *= k;
	a.z *= k;
	return a;
};

//dot product
double dotProduct(Vector4f a, Vector4f b){
	return a.x * b.x + a.y * b.y + a.z * b.z;
};

//cross product
Vector4f crossProduct(Vector4f a, Vector4f b){
	Vector4f m;
	m.x = a.y * b.z - a.z * b.y;
	m.y = a.z * b.x - a.x * b.z; //this may need to be swapped
	m.z = a.x * b.y - a.y * b.x;
	return m;
};

//add matrices
Matrix4x4f addMatrices(Matrix4x4f a, Matrix4x4f b){
	//add
	int i;
	int j;
	
	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			a.data[i][j] += b.data[i][j];
		};
	};
	
	return a;
};

//subtract matrices
Matrix4x4f subtractMatrices(Matrix4x4f a, Matrix4x4f b){
	//subtract
	int i;
	int j;
	
	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			a.data[i][j] -= b.data[i][j];
		};
	};
	
	return a;
};

//matrix scalar product
Matrix4x4f matrixScalarProduct(Matrix4x4f m, double k){
	//multiply
	int i;
	int j;
	
	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			m.data[i][j] *= k;
		};
	};
	
	return m;
};

//matrix vector product
Vector4f matrixVectorProduct(Matrix4x4f m, Vector4f v){
	//multiply vector components
	Vector4f result;
	result.x = m.data[0][0] * v.x + m.data[0][1] * v.y + m.data[0][2] * v.z + m.data[0][3] * v.w;
	result.y = m.data[1][0] * v.x + m.data[1][1] * v.y + m.data[1][2] * v.z + m.data[1][3] * v.w;
	result.z = m.data[2][0] * v.x + m.data[2][1] * v.y + m.data[2][2] * v.z + m.data[2][3] * v.w;
	result.w = m.data[3][0] * v.x + m.data[3][1] * v.y + m.data[3][2] * v.z + m.data[3][3] * v.w;
	return result;
};

//matrix product
Matrix4x4f matrixProduct(Matrix4x4f a, Matrix4x4f b){
	//iterate through matrices
	int i;
	int j;
	
	Matrix4x4f result;
	
	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			result.data[i][j] = a.data[i][0] * b.data[0][j] + a.data[i][1] * b.data[1][j] + a.data[i][2] * b.data[2][j] + a.data[i][3] * b.data[3][j];
		};
	};
	
	return result;
};

//create identity matrix
Matrix4x4f createIdentityMatrix(){
	Matrix4x4f result = {0};
	
	int i;
	
	for(i = 0; i < 4; i++){
		result.data[i][i] = 1;
	};
	
	return result;
};

//create enlargement matrix
Matrix4x4f createEnlargementMatrix(Vector4f scale){
	Matrix4x4f result = createIdentityMatrix();
	result.data[0][0] *= scale.x;
	result.data[1][1] *= scale.y;
	result.data[2][2] *= scale.z;
	result.data[3][3] = 1;
	return result;
};

/*
	A point (x, y) can be written as (lcos(a), lsin(a)), for magnitude l and angle a.
	When we rotate this by angle b, we get (lcos(a + b), lsin(a + b))
	lcos(a + b) = lcos(a)cos(b) - lsin(a)sin(b)
	lsin(a + b) = lsin(a)cos(b) + lcos(a)sin(b)
	
	new x = xcos(b) - ysin(b)
	new y = ycos(b) + xsin(b)
*/

//create rotation matrix
//create x rotation matrix
Matrix4x4f createXRotationMatrix(double angle){
	/*
		Rotate about x axis
		Replace x axis with z
		[1 0       0      0]
		[0 cos(a)  sin(a) 0]
		[0 -sin(a) cos(a) 0]
		[0 0       0      1]
	*/
	double cosAngle = cos(angle);
	double sinAngle = sin(angle);
	
	Matrix4x4f result = createIdentityMatrix();
	result.data[1][1] = cosAngle;
	result.data[1][2] = sinAngle;
	result.data[2][1] = -sinAngle;
	result.data[2][2] = cosAngle;
	
	return result;
};

//create y rotation matrix
Matrix4x4f createYRotationMatrix(double angle){
	/*
		Rotate about y axis
		Replace y axis with z
		[cos(a) 0 sin(a) 0]
		[0      1 0       0]
		[-sin(a) 0 cos(a)  0]
		[0      0 0       1]
		
		Remember, we are treating the Z axis like the X axis
	*/
	double cosAngle = cos(angle);
	double sinAngle = sin(angle);
	
	Matrix4x4f result = createIdentityMatrix();
	result.data[0][0] = cosAngle;
	result.data[2][0] = -sinAngle;
	result.data[0][2] = sinAngle;
	result.data[2][2] = cosAngle;
	
	return result;
};

//create z rotation matrix
Matrix4x4f createZRotationMatrix(double angle){
	/*
		Rotate about z axis
		[cos(a) -sin(a) 0 0]
		[sin(a) cos(a)  0 0]
		[0      0       1 0]
		[0      0       0 1]
	*/
	double cosAngle = cos(angle);
	double sinAngle = sin(angle);
	
	Matrix4x4f result = createIdentityMatrix();
	result.data[0][0] = cosAngle;
	result.data[1][0] = sinAngle;
	result.data[0][1] = -sinAngle;
	result.data[1][1] = cosAngle;
	
	return result;
};

//create rotation matrix
Matrix4x4f createRotationMatrix(Vector4f rotation){
	
	/*
		Rotate about x-axis, then y-axis, then z-axis
		Z(YX)
	*/
	return matrixProduct(createZRotationMatrix(rotation.z), matrixProduct(createYRotationMatrix(rotation.y), matrixProduct(createXRotationMatrix(rotation.x), createIdentityMatrix())));
};

//create camera rotation matrix
Matrix4x4f createCameraRotationMatrix(Vector4f rotation){
	
	/*
		Rotate about y-axis, then x-axis, then z-axis
		Z(XY)
	*/
	return matrixProduct(createZRotationMatrix(rotation.z), matrixProduct(createXRotationMatrix(rotation.x), matrixProduct(createYRotationMatrix(rotation.y), createIdentityMatrix())));
};

//create transformation matrix
Matrix4x4f createTransformationMatrix(Vector4f enlargement, Vector4f rotation, Vector4f position){
	Matrix4x4f result = createEnlargementMatrix(enlargement);
	result = matrixProduct(createRotationMatrix(rotation), result);
	result.data[0][3] = position.x;
	result.data[1][3] = position.y;
	result.data[2][3] = position.z;
	return result;
};

//transform triangle
Triangle transformTriangle(Triangle triangle, Matrix4x4f transform){
	int i;
	
	for(i = 0; i < 3; i++){
		triangle.vertices[i] = matrixVectorProduct(transform, triangle.vertices[i]);
	};
	
	return triangle;
};

//get 2d line intersection
double get2dLineIntersection(double s1x, double s1y, double d1x, double d1y, double s2x, double s2y, double d2x, double d2y, int * doesIntersect){
	//check if both start at the same coordinate
	if(s1x == s2x && s1y == s2y){
		*doesIntersect = 1;
		return 0;
	} else if(d2x == 0){
		//if d2x is zero, then we should check to see if d1.x is zero
		if(d1x == 0){
			if(s1x == s2x){
				if(d1y == 0 && d2y == 0){
					//do not intersect
					*doesIntersect = 0;
					return 0;
				};
				
				//does intersect - lines have vertical components
				*doesIntersect = 1;
				return 0;
			};
			
			//do not intersect
			*doesIntersect = 0;
			return 0;
 		};

		//s1.x + k * d1.x = s2.x
		//(s2.x - s1.x) / d1.x = k
		*doesIntersect = 1;
		return (s2x - s1x) / d1x;
	};
	
	//an intersection will occur
	*doesIntersect = 1;
	return (s2y - s1y + (d2y / d2x) * (s1x - s2x)) / (d1y - (d2y / d2x) * d1x);
};

//get line plane intersection
double getLinePlaneIntersection(Vector4f s, Vector4f d, Vector4f r, Vector4f n){
	/*
		Plane:
		(p - r) . n = 0
		p . n - r . n = 0
		p . n = r . n
		
		Line
		p = s + k * d 
		
		(s + k * d) . n = r . n
		nx * (sx + k * dx) + ny * (sy + k * dy) + nz * (sz + k * dz) = r . n
		k(dx * nx + dy * ny + dz * nz) + nx * sx + ny * sy + nz * sz = r . n
		k = ((r.n) - (nx * sx + ny * sy + nz * sz)) / (dx * nx + dy * ny + dz * nz)
	*/
	if(d.x * n.x + d.y * n.y + d.z * n.z == 0){
		return 0;
	};
	return (dotProduct(r, n) - (n.x * s.x + n.y * s.y + n.z * s.z)) / (d.x * n.x + d.y * n.y + d.z * n.z);
};

//direction to angle
Vector4f directionToAngle(Vector4f d){
	/*
		Convert Pith and Yaw to direction vector;
		
		First rotate yaw
	*/
	
	//normalise direction
	d = normaliseVector(d);
	
	//reset angle
	Vector4f angle = {0};
	
	/*
		Imagine that the vector is created, starting with a unit vector in the Z direction.
		We then rotate around the x-axis, and then the y-axis. This will form two right-angled triangles.
		One is in the XZ plane, and one is in the YZ plane.
		
		For the one in the XZ plane, we can get the rotation around the y-axis:
		Opposite = x
		Adjacent = z
		Angle = atan2(x, z);
		
		For the one in the YZ plane, we can get the rotation around the x-axis:
		Opposite = y
		Adjacent = sqrt(x^2 + z^2)
		Angle = atan2(y, sqrt(x^2 + z^2))
	*/
	
	angle.y = atan2(d.x, d.z);
	angle.x = atan2(d.y, sqrt(d.x * d.x + d.z * d.z));
	
	
	/*
	//get angle
	angle.x = asin2(d.y);//tan2(d.z, sqrt(d.x * d.x + d.y * d.y)); //atan2 is between -PI and PI
	angle.y = asin2(d.x / (cos(angle.x)));
	angle.z = 0;//atan2(d.y, d.x);
	*/
	
	return angle;
};

//check basic entity collision
int checkBasicEntityCollision(Entity * e1, Entity * e2, double tolerance){
	//check for collision along either axis
	double distance = vectorMagnitude(subtractVectors(e2->position, e1->position));
	if(distance < tolerance * (e1->size.x + e2->size.x) / 2.f || distance < tolerance * (e1->size.y + e2->size.y) / 2.f || distance < tolerance * (e1->size.z + e2->size.z) / 2.f){
		return 1;
	};
	return 0;
};

//check advanced enemy collision
int checkAdvancedEntityCollision(Entity * e1, Entity * e2, double tolerance){
	//generate transformation matrix for e1 and e2
	//enlargement, rotation, position
	Matrix4x4f e1Transform = createTransformationMatrix(e1->model.scale, e1->model.rotation, e1->position);
	Matrix4x4f e2Transform = createTransformationMatrix(e2->model.scale, e2->model.rotation, e2->position);
	
	//get direction of e2
	Matrix4x4f e2Rotation = createRotationMatrix(e2->rotation);
	Vector4f xDirection = {0};
	xDirection.x = 1;
	Vector4f yDirection = {0};
	yDirection.y = 1;
	Vector4f zDirection = {0};
	zDirection.z = 1;
	
	xDirection = matrixVectorProduct(e2Rotation, xDirection);
	yDirection = matrixVectorProduct(e2Rotation, yDirection);
	zDirection = matrixVectorProduct(e2Rotation, zDirection);
	
	//iterate through vertices
	int i;
	int j;
	int k;
	int l;
	
	for(i = 0; i < e1->model.mesh->numberOfTriangles; i++){
		//transform triangle
		Triangle transformedTriangle = transformTriangle(e1->model.mesh->triangles[i], e1Transform);
		
		//iterate through vertices
		for(j = 0; j < 3; j++){
			//get vertex vector
			Vector4f vertexVector = subtractVectors(e2->position, transformedTriangle.vertices[j]);
			
			//get distances along each axis
			double xDistance = dotProduct(xDirection, vertexVector);
			double yDistance = dotProduct(yDirection, vertexVector);
			double zDistance = dotProduct(zDirection, vertexVector);
			
			//check if inside e2
			if(fabs(xDistance) < e2->size.x / 2 && fabs(yDistance) < e2->size.y / 2 && fabs(zDistance) < e2->size.z / 2){
				return 1;
			};
		};
	};
	
	return 0;
};

//check terrain collision
Entity * checkTerrainCollision(Entity * e){
	//the terrain collision checks perfectly against model vertices, therefore model transformations are used rather than entity ones
	//create transformation matrix
	Matrix4x4f transform = createTransformationMatrix(e->model.scale, e->model.rotation, e->position);
	
	//iterate through vertices
	int i;
	int j;
	int k;
	
	for(i = 0; i < e->model.mesh->numberOfTriangles; i++){
		//transform triangle
		Triangle transformedTriangle = transformTriangle(e->model.mesh->triangles[i], transform);
		
		for(j = 0; j < NUMBER_OF_ENTITIES; j++){
			//check if terrain is occupied
			if(terrain[j].alive == 1){
				for(k = 0; k < 3; k++){
					//check if any of the vertices are inside the block
					if((transformedTriangle.vertices[k].x > terrain[j].position.x - terrain[j].size.x / 2 && transformedTriangle.vertices[k].x < terrain[j].position.x + terrain[j].size.x / 2) && (transformedTriangle.vertices[k].y > terrain[j].position.y - terrain[j].size.y / 2 && transformedTriangle.vertices[k].y < terrain[j].position.y + terrain[j].size.y / 2) && (transformedTriangle.vertices[k].z > terrain[j].position.z - terrain[j].size.z / 2 && transformedTriangle.vertices[k].z < terrain[j].position.z + terrain[j].size.z / 2)){
						return &terrain[j];
					};
				};
			};
		};
	};
	
	return (Entity *) 0;
};

//standardise angle
double standardiseAngle(double a){
	//set angle to between PI and - PI
	while(a > PI){
		a -= 2 * PI;
	};
	
	while(a < -PI){
		a += 2 * PI;
	};
	
	return a;
};