#ifndef TYPES_H_
#define TYPES_H_

#include <eigen3/Eigen/Core>

namespace Eigen {
  typedef Eigen::Matrix<float,    3, 4>   Matrix3x4f  ;
  typedef Eigen::Matrix<double,   3, 4>   Matrix3x4d  ;
  typedef Eigen::Matrix<uint8_t,  3, 1>   Vector3ub   ;
  typedef Eigen::Matrix<uint8_t,  4, 1>   Vector4ub   ;
  typedef Eigen::Matrix<double,   6, 1>   Vector6d    ;
}
// Unique identifier for cameras.
typedef uint32_t camera_t;

// Unique identifier for images.
typedef uint32_t image_t;

// Each image pair gets a unique ID, see `Database::ImagePairToPairId`.
typedef uint64_t image_pair_t;

// Index per image, i.e. determines maximum number of 2D points per image.
typedef uint32_t point2D_t;

// Unique identifier per added 3D point. Since we add many 3D points,
// delete them, and possibly re-add them again, the maximum number of allowed
// unique indices should be large.
typedef uint64_t point3D_t;

// Values for invalid identifiers or indices.
const camera_t kInvalidCameraId = std::numeric_limits<camera_t>::max();
const image_t kInvalidImageId = std::numeric_limits<image_t>::max();
const image_pair_t kInvalidImagePairId =std::numeric_limits<image_pair_t>::max();
const point2D_t kInvalidPoint2DIdx = std::numeric_limits<point2D_t>::max();
const point3D_t kInvalidPoint3DId = std::numeric_limits<point3D_t>::max();


#define EIGEN_STL_UMAP(KEY, VALUE)                                      \
    std::unordered_map<KEY, VALUE, std::hash<KEY>, std::equal_to<KEY>,  \
    Eigen::aligned_allocator<std::pair<KEY const, VALUE>>>

#endif  // TYPES_H_
