cmake_minimum_required(VERSION 3.8)
project(assignment1_rt)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(turtlesim REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(custom_interface REQUIRED)

add_executable(ui_node src/ui_node.cpp)
add_executable(distance src/distance_node.cpp)
ament_target_dependencies(ui_node rclcpp std_msgs custom_interface geometry_msgs)
ament_target_dependencies(distance rclcpp std_msgs geometry_msgs custom_interface turtlesim)



install(TARGETS
  ui_node 
  distance
  DESTINATION lib/${PROJECT_NAME}
)
ament_package()
