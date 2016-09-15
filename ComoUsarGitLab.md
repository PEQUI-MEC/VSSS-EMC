# Como Instalar o GitLab no Ubuntu
- aptitude install git (ou sudo aptitude install git)
(não sei se apt-get funciona, mas se não tiver aptitude, pegue com sudo apt-get aptitude).

# Como baixar o projeto do gitlab
- git clone [url do projeto] -b [nome da branch]

# Como adicionar um arquivo no projeto
- git status
- git add [arquivo]
- git commit (vai abrir um arquivo para que você digite o as alterações ou adições feitas)
- Digite as alterações e salve.
- git remote -v
- git push origin [nome da branch]
- digitar login e senha

# Como configurar seu gitlab
- git config --global user.name "seu nome"
- git config --global user.email "seu@email"
- git config --global core.editor [nome do seu editor de texto]

