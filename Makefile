# VSSS 2017 MAKEFILE
# Escrito por Bryan
#
#### TUTORIAL ####
# Para adicionar novos objetos ao projeto, você deve criar uma nova regra (veja "CRIANDO NOVAS REGRAS") para sua classe
# Depois disso, adicione a regra como dependência da regra "VSSS" e linke o objeto 
# na compilação do executável adicionando seu "arquivo.o" como argumento
# 
## CRIANDO NOVAS REGRAS ##
# Uma regra é uma especificação para o make sobre o que fazer com um arquivo.
# Ela deve respeitar uma sintaxe e identação específica (veja na especificação abaixo).
#
# Exemplo de regra:
# arquivo.o: dependencia.o arquivo.cpp
# 	g++ arquivo.cpp -o arquivo.o
#
# O nome desta regra deve ser o nome do objeto que será gerado
# Após o nome e ":", você inclui as dependências (que podem ser outras regras ou arquivos) 
# e o arquivo fonte que deverá ser monitorado
# Na linha de baixo, você escreve os comandos de compilação, ou seja, 
# o comando que deveria ser executado no terminal para compilar sua classe
# Os comandos de compilação devem ser identados por um único tab, senão o make não entende sua regra.
# Você pode utilizar as variáveis abaixo em suas regras de compilação e pode criar novas variáveis,
# mas deixe-as no início do arquivo para ficar mais fácil de entender

# compilador padrão do projeto
COMPILER = g++ -std=c++11
# flags pra usar na compilação da fuzzy
FUZZYFLAGS = -c -g -m64 -pipe -Wall -W
# flags para o pack-capture
CAPFLAGS = -w -I"pack-capture" `pkg-config gtkmm-3.0 --cflags` -O3 -Wall -c
# isso faz com que o make seja sensível a alterações a cpps e hpps que não são objetos, são só referenciados
SOURCES = $(shell find . -name "*.cpp") $(shell find . -name "*.hpp")

# roda o programa se estiver compilado
RUN: VSSS
	sudo -S ./VSSS

# isso compila o projeto
# ele gera primeiro os objetos que são dependências e depois linka tudo
VSSS: cc/main.o cc/vision/vision.o cc/vision/tag.o cc/TestFrame.o pack-capture/capture/v4lcap.o pack-capture-gui/capture-gui/V4LInterface-aux.o pack-capture-gui/capture-gui/V4LInterface-events.o cc/Fuzzy/FuzzyController.o cc/Fuzzy/FuzzyFunction.o cc/Fuzzy/Rules.o
	$(COMPILER) -w -L"pack-capture" -L"pack-capture-gui" -L"/usr/local/lib" -L"/lib64" -o "VSSS"  "cc/main.o" "cc/vision/vision.o" "cc/vision/tag.o" "cc/TestFrame.o" "cc/Fuzzy/FuzzyController.o" "cc/Fuzzy/FuzzyFunction.o" "cc/Fuzzy/Rules.o" -lpack-capture-gui -lpack-capture -lopencv_shape -lopencv_stitching -lopencv_objdetect -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_ml -lopencv_imgproc -lopencv_flann -lopencv_core -lboost_thread -lboost_system `pkg-config gtkmm-3.0 libv4l2 libv4lconvert --libs`

# verifica se algum hpp ou cpp foi alterado e compila a main se precisar
cc/main.o: cc/main.cpp $(SOURCES)
	$(COMPILER) -w -I"pack-capture-gui" -I"pack-capture" -O0 -g3 -Wall -c "cc/main.cpp" -o "cc/main.o" `pkg-config gtkmm-3.0 --cflags`

# objeto da visão
cc/vision/vision.o: cc/vision/vision.cpp
	$(COMPILER) -c `pkg-config --cflags opencv` "cc/vision/vision.cpp" -o "cc/vision/vision.o" -lboost_thread -lboost_system

# objeto das tags
cc/vision/tag.o: cc/vision/tag.cpp
	g++ -c `pkg-config --cflags opencv` "cc/vision/tag.cpp" `pkg-config --libs opencv` -o "cc/vision/tag.o"

# objeto do frame de teste
cc/TestFrame.o: cc/TestFrame.cpp
	g++ `pkg-config gtkmm-3.0 --cflags` -O3 -Wall -c "cc/TestFrame.cpp" -o "cc/TestFrame.o"

# objeto do v4lcap (captura)
pack-capture/capture/v4lcap.o: pack-capture/capture/v4lcap.cpp
	g++ -O0 -g3 -w -c "pack-capture/capture/v4lcap.cpp" -o "pack-capture/capture/v4lcap.o"
	ar -r "pack-capture/libpack-capture.a" "pack-capture/capture/v4lcap.o"

# objeto da interface
pack-capture-gui/capture-gui/V4LInterface-aux.o: pack-capture-gui/capture-gui/V4LInterface-aux.cpp
	g++ $(CAPFLAGS) "pack-capture-gui/capture-gui/V4LInterface-aux.cpp" -o "pack-capture-gui/capture-gui/V4LInterface-aux.o"

# objeto de eventos da interface
pack-capture-gui/capture-gui/V4LInterface-events.o: pack-capture-gui/capture-gui/V4LInterface-events.cpp
	g++ $(CAPFLAGS) "pack-capture-gui/capture-gui/V4LInterface-events.cpp" -o "pack-capture-gui/capture-gui/V4LInterface-events.o"
	ar -r  "pack-capture-gui/libpack-capture-gui.a"  "pack-capture-gui/capture-gui/V4LInterface-aux.o" "pack-capture-gui/capture-gui/V4LInterface-events.o"

# objeto do controlador da fuzzy
cc/Fuzzy/FuzzyController.o: cc/Fuzzy/FuzzyController.cpp
	$(COMPILER) $(FUZZYFLAGS) "cc/Fuzzy/FuzzyController.cpp" -o "cc/Fuzzy/FuzzyController.o"

# objeto das funções da fuzzy
cc/Fuzzy/FuzzyFunction.o: cc/Fuzzy/FuzzyFunction.cpp
	$(COMPILER) $(FUZZYFLAGS) "cc/Fuzzy/FuzzyFunction.cpp" -o "cc/Fuzzy/FuzzyFunction.o"

# objeto das regras
cc/Fuzzy/Rules.o: cc/Fuzzy/Rules.cpp
	$(COMPILER) $(FUZZYFLAGS) "cc/Fuzzy/Rules.cpp" -o "cc/Fuzzy/Rules.o"

# regras de limpeza
# isso apaga todos os objetos compilados e o executável
clean:
	find . -name "*.o" -type f -delete
	rm -f VSSS
