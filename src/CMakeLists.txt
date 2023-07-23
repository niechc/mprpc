
set(SRC_LIST mprpcapplication.cc 
            mprpcconfig.cc 
            rpcheader.pb.cc 
            mprpcprovider.cc 
            mprpcchannel.cc 
            mprpccontroller.cc 
            logger.cc
            zookeeperutil.cc
)

add_library(mprpc ${SRC_LIST})

target_link_libraries(mprpc muduo_net muduo_base pthread zookeeper_mt)

set_target_properties(mprpc PROPERTIES PUBLIC_HEADER "${CMAKE_CURRENT_LIST_DIR}/include/logger.h;\
${CMAKE_CURRENT_LIST_DIR}/include/logqueue.h;\
${CMAKE_CURRENT_LIST_DIR}/include/mprpcapplication.h;\
${CMAKE_CURRENT_LIST_DIR}/include/mprpcconfig.h;\
${CMAKE_CURRENT_LIST_DIR}/include/mprpccontroller.h;\
${CMAKE_CURRENT_LIST_DIR}/include/mprpcprovider.h;\
${CMAKE_CURRENT_LIST_DIR}/include/rpcheader.pb.h;\
${CMAKE_CURRENT_LIST_DIR}/include/zookeeperutil.h;\
${CMAKE_CURRENT_LIST_DIR}/include/mprpcchannel.h")      