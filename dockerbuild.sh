# Evitar erro com o uso de video
xhost +local:docker

## Buildando o docker
# docker build . -f Dockerfile -t nomedodocker
docker build . -f dockerfile -t pequi-vsss
