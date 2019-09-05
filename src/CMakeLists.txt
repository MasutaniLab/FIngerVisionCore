set(CMAKE_CXX_STANDARD 11)
set(CORE_DIR ${CMAKE_SOURCE_DIR}/fv_core)
set(UTIL_DIR ${CMAKE_SOURCE_DIR}/ay_vision)
set(CPP_DIR ${CMAKE_SOURCE_DIR}/ay_cpp)
set(comp_srcs FingerVisionCore.cpp FingerVisionSVC_impl.cpp
  ${CORE_DIR}/blob_tracker2.cpp ${CORE_DIR}/prox_vision.cpp ${UTIL_DIR}/vision_util.cpp
  ${CORE_DIR}/blob_tracker2.h ${CORE_DIR}/prox_vision.h ${UTIL_DIR}/vision_util.h
  ${CPP_DIR}/gettimeofday.cpp)
set(standalone_srcs FingerVisionCoreComp.cpp)

#Boost
set(BOOST_ALL_NO_LIB)
set(boost_packages thread)
find_package(Boost REQUIRED COMPONENTS ${boost_packages})
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

#OpenCV
find_package(OpenCV 2 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Eigen
if(NOT EIGEN_DIR)
  if(UNIX)
    include(FindPkgConfig)
    pkg_check_modules(EIGEN eigen3)
    if(EIGEN_FOUND)
      set(EIGEN_DIR ${EIGEN_INCLUDE_DIRS})
    endif()
  endif()
endif()
set(EIGEN_DIR ${EIGEN_DIR} CACHE PATH "The directory of the Eigen library")
if(NOT EIGEN_DIR)
  message(FATAL_ERROR "Please specify the Eigen directory to EIGEN_DIR.")
else()
  set(EIGEN_INCLUDE_DIRS ${EIGEN_DIR})
  include_directories(${EIGEN_INCLUDE_DIRS})
endif()

if(${OPENRTM_VERSION_MAJOR} LESS 2)
  set(OPENRTM_CFLAGS ${OPENRTM_CFLAGS} ${OMNIORB_CFLAGS})
  set(OPENRTM_INCLUDE_DIRS ${OPENRTM_INCLUDE_DIRS} ${OMNIORB_INCLUDE_DIRS})
  set(OPENRTM_LIBRARY_DIRS ${OPENRTM_LIBRARY_DIRS} ${OMNIORB_LIBRARY_DIRS})
endif()

if (DEFINED OPENRTM_INCLUDE_DIRS)
  string(REGEX REPLACE "-I" ";"
    OPENRTM_INCLUDE_DIRS "${OPENRTM_INCLUDE_DIRS}")
  string(REGEX REPLACE " ;" ";"
    OPENRTM_INCLUDE_DIRS "${OPENRTM_INCLUDE_DIRS}")
endif (DEFINED OPENRTM_INCLUDE_DIRS)

if (DEFINED OPENRTM_LIBRARY_DIRS)
  string(REGEX REPLACE "-L" ";"
    OPENRTM_LIBRARY_DIRS "${OPENRTM_LIBRARY_DIRS}")
  string(REGEX REPLACE " ;" ";"
    OPENRTM_LIBRARY_DIRS "${OPENRTM_LIBRARY_DIRS}")
endif (DEFINED OPENRTM_LIBRARY_DIRS)

if (DEFINED OPENRTM_LIBRARIES)
  string(REGEX REPLACE "-l" ";"
    OPENRTM_LIBRARIES "${OPENRTM_LIBRARIES}")
  string(REGEX REPLACE " ;" ";"
    OPENRTM_LIBRARIES "${OPENRTM_LIBRARIES}")
endif (DEFINED OPENRTM_LIBRARIES)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME})
include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_BINARY_DIR}/idl)
include_directories(${OPENRTM_INCLUDE_DIRS})
include_directories(${CORE_DIR})
include_directories(${UTIL_DIR}/..)

add_definitions(${OPENRTM_CFLAGS})

MAP_ADD_STR(comp_hdrs "../" comp_headers)

link_directories(${OPENRTM_LIBRARY_DIRS})

add_library(${PROJECT_NAME} ${LIB_TYPE} ${comp_srcs}
  ${comp_headers} ${ALL_IDL_SRCS})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
set_source_files_properties(${ALL_IDL_SRCS} PROPERTIES GENERATED 1)
if(NOT TARGET ALL_IDL_TGT)
 add_custom_target(ALL_IDL_TGT)
endif(NOT TARGET ALL_IDL_TGT)
add_dependencies(${PROJECT_NAME} ALL_IDL_TGT)
target_link_libraries(${PROJECT_NAME} ${OPENRTM_LIBRARIES}
  ${OpenCV_LIBS} Boost::thread Boost::disable_autolinking)

add_executable(${PROJECT_NAME}Comp ${standalone_srcs}
  ${comp_srcs} ${comp_headers} ${ALL_IDL_SRCS})
add_dependencies(${PROJECT_NAME}Comp ALL_IDL_TGT)
target_link_libraries(${PROJECT_NAME}Comp ${OPENRTM_LIBRARIES}
  ${OpenCV_LIBS} Boost::thread Boost::disable_autolinking)

install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}Comp
    EXPORT ${PROJECT_NAME}
    RUNTIME DESTINATION ${INSTALL_PREFIX} COMPONENT component
    LIBRARY DESTINATION ${INSTALL_PREFIX} COMPONENT component
    ARCHIVE DESTINATION ${INSTALL_PREFIX} COMPONENT component)

install(FILES ${PROJECT_SOURCE_DIR}/RTC.xml DESTINATION ${INSTALL_PREFIX}
        COMPONENT component)
