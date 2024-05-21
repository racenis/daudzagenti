// VECTOR.C - libgoetia
// Vector, shape function implementation.

#include <stdio.h>
#include <math.h>

#include <vector.h>

#include <value.h>

/// Calculates vector componentwise addition.
Vector3 goetia_vec3_add(Vector3 vec1, Vector3 vec2) {
	vec1.x += vec2.x;
	vec1.y += vec2.y;
	vec1.z += vec2.z;
	
	return vec1;
}

/// Calculates vector componentwise subtraction.
Vector3 goetia_vec3_sub(Vector3 vec1, Vector3 vec2) {
	vec1.x -= vec2.x;
	vec1.y -= vec2.y;
	vec1.z -= vec2.z;
	
	return vec1;
}

/// Calculates vector componentwise multiplication.
Vector3 goetia_vec3_mul(Vector3 vec1, Vector3 vec2) {
	vec1.x *= vec2.x;
	vec1.y *= vec2.y;
	vec1.z *= vec2.z;
	
	return vec1;
}

/// Calculates vector componentwise division.
Vector3 goetia_vec3_div(Vector3 vec1, Vector3 vec2) {
	vec1.x /= vec2.x;
	vec1.y /= vec2.y;
	vec1.z /= vec2.z;
	
	return vec1;
}

/// Calculates vector dot product.
float goetia_vec3_dot(Vector3 vec1, Vector3 vec2) {
	vec1 = goetia_vec3_mul(vec1, vec2);
	return vec1.x + vec1.y + vec1.z;
}

/// Calculates vector cross product.
Vector3 goetia_vec3_crs(Vector3 vec1, Vector3 vec2) {
	Vector3 vec;
	vec.x = vec1.y * vec2.z - vec1.z * vec2.y;
    vec.y = vec1.x * vec2.z - vec1.z * vec2.x;
    vec.z = vec1.x * vec2.y - vec1.y * vec2.x;
	return vec;
}

/// Calculates length of a vector.
float goetia_vec3_len(Vector3 vec1) {
	return sqrtf(goetia_vec3_dot(vec1, vec1));
}

/// Normalizes a vector.
Vector3 goetia_vec3_nrm(Vector3 vec1) {
	float len = goetia_vec3_len(vec1);
	Vector3 div = {len, len, len};
	return goetia_vec3_div(vec1, div);
}

/// Calculates distance between two points.
float goetia_vec3_dst(Vector3 vec1, Vector3 vec2) {
	return goetia_vec3_len(goetia_vec3_sub(vec1, vec2));
}

/// Sanitizes a vector.
/// If any component of the vector is either a nan or an inf, then returns a
/// vector with all components set to 0.0f. Otherwise returns the same vector.
/// @return Either the input vector or a null vector.
Vector3 goetia_vec3_san(Vector3 vec) {
	if (isnan(vec.x) || isinf(vec.x) || 
		isnan(vec.y) || isinf(vec.y) || 
		isnan(vec.z) || isinf(vec.z)
	) {
		return goetia_vec3_scl(0.0f);
	}
	
	return vec;
}

/// Creates a vector from a scalar.
/// If passed scalar x as a parameter, then will create a vector (x, x, x).
/// @return Vector from the scalar.
Vector3 goetia_vec3_scl(float scalar) {
	Vector3 vec = {scalar, scalar, scalar};
	return vec;
}

/// Prints vector to console.
void goetia_vec3_print(Vector3 vec) {
	printf("[%.2f, %.2f, %.2f]", vec.x, vec.y, vec.z);
}

/// Converts shape enum to an atom.
atom goetia_shape_ToAtom(int shape) {
	switch (shape) {
		default:
		case SHAPE_NONE:		return ATOM_NULL;
		case SHAPE_SPHERE:		return ATOM_SPHERE;
		case SHAPE_CYLINDER:	return ATOM_CYLINDER;
		case SHAPE_CUBE:		return ATOM_CUBE;
	}
}

/// Converts an atom to a shape enum.
int goetia_shape_FromAtom(atom atom) {
	switch (atom) {
		default:
		case ATOM_NULL:			return SHAPE_NONE;
		case ATOM_SPHERE:		return SHAPE_SPHERE;
		case ATOM_CYLINDER:		return SHAPE_CYLINDER;
		case ATOM_CUBE:			return SHAPE_CUBE;
	}
}