#include "support_measurement.h"

// Inlier Support Measurer
InlierSupportMeasurer::Support InlierSupportMeasurer::Evaluate(const std::vector<double>& residuals, const double max_residual) {
  
  Support support;
  
  support.num_inliers = 0;
  support.residual_sum = 0;

  for (const auto residual : residuals) {
    if (residual <= max_residual) {
      support.num_inliers += 1;
      support.residual_sum += residual;
    }
  }

  return support;
}

bool InlierSupportMeasurer::Compare(const Support& support1, const Support& support2) {
  
  if (support1.num_inliers > support2.num_inliers) {  return true;  } 
  else {  return  support1.num_inliers == support2.num_inliers       && 
                  support1.residual_sum < support2.residual_sum; 
        }
}

// MSAC Estimator Support Measurer
MEstimatorSupportMeasurer::Support MEstimatorSupportMeasurer::Evaluate(const std::vector<double>& residuals, const double max_residual) {
  
  Support support;
  support.num_inliers = 0;
  support.score = 0;

  for (const auto residual : residuals) {
    if (residual <= max_residual) {
      support.num_inliers += 1;
      support.score += residual;
    } 
    else {
      support.score += max_residual;
    }
  }

  return support;
}

bool MEstimatorSupportMeasurer::Compare(const Support& support1,
                                        const Support& support2) {
  return support1.score < support2.score;
}
