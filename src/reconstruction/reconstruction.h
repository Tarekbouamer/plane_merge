
#ifndef RECONSTRUCTION_H_
#define RECONSTRUCTION_H_

// #include <unordered_map>
// #include <unordered_set>
// #include <vector>

// #include <eigen3/Eigen/Core>

// #include "camera.h"
// #include "database.h"
// #include "image.h"
// #include "types.h"



// class Reconstruction {
//  public:

//   Reconstruction();

//   // Get number of objects.
//   inline size_t NumCameras() const;
//   inline size_t NumImages() const;
//   inline size_t NumRegImages() const;
//   inline size_t NumPoints3D() const;
//   inline size_t NumImagePairs() const;

//   // Get const objects.
//   inline const class Camera& Camera(const camera_t camera_id) const;
//   inline const class Image& Image(const image_t image_id) const;
//   inline const class Point3D& Point3D(const point3D_t point3D_id) const;

//   // Get mutable objects.
//   inline class Camera& Camera(const camera_t camera_id);
//   inline class Image& Image(const image_t image_id);
//   inline class Point3D& Point3D(const point3D_t point3D_id);

//   // Get reference to all objects.
//   inline const EIGEN_STL_UMAP(camera_t, class Camera) & Cameras() const;
//   inline const EIGEN_STL_UMAP(image_t, class Image) & Images() const;  
//   inline const EIGEN_STL_UMAP(point3D_t, class Point3D) & Points3D() const;


//   // Identifiers of all 3D points.
//   std::unordered_set<point3D_t> Point3DIds() const;

//   // Check whether specific object exists.
//   inline bool ExistsCamera(const camera_t camera_id) const;
//   inline bool ExistsImage(const image_t image_id) const;
//   inline bool ExistsPoint3D(const point3D_t point3D_id) const;

//   // Load data from given `DatabaseCache`.
//   void Load(const Database& database);

//   // Setup all relevant data structures before reconstruction. Note the
//   // correspondence graph object must live until `TearDown` is called.
//   //void SetUp(const CorrespondenceGraph* correspondence_graph);

//   // Finalize the Reconstruction after the reconstruction has finished.
//   //
//   // Once a scene has been finalized, it cannot be used for reconstruction.
//   //
//   // This removes all not yet registered images and unused cameras, in order to
//   // save memory.
//   void TearDown();

//   // Add new camera. There is only one camera per image, while multiple images
//   // might be taken by the same camera.
//   void AddCamera(const class Camera& camera);

//   // Add new image.
//   void AddImage(const class Image& image);

//   // Add new 3D object, and return its unique ID.
//   //point3D_t AddPoint3D( const Eigen::Vector3d& xyz, const Track& track,
//   //                      const Eigen::Vector3ub& color = Eigen::Vector3ub::Zero());

//   // Normalize scene by scaling and translation to avoid degenerate
//   // visualization after bundle adjustment and to improve numerical
//   // stability of algorithms.
//   //
//   // Translates scene such that the mean of the camera centers or point
//   // locations are at the origin of the coordinate system.
//   //
//   // Scales scene such that the minimum and maximum camera centers are at the
//   // given `extent`, whereas `p0` and `p1` determine the minimum and
//   // maximum percentiles of the camera centers considered.
//   void Normalize(const double extent = 10.0, const double p0 = 0.1,
//                  const double p1 = 0.9, const bool use_images = true);

//   // Find specific image by name. Note that this uses linear search.
//   const class Image* FindImageWithName(const std::string& name) const;


//   // Read data from text or binary file. Prefer binary data if it exists.
//   void Read(const std::string& path);
//   void Write(const std::string& path) const;

//   // Read data from binary/text file.
//   void ReadText(const std::string& path);
//   void ReadBinary(const std::string& path);

//   // Write data from binary/text file.
//   void WriteText(const std::string& path) const;
//   void WriteBinary(const std::string& path) const;


//  private:
//   void ReadCamerasText(const std::string& path);
//   void ReadImagesText(const std::string& path);
//   void ReadPoints3DText(const std::string& path);
  
//   void ReadCamerasBinary(const std::string& path);
//   void ReadImagesBinary(const std::string& path);
//   void ReadPoints3DBinary(const std::string& path);

//   void WriteCamerasText(const std::string& path) const;
//   void WriteImagesText(const std::string& path) const;
//   void WritePoints3DText(const std::string& path) const;
  
//   void WriteCamerasBinary(const std::string& path) const;
//   void WriteImagesBinary(const std::string& path) const;
//   void WritePoints3DBinary(const std::string& path) const;

//   //const CorrespondenceGraph* correspondence_graph_;

//   EIGEN_STL_UMAP(camera_t,  class Camera  )     cameras_;
//   EIGEN_STL_UMAP(image_t,   class Image   )     images_;
//   EIGEN_STL_UMAP(point3D_t, class Point3D )     points3D_;

// };

// ////////////////////////////////////////////////////////////////////////////////
// // Implementation
// ////////////////////////////////////////////////////////////////////////////////

// size_t Reconstruction::NumCameras() const { return cameras_.size(); }

// size_t Reconstruction::NumImages() const { return images_.size(); }

// size_t Reconstruction::NumPoints3D() const { return points3D_.size(); }


// const class Camera& Reconstruction::Camera(const camera_t camera_id) const {
//   return cameras_.at(camera_id);
// }

// const class Image& Reconstruction::Image(const image_t image_id) const {
//   return images_.at(image_id);
// }

// const class Point3D& Reconstruction::Point3D(const point3D_t point3D_id) const {
//   return points3D_.at(point3D_id);
// }


// const EIGEN_STL_UMAP(camera_t, Camera) & Reconstruction::Cameras() const {
//   return cameras_;
// }

// const EIGEN_STL_UMAP(image_t, class Image) & Reconstruction::Images() const {
//   return images_;
// }

// const EIGEN_STL_UMAP(point3D_t, Point3D) & Reconstruction::Points3D() const {
//   return points3D_;
// }

// bool Reconstruction::ExistsCamera(const camera_t camera_id) const {
//   return cameras_.find(camera_id) != cameras_.end();
// }

// bool Reconstruction::ExistsImage(const image_t image_id) const {
//   return images_.find(image_id) != images_.end();
// }

// bool Reconstruction::ExistsPoint3D(const point3D_t point3D_id) const {
//   return points3D_.find(point3D_id) != points3D_.end();
// }

#endif  // RECONSTRUCTION_H_
