g++ -std=c++11 -c `pkg-config --cflags opencv` "cc/vision/vision.cpp" -o "cc/vision/vision.o" -lboost_thread -lboost_system
g++ -std=c++11 -c `pkg-config gtkmm-3.0 --cflags` "cc/vision/visionGUI.cpp" -o "cc/vision/visionGUI.o"
g++ -std=c++11 -c `pkg-config --cflags opencv` "cc/vision/tag.cpp" `pkg-config --libs opencv` -o "cc/vision/tag.o"
g++ -std=c++11 -c `pkg-config --cflags opencv` "cc/vision/gmm.cpp" `pkg-config --libs opencv` -o "cc/vision/gmm.o" -lboost_thread -lboost_system
g++ -std=c++11 `pkg-config gtkmm-3.0 --cflags` -O3 -Wall -c "cc/TestFrame.cpp" -o "cc/TestFrame.o"

g++ -std=c++11 -O0 -g3 -w -c "pack-capture/capture/v4lcap.cpp" -o "pack-capture/capture/v4lcap.o"
ar -r "pack-capture/libpack-capture.a" "pack-capture/capture/v4lcap.o"
g++ -std=c++11 -w -I"pack-capture" `pkg-config gtkmm-3.0 --cflags` -O3 -Wall -c "pack-capture-gui/capture-gui/V4LInterface-aux.cpp" -o "pack-capture-gui/capture-gui/V4LInterface-aux.o"
g++ -std=c++11 -w -I"pack-capture" `pkg-config gtkmm-3.0 --cflags` -O3 -Wall -c "pack-capture-gui/capture-gui/V4LInterface-events.cpp" -o "pack-capture-gui/capture-gui/V4LInterface-events.o"
ar -r  "pack-capture-gui/libpack-capture-gui.a"  "pack-capture-gui/capture-gui/V4LInterface-aux.o" "pack-capture-gui/capture-gui/V4LInterface-events.o"

g++ -std=c++11 -c -g -m64 -pipe -Wall -W -std=c++11 "cc/Fuzzy/FuzzyController.cpp" -o "cc/Fuzzy/FuzzyController.o"
g++ -std=c++11 -c -g -m64 -pipe -Wall -W -std=c++11 "cc/Fuzzy/FuzzyFunction.cpp" -o "cc/Fuzzy/FuzzyFunction.o"
g++ -std=c++11 -c -g -m64 -pipe -Wall -W -std=c++11 "cc/Fuzzy/Rules.cpp" -o "cc/Fuzzy/Rules.o"

g++ -std=c++11 -w -I"pack-capture-gui" -I"pack-capture" -O0 -g3 -Wall -c "cc/main.cpp" -o "cc/main.o" `pkg-config gtkmm-3.0 --cflags`
g++ -std=c++11 -w -L"pack-capture" -L"pack-capture-gui" -L"/usr/local/lib" -L"/lib64" -o "VSSS"  "cc/main.o" "cc/vision/vision.o" "cc/vision/visionGUI.o" "cc/vision/tag.o" "cc/vision/gmm.o" "cc/TestFrame.o" "cc/Fuzzy/FuzzyController.o" "cc/Fuzzy/FuzzyFunction.o" "cc/Fuzzy/Rules.o" -lpack-capture-gui -lpack-capture -lopencv_shape -lopencv_stitching -lopencv_objdetect -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_ml -lopencv_imgproc -lopencv_flann -lopencv_core -lboost_thread -lboost_system `pkg-config gtkmm-3.0 libv4l2 libv4lconvert --libs`
