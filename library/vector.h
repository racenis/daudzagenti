// VECTOR.H - libgoetia
// Vector, shape, function definitions.

#ifndef VECTOR_H
#define VECTOR_H

typedef int atom;

enum {
	SHAPE_NONE,
	SHAPE_SPHERE,
	SHAPE_CYLINDER,
	SHAPE_CUBE
};

typedef struct Vector3 {
	float x;
	float y;
	float z;
} Vector3;

typedef struct Shape {
	int type;		// SHAPE_SPHERE or whatever
	float radius;
	float height;
} Shape;

Vector3 goetia_vec3_add(Vector3 vec1, Vector3 vec2);
Vector3 goetia_vec3_sub(Vector3 vec1, Vector3 vec2);
Vector3 goetia_vec3_mul(Vector3 vec1, Vector3 vec2);
Vector3 goetia_vec3_div(Vector3 vec1, Vector3 vec2);
float goetia_vec3_dot(Vector3 vec1, Vector3 vec2);
Vector3 goetia_vec3_crs(Vector3 vec1, Vector3 vec2);
float goetia_vec3_len(Vector3 vec1);
Vector3 goetia_vec3_nrm(Vector3 vec1);
float goetia_vec3_dst(Vector3 vec1, Vector3 vec2);
Vector3 goetia_vec3_san(Vector3 vec);
Vector3 goetia_vec3_scl(float scalar);

void goetia_vec3_print(Vector3 vec);

atom goetia_shape_ToAtom(int shape);
int goetia_shape_FromAtom(atom atom);

#endif // VECTOR_H