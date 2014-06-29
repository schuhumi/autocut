#ifndef _VECTOR_h_
#define _VECTOR_h_

typedef struct {
	float x;
	float y;
} type_VECTOR;

#define type_POINT type_VECTOR

void VECTOR_ROTATE (type_VECTOR *vector, float radian);
void VECTOR_MULTIPLY (type_VECTOR *vector, float scalar);
void VECTOR_NEWBASIS (type_VECTOR *vectorInOldBasis, type_VECTOR *XbaseVector, type_VECTOR *YbaseVector, type_VECTOR *newVector);
void VECTOR_FROMPOINTS (type_POINT *pointFrom, type_POINT *pointTo, type_VECTOR *vector);
void VECTOR_SETPOLAR (type_VECTOR *vector, float angle, float length);
float VECTOR_GETLENGTH (type_VECTOR *vector);
void VECTOR_SETLENGTH (type_VECTOR *vector, float length);
float VECTOR_GETRADIAN (type_VECTOR *vector);
void VECTOR_ADD (type_VECTOR *vector1, type_VECTOR *vector2, type_VECTOR *result);
void VECTOR_SUBSTRACT (type_VECTOR *vector1, type_VECTOR *vector2, type_VECTOR *result);

#endif
