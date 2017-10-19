COMPILER = g++ -std=c++11
FUZZYFLAGS = -c -g -m64 -pipe -Wall -W
CAPFLAGS = -w -I"pack-capture" `pkg-config gtkmm-3.0 --cflags` -O3 -Wall -c
SOURCES = $(shell find . -name "*.cpp") $(shell find . -name "*.hpp")

RUN: VSSS
	sudo -S ./VSSS

VSSS: cc/main.o cc/vision/vision.o cc/vision/tag.o cc/TestFrame.o pack-capture/capture/v4lcap.o pack-capture-gui/capture-gui/V4LInterface-aux.o pack-capture-gui/capture-gui/V4LInterface-events.o cc/Fuzzy/FuzzyController.o cc/Fuzzy/FuzzyFunction.o cc/Fuzzy/Rules.o
	$(COMPILER) -w -L"pack-capture" -L"pack-capture-gui" -L"/usr/local/lib" -L"/lib64" -o "VSSS"  "cc/main.o" "cc/vision/vision.o" "cc/vision/tag.o" "cc/TestFrame.o" "cc/Fuzzy/FuzzyController.o" "cc/Fuzzy/FuzzyFunction.o" "cc/Fuzzy/Rules.o" -lpack-capture-gui -lpack-capture -lopencv_shape -lopencv_stitching -lopencv_objdetect -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_ml -lopencv_imgproc -lopencv_flann -lopencv_core -lboost_thread -lboost_system `pkg-config gtkmm-3.0 libv4l2 libv4lconvert --libs`

cc/main.o: cc/main.cpp $(SOURCES)
	$(COMPILER) -w -I"pack-capture-gui" -I"pack-capture" -O0 -g3 -Wall -c "cc/main.cpp" -o "cc/main.o" `pkg-config gtkmm-3.0 --cflags`

cc/vision/vision.o: cc/vision/vision.cpp
	$(COMPILER) -c `pkg-config --cflags opencv` "cc/vision/vision.cpp" -o "cc/vision/vision.o" -lboost_thread -lboost_system

cc/vision/tag.o: cc/vision/tag.cpp
	g++ -c `pkg-config --cflags opencv` "cc/vision/tag.cpp" `pkg-config --libs opencv` -o "cc/vision/tag.o"

cc/TestFrame.o: cc/TestFrame.cpp
	g++ `pkg-config gtkmm-3.0 --cflags` -O3 -Wall -c "cc/TestFrame.cpp" -o "cc/TestFrame.o"

pack-capture/capture/v4lcap.o: pack-capture/capture/v4lcap.cpp
	g++ -O0 -g3 -w -c "pack-capture/capture/v4lcap.cpp" -o "pack-capture/capture/v4lcap.o"
	ar -r "pack-capture/libpack-capture.a" "pack-capture/capture/v4lcap.o"

pack-capture-gui/capture-gui/V4LInterface-aux.o: pack-capture-gui/capture-gui/V4LInterface-aux.cpp
	g++ $(CAPFLAGS) "pack-capture-gui/capture-gui/V4LInterface-aux.cpp" -o "pack-capture-gui/capture-gui/V4LInterface-aux.o"

pack-capture-gui/capture-gui/V4LInterface-events.o: pack-capture-gui/capture-gui/V4LInterface-events.cpp
	g++ $(CAPFLAGS) "pack-capture-gui/capture-gui/V4LInterface-events.cpp" -o "pack-capture-gui/capture-gui/V4LInterface-events.o"
	ar -r  "pack-capture-gui/libpack-capture-gui.a"  "pack-capture-gui/capture-gui/V4LInterface-aux.o" "pack-capture-gui/capture-gui/V4LInterface-events.o"

cc/Fuzzy/FuzzyController.o: cc/Fuzzy/FuzzyController.cpp
	$(COMPILER) $(FUZZYFLAGS) "cc/Fuzzy/FuzzyController.cpp" -o "cc/Fuzzy/FuzzyController.o"

cc/Fuzzy/FuzzyFunction.o: cc/Fuzzy/FuzzyFunction.cpp
	$(COMPILER) $(FUZZYFLAGS) "cc/Fuzzy/FuzzyFunction.cpp" -o "cc/Fuzzy/FuzzyFunction.o"

cc/Fuzzy/Rules.o: cc/Fuzzy/Rules.cpp
	$(COMPILER) $(FUZZYFLAGS) "cc/Fuzzy/Rules.cpp" -o "cc/Fuzzy/Rules.o"

clean:
	find . -name "*.o" -type f -delete
	rm -f VSSS
