cmake_minimum_required(VERSION 2.6) 
project(sysintercept)

set(CMAKE_CXX_FLAGS /EHsc)

#########################
# Find libs on system
set(BOOST_LIBRARY_DIR C:/Boost/stage/lib)
find_package(Boost REQUIRED COMPONENTS)
message(${Boost_INCLUDE_DIRS})
message(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
#link_directories(${Boost_LIBRARY_DIRS})

#############################
# Include common things
include_directories(distorm ncodehook ninjectlib SyscallInterceptorCommon)
add_library(distorm STATIC IMPORTED)
set_target_properties(distorm PROPERTIES IMPORTED_LOCATION (${CMAKE_SOURCE_DIR}/distorm/))

#########################
# NCodeHook
file(GLOB sources "ncodehook/*.cpp")
add_library(ncodehook STATIC ${sources})

#########################
# NInjectLib
file(GLOB sources "ninjectlib/*.cpp")
add_library(ninjectlib STATIC ${sources})

#########################
# NInjectLib
file(GLOB sources "SyscallInterceptorCommon/*.cpp")
add_library(sysinterceptCommon STATIC ${sources})

#####
#

set(libraries
	distorm ${Boost_LIBRARIES})
	
#########################
# SyscallInterceptorDll
file(GLOB sources "SyscallInterceptorDll/*.cpp")
add_library(sysinterceptDll SHARED ${sources})
target_link_libraries(sysinterceptDll ${libraries})

#########################
# SyscallInterceptor
file(GLOB sources "SyscallInterceptor/*.cpp")
add_executable(sysintercept ${sources})
target_link_libraries(sysintercept ${libraries})



#add_subdirectory(SyscallInterceptorDll)
#add_subdirectory(SyscallInterceptor)