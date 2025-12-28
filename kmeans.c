#include <stdlib.h>
#include <math.h>
#define _GNU_SOURCE 
#include <stdio.h>

#define ERROR_NUM_CLUSTERS "Incorrect number of clusters!"
#define ERROR_MAX_ITER "Incorrect maximum iteration!"
#define ERROR_OCCURED "An Error Has Occurred"
#define MAX_ITER_DEFAULT 400  /* Default maximum iterations */ 
#define EPS 0.001

struct vector;
struct cord;


int isInteger(char *str);
double compute_distance(const struct vector *v1, const struct vector *v2, int dim);
int find_dim(const struct vector *vec);
struct vector *initialize_centroids(const struct vector *head_vec, int K, int dim);
int find_closest_centroid(const struct vector *centroids, const struct vector *vectorX, int K, int dim);
void print_the_result(struct vector *centroids_for_print, int K, int dim );
void reset_vectors_to_zero(struct vector *head_vec);
struct vector *add_coordinates_from_other_vector(struct vector *v1, struct vector *v2, int dim);
struct vector *initialize_sum_vectors(int K, int dim);
void free_vector_list(struct vector *head_vec);




struct cord
{
    double value;
    struct cord *next;
};
struct vector
{
    struct vector *next;
    struct cord *cords;
};


