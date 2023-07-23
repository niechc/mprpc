mprpc is a C++ RPC library using m(muduo) and p(protobuf)  
- muduo for request and response to send and recv  
- protobuf for serialize, parse and RPC call  
- zookeeper for service discovery


### Requires
- muduo  
  see <url>https://github.com/chenshuo/muduo</url>
- protobuf  
  see <url>https://github.com/protocolbuffers/protobuf</url>
- zookeeper  
  see <url>https://github.com/apache/zookeeper</url>

### To build
```shell
git clone https://github.com/niechc/mprpc.git
mkdir build
cd build
cmake ..
make
sudo make install
```
### To use
see the [example](./example/README.md)
