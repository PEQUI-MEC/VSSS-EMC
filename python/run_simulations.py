#!/usr/bin/python3
import fast_simulation_multithread as simulations
import json
import itertools

with open("base_configs.json") as json_file:
    base_settings = json.load(json_file)

test_settings = base_settings.copy()

def product_of_params(inp):
    return (dict(zip(inp.keys(), values)) for values in itertools.product(*inp.values()))

params = {
    #"kgz": [0.2, 0.3, 0.4],
    #"default_target_velocity": [1.4],
    #"position_control_error_weight" : [10, 20],
    #"position_control_orientation_weight": [10, 30]
}

params_iter = product_of_params(params)

simulations.load_other_branch("fast_simulation")

for param in params_iter:
    print(f"testing parameters {param}")
    test_settings["Team"]["Parameters"] = param
    simulations.simulate_games(thread_count=12, game_multiplier=6, print_partial_results = True,
        this_configs = test_settings, other_configs = base_settings)

