cmake_minimum_required(VERSION 3.15)
project(MySQLClassicExample)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MYSQL_CONCPP_DIR "/opt/mysql-connector-c++-9.4")

add_executable(MySQLClassicExample main.cpp)

target_include_directories(MySQLClassicExample PRIVATE
    ${MYSQL_CONCPP_DIR}/include
)

target_link_directories(MySQLClassicExample PRIVATE
    ${MYSQL_CONCPP_DIR}/lib64
)

# Only the MySQL connector (no ssl/crypto)
target_link_libraries(MySQLClassicExample PRIVATE
    mysqlcppconn
)
