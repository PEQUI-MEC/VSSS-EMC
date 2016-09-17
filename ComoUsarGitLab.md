# Como Usar o GitLab

### Instalar o GitLab no Ubuntu
- sudo apt-get install git

### Configurar seu gitlab
- git config --global user.name "seu nome"
- git config --global user.email "seu email"
- git config --global core.editor [nome do seu editor de texto]

### Clonar o projeto do gitlab
- git clone https://gitlab.com/danielfaleirosilva/Pequi-Mecanico-VSSS.git -b vsss


### Como checar a sua branch
Nosso projeto possui duas branches: master e vsss. A branch master é uma espécie de backup do nosso projeto, já a branch vsss é onde as modificações **DEVEM** ser feitas.
Todos os comandos deste guia já estão espeficiando a branch certa a ser modificada: vsss (proteção contra leitoagens). Se você utilizar o comando abaixo e não retornar vsss,
por favor dê o comando de clone citado acima para baixar a branch correta. Se você tentar baixar a branch master, lembre-se que ela provavelmente não é a última versão do
projeto e ela **NÃO DEVE** ser usada para fazer modificações.
- git branch

### Baixar a última versão do projeto no seu computador
Se você tentar baixar a última versão do projeto, o git provavelmente vai dizer que não pode fazer isso porque há arquivos no seu computador que estão diferentes do projeto. Neste caso,
se você tem certeza de que essas mudanças são descartáveis, dê o comando **git checkout .** para que ele ignore as mudanças. Caso as mudanças sejam importantes, envie-as ao projeto, como
o próprio git vai te sugerir.
- git pull

### Visualizar as mudanças do projeto feitas no seu computador
- git status

### Adicionar arquivos para serem enviados para o projeto
- git add [nome do arquivo]
    * para enviar um arquivo específico
- git add --all
    * para enviar todos os arquivos (o git sabe quais foram realmente modificados)

### Fazer um commit (dizer quais foram as mudanças feitas por você)
- git commit -m "Digite suas mudanças aqui, entre aspas duplas"


### Enviar as mudanças para o projeto
Será necessário digitar seu usuário e senha do gitlab após enviar o comando abaixo.
- git push origin vsss

