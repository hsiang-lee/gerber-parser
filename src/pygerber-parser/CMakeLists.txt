find_package(Qt6 COMPONENTS Core Widgets Gui REQUIRED)
set(CMAKE_AUTORCC ON)

# 查找Python和pybind11
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# 设置模块名称和输出路径
set(MODULE_NAME pygerber_parser)

# 获取pybind11
add_subdirectory(${CMAKE_SOURCE_DIR}/3rdparty/pybind11 ${CMAKE_BINARY_DIR}/pybind11)

file(GLOB Source ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# 创建Python模块
pybind11_add_module(${MODULE_NAME} ${Source})

# 设置模块属性
set_target_properties(${MODULE_NAME} PROPERTIES
    PREFIX ""
    SUFFIX "${Python3_SOABI}${CMAKE_SHARED_MODULE_SUFFIX}"
)

# 链接库
target_link_libraries(${MODULE_NAME} PRIVATE 
    gerber_engine 
    Qt6::Core 
    Qt6::Widgets 
    Qt6::Gui
)

# 包含目录
target_include_directories(${MODULE_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/parser/src
)

# 安装Python模块
install(TARGETS ${MODULE_NAME} DESTINATION ${Python3_SITEARCH})