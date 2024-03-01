include(.env.cmake)
function(add_shaders TARGET_NAME)
    set(SHADER_SOURCE_FILES ${ARGN}) # the rest of arguments to this function will be assigned as shader source files
    set(SHADER_BINARIES "${CMAKE_CURRENT_SOURCE_DIR}/shaders")

    # Validate that source files have been passed
    list(LENGTH SHADER_SOURCE_FILES FILE_COUNT)
    if(FILE_COUNT EQUAL 0)
        message(FATAL_ERROR "Cannot create a shaders target without any source files")
    endif()

    set(SHADER_COMMANDS)
    set(SHADER_PRODUCTS)
    file(MAKE_DIRECTORY ${SHADER_BINARIES})

    foreach(SHADER_SOURCE IN LISTS SHADER_SOURCE_FILES)
        cmake_path(ABSOLUTE_PATH SHADER_SOURCE NORMALIZE)
        cmake_path(GET SHADER_SOURCE FILENAME SHADER_NAME)

        # Build command
        list(APPEND SHADER_COMMANDS COMMAND)
        list(APPEND SHADER_COMMANDS Vulkan::glslc)
        list(APPEND SHADER_COMMANDS "${SHADER_SOURCE}")
        list(APPEND SHADER_COMMANDS "-o")
        list(APPEND SHADER_COMMANDS "${SHADER_BINARIES}/${SHADER_NAME}.spv")

        # Add product
        list(APPEND SHADER_PRODUCTS "${SHADER_BINARIES}/${SHADER_NAME}.spv")

    endforeach()

    add_custom_target(${TARGET_NAME} ALL
            ${SHADER_COMMANDS}
            COMMENT "Compiling Shaders [${TARGET_NAME}]"
            SOURCES ${SHADER_SOURCE_FILES}
            BYPRODUCTS ${SHADER_PRODUCTS}
    )
endfunction()

cmake_minimum_required(VERSION 3.27)
project(vlk_test)
set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/ze_window.hpp
        src/ze_window.cpp
        src/ze_app.hpp
        src/ze_app.cpp
        src/ze_pipeline.hpp
        src/ze_pipeline.cpp
        src/ze_device.hpp
        src/ze_device.cpp
        src/ze_swap_chain.hpp
        src/ze_swap_chain.cpp
        src/ze_model.hpp
        src/ze_model.cpp
        src/ze_game_object.hpp
        src/ze_renderer.hpp
        src/ze_renderer.cpp
        src/ze_camera.hpp
        src/ze_camera.cpp
        src/keyboard_movement_controller.hpp
        src/keyboard_movement_controller.cpp
        src/ze_utils.hpp
        src/ze_game_object.cpp
        src/ze_buffer.hpp
        src/ze_buffer.cpp
        src/ze_frame_info.hpp
        src/ze_descriptors.hpp
        src/ze_descriptors.cpp
        src/systems/point_light_system.cpp
        src/systems/simple_render_system.cpp
)

find_package(Vulkan REQUIRED)
file(GLOB_RECURSE GLSL_SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/src/shaders/*.frag"
        "${PROJECT_SOURCE_DIR}/src/shaders/*.vert"
)
add_shaders(${PROJECT_NAME} ${GLSL_SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

include_directories(${TINYOBJ_PATH})

add_subdirectory(${GLFW_PATH})
include_directories(${GLFW_PATH}/include)
target_link_libraries(${PROJECT_NAME} glfw)

add_subdirectory(${GLM_PATH})
include_directories(${GLM_PATH})
target_link_libraries(${PROJECT_NAME} glm)

