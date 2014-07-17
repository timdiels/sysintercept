cmake_minimum_required(VERSION 2.6) 
project(sysintercept)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)

include_directories(windows_hdrs)

#########################
# Find libs on system

find_package(ICU REQUIRED COMPONENTS uc i18n io data lx)
include_directories(${ICU_INCLUDE_DIRS})

ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK)
#ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
#SET(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS log system regex locale filesystem thread_win32)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

find_package(CodeSynthesisXSD REQUIRED)
include_directories(${CODESYNTHESIS_XSD_INCLUDE_DIR})

find_package(XercesC REQUIRED)
include_directories(${XERCESC_INCLUDE_DIRS})

#############################
# Distorm
include_directories(distorm/include)
file(GLOB sources "distorm/src/*.c")
add_library(distorm STATIC ${sources})

#########################
# NCodeHook
include_directories(ncodehook)
file(GLOB sources "ncodehook/*.cpp")
add_library(ncodehook STATIC ${sources})

#########################
# NInjectLib
include_directories(ninjectlib)
file(GLOB sources "ninjectlib/*.cpp")
add_library(ninjectlib STATIC ${sources})

#########################
# SyscallInterceptorCommon
file(GLOB sources "SyscallInterceptorCommon/*.cpp")
add_library(sysinterceptCommon STATIC ${sources})
target_link_libraries(sysinterceptCommon ${ICU_LIBRARIES} ${Boost_LIBRARIES})

#####
#
include_directories(SyscallInterceptorCommon)
set(libraries sysinterceptCommon ${XERCESC_LIBRARIES})
	
#########################
# SyscallInterceptorDll
file(GLOB sources "SyscallInterceptorDll/*.cpp")
add_library(sysinterceptDll SHARED ${sources})
target_link_libraries(sysinterceptDll distorm ${libraries})

#########################
# SyscallInterceptor
file(GLOB sources "SyscallInterceptor/*.cpp")
add_executable(sysintercept ${sources})
target_link_libraries(sysintercept ncodehook ninjectlib ${libraries})

#add_subdirectory(SyscallInterceptorDll)
#add_subdirectory(SyscallInterceptor)