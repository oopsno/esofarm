cmake_minimum_required(VERSION 3.5)
project(brainf)

set(CMAKE_CXX_STANDARD 14)

# Config LLVM using llvm-config
macro(get_llvm_config FLAG VAR)
    execute_process(COMMAND "llvm-config" "--${FLAG}"
            OUTPUT_VARIABLE ${VAR})
    string(REGEX REPLACE "\n" "" ${VAR} ${${VAR}})
    string(STRIP ${${VAR}} ${VAR})
endmacro(get_llvm_config)

get_llvm_config(obj-root BR_LLVM_ROOT)

list(APPEND CMAKE_PREFIX_PATH "${BR_LLVM_ROOT}/share/llvm")

find_package(LLVM 3.8 REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(include ${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs all)

add_library(bfparser src/parser.cpp src/operator.cpp)
add_executable(brainf main.cpp)
target_link_libraries(brainf bfparser)
