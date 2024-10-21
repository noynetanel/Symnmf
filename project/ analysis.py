import sys
import math
import numpy as np
import os
import kmeans
import sklearn.metrices import silhouette_score
import utils
import symnmfmodule as sy 

def err():
    print("An Error Has Occurred")
    exit()

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

def kmeans(K, input_data, iter=300):
    centroids = kmeans.k_means(K, input_data, iter=300)
    return centroids

def symnmf(n, d, data_points, k):
    # Calculate W
    W_flat = sy.normSilent(n, d, data_points) 
    W = np.reshape(W_flat, (n, n)) 
    
    # Initialize H matrix
    H = utils.init_H(W, k, n).tolist()
    W = W.tolist()
    try:
        return np.reshape(sy.calcSymnmf(n,k,W,H), (n,k)).tolist()
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

def silhouette_score_calc(data_points, centroids):
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

        
def main():
    if len(sys.argv) < 3:
        err()

    k = k_to_int(argv[1])
    file_name = check_file_name(argv[2])
    data_points = helpers.points_from_file()
    n = len(data_points)
    d = len(data_points[0])
    centroids_of_kmeans = kmeans(k,file_name)
    labels_of_kmeans = data_points_centroid_ind(data_points, centroids_of_kmeans)
    H_matrix_symnmf = symnmf(n, d, data_points, k)
    labels_of_symnmf = max_indices(H_matrix_symnmf)
    # print the results 
    print("nmf: %4f" % silhouette_score_calc(data_points,labels_of_symnmf))
    print("kmeans: %.4f" % silhouette_score_calc(data_points, labels_of_kmeans))

if name == "__main__":
    main()