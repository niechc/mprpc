set(SRC_LIST ../example.pb.cc provider.cc)

add_executable(provider ${SRC_LIST})

target_link_libraries(provider mprpc protobuf)
target_include_directories(provider PRIVATE ${PROJECT_SOURCE_DIR}/example)