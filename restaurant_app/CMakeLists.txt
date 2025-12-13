cmake_minimum_required(VERSION 3.15)
project(restaurant_app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MYSQL_CONCPP_DIR "/opt/mysql-connector-c++-9.4")

add_executable(restaurant_app main.cpp)

target_include_directories(restaurant_app PRIVATE
    ${MYSQL_CONCPP_DIR}/include
)

target_link_directories(restaurant_app PRIVATE
    ${MYSQL_CONCPP_DIR}/lib64
)

target_link_libraries(restaurant_app PRIVATE
    mysqlcppconn
)
