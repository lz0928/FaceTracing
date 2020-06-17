
cmake_minimum_required(VERSION 3.4.1)


add_library( # Sets the name of the library.
             native-lib

             SHARED

             native-lib.cpp )

include_directories(include)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/../jniLibs/armeabi-v7a")
message("CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}")


find_library( # Sets the name of the path variable.
              log-lib

              log )

# Specifies libraries CMake should link to yyies, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib

                       ${log-lib} opencv_java3 android)