cmake_minimum_required(VERSION 3.19)

set(_project_name "injector") 
project(${_project_name})

set(_src_root_path "${PROJECT_SOURCE_DIR}/src")
set(_header_root_path "${PROJECT_SOURCE_DIR}/include")
set(_target ${_project_name})

set(_sources_list 
"${_src_root_path}/main.cpp"
"${_src_root_path}/process_id_snooper.cpp"
"${_src_root_path}/process_injector.cpp"
"${_src_root_path}/load_library_r.cpp"
)
set(_headers_list
"${_header_root_path}/process_id_snooper.h"
"${_header_root_path}/process_injector.h"
"${_header_root_path}/load_library_r.h"
"${_header_root_path}/reflective_dll_injection.h"
)

file(TOUCH 
    "${CMAKE_BINARY_DIR}/generated/src/${_injection_binary_name}.cpp"
    "${CMAKE_BINARY_DIR}/generated/include/${_injection_binary_name}.h"
)

set(_generated_sources_list 
    "${CMAKE_BINARY_DIR}/generated/src/${_injection_binary_name}.cpp"
)

set(_generated_headers_list 
    "${CMAKE_BINARY_DIR}/generated/include/${_injection_binary_name}.h"
)

source_group(TREE ${_src_root_path} PREFIX "Source files" FILES ${_sources_list})
source_group(TREE ${_header_root_path} PREFIX "Header files" FILES ${_headers_list})
add_executable(${_target} ${_sources_list} ${_headers_list} ${_generated_sources_list} ${_generated_headers_list})

target_include_directories(${_target} PRIVATE ${_header_root_path} "${CMAKE_BINARY_DIR}/generated/include")
target_include_directories(${_target} PRIVATE "${CMAKE_SOURCE_DIR}/_3rd_party/")
target_compile_features(${_target} PUBLIC cxx_std_20)
target_compile_definitions(${_target} PUBLIC WIN_X64)

set_property(TARGET ${_target} PROPERTY RUNTIME_OUTPUT_NAME "${_target}")
set_property(TARGET ${_target} PROPERTY DEBUG_POSTFIX "D")
set_property(TARGET ${_target} PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<0:>")

add_dependencies(injector generate_injection_code)