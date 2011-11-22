#ifndef TEATREE_CONFIG_H
#define TEATREE_CONFIG_H

#ifdef EIGEN_CORE_H
# error "config.h must be included before Eigen"
#endif

#define EIGEN_ARRAYBASE_PLUGIN  "utils/eigen_addons.hpp"
#define EIGEN_MATRIXBASE_PLUGIN "utils/eigen_addons.hpp"

#define TEATREE_BUILD_DATE "@TEATREE_BUILD_DATE@"

#define TEATREE_VERSION_MAJOR @TEATREE_VERSION_MAJOR@

#define TEATREE_VERSION_MINOR @TEATREE_VERSION_MINOR@

#define TEATREE_VERSION "@TEATREE_VERSION@"

#cmakedefine TEATREE_ENABLE_OPENMP

#endif // TEATREE_CONFIG_H
