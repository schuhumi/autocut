/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#include <math.h>
#include "VECTOR.h"

void VECTOR_ROTATE (type_VECTOR *vector, float radian)
{
	float xnew=0, ynew=0;
	xnew = (vector->x)*cos(radian)-(vector->y)*sin(radian);
	ynew = (vector->x)*sin(radian)+(vector->y)*cos(radian);
	vector->x = xnew;
	vector->y = ynew;
}

void VECTOR_MULTIPLY (type_VECTOR *vector, float scalar)
{
	vector->x *= scalar;
	vector->y *= scalar;
}

void VECTOR_NEWBASIS (type_VECTOR *vectorInOldBasis, type_VECTOR *XbaseVector, type_VECTOR *YbaseVector, type_VECTOR *newVector)
{
	float lamda1, lamda2;
	
	//vectorInOldBasis = lamda1*XbaseVector + lamda2*YbaseVector;
	//vectorInOldBasis->x = lamda1*(XbaseVector->x) + lamda2*(YbaseVector->x);
	//vectorInOldBasis->y = lamda1*(XbaseVector->y) + lamda2*(YbaseVector->y);
	
	lamda1 = (vectorInOldBasis->x - vectorInOldBasis->y)/(XbaseVector->x - XbaseVector->y);
	lamda2 = (vectorInOldBasis->x - lamda1*XbaseVector->x)/(YbaseVector->x);
	
	
	newVector->x = lamda1;
	newVector->y = lamda2;
}

void VECTOR_SETPOLAR (type_VECTOR *vector, float angle, float length)
{
	vector->x = cos(angle)*length;
	vector->y = sin(angle)*length;
}


float VECTOR_GETLENGTH (type_VECTOR *vector)
{
	return sqrt((vector->x)*(vector->x) + (vector->y)*(vector->y));
}

void VECTOR_SETLENGTH (type_VECTOR *vector, float length)
{
	float lamda, x, y;
	x = vector->x;
	y = vector->y;
	lamda = sqrt( (length*length) / ((x*x)+(y*y)));
	vector->x = x*lamda;
	vector->y = y*lamda;
}


float VECTOR_GETRADIAN (type_VECTOR *vector)
{
	
}

void VECTOR_FROMPOINTS (type_POINT *pointFrom, type_POINT *pointTo, type_VECTOR *vector)
{
	vector->x = pointTo->x - pointFrom->x;
	vector->y = pointTo->y - pointFrom->y;
}


void VECTOR_ADD (type_VECTOR *vector1, type_VECTOR *vector2, type_VECTOR *result)
{
	result->x = vector1->x + vector2->x;
	result->y = vector1->y + vector2->y;
}


void VECTOR_SUBSTRACT (type_VECTOR *vector1, type_VECTOR *vector2, type_VECTOR *result)
{
	result->x = vector1->x - vector2->x;
	result->y = vector1->y - vector2->y;
}



