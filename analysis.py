import sys
import math
import os
import numpy as np
import kmeans
from sklearn.metrics import silhouette_score
import symnmfmodule as sy 
np.random.seed(1234)

def err():
    print("An Error Has Occurred")
    sys.exit(1)

def k_to_int(k):
    try:
        return int(k)
    except:
        err()

def check_file_name(file_name):
    if not file_name.endswith('.txt'):
        err()
    if not os.path.isfile(file_name):
        err()
    return file_name

def Kmeans(K, input_data, iter=300):
    centroids = kmeans.k_means(K, input_data, iter=300)
    return centroids
    
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


    
def symnmf(X, k, n):
    # Calculate W
    W = sy.norm(X) 
    # Initialize H matrix
    H = init_of_H(W, k, n).tolist()
    try:
        return sy.symnmf(k, W, H)
    except:
        err()




def max_indices(matrix): # matrix is list of lists 
    max_ind = []
    for lst in matrix:
        if len(lst) == 0:
            max_ind.append(None) # empty list
        else:
            max_ind.append(lst.index(max(lst)))
    return max_ind
        
def euclidean_distance(p, q):
    """
       calculate euclidean distance
    """
    squared_sum = 0
    for i in range(len(p)):
        squared_sum += (p[i] - q[i]) ** 2
    sum = math.sqrt(squared_sum)
    return sum

def silhouette_score_calc(data_points, labels):
    if len(np.unique(labels)) > 1:
        return silhouette_score(data_points, labels)
    else:
        return -1 # if there is 1 cluster then the score is not defined 

def data_points_centroid_ind(data_points, centroids):
    indices = [] # get a list of the indices of the closet centroids to the data_points 
    for point in data_points:
        min_dist = float('inf')
        closet_centroid_index = 0
        for i in range(len(centroids)):
            curr_dist= euclidean_distance(point, centroids[i])
            if(curr_dist < min_dist):
                min_dist = curr_dist
                closet_centroid_index = i
        indices.append(closet_centroid_index)
    return indices

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

def main():
    if len(sys.argv) < 3:
        err()
    k = k_to_int(sys.argv[1])
    file_name = check_file_name(sys.argv[2])
    data_points = data_points_from_file(file_name)
    n = len(data_points)
    centroids_of_kmeans = Kmeans(k,file_name)
    labels_of_kmeans = data_points_centroid_ind(data_points, centroids_of_kmeans)
    H_matrix_symnmf = symnmf(data_points, k, n)
    labels_of_symnmf = max_indices(H_matrix_symnmf)
    # print the results 
    print("nmf: %.4f" % silhouette_score_calc(data_points, labels_of_symnmf))
    print("kmeans: %.4f" % silhouette_score_calc(data_points, labels_of_kmeans))

if __name__ == "__main__":
    main()