#!/usr/bin/python3
import subprocess
import re
import time
import sys
import json
from git import Repo
import shutil
import os

ansi_escape = re.compile(r'\x1B(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])')

def load_other_branch(other_branch):
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
    os.chdir("/root/src")

def to_full_configs(configs, team_color):
    full_configs = configs.copy()
    full_configs["Team"]["Color"] = team_color
    full_configs["Team"]["Strategy"] = "Manual Strategy"
    full_configs["Adversary"] = full_configs["Team"].copy()
    full_configs["Adversary"]["Strategy"] = "No Strategy"
    if team_color == 0:
        full_configs["Team"]["Inverted"] = False
        full_configs["Adversary"]["Color"] = 1
        full_configs["Adversary"]["Inverted"] = True
    else:
        full_configs["Team"]["Inverted"] = True
        full_configs["Adversary"]["Color"] = 0
        full_configs["Adversary"]["Inverted"] = False
    return full_configs

def run_vsss_emc(bin_name, configs, team_color, i):
    full_configs = to_full_configs(configs, team_color)
    return subprocess.Popen([bin_name, json.dumps(full_configs), str(i+1)],
                                    stdout=subprocess.PIPE,
                                    stderr=subprocess.PIPE)

def init_programs(thread_count, this_configs, other_configs):
    referee_list = []
    for i in range(thread_count):
        referee_list.append(None)
    vsss_emc_list = []
    firasim_list = []

    for i in range(thread_count):
        if i % 2 == 0: # even threads use yellow for this code and blue for other code
            this_vsss = run_vsss_emc('/root/src/VSSS', this_configs, 1, i)
            other_vsss = run_vsss_emc('/root/src/other_code/VSSS', other_configs, 0, i)
        else:
            this_vsss = run_vsss_emc('/root/src/VSSS', this_configs, 0, i)
            other_vsss = run_vsss_emc('/root/src/other_code/VSSS', other_configs, 1, i)
        vsss_emc_list.append((this_vsss, other_vsss))
        firasim_pipe = subprocess.Popen(['/root/FIRASim/bin/FIRASim', '-H', '--fast', '--id', str(i+1)],
                                        stdout=subprocess.PIPE,
                                        stderr=subprocess.PIPE)
        firasim_list.append(firasim_pipe)

    return referee_list, vsss_emc_list, firasim_list

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

def is_done(referee):
    return (referee is not None) and (referee.poll() is not None)

def start_referee(ref_id):
    return subprocess.Popen(['/root/VSSReferee/bin/VSSReferee',
                            '--3v3', '--record', 'false', '--fast', '--headless', '--id', str(ref_id)],
                            stdout=subprocess.PIPE)

def process_and_quit(referee, i):
    output, errs = referee.communicate()
    data = parse_output(output)
    if "blue_goals" in data and "yellow_goals" in data:
        if i % 2 == 0:
            this_goals = int(data["yellow_goals"])
            other_goals = int(data["blue_goals"])
        else:
            this_goals = int(data["blue_goals"])
            other_goals = int(data["yellow_goals"])
        referee.terminate()
        return True, this_goals, other_goals
    else:
        print("could not find goals in output")
        return False, 0, 0

def simulate_games(thread_count, game_multiplier, print_partial_results, this_configs, other_configs):
    running_games = 0
    completed_games = 0
    total_this = 0
    total_other = 0
    games = thread_count * game_multiplier

    referee_list, vsss_emc_list, firasim_list = init_programs(thread_count, this_configs, other_configs)

    print(f"Simulating {games} games with {thread_count} threads")
    while games > completed_games:
        time.sleep(0.1)
        for i, referee in enumerate(referee_list):
            if is_done(referee):
                success, this_goals, other_goals = process_and_quit(referee, i)
                referee_list[i] = None
                running_games -= 1
                if success:
                    total_this += this_goals
                    total_other += other_goals
                    completed_games += 1
                    if (print_partial_results):
                        print(f"{completed_games}/{games} - this: {this_goals}, other: {other_goals}, "
                            f"avg this: {total_this/completed_games:.2f}, avg other: {total_other/completed_games:.2f}")
                else:
                    print("failed to complete game")
            if games > (completed_games + running_games) and referee_list[i] is None:
                referee_list[i] = start_referee(i+1)
                running_games += 1

    print(f"Average this: {total_this/games:.3f}")
    print(f"Average other: {total_other/games:.3f}")

    for this_vsss, other_vsss in vsss_emc_list:
        this_vsss.terminate()
        other_vsss.terminate()

    for firasim in firasim_list:
        firasim.terminate()