int main(int argc, char **argv)
{  
    int K, max_iter;

    struct vector *head_vec, *curr_vec;
    struct cord *head_cord, *curr_cord;
    int i, j, p, z, y, iter, i_for_epsilon, y_for_epsilon;
    int N = 0;
    int dim = 0;
    double n;
    char c;

    struct vector *centorids;
    int *count_in_cluster;
    struct vector *sum_vectors;
    int closest_centorid_index;
    struct vector *curr_vector_X;
    struct vector *curr_sum_cluster;
    struct cord *curr_cord_in_sum;
    struct vector *last_centroids;

    struct vector *curr_centorid; 
    struct vector *curr_last_centroids; 
    struct cord *curr_cord_centroid;
    struct cord *curr_cord_last_centroid;
    struct cord *first_vec_cord;
    int flag;
    struct vector *temp_vec;
    struct vector *to_delete;
    

    /* The error when the user entered more or less arguments tham needed */
    if (argc < 2 || argc > 3) { 
        printf("%s\n", ERROR_OCCURED); 
        return 1;
    }

    if(!isInteger(argv[1])) {
        printf("%s\n", ERROR_NUM_CLUSTERS);
        return 1;
    }

    K = atoi(argv[1]);

    if (argc == 3) {
        max_iter = atoi(argv[2]);
    } else {
        max_iter = MAX_ITER_DEFAULT; /* If no arguments we put 400 */
    }

    if (K <= 1) { 
        printf("%s\n", ERROR_NUM_CLUSTERS);
        return 1;
    }
    if (max_iter <= 1 || max_iter >= 800) {
        printf("%s\n", ERROR_MAX_ITER);
        return 1;
    }

    
    head_cord = malloc(sizeof(struct cord));

    if (head_cord == NULL) {
        printf("%s\n", ERROR_OCCURED); 
        return 1; 
    }

    curr_cord = head_cord;
    curr_cord->next = NULL;

    head_vec = malloc(sizeof(struct vector));

    if (head_vec == NULL) {
        printf("%s\n", ERROR_OCCURED); 
        return 1; 
    }

    curr_vec = head_vec;
    curr_vec->next = NULL;


    while (scanf("%lf%c", &n, &c) == 2)
    {

        if (c == '\n')
        {
            curr_cord->value = n;
            curr_vec->cords = head_cord;
            curr_vec->next = malloc(sizeof(struct vector));
            curr_vec = curr_vec->next;
            curr_vec->next = NULL;
            head_cord = malloc(sizeof(struct cord));
            curr_cord = head_cord;
            curr_cord->next = NULL;
            N++;
            continue;
        }

       

        curr_cord->value = n;
        curr_cord->next = malloc(sizeof(struct cord));
        curr_cord = curr_cord->next;
        curr_cord->next = NULL;
        
    }

    temp_vec = head_vec;
    
        /* Iterate to the N-th vector (the last valid one) */
    for(i = 0; i < N - 1; i++) {
        temp_vec = temp_vec->next;
    }

        /* If there is an extra node after the N-th vector, delete it */
    if (temp_vec->next != NULL) {
        to_delete = temp_vec->next;
        
        /* Free the unused coordinate head allocated for the non-existent line */
        free(head_cord); 
        
        /* Free the empty vector struct */
        free(to_delete);
        
        /* Set next to NULL so the list ends properly */
        temp_vec->next = NULL;
    }

    if(K >= N) {
       printf("%s\n", ERROR_NUM_CLUSTERS); 
       return 1;
    }

    dim = find_dim(head_vec);

    centorids = initialize_centroids(head_vec, K, dim);
    sum_vectors = initialize_sum_vectors(K,dim);

    count_in_cluster = calloc(K, sizeof(int));

    curr_vector_X = head_vec;
    curr_sum_cluster = sum_vectors;

    for(iter = 0; iter<max_iter; iter++) {

        curr_sum_cluster = sum_vectors;

        for(j=0; j<K; j++) count_in_cluster[j] = 0;

        for(i = 0; i<N; i++) {
                closest_centorid_index = find_closest_centroid(centorids, curr_vector_X, K, dim);
                for(p = 0; p<closest_centorid_index; p++) {
                    curr_sum_cluster = curr_sum_cluster->next;
                }
                curr_sum_cluster = add_coordinates_from_other_vector(curr_sum_cluster, curr_vector_X, dim);
                count_in_cluster[closest_centorid_index]++;
                /*if(curr_vector_X->next == NULL) {
                    break;
                } */
                curr_vector_X = curr_vector_X->next;
                curr_sum_cluster = sum_vectors;


        }
    curr_sum_cluster = sum_vectors;
    for(z = 0; z < K; z++)
    {
        curr_cord_in_sum = curr_sum_cluster->cords;
        
        if(count_in_cluster[z] != 0) {
            /* Normal case: Divide sum by number of points */
            for(y = 0; y < dim; y++) {
                curr_cord_in_sum->value = curr_cord_in_sum->value / count_in_cluster[z];
                curr_cord_in_sum = curr_cord_in_sum->next;
            }
        }
        else {
            /* Empty Cluster case: Copy coordinates from the very first input vector */
            first_vec_cord = head_vec->cords;
            for(y = 0; y < dim; y++) {
                curr_cord_in_sum->value = first_vec_cord->value;
                curr_cord_in_sum = curr_cord_in_sum->next;
                first_vec_cord = first_vec_cord->next;
            }
        }
        
        /* Move to the next cluster in the sum_vectors list */
        curr_sum_cluster = curr_sum_cluster->next;
    }
    last_centroids = centorids;
    centorids = initialize_centroids(sum_vectors, K, dim);
    

    curr_centorid = centorids;
    curr_last_centroids = last_centroids;
    flag = 1;
    for(i_for_epsilon = 0; i_for_epsilon<K; i_for_epsilon++) {
        curr_cord_centroid = curr_centorid->cords;
        curr_cord_last_centroid = curr_last_centroids->cords;
        /*we need to check previous_centorid[i] - new_centroid[i] < epsilon */
        for(y_for_epsilon = 0; y_for_epsilon<dim; y_for_epsilon++) {
            if(fabs(curr_cord_centroid->value-curr_cord_last_centroid->value)>=EPS) {
                flag = 0;
                break;
            }
            curr_cord_centroid = curr_cord_centroid->next;
            curr_cord_last_centroid = curr_cord_last_centroid->next;
            
        }

        if(!flag) {
            break;
        }
    }

    if(flag) {
        free_vector_list(last_centroids);
        break;
    }
    
    curr_vector_X = head_vec;
    reset_vectors_to_zero(sum_vectors);
    free_vector_list(last_centroids);


    }
    
    print_the_result(centorids, K, dim);

    free(count_in_cluster);
    free_vector_list(centorids);
    free_vector_list(head_vec);
    free_vector_list(sum_vectors);
 
    return 0;
}

