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
double compute_distance(struct vector *v1, struct vector *v2, int dim);
int find_dim(const struct vector *vec);
struct vector *initialize_centroids(const struct vector *head_vec, int K, int dim);



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



    struct vector *head_vec, *curr_vec, *next_vec;
    struct cord *head_cord, *curr_cord, *next_cord;
    int i, j;
    int N = 0;
    int dim = 0;
    double n;
    char c;
    struct vector *centorids;

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

    if(K >= N) {
       printf("%s\n", ERROR_NUM_CLUSTERS); 
       return 1;
    }

    dim = find_dim(head_vec);

    centorids = initialize_centroids(head_vec, K, dim);


    

    return 0;
}


/*we want to create centroids as a list do a deep copy from the first K vectors*/
struct vector *initialize_centroids(const struct vector *head_vec, int K, int dim) {
    struct vector *centroidX;
    centroidX = malloc(sizeof(struct vector));
    centroidX = head_vec
    for()
}


double compute_distance(struct vector *v1, struct vector *v2, int dim) {

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