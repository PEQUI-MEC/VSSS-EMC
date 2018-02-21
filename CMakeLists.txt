#Para acrescentar novos arquivos:
#Acrescente o .cpp em set(SOURCE_FILES ...) e o diretorio dos .h e .hpp em include_directories()

#Para incluir novas biliotecas:
#utilize pkg_check_modules(NOME pacotes) para defini-la, seguido de link_directories(${NOME_LIBRARY_DIRS}),
#acrescente ${NOME_INCLUDE_DIRS} em include_directories()
#e insira target_link_libraries(VSSS ${NOME_LIBRARIES}) no final do arquivo

cmake_minimum_required(VERSION 3.5)
project(VSSS)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

#Encontra bibliotecas
find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-3.0)
link_directories(${GTKMM_LIBRARY_DIRS})

pkg_check_modules(V4L2 libv4l2 libv4lconvert)
link_directories(${V4L2_LIBRARY_DIRS})

find_package(Boost REQUIRED COMPONENTS date_time filesystem wserialization system serialization thread regex)

find_package(OpenCV REQUIRED)

find_library(LIBXBEE libxbee.so libxbeep.so)

#Define arquivos a serem compilados. Novos arquivos devem ser inclusos aqui
set(SOURCE_FILES cc/main.cpp
        cc/controlGUI.cpp
        cc/filechooser.cpp
        cc/strategyGUI.hpp
        cc/TestFrame.cpp
        cc/Fuzzy/FuzzyController.cpp
        cc/Fuzzy/FuzzyFunction.cpp
        cc/Fuzzy/Rules.cpp
	cc/vision/visionROI.cpp
        cc/vision/gmm.cpp
        cc/vision/tag.cpp
        cc/vision/vision.cpp
        cc/vision/visionGUI.cpp
        pack-capture/capture/v4lcap.cpp
		pack-capture/capture/v4ldev.hpp
        pack-capture-gui/capture-gui/ImageView.cpp
        pack-capture-gui/capture-gui/Robot.hpp
        pack-capture-gui/capture-gui/V4LInterface-aux.cpp
        pack-capture-gui/capture-gui/V4LInterface-events.cpp
		pack-capture-gui/capture-gui/V4LInterface.hpp
        cc/camcap.hpp
        cc/Constants.hpp
        cc/Kalman_Filter.hpp
        cc/Strategy.hpp
        cc/Xbee.cpp cc/Xbee.h
        cc/Messenger.cpp cc/Messenger.h)

#Define diretorios com .h e .hpp. Novos diretorios devem ser inclusos aqui
include_directories(cc/
                    cc/Fuzzy/
                    cc/vision/
                    pack-capture/capture/
                    pack-capture-gui/capture-gui/
                    ${GTKMM_INCLUDE_DIRS}
                    ${Boost_INCLUDE_DIRS}
                    ${OpenCV_INCLUDE_DIRS}
                    ${V4L2_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "-g -m64 -pipe -lopencv_shape -lopencv_stitching -lopencv_objdetect -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_ml -lopencv_imgproc -lopencv_flann -lopencv_core -lboost_thread -lboost_system -lboost_filesystem")
                    
add_executable(VSSS ${SOURCE_FILES})

target_link_libraries(VSSS ${GTKMM_LIBRARIES})
target_link_libraries(VSSS ${Boost_LIBRARIES})
target_link_libraries(VSSS ${OpenCV_LIBS})
target_link_libraries(VSSS ${V4L2_LIBRARIES})
target_link_libraries(VSSS ${LIBXBEE}) 