void free_vector_list(struct vector *head_vec) {
    struct vector *curr_vec = head_vec;
    struct vector *next_vec;
    struct cord *curr_cord, *next_cord;

    while (curr_vec != NULL) {
        curr_cord = curr_vec->cords;
        while (curr_cord != NULL) {
            next_cord = curr_cord->next;
            free(curr_cord);
            curr_cord = next_cord;
        }
        next_vec = curr_vec->next;
        free(curr_vec);
        curr_vec = next_vec;
    }
}


void print_the_result(struct vector *centroids_for_print, int K, int dim ) {
    struct vector *curr_vec = centroids_for_print;
    struct cord *curr_cord;
    int i, j;

    for (i = 0; i < K; i++) {
        /* Safety check: stop if list is shorter than K */
        if (curr_vec == NULL) break;

        curr_cord = curr_vec->cords;
        for (j = 0; j < dim; j++) {
            /* Safety check: stop if coords are missing */
            if (curr_cord == NULL) break;

            printf("%.4f", curr_cord->value);

            /* Print comma only if we are NOT at the last coordinate */
            if (j < dim - 1) {
                printf(",");
            }

            curr_cord = curr_cord->next;
        }
        
        printf("\n");
        curr_vec = curr_vec->next;
    }
}


void reset_vectors_to_zero(struct vector *head_vec) {
    struct vector *curr_vec = head_vec;
    struct cord *curr_cord;

    while (curr_vec != NULL) {
        curr_cord = curr_vec->cords;
        while (curr_cord != NULL) {
            curr_cord->value = 0.0;
            curr_cord = curr_cord->next;
        }
        curr_vec = curr_vec->next;
    }
}


struct vector *add_coordinates_from_other_vector(struct vector *v1, struct vector *v2, int dim) {
    struct cord *curr_cord1 = v1->cords;
    const struct cord *curr_cord2 = v2->cords;
    int i;

    for (i = 0; i < dim; i++) {
        /* Add the value from the second vector to the first */
        curr_cord1->value += curr_cord2->value;

        /* Move to the next coordinate */
        curr_cord1 = curr_cord1->next;
        curr_cord2 = curr_cord2->next;
    }

    return v1; /* Return the modified vector (useful for chaining) */
}

struct vector *initialize_sum_vectors(int K, int dim) {
    struct vector *head_vector_zero, *curr_vector_zero;
    struct cord *head_cord_zero, *curr_cord_zero;
    int i,j;

    head_vector_zero = malloc(sizeof(struct vector));
    curr_vector_zero = head_vector_zero;
    curr_vector_zero->next = NULL;

    for(i = 0; i<K; i++) {

        head_cord_zero = malloc(sizeof(struct cord));
        curr_cord_zero = head_cord_zero;
        curr_cord_zero->next = NULL;

        for(j = 0; j<dim; j++) {
            curr_cord_zero->value = 0.0;
            if(j<dim-1) {
                curr_cord_zero->next = malloc(sizeof(struct cord));
                curr_cord_zero = curr_cord_zero->next;
                curr_cord_zero->next = NULL;
            }

        }

        curr_vector_zero->cords = head_cord_zero;

        if(i<K-1) {
            curr_vector_zero->next = malloc(sizeof(struct vector));
            curr_vector_zero = curr_vector_zero->next; /* Move forward */
            curr_vector_zero->next = NULL;
        }
    }
    return head_vector_zero;
}

struct vector *initialize_centroids(const struct vector *head_vec, int K, int dim) {
    struct vector *head_centroid, *curr_centroid;
    struct cord *head_cord, *curr_cord;

