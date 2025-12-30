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
struct vector *add_coordinates_from_other_vector(struct vector *v1, struct vector *v2, int dim);
struct vector *initialize_sum_vectors(int K, int dim);
void divide_vector_by_scalar(struct vector *v, int scalar);
void free_vector_list(struct vector *head_vec); 
void zero_out_vector(struct vector *v);




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

    int i, j, iter, changed;
    int N = 0, dim = 0;
    double n;
    char c;
    int closest_index;

    struct vector *head_vec=NULL, *curr_vec=NULL;
    struct cord *head_cord=NULL, *curr_cord=NULL;

    struct vector *centroids=NULL;
    struct vector *next_centroids=NULL;
    struct vector *sum_vectors=NULL;

    int *count_in_cluster=NULL;
    struct vector *curr_vector_X=NULL;
    struct vector *curr_sum = NULL;
    struct vector *curr_cent = NULL;
    struct vector *curr_next_cent = NULL;


    /*the variables we use once somewhere and then don't*/
    struct vector *temp = NULL;
    struct vector *prev = NULL;

    

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
        if(!isInteger(argv[2])) {
            printf("%s\n", ERROR_MAX_ITER);
            return 1;
        }
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


    head_vec = malloc(sizeof(struct vector));
    if(!head_vec) {
        printf("%s\n", ERROR_OCCURED);
        return 1;
    }

    curr_vec = head_vec;
    curr_vec->next = NULL;

    head_cord = malloc(sizeof(struct cord));
    if (!head_cord) {
        printf("%s\n", ERROR_OCCURED); 
        free(head_vec);
        return 1; 
    }

    curr_cord = head_cord;
    curr_cord->next = NULL;
    curr_vec->cords = head_cord;

    while (scanf("%lf%c", &n, &c) == 2)
    {
        curr_cord->value = n;

        if (c == '\n')
        {
            N++;
            /* Allocate memory for the next vector in the linked list */
            curr_vec->next = malloc(sizeof(struct vector));
            if(curr_vec->next == NULL) {
                printf("%s\n", ERROR_OCCURED);
                free_vector_list(head_vec); /* Free all memory allocated so far */
                return 1;
            }
            
            curr_vec = curr_vec->next;
            curr_vec->next = NULL;
            
            /* CRITICAL: Initialize cords to NULL immediately. 
               If the next malloc fails, free_vector_list needs this to be NULL 
               to avoid accessing uninitialized memory (segfault). */
            curr_vec->cords = NULL; 

            /* Allocate the first coordinate for the new vector */
            head_cord = malloc(sizeof(struct cord));
            if(head_cord == NULL) {
                printf("%s\n", ERROR_OCCURED);
                free_vector_list(head_vec); /* Safe to call because cords is NULL */
                return 1;
            }
            
            curr_cord = head_cord;
            curr_cord->next = NULL;
            curr_vec->cords = head_cord; /* Link the coordinates to the vector */
            continue;
        }

        /* Allocate memory for the next coordinate in the current vector */
        curr_cord->next = malloc(sizeof(struct cord));
        if(curr_cord->next == NULL) {
            printf("%s\n", ERROR_OCCURED);
            free_vector_list(head_vec); /* Free everything on failure */
            return 1;
        }
        curr_cord = curr_cord->next;
        curr_cord->next = NULL;
    }

    /*removing the last NULL vector*/
    temp = head_vec;
    prev = NULL;
    for(i = 0; i<N; i++) {
        prev = temp;
        temp = temp->next;
    }

    /*release the last empty vector*/
    if(prev != NULL) {
        prev->next = NULL;
        free_vector_list(temp);
    }
    if(N == 0) {
        printf("%s\n", ERROR_OCCURED); 
        free_vector_list(head_vec);
        return 1;
    }

    if(K >= N) {
       printf("%s\n", ERROR_NUM_CLUSTERS); 
       free_vector_list(head_vec);
       return 1;
    }

    dim = find_dim(head_vec);

    /*initialization*/
    centroids = initialize_centroids(head_vec, K, dim);
    if(centroids == NULL) {
         printf("%s\n", ERROR_NUM_CLUSTERS); 
         free_vector_list(head_vec);
         return 1;
    }

    sum_vectors = initialize_sum_vectors(K,dim);
    if(sum_vectors == NULL) {
         printf("%s\n", ERROR_NUM_CLUSTERS); 
         free_vector_list(head_vec);
         free_vector_list(centroids);
         if(count_in_cluster != NULL) {
            free(count_in_cluster);
         }
         return 1;
    }
    count_in_cluster = calloc(K, sizeof(int));
    if(!count_in_cluster) {
        printf("%s\n", ERROR_OCCURED);
        free_vector_list(head_vec);
        free_vector_list(centroids);
        free_vector_list(sum_vectors);
        return 1;
    }


    for(iter = 0; iter<max_iter; iter++) {

        /*initialize the counter with 0's*/
        for(j=0; j<K; j++) count_in_cluster[j] = 0;

        /*initialize the sum vector with 0's*/
        curr_sum = sum_vectors;
        while(curr_sum != NULL) {
            zero_out_vector(curr_sum);
            curr_sum = curr_sum->next;
        }

        curr_vector_X = head_vec;

        while(curr_vector_X != NULL) {
            closest_index = find_closest_centroid(centroids, curr_vector_X, K,dim);

            curr_sum = sum_vectors;
            for(j = 0; j<closest_index; j++) {
                curr_sum = curr_sum->next;
            }

            add_coordinates_from_other_vector(curr_sum, curr_vector_X, dim);
            count_in_cluster[closest_index]++;
            curr_vector_X = curr_vector_X->next;
        }


        /*compute new centroids*/
        next_centroids = initialize_centroids(sum_vectors, K, dim);
        if(next_centroids == NULL) {
            printf("%s\n", ERROR_OCCURED);

            free_vector_list(head_vec);
            free_vector_list(centroids);
            free_vector_list(sum_vectors);
            if(count_in_cluster != NULL) {
                free(count_in_cluster);
            }
            return 1;
        }

        curr_next_cent = next_centroids;
        curr_sum = sum_vectors;

        for(j=0; j<K; j++) {
            if(count_in_cluster[j] != 0) {
                divide_vector_by_scalar(curr_next_cent, count_in_cluster[j]);
            } else {
                /*case of empty cluster, create a deep copy of coordinates of the first input vector*/
                struct cord *source_cord = head_vec->cords;
                struct cord *destination_cord = curr_next_cent->cords;

                while(source_cord != NULL && destination_cord != NULL) {
                    destination_cord->value = source_cord->value;
                    source_cord = source_cord->next;
                    destination_cord = destination_cord->next;
                }
            }
            curr_next_cent = curr_next_cent->next;
        }

        
        /*checking convergence*/
        changed = 0;
        curr_cent = centroids;
        curr_next_cent = next_centroids;

        for(j = 0; j<K; j++) {
            if(compute_distance(curr_cent, curr_next_cent, dim) >= EPS) {
                changed = 1;
            }
            curr_cent = curr_cent->next;
            curr_next_cent = curr_next_cent->next;
        }

    
        /*change centorids*/
        free_vector_list(centroids);
        centroids = next_centroids;

        if(!changed) {
            break;
        }
    }
    

    print_the_result(centroids, K, dim);

    /*release memory*/
    free(count_in_cluster);
    free_vector_list(centroids);
    free_vector_list(head_vec);
    free_vector_list(sum_vectors);
 
    return 0;
}



