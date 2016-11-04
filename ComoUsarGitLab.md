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
- git branch

### Como mudar de branch
- git checkout [nome da branch]

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
- git push origin [nome da branch]

### Falha ao enviar o projeto: conflitos
Caso, ao tentar enviar as suas mudanças, o comando git push falhar por causa de conflitos de arquivos, dê um **git pull**. 
O git pull também retornará conflitos, é normal. Depois disso, dê o **git push** como especificado no subtópico acima e
deve funcionar.

### (NÃO FUNCIONANDO) Script para submeter as mudanças feitas no projeto
Dentro do projeto, há o arquivo **VSS_GIT.sh**. Ele contém um script para envio automático das mudanças.
Para rodá-lo, basta digitar **sh VSS_GIT.sh "Digite suas mudanças aqui, entre aspas duplas"**. Todas as mudanças realizadas por esse script serão cadastradas
pela conta do Pequi Mecânico (pqverysmall@gmail.com).