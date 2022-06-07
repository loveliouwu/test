- cmake_minimum_required (VERSION 2.8)     
指定cmake最低版本  

- project(project_name)  
设置工程名
`project(my_test_demo)`   

- set(SRC_LIST ..)   
设置变量名 ，即将后面的值赋给前面 ，多个值之间用空格分隔
可以通过一些常用的变量
```C
set(SRC_LIST
    ./main.c
    ./func.c
    {PROJECT_SOURCE_DIR}/src/test.c
)
```   
- -D 设置cmake变量值  
```shell
cmake -D CMAKE_BUILD_TYPE="Debug" ..
cmake -D BUILD_TCP_MODE=1 ..
```
- 清理cmake      
由于cmake没有提供类似于clean之类的命令，因此生成的中间文件需要自己手动删除，但是可以通过在运行cmake时指定输出目录来创建一个单独文件夹，之后清理直接删除该文件夹就行了   
`cmake -S . -B tmp_dir`  
或者在运行cmake之前创建一个build文件夹，在该文件夹下运行
`cmake ..`即cmake会在当前执行该命令的路径下生成临时文件，并且生成的可执行文件也会在当前目录下

- target_include_directories(test_include1 test_include2)   
该命令使用来向工程添加多个指定头文件或者依赖库的搜索路径、路径之间用空格分隔。      

- 生成静态库   add_libary(hello_library STATIC src/hello.cpp)  
`add_library()`函数用于从某些源文件创建一个库，默认生成在构建文件夹。 modern CMake建议将源文件直接传递给生成的库即hello_library，而不是先把hello.cpp赋给一个变量。
```
#1 
project(hello_library)
add_library(hello_library STATIC src/hello.cpp)
target_include_driectories(hello_library PUBLIC ${PROJECT_SOURCE_DIR}/include)
add_executable(hello_binary src/main.cpp)#指定生成的可执行文件和依赖的源文件
target_link_libraries(hello_binary PRIVATE hello_library)#链接可执行文件和静态源
```
例子：   
```
cmake_minimum_required(VERSION 2.8)
##create a library  pe100_cmake_lib.a
SET(LIB_SRC_LIST 
	./src/pcie_app.c
	./src/pe100_op.c
	./src/PE_sm3.c
	./src/PE_sm4.c
	./src/sdf_common.c
	./src/SDF_Interface.c
	./src/sdf_netlink.c
	./src/sdf_openssl_interface.c
)
add_library(pe100_cmake_lib STATIC ${LIB_SRC_LIST})

target_include_directories(pe100_cmake_lib PRIVATE ./include)

add_executable(test.a ./test/main.c)
target_include_directories(test.a  PRIVATE ./include/)
target_link_libraries(test.a PRIVATE pe100_cmake_lib)
```

- private public interface的范围解释   
    - PRIVATE  目录被添加到目标(库)的包含路径中，如果该目标(库)作为其他目标的源，则该包含路径不可继承  
    - PUBLIC    与上面描述相反，路径被继承   
    - INTERFACE 目录没有被添加到目标的包含路径中，而是链接了这个库的其他目标(库或者可执行程序)包含路径中，即自己不用，但是给其他调用自己的目标使用   
按照上面的例子:
```
1、如果编译库的时候使用PRIVATE则编译可执行程序时需要重新调用`target_include_directories`包含头文件。  
2、如果编译库的时候使用PUBLIC，则编译可执行程序时`target_link_libraries`会将该库的PUBLIC路径都给继承，因此无需再调用`target_include_directories`
3、如果编译库的时候使用INTERFACE，则编译库本身不包含这个路径，因此需要再调用PRIVATE或PUBLIC来包含，但是链接该库的目标则包含了INTERFACE所指的路径
```


- target_link_libraries(hello_binary PRIVATE hello_library)    
告诉cmake在链接期间将hello_library链接到hello_binary可执行程序，同时这个被链接的库如果有INTERFACE或者PUBLIC属性的包含目录，那么，这个包含目录也会被传递给这个可执行文件。



- 创建静态库 `.so`
add_library()函数用于从某些源文件创建一个动态库，默认生成在构建文件夹。   
```cmake
add_library(hello_library SHARED src/hello.cpp)  

```


- 创建别名名称 ALIAS  
```cmake
#给hello_library取个别名hello::library
add_library(hello::library ALIAS hello_library)
add_executable(hello_binary ./src/main.c)
target_link_libraries(hello_binary PRIVATE hello::library)
``` 


- 常用变量 `BUILD_SHARED_LIBS`   
用来控制生成的库的类型是动态库还是静态库，默认为off，则默认是生成动态库  通过使用`set(BUILD_SHARED_LIBS ON)`打开 如果在`add_library(hello_lib SHARED ${SRC_LIST})`中未指定生成的库类型即`add_library(hello_lib ${SRC_LIST})`，则默认使用BUILD_SHARED_LIBS的值



