set(SRC_LIST ../example.pb.cc consumer.cc)

add_executable(consumer ${SRC_LIST})

target_link_libraries(consumer mprpc protobuf)
target_include_directories(consumer PRIVATE ${PROJECT_SOURCE_DIR}/example)