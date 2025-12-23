import sys
import math

ERROR_NUM_CLUSTERS = "Incorrect number of clusters!"
ERROR_MAX_ITER = "Incorrect maximum iteration!"
MAX_ITER_DEFAULT = 400  # Default maximum iterations
EPS = 0.0001

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

    dim = len(data_points[0])
    

    arr_centriods = initialize_centroids(data_points,k)
    for i in range(max_iter):
        arr_clusters = update_all_clusters(arr_centriods,data_points,k)
        arr_centriods_copy = arr_centriods.copy()
        arr_centriods = update_centers(arr_clusters, data_points, dim)
        counter = 0
        for index in range(k):
            distance = compute_distance(arr_centriods[index], arr_centriods_copy[index])
            if distance<EPS:
                counter+=1

        if counter == k:
            break
        


    
    for centroid in arr_centriods:
        formatted_coords = ['%.4f' % coord for coord in centroid]
        print(','.join(formatted_coords))
        
                    



        





    # Print Output
    # Example of how to format:
    # for centroid in final_centroids:
    #     print(','.join(f'{coord:.4f}' for coord in centroid))

   


def update_centers(arr_clusters, data_points, dim):
    arr_centroids = []
    counter = 0

    for clusterX in arr_clusters:
       

        k = len(clusterX)
        sum_point = [0.0 for i in range(dim)]
        for point in clusterX:
            for i in range(dim):
                sum_point[i]+=point[i]
        centroidX = [0.0 for i in range(dim)]        


        for i in range(len(sum_point)):
            if k>0:
                centroidX[i] = sum_point[i]/k
            else:
                centroidX = data_points[counter]    
                counter+=1
                break 
                
                
        arr_centroids.append(centroidX)
    return arr_centroids


def update_all_clusters(arr_centriods,data_points,k):
    
    arr_clusters = []
    for i in range(k):
        arr_clusters.append([])

    for vectorX in data_points:
        min_distance = compute_distance(arr_centriods[0], vectorX)
        number_of_min_centr = 0
        number_of_curr_centr = -1
        for centroidX in arr_centriods:
            number_of_curr_centr+=1
            distance = compute_distance(vectorX, centroidX)
            if distance < min_distance:
                min_distance = distance
                number_of_min_centr = number_of_curr_centr
        arr_clusters[number_of_min_centr].append(vectorX)
        
    return arr_clusters





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