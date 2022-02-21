cmake_minimum_required (VERSION 3.19)

set(_project_name "present_hook")
project(${_project_name})

set(_src_root_path "${PROJECT_SOURCE_DIR}/src")
set(_header_root_path "${PROJECT_SOURCE_DIR}/include")
set(_target ${_project_name})

set(_sources_list 
"${_src_root_path}/dllmain.cpp"
"${_src_root_path}/reflective_loader.cpp"
"${_src_root_path}/d3d11_hook.cpp"
"${_src_root_path}/imgui.cpp"
"${_src_root_path}/imgui_demo.cpp"
"${_src_root_path}/imgui_draw.cpp"
"${_src_root_path}/imgui_tables.cpp"
"${_src_root_path}/imgui_widgets.cpp"
"${_src_root_path}/overlay.cpp"
)

set(_headers_list
"${_header_root_path}/reflective_loader.h"
"${_header_root_path}/reflective_dll_injection.h"
"${_header_root_path}/d3d11_hook.h"
"${_header_root_path}/vtbls.h"
"${_header_root_path}/imconfig.h"
"${_header_root_path}/imgui.h"
"${_header_root_path}/imgui_internal.h"
"${_header_root_path}/imstb_textedit.h"
"${_header_root_path}/imstb_rectpack.h"
"${_header_root_path}/imstb_truetype.h"
"${_header_root_path}/overlay.h"
)


source_group(TREE ${_src_root_path} PREFIX "Source files" FILES ${_sources_list})
source_group(TREE ${_header_root_path} PREFIX "Header files" FILES ${_headers_list})
add_library(${_target} MODULE ${_sources_list} ${_headers_list})
set_property(TARGET ${_target} PROPERTY LIBRARY_OUTPUT_NAME "${_target}")
set_property(TARGET ${_target} PROPERTY DEBUG_POSTFIX "D")
set_property(TARGET ${_target} PROPERTY LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<0:>")
target_include_directories(${_target} PRIVATE ${_header_root_path})
target_link_libraries(${_target} PRIVATE "d3d11.lib" "d3dcompiler.lib" "dxgi.lib" "Comctl32.lib")
target_compile_features(${_target} PUBLIC cxx_std_20)
target_compile_definitions(${_target} PUBLIC WIN_X64 REFLECTIVEDLLINJECTION_VIA_LOADREMOTELIBRARYR REFLECTIVEDLLINJECTION_CUSTOM_DLLMAIN)

find_package(Python COMPONENTS Interpreter REQUIRED)

set(_generated_files_dir "${CMAKE_BINARY_DIR}/generated")
set(_injection_binary_name "dll_injection_binary" PARENT_SCOPE)
file(MAKE_DIRECTORY "${_generated_files_dir}/src")
file(MAKE_DIRECTORY "${_generated_files_dir}/include")

add_custom_target(
    generate_injection_code
    ALL 
    COMMAND python "${_src_root_path}/generate_injection_code.py" -s "${CMAKE_BINARY_DIR}/bin/${_target}$<$<CONFIG:Debug,RelWithDebInfo>:D>.dll" -o "${_generated_files_dir}" -n dll_injection_binary
    BYPRODUCTS "${_generated_files_dir}/src/dll_injection_binary.cpp" "${_generated_files_dir}/include/dll_injection_binary.h"
    DEPENDS "${CMAKE_BINARY_DIR}/bin/${_target}$<$<CONFIG:Debug,RelWithDebInfo>:D>.dll"
    COMMENT "Generating injection code"
)

add_dependencies(generate_injection_code ${_target})
