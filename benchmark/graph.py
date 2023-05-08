#!/usr/bin/env python3

import matplotlib.pyplot as plt
import json
from pprint import pprint
from argparse import ArgumentParser
import numpy as np
from math import comb


def get_expected_stern(n, k, omega):
    p = int(0.002 * n) if int(0.002 * n) > 0 else 1
    t = comb(k // 2, )
    return comb(n, omega) / comb()

if __name__ == "__main__":
    parser = ArgumentParser()

    parser.add_argument("input_data")

    args = parser.parse_args()

    with open(args.input_data) as f:
        data = json.load(f)

    algs = ["stern", "MMT"]
    fig, ax = plt.subplots(nrows=2, ncols=1)

    d_iterations = {}
    d_duration = {}
    for alg in algs:
        if alg not in data:
            continue

        iterations, durations = [], []

        for info in data[alg]:
            iterations.append(info["iterations_count"])
            durations.append(info["duration"])

        
        d_iterations[alg + "_average"] = np.mean(iterations)
        d_duration[alg + "_average"] = np.mean(durations)

    ax[0].bar()

