#!/usr/bin/python3
import subprocess
import re
import time
import sys
import json
from git import Repo
import shutil
import os

running_games = 0
completed_games = 0
thread_count = 12
games = thread_count * 6

other_branch = "fast_simulation"

def get_other_code():
    path = os.path.abspath("/root/src/other_code")
    if os.path.isdir(path):
        repo = Repo(path)
        repo.git.checkout(other_branch)
        repo.git.pull()
        pass
    else:
        os.mkdir(path)
        repo = Repo.clone_from("https://github.com/PEQUI-MEC/VSSS-EMC.git", path)
        repo.git.checkout(other_branch)
    build_path = os.path.join(path, "build")
    if not os.path.isdir(build_path):
        os.mkdir(build_path)
    os.chdir(build_path)
    os.system("cmake .. && make -j12")

referee_list = []
for i in range(thread_count):
    referee_list.append(None)
vsss_emc_list = []
firasim_list = []

ansi_escape = re.compile(r'\x1B(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])')

total_this = 0
total_other = 0
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

def run_vsss_emc(bin_name, config_filename):
    json_file = open(config_filename)
    configs = json.load(json_file)
    json_file.close()
    return subprocess.Popen([bin_name, json.dumps(configs), str(i+1)],
                                    stdout=subprocess.PIPE,
                                    stderr=subprocess.PIPE)

for i in range(thread_count):
    if i % 2 == 0:
        this_vsss = run_vsss_emc('/root/src/VSSS', 'yellow_headless.json')
        other_vsss = run_vsss_emc('/root/src/other_code/VSSS', 'blue_headless.json')
    else:
        this_vsss = run_vsss_emc('/root/src/VSSS', 'blue_headless.json')
        other_vsss = run_vsss_emc('/root/src/other_code/VSSS', 'yellow_headless.json')
    vsss_emc_list.append((this_vsss, other_vsss))
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

def process_and_quit(referee, i):
    global total_this, total_other
    output, errs = referee.communicate()
    data = parse_output(output)
    if "blue_goals" in data and "yellow_goals" in data:
        if i % 2 == 0:
            this_goals = int(data["yellow_goals"])
            other_goals = int(data["blue_goals"])
        else:
            this_goals = int(data["blue_goals"])
            other_goals = int(data["yellow_goals"])
        total_this += this_goals
        total_other += other_goals
        referee.terminate()
        return True, this_goals, other_goals
    else:
        print("could not find goals in output")
        return False, 0, 0

get_other_code()
print(f"Simulating {games} games with {thread_count} threads")
while games > completed_games:
    time.sleep(0.1)
    for i, referee in enumerate(referee_list):
        if is_done(referee):
            success, this_goals, other_goals = process_and_quit(referee, i)
            referee_list[i] = None
            running_games -= 1
            if success:
                completed_games += 1
                print(f"{completed_games}/{games} - this: {this_goals}, {other_branch}: {other_goals}, "
                    f"avg this: {total_this/completed_games:.2f}, avg {other_branch}: {total_other/completed_games:.2f}")
            else:
                print("failed to complete game")
        if games > (completed_games + running_games) and referee_list[i] is None:
            referee_list[i] = start_referee(i+1)
            running_games += 1

print(f"Average this: {total_this/games:.3f}")
print(f"Average {other_branch}: {total_other/games:.3f}")

for this_vsss, other_vsss in vsss_emc_list:
    this_vsss.terminate()
    other_vsss.terminate()

for firasim in firasim_list:
    firasim.terminate()
