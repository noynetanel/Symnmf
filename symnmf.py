import math
import sys
import symnmfmodule as sy
import numpy as np
import os

# Set seed for reproducibility
np.random.seed(1234)

def err():
    """Prints an error message and exits."""
    print('An Error Has Occurred')
    sys.exit(1)

def k_to_int(k):
    """Validate 'k' argument."""
    try:
        return int(k)
    except ValueError:
        err()

def check_file_name(file_name):
    """Validate 'file_name' argument."""
    if not file_name.endswith('.txt') or not os.path.isfile(file_name):
        err()
    return file_name

def check_goal_value(goal):
    """Validate 'goal' argument."""
    valid_goals = ['symnmf', 'sym', 'ddg', 'norm']
    if goal not in valid_goals:
        err()
    return goal

def output_matrix(matrix):
    """Output matrix rows as comma separated values."""
    for row in matrix:
        formatted_values = [f"{value:.4f}" for value in row]
        print(",".join(formatted_values))

def data_points_from_file(file_name):
    """Load data points from a file into a list of lists."""
    data = []
    try:
        with open(file_name, 'r') as file:
            for line in file:
                data.append([float(x) for x in line.split(",")])
    except FileNotFoundError:
        print("An Error Has Occured")
        sys.exit(1)
    except Exception as e:
        print("An Error Has Occured")
        sys.exit(1)
    return data

def init_of_H(W, k, n):
    """Initialize matrix H of size (n x k) with random values."""
    m = np.mean(W)
    upper_bound = 2 * np.sqrt(m / k)
    H = np.random.uniform(0, upper_bound, size=(n, k))
    return H.tolist()

def main():
    if len(sys.argv) < 4:
        err()
    # Read and validate arguments
    k = k_to_int(sys.argv[1])
    goal = check_goal_value(sys.argv[2])
    file_name = check_file_name(sys.argv[3])
    
    # Load data and set dimensions
    X = data_points_from_file(file_name)
    n, d = len(X), len(X[0])
   
    # Perform operation based on goal
    if goal == "sym":
        res = sy.sym(X)
    elif goal == "symnmf":
        W = sy.norm(X)
        H = init_of_H(W, k, n)
        res = sy.symnmf(k, W, H)
    elif goal == "ddg":
        res = sy.ddg(X)
    elif goal == "norm":
        res = sy.norm(X)
    else:
        err()
    
    output_matrix(res)

if __name__ == "__main__":
    main()
