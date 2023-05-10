#!/usr/bin/env python3

import matplotlib.pyplot as plt
import json
from pprint import pprint
from argparse import ArgumentParser
import numpy as np
from math import comb
import pandas as pd


class GetParams:
    def __init__(self, params) -> None:
        self.params = params
        self.n = params["n"]
        self.k = params["k"]
        self.omega = params["omega"]

    def _get_expected_stern(self, params_optimized):
        p = int(params_optimized["p"]) * 2
        t = comb(self.k // 2, p // 2)
        t1 = comb(self.n - self.k - params_optimized["l"], self.omega - p)
        return comb(self.params["n"], self.params["omega"]) / (t * t * t1)
    
    def _get_expected_mmt(self, params_optimized):
        p = int(params_optimized["p"]) * 4
        t = comb((self.k + params_optimized["l1"] + params_optimized["l2"]) // 2, p // 2)
        t1 = comb(self.n - self.k - params_optimized["l1"] - params_optimized["l2"], self.omega - p)
        return comb(self.params["n"], self.params["omega"]) / (t * t * t1)
    
    def _get_expected_fs_isd(self, params_optimized):
        p = int(params_optimized["p"]) * 2
        t = comb((self.k + params_optimized["l"]) // 2, p // 2)
        t1 = comb(self.n - self.k - params_optimized["l"], self.omega - p)
        return comb(self.params["n"], self.params["omega"]) / (t * t * t1)
    
    def get_expected(self, alg: str, params_optimized: dict) -> int:
        if alg == "stern" or alg == "stern_hash":
            return self._get_expected_stern(params_optimized)
        if alg == "FS_ISD":
            return self._get_expected_fs_isd(params_optimized)
        elif alg == "MMT":
            return self._get_expected_mmt(params_optimized)
        else:
            raise ValueError(f"unknown alg {alg}")
        

if __name__ == "__main__":
    parser = ArgumentParser()

    parser.add_argument("input_data")

    args = parser.parse_args()

    with open(args.input_data) as f:
        data = json.load(f)

    algs = ["stern", "MMT", "FS_ISD", "stern_hash", "MMT_hash"]

    d_iterations = {}
    d_duration = {}
    d_expected = {}

    params = GetParams(data["params"])

    for alg in algs:
        if alg not in data:
            continue

        iterations, durations = [], []

        for info in data[alg]:
            iterations.append(info["iterations_count"])
            durations.append(info["duration"])

        
        d_iterations[alg + "_average"] = np.mean(iterations)
        d_duration[alg + "_average"] = np.mean(durations)
        d_expected[alg] = params.get_expected(alg, data[alg + "_params"])

    df_iters = pd.DataFrame({
        'algorithm': algs, 
        "average number of iterations": d_iterations.values(), 
        "expected number of iterations": d_expected.values()
    })

    df_durs = pd.DataFrame({'algorithm': algs, "average time(ms)": d_duration.values()})

    ax_iters = df_iters.plot(kind='bar', x='algorithm')
    ax_durs = df_durs.plot(kind='bar', x='algorithm')

    plt.show()