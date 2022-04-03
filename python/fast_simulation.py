#!/usr/bin/python3
import subprocess
import re

#game_count = 10

ansi_escape = re.compile(r'\x1B(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])')

def parse_output(output):
    data = {}
    lines = output.splitlines()
    for line in lines:
        line = line.decode('unicode_escape')
        line = ansi_escape.sub('', line)
        parts = line.split(' ')
        if (len(parts) == 3 and parts[0] == "[STATS]"):
            data[parts[1]] = parts[2]
        print(parts)
    return data

#for game in range(game_count)
pipe = subprocess.Popen(['/home/thiago/Workspace/VSSReferee/bin/VSSReferee', '--3v3', '--record', 'false', '--fast'
                         , '--headless'
                         ]
                        #,stdout=subprocess.PIPE
                        )
output, errs = pipe.communicate()
#output = result.stdout.decode('utf-8')
print("done")
print(output)
data = parse_output(output)
print(data)
