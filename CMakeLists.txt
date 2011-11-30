##################################################
# Main build file for Teatree
##################################################
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(teatree)

# Add our custom CMake module dir
SET(CMAKE_MODULE_PATH ${teatree_SOURCE_DIR}/cmake)

SET(TEATREE_VERSION_MAJOR 0)
SET(TEATREE_VERSION_MINOR 1)
SET(TEATREE_VERSION "${TEATREE_VERSION_MAJOR}.${TEATREE_VERSION_MINOR}")

##################################################
# Internal includes, these must come before the
# located dependencies!
##################################################
INCLUDE_DIRECTORIES(src)

##################################################
# External dependencies
##################################################

SET(TEATREE_BOOST_LIBS
    chrono
    program_options
    serialization)

# Find boost
FIND_PACKAGE(Boost REQUIRED ${TEATREE_BOOST_LIBS})

# Find Eigen 3
FIND_PACKAGE(Eigen3 REQUIRED)

# Update the include and link directories
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR})

##################################################
# Build type
##################################################
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo)
ENDIF()

##################################################
# Optimisation
##################################################

# Auto-optimise for CPU arch for non-debug builds
IF(NOT CMAKE_BUILD_TYPE MATCHES Debug)
  INCLUDE(OptimizeForArchitecture)
  OptimizeForArchitecture()
  ADD_DEFINITIONS("-O3 -ffast-math")
# For debug builds enable checked iterators
ELSE()
  ADD_DEFINITIONS("-D_GLIBCXX_DEBUG")
ENDIF()

# For release builds remove all debugging info
IF(CMAKE_BUILD_TYPE MATCHES Release)
  ADD_DEFINITIONS("-DNDEBUG")
ENDIF()

ADD_DEFINITIONS("-W -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-sign-compare")

##################################################
# OpenMP
##################################################
OPTION(TEATREE_USE_OPENMP "Use OpenMP" TRUE)
IF(TEATREE_USE_OPENMP)
  FIND_PACKAGE(OpenMP)

  IF(OPENMP_FOUND)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

    # Hack required for linking OpenMP applications
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_C_FLAGS}")

    SET(TEATREE_ENABLE_OPENMP TRUE)
  ENDIF()
ENDIF()

##################################################
# Date & compiler info
##################################################
INCLUDE(Today)
TODAY(TEATREE_BUILD_DATE)

INCLUDE(GetGitRevisionDescription)
GET_GIT_HEAD_REVISION(GIT_REFSPEC TEATREE_GIT_SHA1)

##################################################
# Config.h
##################################################
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/config.h)

##################################################
# Source files
##################################################
SET(TEATREE_HEADERS
    src/mac/base.hpp
    src/mac/opening_angle.hpp
    src/efield/moments/moments_2d.hpp
    src/efield/moments/moments_3d.hpp
    src/efield/moments/recurse.hpp
    src/efield/base.hpp
    src/efield/plummer.hpp
    src/particle/moments/shift_2d.hpp
    src/particle/moments/shift_3d.hpp
    src/particle/moments/shift_recurse.hpp
    src/particle/moments/shift.hpp
    src/particle/moments/visitor.hpp
    src/particle/moments.hpp
    src/particle/particle.hpp
    src/particle/partition.hpp
    src/particle/pseudo_particle.hpp
    src/particle/pseudo_particle_visitor.hpp
    src/particle/typedefs.hpp
    src/pusher/base.hpp
    src/pusher/verlet.hpp
    src/simulation/options.hpp
    src/tree/branch.hpp
    src/tree/visitor.hpp
    src/utils/aligned_pool_allocator.hpp
    src/utils/eigen_addons.hpp
    src/utils/ipow.hpp
    src/utils/openmp.hpp
    src/utils/pool_object.hpp)

##################################################
# Executable
##################################################
ADD_EXECUTABLE(teatree src/teatree.cpp ${TEATREE_HEADERS})
TARGET_LINK_LIBRARIES(teatree ${Boost_LIBRARIES})

##################################################
# Unit tests
##################################################
INCLUDE(CTest)

# Integrate boost::test with CTest
INCLUDE(BoostTestTargets)

ADD_BOOST_TEST(tree
  SOURCES tests/tree.cpp
  TESTS construction2d construction3d)
ADD_BOOST_TEST(fields
  SOURCES tests/fields.cpp
  TESTS plummer2d pseudo2d)
ADD_BOOST_TEST(pusher
  SOURCES tests/pusher.cpp
  TESTS order)
ADD_BOOST_TEST(serialization
  SOURCES tests/serialization.cpp
  TESTS eigen simulation_options_t accel_eval_t pusher_verlet_t
  LIBRARIES ${Boost_LIBRARIES})