- 设置编译类型：  
共4种：  Release、Debug、MinSizeRel(最小体积版本)、RelWithDebInfo(即优化又能调试)。   
通过cmake设置的方式:
1、`cmake .. -D CMAKE_BUILD_TYPE=Release `
2、`set(CMAKE_BUILD_TYPE Debug)`


- `set()`命令   
该命令可以为普通变量、缓存变量、环境变量赋值。
可以同时设置零个或多个参数，多个参数将以分号 
    - 正常变量 
    ```cmake
    set(<variable> <value>... [PARENT_SCOPE])
    ```
    vaiable只能有一个，value可以有多个，当value值为空时相当于unset，即取消该变量的值，[PARENT_SCOPE]父作用域，除此外还有function scope函数作用域和directory scope目录作用域
    设置的变量值作用域为整个CMakeLists.txt文件，当这个语句加入PARENT_SCOPE后，表示要设置的变量是父目录种的CMakeLists.txt设置的变量   
    在父目录下的CMakeLists.txt中通过`add_subdirectory(sub_path)`调用子目录的CMakeLists.txt    
    此外，当父目录调用子目录的CMakeLists.txt时，父目录中的变量也继承给子目录，所以子目录的CMakeLists.txt可以获取和修改该值

    - CACHE变量   
    ```cmake
    set(<variable> <value>... CHCHE <type> <docstring> [FORCE])
    ```   
    CACHE变量，就是在运行cmake的时候，变量的值可能被缓存到一份文件里，即build命令下的CMakeCache.txt，当你重新运行cmake的时候，那些变量会默认使用这个缓存里的值，这个变量是全局变量，整个CMake工程都可以使用这些变量。  
    这个文件里，只要运行cmake ..命令，自动会出现一些值，比如(CMAKE_INSTALL_PREFIX),如果设置set(CMAKE_INSTALL_PREFIX "/usr")，虽然CACHE缓存文件里还有这个变量，但是因为我们显式的设置了一个明文CMAKE_INSTALL_PREFIX的正常变脸，所以之后使用CMAKE_INSTALL_PREFIX,值是我们设置的正常变量的值。  
    加上CACHE关键字，<type>和<docstring>是必需的。 
    <type>被CMake GUI用来选择一个窗口，让用户设置值，

    - type 5个取值  
    BOOL: 则<docstring>为布尔"ON/OFF"值 
    FILEPATH: 则<docstring>为磁盘上文件的路径  
    PATH: <docstring>为磁盘上目录的路径   
    STRING: <docstring>为一行文字   
    INTERNAL: <docstring>为一行文字，与上一个的区别是他们不显示内部条目，可以用于在运行之间持久存储变量



- 编译选项 CMAKE_CXX_FLAGS CMAKE_C_FLAGS  
    - 方法1   
    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -DEX2" CACHE STRING "Set C++ Compiler Flags" FORCE)  
    可为执行文件添加私有编译定义  
    target_compile_definitions(cmake_examples_compile_flags PRIVATE EX3) 
    如果这个目标是一个库(cmake_examples_compile_flags),编译器在编译目标时添加定义-DEX3
  

- target_compile_definitions(
    <target> 
    <INTERFACE|PUBLIC|PRIVATE> [item1...]
    <INTERFACE|PUBLIC|PRIVATE> [item2...]
)    
是给`target`添加编译选项，`target`指的是由`add_executable()`产生的可执行文件或`add_library()`添加进来的库。  

- 设置默认编译标志    
默认的CMAKE_CXX_FLAGS为空或包含于构建类型的标志  

- 设置链接标志: CMAKE_LINKER_FLAGS



