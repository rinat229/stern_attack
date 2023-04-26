#!/usr/bin/env python3

from argparse import ArgumentParser
from pathlib import Path
import numpy as np


def generate_matrix(input_matrix_text) -> str:
    bin_matrix = np.array([[int(i) for i in row] for row in input_matrix_text], dtype=np.uint8)
    print(bin_matrix.shape)
    bin_matrix = np.hstack([np.eye(bin_matrix.shape[1]), bin_matrix.T])
    str_ans = ["".join([str(int(i)) for i in row]) for row in bin_matrix]

    return "\n".join(str_ans)


if __name__ == "__main__":
    parser = ArgumentParser()

    parser.add_argument("input_matrix")
    parser.add_argument("output_matrix")

    args = parser.parse_args()

    input_matrix_text = Path(args.input_matrix).read_text().split("\n")

    Path(args.output_matrix).write_text(generate_matrix(input_matrix_text))