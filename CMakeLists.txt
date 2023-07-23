cmake_minimum_required(VERSION 3.0)

project(mprpc VERSION 1.0)

set(CMAKE_BUILD_TYPE "Debug")

include_directories(${PROJECT_SOURCE_DIR}/src/include)


add_subdirectory(${PROJECT_SOURCE_DIR}/src)

add_subdirectory(${PROJECT_SOURCE_DIR}/example)

install(TARGETS mprpc
        EXPORT MprpcTargets
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include/mprpc
)
install(TARGETS provider consumer
        EXPORT example
        RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        MprpcConfigVersion.cmake
        VERSION ${PACKAGE_VERSION}
        COMPATIBILITY AnyNewerVersion
)    

install(EXPORT MprpcTargets
    FILE MprpcTargets.cmake
    NAMESPACE Mprpc::
    DESTINATION lib/cmake/Mprpc
)

configure_file(MprpcConfig.cmake.in MprpcConfig.cmake @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/MprpcConfig.cmake"
                "${CMAKE_CURRENT_BINARY_DIR}/MprpcConfigVersion.cmake"
        DESTINATION lib/cmake/Mprpc
)