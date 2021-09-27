[![Codacy Badge](https://api.codacy.com/project/badge/Grade/7646cb63b79d4188b5b8dbec8fe8fd02)](https://www.codacy.com/app/danielfaleiro/VSSS-EMC?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=PEQUI-MEC/VSSS-EMC&amp;utm_campaign=Badge_Grade)
[![Travis build](https://img.shields.io/travis/PEQUI-MEC/VSSS-EMC/master.svg)](https://travis-ci.org/PEQUI-VSSS/VSSS-EMC)



# Introdução

Repositório destinado ao versionamento dos sistemas de controle e movimentação de robôs para participacao em competições de [IEEE VSSS](http://www.cbrobotica.org/?page_id=81&lang=pt). O grupo é misto e é composto por integrantes de Engenharia Elétrica, Engenharia da Computação, Engenharia de Software e Ciência da Computação da Universidade Federal de Goiás - [UFG](https://www.ufg.br/) - Goiânia.

# Motivação

Com a diversificação nas técnicas de manejo e controle de robôs e técnicas de implantação de software, tentamos abordar sistemas de Open Source para a produção de mecanismos completos de tracking por imagens de câmera, controle de sistemas motorizados e eventual implementação de inteligência estratégica.

# Instalação e uso

Instale o [docker](https://docs.docker.com/engine/install/ubuntu/) e [docker-compose](https://docs.docker.com/compose/install/).
Compile a imagem:

`docker-compose build`

Para rodar o programa, execute o `docker-compose up` listando os serviços desejados. Por exemplo, para rodar o VSSS-EMC com interface gráfica, execute:

`docker-compose up pequi`

E para simular um jogo no FIRASim controlando apenas o time amarelo, execute:

`docker-compose up yellow_headless firasim_gui referee`

Os serviços do docker-compose disponíveis são:

**pequi**: Compila e executa o programa VSSS-EMC com interface gráfica

**yellow_headless**: Executa jogo simulado controlando os robôs amarelos. O VSSS-EMC roda sem interface gráfica. É necessário executar o simulador (FIRASim) e o árbitro (VSSReferee) para que o jogo ocorra.

**blue_headless**: Executa jogo simulado controlando os robôs azuis. O VSSS-EMC roda sem interface gráfica. É necessário executar o simulador (FIRASim) e o árbitro (VSSReferee) para que o jogo ocorra.

**firasim_gui**: Roda o simulador FIRASim com interface gráfica.

**firasim_headless**: Roda o simulador FIRASim sem interface gráfica.

**referee**: Roda o árbitro VSSReferee.

**workspace**: Container com todas as bibliotecas, para ser usado com IDEs.

Para saber mais, consulte nossa [Wiki](https://github.com/PEQUI-MEC/VSSS-EMC/wiki) (em desenvolvimento) 

# Contribuidores

Projeto criado e mantido pelo [PEQUI MECÂNICO](https://www.facebook.com/NucleoPMec/) e pela comunidade que se dispõe a colaborar.
Leia [code-of-conduct](/docs/CODE_OF_CONDUCT.md) e então [contributing](/docs/CONTRIBUTING.md).
Contato no facebook do [PEQUI MECÂNICO](https://www.facebook.com/NucleoPMec/) ou via email pqverysmall@gmail.com



O PROJETO NÃO POSSUI FINS LUCRATIVOS E QUALQUER ARRECADAÇÃO SERÁ TOTALMENTE INVESTIDA EM NOVAS PESQUISAS PARA APOIO A COMUNIDADE CIENTÍFICA E LOCAL.
