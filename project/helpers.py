import sys
import numpy as np

np.random.seed(1234)

def data_points_from_file(file_name):
    """
    Get the data points from the file.
    Returns: list of lists - list of data points 
    """
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
    """
    Initialize the matrix H of size (n x k) with random values from the interval 
    [0, 2 * sqrt(m / k)], where m is the average of all entries in W.
    
    Arguments:
    W (numpy array): The matrix W.
    n (int): Number of rows of matrix H.
    k (int): Number of columns of matrix H.
    
    Returns:
    H (numpy array): The initialized matrix H of size (n x k).
    """
    # Step 1: Calculate the mean of all entries in W
    m = np.mean(W)
    
    # Step 2: Calculate the upper bound for random initialization
    upper_bound = 2 * np.sqrt(m / k)
    
    # Step 3: Initialize H with random values from the interval [0, upper_bound]
    H = np.random.uniform(0, upper_bound, size=(n, k))
    
    return H



