cmake_minimum_required(VERSION 2.8)
project(EAVLAB)

#-----------------------------------------------------------------------------
# Run all the CMake configuration tests for our build environment
#-----------------------------------------------------------------------------
#INCLUDE (cmake/ConfigureChecks.cmake)
#SET(CMAKE_MODULE_PATH ${EAVL_SOURCE_DIR}/cmake)

#-----------------------------------------------------------------------------
# Find EAVL
#-----------------------------------------------------------------------------
find_package(EAVL NO_MODULE)
include_directories(${EAVL_INCLUDE_DIR})

#-----------------------------------------------------------------------------
# Find Qt4
#-----------------------------------------------------------------------------
SET(QT_USE_QTOPENGL TRUE)
find_package(Qt4 REQUIRED)
INCLUDE( ${QT_USE_FILE} )
ADD_DEFINITIONS(${QT_DEFINITIONS})

#-----------------------------------------------------------------------------
# Find OpenGL
#-----------------------------------------------------------------------------
find_package(OpenGL)
include_directories( ${OPENGL_INCLUDE_DIRS})

#-----------------------------------------------------------------------------
# Find Boost
#-----------------------------------------------------------------------------
option(Boost_USE_STATIC_LIBS "Use static Boost libraries" OFF)
SET(Boost_ADDITIONAL_VERSIONS "1.48" "1.48.0" "1.49" "1.49.0")
SET(Boost_DEBUG ON)
find_package(Boost 1.48 REQUIRED)
link_directories(${Boost_LIBRARY_DIRS})

#-----------------------------------------------------------------------------
# Find CUDA
#-----------------------------------------------------------------------------
option (BUILD_CUDA "Build CUDA support" ON)
IF (BUILD_CUDA)
  find_package(CUDA)
  IF (CUDA_FOUND)
    SET(HAVE_CUDA 1)
    include_directories(${CUDA_INCLUDE_DIRS})
  ENDIF (CUDA_FOUND)

  option (CUDA_OLDGPU "Build CUDA : support compute capability pre-2.0 GPUs" OFF)
  IF (CUDA_OLDGPU) 
    SET(HAVE_OLD_GPU 1)
    SET(CUDA_NVCC_FLAGS_USER "--gpu-architecture compute_13" CACHE STRING "CUDA NVCC compile flags")
    SET(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS_USER} CACHE INTERNAL "CUDA NVCC compile flags" FORCE)
    MESSAGE("Pre compute 2 GPUs are not fully supported. Not all functions have been implemented and some link errors may occur")
  ELSE (CUDA_OLDGPU)
    IF(NOT WIN32)
      SET(CUDA_NVCC_FLAGS_USER "-gencode=arch=compute_20,code=sm_20 -gencode=arch=compute_20,code=compute_20" CACHE STRING "CUDA NVCC compile flags")
    ELSE (NOT WIN32)
      SET(CUDA_NVCC_FLAGS_USER "--gpu-architecture=compute_20" CACHE STRING "CUDA NVCC compile flags")
    ENDIF (NOT WIN32)
    SET(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS_USER} CACHE INTERNAL "CUDA NVCC compile flags" FORCE)
  ENDIF (CUDA_OLDGPU) 

ENDIF (BUILD_CUDA)

#-----------------------------------------------------------------------------
# Include directories
#-----------------------------------------------------------------------------
include_directories(
  ${EAVLAB_BINARY_DIR}
  ${EAVLAB_SOURCE_DIR}
)
IF (HAVE_CUDA)
  include_directories(
    "${CUDA_INCLUDE_DIRS}"
    "${EAVL_SOURCE_DIR}/src/operations"
  )
ENDIF (HAVE_CUDA)

#-----------------------------------------------------------------------------
# setup src
#-----------------------------------------------------------------------------
set(EAVLAB_QT_HDR
  Attribute.h
  EL1DWindow.h
  EL2DWindow.h
  EL3DWindow.h
  ELAttributeControl.h
  ELBasicInfoWindow.h
  ELEmptyWindow.h
  ELMainWindow.h
  ELPipelineBuilder.h
  ELPolarWindow.h
  ELSources.h
  ELWindowFrame.h
  ELWindowManager.h
  ELSurfacePlotSettings.h
  ELPlotList.h
)

set(EAVLAB_QT_SRC
  Attribute.cpp
  EL1DWindow.cpp
  EL2DWindow.cpp
  EL3DWindow.cpp
  ELAttributeControl.cpp
  ELBasicInfoWindow.cpp
  ELEmptyWindow.cpp
  ELMainWindow.cpp
  ELPipelineBuilder.cpp
  ELPolarWindow.cpp
  ELSources.cpp
  ELWindowFrame.cpp
  ELWindowManager.cpp
  Pipeline.cpp
  XMLTools.cpp
)

set(EAVLAB_SRC
  Pipeline.cpp
  XMLTools.cpp
)

#--------------------------------------------------
# make sure all *.h files for ui are wrapped by moc
#--------------------------------------------------
QT4_WRAP_CPP(
  EAVLAB_MOC_SRCS 
    ${EAVLAB_QT_HDR}
)

#add_library(EAVLab STATIC ${EAVLAB_SRC} ${EAVLAB_QT_SRC} ${EAVLAB_MOC_SRCS})
#target_link_libraries(EAVLab  
#  eavl_math
#  eavl_common
#  eavl_filters
#  eavl_exporters
#  eavl_importers
#  eavl_rendering
#  ${QT_LIBRARIES}
#  ${QT_QTOPENGL_LIBRARY}
#  ${OPENGL_LIBRARIES}
#)

add_executable(EAVL-GUI
  main.cpp
  ${EAVLAB_SRC} ${EAVLAB_QT_SRC} ${EAVLAB_MOC_SRCS}
)
target_link_libraries(EAVL-GUI
  eavl_math
  eavl_common
  eavl_filters
  eavl_fonts
  eavl_exporters
  eavl_importers
  eavl_rendering
  ${QT_LIBRARIES}
  ${QT_QTOPENGL_LIBRARY}
  ${OPENGL_LIBRARIES}
)

#-----------------------------------------------------------------------------
# Generate the eavlConfig.h file containing settings needed by compilation
#-----------------------------------------------------------------------------
#CONFIGURE_FILE (cmake/eavlConfig.h.in ${EAVL_BINARY_DIR}/eavlConfig.h @ONLY)

#-----------------------------------------------------------------------------
# Testing
#-----------------------------------------------------------------------------
option(BUILD_TESTING "Enable testing" ON)
IF(BUILD_TESTING)
  enable_testing()
  #add_subdirectory(test)
ENDIF(BUILD_TESTING)

