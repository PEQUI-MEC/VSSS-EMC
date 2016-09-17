# Como Usar o GitLab

### Instalar o GitLab no Ubuntu
- aptitude install git (ou sudo aptitude install git)
(não sei se apt-get funciona, mas se não tiver aptitude, pegue com sudo apt-get aptitude).

### Configurar seu gitlab
- git config --global user.name "seu nome"
- git config --global user.email "seu email"
- git config --global core.editor [nome do seu editor de texto]

### Clonar o projeto do gitlab
- git clone [url do projeto] -b [nome da branch]

### Baixar a última versão do projeto no seu computador
Se você tentar baixar a última versão do projeto, o git provavelmente vai dizer que não pode fazer isso porque há arquivos no seu computador que estão diferentes do projeto. Neste caso,
se você tem certeza de que essas mudanças são descartáveis, dê o comando **git checkout .** para que ele ignore as mudanças. Caso as mudanças sejam importantes, envie-as ao projeto, como
o próprio git vai te sugerir.
- git pull

### Visualizar as mudanças do projeto feitas no seu computador
- git status

### Adicionar arquivos para serem enviados para o projeto
- git add [nome do arquivo]
Caso queira enviar todos os arquivos (ele sabe quais foram modificados ou não), digite o comando abaixo:
- git add --all

### Fazer um commit (dizer quais foram as mudanças feitas por você)
- git commit -m "Digite suas mudanças aqui, entre aspas duplas"


### Enviar as mudanças para o projeto
Será necessário digitar seu usuário e senha do gitlab após enviar o comando abaixo.
- git push origin [nome da branch]

