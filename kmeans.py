import math
import sys

def k_means(K, input_data, iter=300):
    """
       k_mean clustering algorithm
    """
    # 1: Initialize centroids as first k datapoints: µk = xk, ∀k ∈ K
    data_points = read_data(input_data)
    N = len(data_points)
    epsilon = 0.01
    prev_centroids = data_points[:K]
    X = data_points

    # until convergence
    for _ in range(iter):
        clusters = [[] for _ in range(K)]
        for Xi in X:
            # Assign every xi i to the closest cluster
            distances = [euclidean_distance(Xi, centroid) for centroid in prev_centroids]
            min_index_cluster = distances.index(min(distances))
            clusters[min_index_cluster].append(Xi)
        # Update the centroids to the mean
        new_centroids = []
        for cluster in clusters:
            if cluster:
                new_centroids.append([sum(dim) / len(cluster) for dim in zip(*cluster)])
            else:
                new_centroids.append([0] * len(X[0]))

        delta_centroid = [euclidean_distance(new_centroid, prev_centroid) for new_centroid, prev_centroid in
                          zip(new_centroids, prev_centroids)]
        if max(delta_centroid) < epsilon:
            break
        prev_centroids = new_centroids.copy()
    return new_centroids


def euclidean_distance(p, q):
    """
       calculate euclidean distance
    """
    squared_sum = 0
    for i in range(len(p)):
        squared_sum += (p[i] - q[i]) ** 2
    sum = math.sqrt(squared_sum)
    return sum


def read_data(file_name):
    """
       read data from file
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


if __name__ == "__main__":
    try:
        k = int(sys.argv[1])
        if len(sys.argv) == 4:
            input_data = sys.argv[3]
        else:
            input_data = sys.argv[2]
        data_points = read_data(input_data)
        N = len(data_points)
        assert 1 < k and k < N
    except:
        print("Invalid number of clusters!")
        sys.exit(1)

    try:
        if len(sys.argv) == 4:
            iter = int(sys.argv[2])
            assert 1 < iter and iter < 1000
        else:
            iter = 300
    except:
        print("Invalid maximum iteration!")
        sys.exit(1)

    try:
        k_means(k, input_data, iter)
    except:
        print("An Error Has Occured")
        sys.exit(1)

    sys.exit(0)