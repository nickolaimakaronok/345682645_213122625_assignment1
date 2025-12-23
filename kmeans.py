import sys
import math

ERROR_NUM_CLUSTERS = "Incorrect number of clusters!"
ERROR_MAX_ITER = "Incorrect maximum iteration!"
MAX_ITER_DEFAULT = 400  # Default maximum iterations

def main():
    # Parse Command Line Arguments
    # The command is: python3 kmeans.py K [iter] < input_data.txt(other file name)
    try:
        if len(sys.argv) < 2:
            print("An Error Has Occurred")
            sys.exit(1)

        # Parse K
        try:
            k = int(sys.argv[1])
            if k <= 0:
                print(ERROR_NUM_CLUSTERS)
                sys.exit(1)
        except ValueError:
            # If K is not a NATURAL number
            print(ERROR_NUM_CLUSTERS)
            sys.exit(1)

        # Parse iter (optional)
        max_iter = MAX_ITER_DEFAULT  # Default value
        if len(sys.argv) > 2:
            try:
                max_iter = int(sys.argv[2])
                if max_iter <= 0:
                    print(ERROR_MAX_ITER)
                    sys.exit(1)
            except ValueError:
                print(ERROR_MAX_ITER)
                sys.exit(1)

        # Validation
        # Note: We check 1 < iter < 800 
        if not (1 < max_iter < 800):
            print(ERROR_MAX_ITER)
            sys.exit(1)

    except Exception:
        print("An Error Has Occurred")
        sys.exit(1)

    # Read Input Data
    data_points = []
    data_points = read_input()

    #Final Validation for K
    N = len(data_points)
    if not (1 < k < N):
        print("Invalid number of clusters!")
        sys.exit(1)

    #print("CHECK CLUSTERS:", data_points) 

    #The first iteration

    arr_clusters = first_initialisation_of_clusters(data_points, k)




        





    # Print Output
    # Example of how to format:
    # for centroid in final_centroids:
    #     print(','.join(f'{coord:.4f}' for coord in centroid))



def first_initialisation_of_clusters(data_points, k):
    arr_clusters = []
    centroids = initialize_centroids(data_points, k)

    for i in range(k):
        new_arr = []
        new_arr.append(centroids[i])
        arr_clusters.append(new_arr)
    
    for vectorX in data_points[k:]:
        min_dis = compute_distance(vectorX, centroids[0])
        closest_centroid = centroids[0]
        centoid_num = -1
        closest_centroid_num = 0
        for centroidX in centroids:
            distance = compute_distance(vectorX, centroidX)
            centroid_num+=1
            if distance<min_dis:
                closest_centroid = centroidX
                min_dis = distance
                closest_centroid_num = centroid_num
                
        arr_clusters[closest_centroid_num].append(vectorX)  

    return arr_clusters      


def update_centers()

def reavaluate_centroids(arr_cluster):



def initialize_centroids(data_points, k):    # Initialize centroids by selecting the first k data points
    return data_points[:k]

def compute_distance(point1, point2): #Computing distances of two vectors(points)
    sum = 0
    for i in range(len(point1)):
        sum += (point1[i]-point2[i])**2
    
    distance = math.sqrt(sum)
    return distance

#def update_centroids()

        

def read_input():
    data_points = []
    try:
        for line in sys.stdin:
            if not line.strip():  # Skip empty lines
                continue
            vector = [float(x) for x in line.split(',')]
            data_points.append(vector)
    except Exception:
        print("An Error Has Occurred")
        sys.exit(1)
    return data_points


if __name__ == "__main__":
    main()