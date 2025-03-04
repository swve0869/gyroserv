cmake_minimum_required(VERSION 3.28.3)
project(gyroserv)

file(GLOB MY_SOURCES "src/*.cpp" "src/*.hpp" "src/*.h" "include/your-project/*.h" "include/your-project/*.hpp")
add_executable("${PROJECT_NAME}" ${MY_SOURCES})
target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_17)


find_package(Matplot++ REQUIRED)
target_link_libraries("${PROJECT_NAME}" Matplot++::matplot)
