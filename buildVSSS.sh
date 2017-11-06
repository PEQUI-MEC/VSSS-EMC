echo "\n\033[92m|------- VSSS 2017 - PEQUI MECÂNICO -------|\033[0m"
echo "\n\033[92mThis script will build the entire system from scratch.\033[0m\n"

####### CAPTURA #######

# objeto do v4lcap (captura)
echo "\n\n\033[92mCompiling capture object...\033[0m\n"
g++ -std=c++11 -O0 -g3 -w -c "pack-capture/capture/v4lcap.cpp" -o "pack-capture/capture/v4lcap.o"
ar -r "pack-capture/libpack-capture.a" "pack-capture/capture/v4lcap.o"

# objeto ImagView
echo "\n\n\033[92mCompiling ImageView object...\033[0m\n"
g++ -std=c++11 -c `pkg-config gtkmm-3.0 --cflags opencv` "pack-capture-gui/capture-gui/ImageView.cpp" `pkg-config --libs opencv` -o "pack-capture-gui/capture-gui/ImageView.o"


######## VISÃO ########

# objeto da visão
echo "\n\n\033[92mCompiling vision object...\033[0m\n"
g++ -std=c++11 -c `pkg-config --cflags opencv` "cc/vision/vision.cpp" -o "cc/vision/vision.o" -lboost_thread -lboost_system

# objeto das tags
echo "\n\n\033[92mCompiling tag object...\033[0m\n"
g++ -std=c++11 -c `pkg-config --cflags opencv` "cc/vision/tag.cpp" `pkg-config --libs opencv` -o "cc/vision/tag.o"

# objeto do GMM
echo "\n\n\033[92mCompiling GMM object...\033[0m\n"
g++ -std=c++11 -c `pkg-config --cflags opencv` "cc/vision/gmm.cpp" `pkg-config --libs opencv` -o "cc/vision/gmm.o" -lboost_thread -lboost_system


######## ESTRATÉGIA #######

# objeto do controlador da fuzzy
echo "\n\n\033[92mCompiling fuzzy controller object...\033[0m\n"
g++ -std=c++11 -c -g -m64 -pipe -Wall -W "cc/Fuzzy/FuzzyController.cpp" -o "cc/Fuzzy/FuzzyController.o"

# objeto das funções da fuzzy
echo "\n\n\033[92mCompiling fuzzy functions object...\033[0m\n"
g++ -std=c++11 -c -g -m64 -pipe -Wall -W "cc/Fuzzy/FuzzyFunction.cpp" -o "cc/Fuzzy/FuzzyFunction.o"

# objeto das regras
echo "\n\n\033[92mCompiling fuzzy rules object...\033[0m\n"
g++ -std=c++11 -c -g -m64 -pipe -Wall -W "cc/Fuzzy/Rules.cpp" -o "cc/Fuzzy/Rules.o"


####### CONTROLE #######

# objeto SerialW
echo "\n\n\033[92mCompiling SerialW object...\033[0m\n"
g++ -std=c++11 -c "cc/SerialW.cpp" -o "cc/SerialW.o"



####### INTERFACE ######

# objeto visionGUI
echo "\n\n\033[92mCompiling visionGUI object...\033[0m\n"
g++ -std=c++11 -c `pkg-config gtkmm-3.0 --cflags` "cc/vision/visionGUI.cpp" -o "cc/vision/visionGUI.o"

# objeto controlGUI
echo "\n\n\033[92mCompiling controlGUI object...\033[0m\n"
g++ -std=c++11 `pkg-config gtkmm-3.0 --cflags` -O3 -c "cc/controlGUI.cpp" -o "cc/controlGUI.o"

# objeto do frame de teste
echo "\n\n\033[92mCompiling test frame object...\033[0m\n"
g++ -std=c++11 `pkg-config gtkmm-3.0 --cflags` -O3 -Wall -c "cc/TestFrame.cpp" -o "cc/TestFrame.o"

# objeto FileChooser
echo "\n\n\033[92mCompiling FileChooser object...\033[0m\n"
g++ -std=c++11 `pkg-config gtkmm-3.0 --cflags` -O3 -Wall -c "cc/filechooser.cpp" -o "cc/filechooser.o"

# objeto da interface
echo "\n\n\033[92mCompiling interface object...\033[0m\n"
g++ -std=c++11 -w -I"pack-capture" `pkg-config gtkmm-3.0 --cflags` -O3 -Wall -c "pack-capture-gui/capture-gui/V4LInterface-aux.cpp" -o "pack-capture-gui/capture-gui/V4LInterface-aux.o"

# objeto de eventos da interface
echo "\n\n\033[92mCompiling interface events object...\033[0m\n"
g++ -std=c++11 -w -I"pack-capture" `pkg-config gtkmm-3.0 --cflags` -O3 -Wall -c "pack-capture-gui/capture-gui/V4LInterface-events.cpp" -o "pack-capture-gui/capture-gui/V4LInterface-events.o"
ar -r  "pack-capture-gui/libpack-capture-gui.a"  "pack-capture-gui/capture-gui/V4LInterface-aux.o" "pack-capture-gui/capture-gui/V4LInterface-events.o"


####### MAIN ######

# objeto principal
echo "\n\n\033[92mCompiling main program...\033[0m\n"
g++ -std=c++11 -w -I"pack-capture-gui" -I"pack-capture" -O0 -g3 -Wall -c "cc/main.cpp" -o "cc/main.o" `pkg-config gtkmm-3.0 --cflags`

####### LINKAGEM ######
echo "\n\n\033[92mLinking objects...\033[0m\n"
g++ -std=c++11 -w -L"pack-capture" -L"pack-capture-gui" -L"/usr/local/lib" -L"/lib64" -o "VSSS"  "cc/main.o" "cc/vision/vision.o" "cc/controlGUI.o" "cc/SerialW.o" "cc/filechooser.o" "pack-capture-gui/capture-gui/ImageView.o" "cc/vision/visionGUI.o" "cc/vision/tag.o" "cc/vision/gmm.o" "cc/TestFrame.o" "cc/Fuzzy/FuzzyController.o" "cc/Fuzzy/FuzzyFunction.o" "cc/Fuzzy/Rules.o" -lpack-capture-gui -lpack-capture -lopencv_shape -lopencv_stitching -lopencv_objdetect -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_ml -lopencv_imgproc -lopencv_flann -lopencv_core -lboost_thread -lboost_system `pkg-config gtkmm-3.0 libv4l2 libv4lconvert --libs`

echo "\n\n\033[92mAll done. Run 'sh runVSSS.sh' to open VSSS terminal.\033[0m\n"
