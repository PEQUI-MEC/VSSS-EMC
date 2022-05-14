docker run -v $(pwd)/replacement.json:/root/VSSS/replacement.json --rm --name pequi --network host pequi-vsss bash -c "cd /root/VSSS && /root/VSSS/VSSS yellow_headless.json"

docker container rm pequi-vsss -f
