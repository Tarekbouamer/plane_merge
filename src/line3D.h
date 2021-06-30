#ifndef LINE3D_H_
#define LINE3D_H_

#include <eigen3/Eigen/Dense>

class Line3D {

public:
	Line3D()=default;

	Line3D(const Eigen::Vector3f p1, const Eigen::Vector3f p2);

  const Eigen::Vector3f GetBase() const;
  const Eigen::Vector3f GetDirection() const;

private:
  Eigen::Vector3f _base;
	Eigen::Vector3f _direction;
};

#endif //LINE3D_H_
