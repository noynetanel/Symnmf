import os
import sys
import synmfmodule as sy
import helpers
import numpy as np 

# Set seed for reproducibility
np.random.seed(1234)

def err():
    """
    Prints an error message and exit
    """
    print('An Error Has Occured')
    sys.exit(1)

def k_to_int(k):
    """
    Function to validate 'k' argument
    Returns:
        k (int): the number of clusters for symnmf if valid
        else: prints an err 
    """
    try:
        return int(k)
    except:
        err()

def check_file_name(file_name):
    """
    Function to validate 'file_name' argument
    Returns:
        file_name (str): the name of the input file if valid
        else: prints an err 
    """
    if not file_name.endswith('.txt'):
        err()
    if not os.path.isfile(file_name):
        err()
    return file_name

    
def check_goal_value(goal):
    """
    Function to validate 'goal' argument
    Returns:
        goal (str): The goal of the operation (symnmf, sym, ddg, norm).
    """
    valid_goals = ['symnmf', 'sym', 'ddg', 'norm']
    if goal not in valid_goals:
        err()    
    return goal

def sym_matrix(n, d, data_points):
    """
    Compute a symmetric similarity matrix for n points in d-dimensional space.
    
    Arguments:
    n      -- number of data points
    d      -- number of dimensions for each data point
    data_points -- n x d numpy array where each row is a data point in d-dimensional space
    
    Returns:
    A      -- n x n symmetric similarity matrix
    """
    sy.sym(n, d, data_points)
    
def compute_degree_matrix(similarity_matrix):
    """
    Compute the diagonal degree matrix D from the similarity matrix A.
    
    Arguments:
    similarity_matrix -- n x n numpy array, the similarity matrix A
    
    Returns:
    D -- n x n numpy array, the diagonal degree matrix
    """
    # Number of data points
    n = similarity_matrix.shape[0]
    
    # Calculates the sum of similarities for each data point (row)
    degree_values = np.sum(similarity_matrix, axis=1)
    
    # Creates the diagonal degree matrix D
    D = np.diag(degree_values)
    
    return D

def symnmf(n, d, data_points, k):
    """
    calculates symnmf
    Arguments:
    n (int): The total number of data points.
    d (int): The dimensional size of each data point.
    data_points (list): A list containing the data points.
    k (int): Number of clusters for symnmf 
    """
    # Calculate W
    W_flat = sy.normSilent(n, d, data_points) # we get a flattened version of the similarity matrix
    W = np.reshape(W_flat, (n, n)) # reshapes the flattened similarity matrix into a 2D array with dimensions n*n
    
    # Initialize H matrix
    H = helpers.init_of_H(W, k, n).tolist()
    W = W.tolist()
    try:
        return sy.symnmf(n, k, W, H)
    except:
        err()

def ddg (n, d, data_points):
    """
    Performs the ddg function.

    Arguments:
        n (int): The total number of data points.
        d (int): The dimensional size of each data point.
        data_points (list): A list containing the data points.
    """
    sy.ddg(n, d, points)

def norm(n, d, data_points):
    """
    Performs the norm function.

    Arguments:
        n (int): The total number of data points.
        d (int): The dimensional size of each data point.
        data_points (list): A list containing the data points.
    """
    sy.norm(n, d, data_points)
    
def output_matrix(matrix):
    """
    Output the required matrix separated by a comma,
    such that each row is in a line of its own 

    Arguments:
        matrix (list): the matrix we need to output
    """
    for row in matrix:
        formatted_values = [f"{value:.4f}" for value in row]  # Format each value to four decimal places
        print(",".join(formatted_values))  # Join the formatted values with commas and print

# notice we don't have normSilent function

def main():
    if len(sys.argv) < 4:
        err()
    # Read arguments
    k =  k_to_int(sys.argv[1])
    goal = check_goal_value(sys.argv[2])
    file_name = check_file_name(sys.argv[3])
    data_points = helpers.data_points_from_file(file_name)
    d = len(data_points[0])
    n = len(data_points)

    if goal == "sym":
        sym_matrix(n, d, data_points)
    elif goal == "symnmf":
        symnmf(n, d, data_points, k)
    elif goal =="ddg":
        ddg(n, d, data_points)
    elif goal =="norm":
        norm(n, d, data_points)
    else:
        err()
if name == "__main__":
    main()



