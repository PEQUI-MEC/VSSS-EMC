# Como Usar o GitLab

## Instalar o GitLab no Ubuntu
- aptitude install git (ou sudo aptitude install git)
(não sei se apt-get funciona, mas se não tiver aptitude, pegue com sudo apt-get aptitude).

## Configurar seu gitlab
- git config --global user.name "seu nome"
- git config --global user.email "seu email"
- git config --global core.editor [nome do seu editor de texto]

## Clonar o projeto do gitlab
- git clone [url do projeto] -b [nome da branch]

## Baixar a última versão do projeto no seu computador
Se você tentar baixar a última versão do projeto, o git provavelmente vai dizer que não pode fazer isso porque há arquivos no seu computador que estão diferentes do projeto. Neste caso,
se você tem certeza de que essas mudanças são descartáveis, dê o comando **git checkout .** para que ele ignore as mudanças. Caso as mudanças sejam importantes, dê um commit nelas, como
o próprio git vai te sugerir.
- git pull

## Adicionar um arquivo no projeto
- git status
- git add [arquivo]
- git commit (vai abrir um arquivo para que você digite o as alterações ou adições feitas)
- Digite as alterações, salve e fecha.
- git remote -v
- git push origin [nome da branch]
- digitar login e senha

