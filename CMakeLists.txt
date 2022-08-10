project(gltest C)
cmake_minimum_required(VERSION 3.16)

find_package(PkgConfig REQUIRED)

set(SHADER_SOURCES
    shaders/sample.comp.glsl
    shaders/sample.frag.glsl
    shaders/sample.vert.glsl
)
set(SHADER_INCLUDES
    shaders/include/foo.glsl
    shaders/include/ssbo.glsl
)

foreach(SHADER_SRC ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER_SRC} NAME)
    set(SHADER_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/shader")
    set(SHADER_SPIRV "${SHADER_BIN_DIR}/${SHADER_NAME}.spv")
    set(SHADER_SPIRV_OBJ "${SHADER_SPIRV}.o")
    add_custom_command(
        OUTPUT "${SHADER_SPIRV}"
        DEPENDS "${SHADER_SRC}" "${SHADER_INCLUDES}"

        # Directory Setup
        COMMAND ${CMAKE_COMMAND}
        ARGS -E make_directory "${SHADER_BIN_DIR}"

        # GLSL to SPIR-V
        COMMAND glslangValidator
        ARGS -G -o ${SHADER_SPIRV} ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_SRC}

        # Performance optimizer
        COMMAND spirv-opt -O ${SHADER_SPIRV} -o ${SHADER_SPIRV}
    )
    add_custom_command(
        OUTPUT "${SHADER_SPIRV_OBJ}"
        DEPENDS "${SHADER_SPIRV}"
        WORKING_DIRECTORY "${SHADER_BIN_DIR}"

        # Embed SPIR-V into ELF binary
        COMMAND ${CMAKE_OBJCOPY}
        ARGS -B i386
        --input-target=binary
        --output-target=elf64-x86-64
        --set-section-alignment .foo=8
        --rename-section=.data=.rodata
        "${SHADER_NAME}.spv" "${SHADER_NAME}.spv.o"
    )
    list(APPEND SPIRV_BINARY_FILES "${SHADER_SPIRV_OBJ}")
endforeach()

add_custom_target(
    binary_shader
    DEPENDS ${SPIRV_BINARY_FILES}
)
pkg_check_modules(spirvcross REQUIRED spirv-cross-c-shared)
add_executable(foo foo.c ${SPIRV_BINARY_FILES})
add_dependencies(foo binary_shader)
target_link_libraries(foo PUBLIC ${spirvcross_LIBRARIES})
target_include_directories(foo PUBLIC ${spirvcross_INCLUDE_DIRS})
