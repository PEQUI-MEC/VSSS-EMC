#!/usr/bin/python3
import subprocess
import re
import time
from tqdm import trange

game_count = 2

ansi_escape = re.compile(r'\x1B(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])')

total_left = 0
total_right = 0
def parse_output(output):
    data = {}
    lines = output.splitlines()
    for line in lines:
        line = line.decode('unicode_escape')
        line = ansi_escape.sub('', line)
        parts = line.split(' ')
        if (len(parts) == 3 and parts[0] == "[STATS]"):
            data[parts[1]] = parts[2]
    return data

vsss_emc_pipe = subprocess.Popen(['/root/src/VSSS', 'both_headless.json'],
                                 cwd=r'/root/src',
                                 stdout=subprocess.PIPE)
firasim_pipe = subprocess.Popen(['/root/FIRASim/bin/FIRASim', '-H', '--fast'],
                                stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE)
time.sleep(5)
for game in trange(game_count):
    print(" ")
    referee_pipe = subprocess.Popen(['/root/VSSReferee/bin/VSSReferee',
                                     '--3v3', '--record', 'false', '--fast', '--headless'],
                                    stdout=subprocess.PIPE)
    time.sleep(20)
    output, errs = referee_pipe.communicate()
    data = parse_output(output)
    print(data)
    total_left += int(data["left_goals"])
    total_right += int(data["right_goals"])

print("Avg left: " + str(total_left/game_count))
print("Avg right: " + str(total_right/game_count))

referee_pipe.terminate()
firasim_pipe.terminate()
vsss_emc_pipe.terminate()
