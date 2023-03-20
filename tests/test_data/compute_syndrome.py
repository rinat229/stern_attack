#!/usr/bin/env python3

from argparse import ArgumentParser
from pathlib import Path
import numpy as np

def compute_syndrome(input_matrix_text, input_codeword_text) -> str:
    codeword = np.array([int(i) for i in input_codeword_text])
    bin_matrix = np.array([[int(i) for i in row] for row in input_matrix_text], dtype=np.uint8)
    str_ans = np.apply_along_axis(lambda x: x%2, 0, bin_matrix@codeword.T).tolist()
    str_ans = [str(i) for i in str_ans]

    return "".join(str_ans)


if __name__ == "__main__":
    parser = ArgumentParser(
        prog='Generating syndrome',
        description="example:\n./gen_syndrome.py small/check_matrix.txt small/codeword.txt small/syndrome.txt"
    )

    parser.add_argument("input_matrix")
    parser.add_argument("input_codeword")
    parser.add_argument("output_syndrome")

    args = parser.parse_args()

    input_matrix_text = Path(args.input_matrix).read_text().split("\n")
    input_codeword_text = Path(args.input_codeword).read_text().strip()

    Path(args.output_syndrome).write_text(compute_syndrome(input_matrix_text, input_codeword_text))