void free_vector_list(struct vector *head_vec) {
    struct vector *curr_vec = head_vec;
    struct vector *next_vec;
    struct cord *curr_c, *next_c;

    while (curr_vec != NULL) {
        curr_c = curr_vec->cords;
        while (curr_c != NULL) {
            next_c = curr_c->next;
            free(curr_c);
            curr_c = next_c;
        }
        next_vec = curr_vec->next;
        free(curr_vec);
        curr_vec = next_vec;
    }
}


void print_the_result(struct vector *centroids_for_print, int K, int dim ) {
    const struct vector *curr_vec = centroids_for_print;
    const struct cord *curr_cord;
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


void zero_out_vector(struct vector *v) {
    struct cord *c = v->cords;

    while (c != NULL) {
        c->value = 0.0;
        c = c->next;
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

void divide_vector_by_scalar(struct vector *v, int scalar) {
    struct cord *c = v->cords;
    while(c != NULL) {
        c->value /= scalar;
        c = c->next;
    }
}


struct vector *initialize_sum_vectors(int K, int dim) {
    struct vector *head_v=NULL, *curr_v=NULL, *new_v;
    struct cord *head_c, *curr_c;
    int i,j;

    for(i = 0; i<K; i++) {
        new_v = malloc(sizeof(struct vector));
        if(new_v == NULL) {
            free_vector_list(head_v);
            return NULL;
        }

        new_v->next = NULL;

        head_c = malloc(sizeof(struct cord));
        if(head_c == NULL) {
            free(new_v);
            free_vector_list(head_v);
            return NULL;
        }
        curr_c = head_c;

        for(j = 0; j<dim; j++) {
            curr_c->value = 0.0;
            if(j<dim-1) {
                curr_c->next = malloc(sizeof(struct cord));

                if(curr_c->next == NULL) {
                    new_v->cords = head_c;
                    if(head_v == NULL) {
                        head_v = new_v;
                    } else {
                        curr_v->next = new_v;
                    }
                     free_vector_list(head_v);
                     return NULL;
                }

                curr_c = curr_c->next;
            } else {
                curr_c->next = NULL;
            }

        }

       new_v->cords = head_c;

       if(head_v == NULL) {
          head_v = new_v;
          curr_v = head_v;
       } else {
          curr_v->next = new_v;
          curr_v = curr_v->next;
       }

    }
    return head_v;
}



struct vector *initialize_centroids(const struct vector *head_vec, int K, int dim) {
    struct vector *head_centroid=NULL, *curr_centroid=NULL, *new_centroid;
    const struct vector *source_v = head_vec;
    struct cord *head_cord, *curr_cord;
    const struct cord *source_c;
    int i,j;
    
    for(i=0; i<K && source_v != NULL;i++) {
        new_centroid = malloc(sizeof(struct vector));
        if(new_centroid == NULL) {
            free_vector_list(head_centroid);
            return NULL;
        }
        new_centroid->next = NULL;
        source_c = source_v->cords;
        head_cord = malloc(sizeof(struct cord));
        if(head_cord == NULL) {
            free(new_centroid);
            free_vector_list(head_centroid);
            return NULL;
        }
        curr_cord = head_cord;

        for(j=0; j<dim; j++) {
            curr_cord->value = source_c->value;
            source_c = source_c->next;
            
            if(j <dim -1) {
                curr_cord->next = malloc(sizeof(struct cord));
                if(curr_cord->next == NULL) {
                    new_centroid->cords = head_cord;
                    
                    if(head_centroid == NULL) {
                        head_centroid = new_centroid;
                    } else {
                        curr_centroid->next = new_centroid;
                    }

                    free_vector_list(head_centroid);
                    return NULL;
                }
                curr_cord = curr_cord->next;

            } else {
                curr_cord->next = NULL;
            }
        }

           new_centroid->cords = head_cord;
           
           if(head_centroid == NULL) {
            head_centroid = new_centroid;
            curr_centroid = head_centroid;
           } else {
            curr_centroid->next = new_centroid;
            curr_centroid = curr_centroid->next;
           }

        source_v = source_v->next;   
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
    const struct vector *curr_centroid = centroids;
    double distance, min_distance;
    min_distance = compute_distance(curr_centroid, vectorX, dim);
    curr_centroid = curr_centroid->next;
    for(i = 1; i<K; i++) {
        distance = compute_distance(curr_centroid, vectorX, dim);
        if(distance<min_distance) {
            min_distance = distance;
            min_index = i;
        }
        curr_centroid = curr_centroid->next;
    }
    return min_index;
}
