#include <iostream>

#include "line3D.h"

Line3D::Line3D(const Eigen::Vector3f p0, const Eigen::Vector3f p1): _base(p0), _direction(p1) {};

const Eigen::Vector3f Line3D::GetBase() const { return _base; }
const Eigen::Vector3f Line3D::GetDirection() const { return _direction; }
