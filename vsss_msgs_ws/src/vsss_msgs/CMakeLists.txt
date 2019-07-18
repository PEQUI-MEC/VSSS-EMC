#Para acrescentar novos arquivos:
#Acrescente o .cpp em set(SOURCE_FILES ...) e o diretorio dos .h e .hpp em include_directories()

#Para incluir novas biliotecas:
#utilize pkg_check_modules(NOME pacotes) para defini-la, seguido de link_directories(${NOME_LIBRARY_DIRS}),
#acrescente ${NOME_INCLUDE_DIRS} em include_directories()
#e insira target_link_libraries(VSSS ${NOME_LIBRARIES}) no final do arquivo

cmake_minimum_required(VERSION 3.5)
project(vsss_msgs)

set(CMAKE_CXX_STANDARD 17)
set(LIB_DIR ${PROJECT_SOURCE_DIR}/lib)

FIND_PACKAGE(PkgConfig REQUIRED)

find_package(catkin REQUIRED COMPONENTS roscpp rospy std_msgs geometry_msgs message_generation)
add_message_files(FILES Control.msg)
generate_messages(DEPENDENCIES std_msgs geometry_msgs)
catkin_package(CATKIN_DEPENDS message_runtime)

set(CMAKE_CXX_FLAGS "-std=c++17 -O3 -mavx -pthread -Wl,-rpath")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
