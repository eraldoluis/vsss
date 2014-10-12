//[]------------------------------------------------------------------------[]
//|                                                                          |
//|                        Small Size League software                        |
//|                             Version 1.0                                  |
//|                     Laboratório de Inteligencia Artificial				 |
//| 				 Universidade Federal de Mato Grosso do Sul              |
//|					 Author: Bruno H. Gouveia, Yuri Claure					 |
//|																			 |
//[]------------------------------------------------------------------------[]
//
//  OVERVIEW: Vector.h
//  ========
//  Class definition for vector 2D.

#ifndef VECTOR_H_
#define VECTOR_H_

#include <opencv2/opencv.hpp>

//////////////////////////////////////////////////////////
//
// Vector2D: vector 2D class
// ==========
class Vector2D: public cv::Point {
public:
	Vector2D();
	Vector2D(float x, float y);
	virtual ~Vector2D();

	void normalize(float norm);

	float norm();

	Vector2D rotateLeft(float angleRad = 1.57079633);

	Vector2D rotateRight(float angleRad = 1.57079633);

	void set(Vector2D & vector);

	float operator*(Vector2D & vector);

	double findAngle();

	static float angle(Vector2D & vec1, Vector2D & vec2);

private:
	static float angleDiff(float angle1, float angle2);
};

#endif /* VECTOR_H_ */
