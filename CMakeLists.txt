cmake_minimum_required(VERSION 3.31)
project(CitySmart LANGUAGES C CXX)

# ===============================
# C++ Standard
# ===============================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===============================
# Executable
# ===============================
add_executable(CitySmart
    main.cpp
)

# ===============================
# Force DLL runtime (CRITICAL)
# ===============================
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# ===============================
# Raylib (Windows)
# ===============================
if (WIN32)
    set(RAYLIB_PATH "${CMAKE_SOURCE_DIR}/raylib-lib")

    target_include_directories(CitySmart PRIVATE
        "${RAYLIB_PATH}/include"
    )

    target_link_libraries(CitySmart PRIVATE
        "${RAYLIB_PATH}/lib/raylib.lib"

        opengl32
        gdi32
        winmm
        user32
        shell32
        kernel32
    )

    # Copy raylib.dll
    add_custom_command(TARGET CitySmart POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${RAYLIB_PATH}/lib/raylib.dll"
        $<TARGET_FILE_DIR:CitySmart>
    )
endif()

# ===============================
# Copy PNG assets
# ===============================
file(GLOB ASSETS "${CMAKE_SOURCE_DIR}/*.png")

foreach(asset ${ASSETS})
    add_custom_command(TARGET CitySmart POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${asset}
        $<TARGET_FILE_DIR:CitySmart>
    )
endforeach()