    /* Use const pointers to safely read from input without modifying it */
    const struct vector *curr_initial_vec = head_vec;
    const struct cord *curr_initial_cord = head_vec->cords;

    int i,j;

    /* allocate the first coordinate and centroid nodes */
    head_cord = malloc(sizeof(struct cord));
    curr_cord = head_cord;
    curr_cord->next = NULL;

    head_centroid = malloc(sizeof(struct vector));
    curr_centroid = head_centroid;
    curr_centroid->next = NULL;

    
    for(i=0; i<K;i++) {
        /* Inner Loop: Deep copy coordinates for the current vector */
        for(j = 0; j<dim; j++) {
            curr_cord->value = curr_initial_cord->value;
            curr_initial_cord = curr_initial_cord->next;

            /* Only allocate next node if NOT at the last dimension */
            if (j < dim - 1) {
                curr_cord->next = malloc(sizeof(struct cord));
                curr_cord = curr_cord->next;
                curr_cord->next = NULL;
            }
        }

        /* Attach copied coordinates to current centroid */
        curr_centroid->cords = head_cord;

        /* Move source vector pointer forward */
        curr_initial_vec = curr_initial_vec->next;

        /* Only prepare next centroid if NOT at the last cluster */
        if (i < K - 1) {
            curr_centroid->next = malloc(sizeof(struct vector));
            curr_centroid = curr_centroid->next;
            curr_centroid->next = NULL;
            
            /* Prepare head_cord for the NEXT iteration */
            head_cord = malloc(sizeof(struct cord));
            curr_cord = head_cord;
            curr_cord->next = NULL;
            
            /* Safely reset cord source pointer for the next vector
            We do this here to avoid accessing NULL on the final iteration */
            curr_initial_cord = curr_initial_vec->cords;
        }
    }

    return head_centroid;
}


double compute_distance(const struct vector *v1, const struct vector *v2, int dim) {

    /*
    Calculates the Euclidean distance between two vectors (points). It iterates through the 
    coordinates of both points, sums up the squared differences, and returns the square root 
    of that sum.
    */
    double diff;
    double cord_val1, cord_val2;
    double distance;
    struct cord *curr_cord1, *curr_cord2;
    int i;
    double sum_dist = 0.0;
    curr_cord1 = v1->cords;
    curr_cord2 = v2->cords;
    

    for(i = 0; i<dim; i++) {

        if (curr_cord1 == NULL || curr_cord2 == NULL) {
            return 0.0; 
        }

        cord_val1 = (*curr_cord1).value;
        cord_val2 = (*curr_cord2).value;
        diff = cord_val1 - cord_val2;
        sum_dist += diff * diff;
        curr_cord1 = curr_cord1->next;
        curr_cord2 = curr_cord2->next;
    }
    distance = sqrt(sum_dist);
    return distance;
}



int find_dim(const struct vector *vec)
{
    int dim = 0;
    const struct cord *c;

    if (vec == NULL) {
        return 0;
    }

    c = vec->cords;
    while (c != NULL) {
        dim++;
        c = c->next;
    }

    return dim;
}


int isInteger(char *str) {

    if (str == NULL || *str == '\0') {
        return 0; 
    }
    
    while (*str) {
        if (*str < '0' || *str > '9') {
            return 0; 
        }
        str++;
    }
    return 1; 
}


int find_closest_centroid(const struct vector *centroids, const struct vector *vectorX, int K, int dim) {
    int min_index = 0;
    int i;
    const struct vector *curr_centorid = centroids;
    double distance, min_distance;
    min_distance = compute_distance(curr_centorid, vectorX, dim);
    curr_centorid = curr_centorid->next;
    for(i = 1; i<K; i++) {
        distance = compute_distance(curr_centorid, vectorX, dim);
        if(distance<min_distance) {
            min_distance = distance;
            min_index = i;
        }
        curr_centorid = curr_centorid->next;
    }
    return min_index;
}

