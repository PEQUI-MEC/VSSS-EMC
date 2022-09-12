#!/usr/bin/python3
import subprocess
import re
import time
import sys
import json

running_games = 0
completed_games = 0
thread_count = 12
games = thread_count * 6

referee_list = []
for i in range(thread_count):
    referee_list.append(None)
vsss_emc_list = []
firasim_list = []

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

def run_vsss_emc(config_filename):
    json_file = open(config_filename)
    configs = json.load(json_file)
    json_file.close()
    return subprocess.Popen(['/root/src/VSSS', json.dumps(configs), str(i+1)],
                                    stdout=subprocess.PIPE,
                                    stderr=subprocess.PIPE)

for i in range(thread_count):
    vsss_emc_pipe_yellow = run_vsss_emc('yellow_headless.json')
    vsss_emc_pipe_blue = run_vsss_emc('blue_headless.json')
    vsss_emc_list.append((vsss_emc_pipe_yellow, vsss_emc_pipe_blue))
    #vsss_emc_pipe = subprocess.Popen(['/root/src/VSSS', 'both_headless.json', str(i+1)],
                                    #stdout=subprocess.PIPE,
                                    #stderr=subprocess.PIPE)
    #vsss_emc_list.append(vsss_emc_pipe)
    firasim_pipe = subprocess.Popen(['/root/FIRASim/bin/FIRASim', '-H', '--fast', '--id', str(i+1)],
                                    stdout=subprocess.PIPE,
                                    stderr=subprocess.PIPE)
    firasim_list.append(firasim_pipe)

time.sleep(3)

def is_done(referee):
    return (referee is not None) and (referee.poll() is not None)

def start_referee(ref_id):
    return subprocess.Popen(['/root/VSSReferee/bin/VSSReferee',
                            '--3v3', '--record', 'false', '--fast', '--headless', '--id', str(ref_id)],
                            stdout=subprocess.PIPE)

def process_and_quit(referee):
    global total_left, total_right
    output, errs = referee.communicate()
    data = parse_output(output)
    if "left_goals" in data and "right_goals" in data:
        total_left += int(data["left_goals"])
        total_right += int(data["right_goals"])
        referee.terminate()
        return True, int(data["left_goals"]), int(data["right_goals"])
    else:
        print("could not find goals in output")
        return False, 0, 0

print(f"Simulating {games} games with {thread_count} threads")
while games > completed_games:
    time.sleep(0.1)
    for i, referee in enumerate(referee_list):
        if is_done(referee):
            success, left_goals, right_goals = process_and_quit(referee)
            referee_list[i] = None
            running_games -= 1
            if success:
                completed_games += 1
                print(f"{completed_games}/{games} - left: {left_goals}, right: {right_goals}, "
                    f"avg left: {total_left/completed_games:.2f}, avg right: {total_right/completed_games:.2f}")
            else:
                print("failed to complete game")
        if games > (completed_games + running_games) and referee_list[i] is None:
            referee_list[i] = start_referee(i+1)
            running_games += 1

print(f"Average left: {total_left/games:.3f}")
print(f"Average right: {total_right/games:.3f}")

#for vsss_emc in vsss_emc_list:
    #vsss_emc.terminate()

for vsss_emc_yellow, vsss_emc_blue in vsss_emc_list:
    vsss_emc_yellow.terminate()
    vsss_emc_blue.terminate()

for firasim in firasim_list:
    firasim.terminate()