### cmake demo
- 根目录CMakeLists.txt
```cmake 
cmake_minimum_required(VERSION 2.8)
project(csf_lib)

#设置私有变量，用于存储编译的路径
set(LIBRARY_OUTPUT_PATH_RELEASE ${PROJECT_SOURCE_DIR}/release)
set(LIBRARY_OUTPUT_PATH_DEBUG ${PROJECT_SOURCE_DIR}/debug)

if(${CMAKE_BUILD_TYPE} MATCHES "Release")
    message(STATUS "Release 版本")
    set(BuildType "Release")
else()
    message(STATUS "Debug 版本")
    set(BuildType "Debug")
endif()

set(SOURCE_ROOT ${PROJECT_SOURCE_DIR}/crypto)
set(ASYM_DIR ${SOURCE_ROOT}/asym)

#设置自定义的变量
set(BuildMode "0" CACHE STRING "Set build mode" FORCE)
message(STATUS "Build Mode = " ${BuildMode})
if(BuildMode MATCHES "0")
    message(STATUS "Build Mode is normal C!")
else()
    message(STATUS "Build Mode is intel_x64_asm!")
endif()

#根据自定义变量设置编译CFLAG
set(BuildDLL 0 CACHE INTERNAL "set build dll" FORCE)
message(STATUS "Build DLL Mode = " ${BuildDLL})
if(BuildDLL)
    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-Wall -Wextra -s -fPIE -fPIC")
else()
    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-Wall -Wextra -s -fPIE")
endif()
message(STATUS "CMAKE_C_FLAGS = " ${CMAKE_C_FLAGS})

#start build sub dir
#if build pct mode  
set(ASYM_BUILD_TYPE 0 CACHE INTERNAL "set asym build type 0 or 1" FORCE)
if(ASYM_BUILD_TYPE)
    add_definitions(-D USE_PCT)
endif()

add_subdirectory(${ASYM_DIR})
set(SRC_LISTS ${SRC_LISTS} ${ASYM_SRC_LISTS})

#build static library
add_library(csf_lib STATIC ${SRC_LISTS})

target_include_directories(csf_lib PUBLIC ${PROJECT_SOURCE_DIR}/include/internal)
target_include_directories(csf_lib PUBLIC ${PROJECT_SOURCE_DIR}/include/external)

if(BuildMode MATCHES "1")
    target_include_directories(csf_lib PUBLIC ${PROJECT_SOURCE_DIR}/include/internal/asm/x64)
    add_definitions(-D USE_INTEL_ASM)
else()
    add_definitions(-D USE_NORMAL_C)
endif()

#设置动态库
add_library(csf_shared_lib STATIC ${SRC_LISTS})
target_include_directories(csf_shared_lib PUBLIC ${PROJECT_SOURCE_DIR}/include/internal)
target_include_directories(csf_shared_lib PUBLIC ${PROJECT_SOURCE_DIR}/include/external)

#动态库重命名与静态库同名
set_target_properties(csf_shared_lib PROPERTIES OUTPUT_NAME "csf_lib")
set_target_properties(csf_lib PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(csf_shared_lib PROPERTIES CLEAN_DIRECT_OUTPUT 1)

```

- 子目录CMakeLists.txt  
```cmake
cmake_minimum_required(VERSION 2.8)
project(asym)

set(ASYM_SRC_LISTS ${PROJECT_SOURCE_DIR}/ec/bn_ctx.c)
aux_source_directory(${PROJECT_SOURCE_DIR}/ SM2_SOURCE_LISTS)

if(ASYM_BUILD_TYPE)
    set(ASYM_SRC_LISTS ${ASYM_SRC_LISTS} ${PROJECT_SOURCE_DIR}/ec/ec_pct_data.c)
endif()

if(BuildMode MATCHES "1") #use asm mode
    message(STATUS "ASYM Build with intel_x64_asm mode")
    set(ASYM_SRC_LISTS ${ASYM_SRC_LISTS} ${PROJECT_SOURCE_DIR}/ec/asm/x64/bn_calc.c)
endif()

set(ASYM_SRC_LISTS ${ASYM_SRC_LISTS} ${SM2_SOURCE_LISTS} PARENT_SCOPE)
```

- 在顶层目录下执行  
```shell
mkdir build   //创建一个目录用于存放cmake中间文件
cd build 
cmake ..
make 
make clean
```


- test_demo2  pe100 CMakeFiles.txt
```cmake
cmake_minimum_required(VERSION 2.8)
##create a library
SET(LIB_SRC_LIST 
	./src/pcie_app.c
	./src/pe100_op.c
	./src/PE_sm3.c
	./src/PE_sm4.c
	./src/sdf_common.c
	./src/SDF_Interface.c
	./src/sdf_netlink.c
	./src/sdf_openssl_interface.c
)
#1
#add_library(pe100_cmake_lib STATIC ${LIB_SRC_LIST})
#2
#add_library(pe100_cmake_lib SHARED ${LIB_SRC_LIST})
#3 BUILD_SHARED_LIBS default off
set(BUILD_SHARED_LIBS ON)
set(MY_TMP_VAL "my_tmp_val" CACHE STRING "PRINT MY _____ TMP VAL")
add_library(pe100_cmake_lib ${LIB_SRC_LIST})
add_subdirectory(./test)
message("after test/cmakelists.txt " ${MY_TMP_VAL})
#set build type  Release Debug MinSizeRel RelWithDebInfo
set(CMAKE_BUILD_TYPE Release)  #set to Release ver
set(CMAKE_BUILD_TYPE Debug [FORCE] )  #set to Debug ver
message("CMAKE_C_FLAGS =  " ${CMAKE_C_FLAGS})
#set(CMKAE_C_FLAGS "{CMAKE_C_FLAGS} -g" CACHE STRING "Set C compiler -g")
target_include_directories(pe100_cmake_lib PRIVATE ./include)
target_include_directories(pe100_cmake_lib INTERFACE ./include)

MESSAGE(STATUS "home dir: $ENV{HOME}")

add_executable(test.a ./test/main.c)
target_compile_definitions(test.a  PRIVATE g)
target_include_directories(test.a  PRIVATE ./include/)
target_include_directories(test.a PRIVATE ./out/)
target_link_libraries(test.a PRIVATE sdfop)
```
