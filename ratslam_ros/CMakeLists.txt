cmake_minimum_required(VERSION 2.8.3)
project(ratslam_ros)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)


## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS message_generation std_msgs roscpp sensor_msgs nav_msgs tf visualization_msgs image_transport nav_msgs) ## actionlib_msgs)

find_package(OpenCV)
include_directories(${OpenCV_INCLUDE_DIRS})
find_package(Boost REQUIRED COMPONENTS serialization)
find_package(Irrlicht REQUIRED)
find_package(OpenGL REQUIRED)

# vtk
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

LINK_DIRECTORIES(/usr/lib/x86_64-linux-gnu)

## Generate messages in the 'msg' folder
add_message_files(
   FILES
   TopologicalAction.msg
   TopologicalEdge.msg
   TopologicalMap.msg
   TopologicalNode.msg
   ViewTemplate.msg
)

## Generate actions in the 'msg' folder
#add_action_files(DIRECTORY action FILES DoDishes.action)
#generate_messages(DEPENDENCIES actionlib_msgs)

## Generate added messages and services with any dependencies listed here
generate_messages(
   DEPENDENCIES
   geometry_msgs  std_msgs
)

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if you package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
CATKIN_DEPENDS message_runtime geometry_msgs std_msgs
)


###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
# include_directories(include)
include_directories(
  ${catkin_INCLUDE_DIRS}
)
include_directories(
/home/taiping/catkin_ws/src/ratslam_ros/src/graphics
)

#ratslam library
add_library(ratslam src/ratslam/experience_map.cpp src/ratslam/posecell_network.cpp src/ratslam/local_view_match.cpp src/ratslam/visual_odometry.cpp)

add_library(vtk_multiview src/graphics/local_view_vtk.cxx src/ratslam/local_view_match.cpp)
target_link_libraries(vtk_multiview  ${VTK_LIBRARIES})
# uncomment is you don't have irrlicht installed
add_definitions("-DHAVE_IRRLICHT")

add_executable(ratslam_lv MACOSX_BUNDLE src/main_lv.cpp)
target_link_libraries(ratslam_lv 
  ${catkin_LIBRARIES}
  vtk_multiview
  ratslam
  ${IRRLICHT_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${OpenCV_LIBRARIES}
  libXxf86vm.so.1)

add_executable(ratslam_pc src/main_pc.cpp)
target_link_libraries(ratslam_pc
  ${catkin_LIBRARIES} 
  ratslam
  ${IRRLICHT_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${OpenCV_LIBRARIES}
  libXxf86vm.so.1)

add_executable(ratslam_em src/main_em.cpp)
target_link_libraries(ratslam_em
  ${catkin_LIBRARIES}
  ratslam
  ${IRRLICHT_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${OpenCV_LIBRARIES}
  libXxf86vm.so.1)

add_executable(ratslam_vo src/main_vo.cpp)
target_link_libraries(ratslam_vo
  ${catkin_LIBRARIES}
  ratslam
  ${IRRLICHT_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${OpenCV_LIBRARIES}
  libXxf86vm.so.1)


#config files for devel
set(MEDIA_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/media) #devel use the files in the source dir
file(GLOB infiles "config/*.in")
foreach(infile ${infiles})
  #MESSAGE(STATUS "Process file: ${infile}")

  # Generate output file name
  string(REGEX REPLACE ".in\$" "" outfile "${infile}")
  get_filename_component(outfile "${outfile}" NAME)
  set(outfile "${CATKIN_DEVEL_PREFIX}/${CATKIN_PACKAGE_SHARE_DESTINATION}/config/${outfile}")
  #MESSAGE(STATUS "Output file: ${outfile}")

  configure_file("${infile}" "${outfile}")
endforeach()

#config files for install
set(MEDIA_PATH ${CMAKE_INSTALL_PREFIX}/${CATKIN_PACKAGE_SHARE_DESTINATION}/media) #install use the installed media files
file(GLOB infiles "config/*.in")
foreach(infile ${infiles})
  #MESSAGE(STATUS "Process file: ${infile}")

  # Generate output file name
  string(REGEX REPLACE ".in\$" "" outfile "${infile}")
  get_filename_component(outfile "${outfile}" NAME)
  set(outfile "${CMAKE_CURRENT_BINARY_DIR}/config/${outfile}")
  #MESSAGE(STATUS "Output file: ${outfile}")

  configure_file("${infile}" "${outfile}")
endforeach()

add_dependencies(ratslam_lv ratslam_ros_generate_messages_cpp)
add_dependencies(ratslam_pc ratslam_ros_generate_messages_cpp)
add_dependencies(ratslam_em ratslam_ros_generate_messages_cpp)
add_dependencies(ratslam_vo ratslam_ros_generate_messages_cpp)

install(TARGETS ratslam ratslam_lv ratslam_pc ratslam_em ratslam_vo
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})

install(DIRECTORY launch/
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/launch)

install(DIRECTORY src/media/
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/media)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/config/
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/config)
