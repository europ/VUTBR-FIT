/**
 * IZP
 * 20.12.2015
 * Adrian Toth
 * xtotha01
 * Dokumentacia zdrojoveho kodu proj3.c
 */

/**
 * @defgroup G1 Functions working on clusters.
 * @defgroup G2 Functions working on array of clusters.
 */

/**
 * @brief      Structure of object. Object has id, x co-ordinate, y co-ordinate.
 */
struct obj_t {
    int id;
    float x;
    float y;
};

/**
 * @brief      Structure of cluster. Cluster has size, capacity, pointer on obj_t-type structure/structures.
 */
struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/**
 * @addtogroup G1
 * @{
 */

/**
 * @brief      This function initializes cluster 'c'. Allocates memory for 'cap' capacity objects. Pointer NULL at
 *             array of objects stands for capacity 0.
 *
 * @param      c     pointer on cluster_t-type structure
 * @param[in]  cap   integer value substitute capacity of cluster
 *
 * @pre      c != NULL
 * @pre      cap >= 0
 *
 * @post     c->size == 0
 * @post     c->capacity == cap
 * @post     c->obj != NULL
 */
void init_cluster(struct cluster_t *c, int cap);

/**
 * @brief      Function is removing all objects from cluster 'c' and initializes cluster 'c' on an empty cluster.
 *
 * @param      c     pointer on cluster_t-type structure
 *
 * @post     c->size == 0
 * @post     c->capacity == 0
 * @post     c->obj == NULL
 */
void clear_cluster(struct cluster_t *c);

/**
 * Chunk of cluster objects. Value recommended for reallocation.
 */
extern const int CLUSTER_CHUNK;

/**
 * @brief      Changing the capacity of cluster 'c' on capacity 'new cap'.
 *
 * @param      c        pointer on cluster_t-type structure
 * @param[in]  new_cap  new capacity value
 *
 * @pre      c != NULL
 * @pre      c->capacity >= 0
 * @pre      new_cap >= 0
 *
 * @return     pointer on new clusters_t-type structure
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap);

/**
 * @brief      Add object 'obj' at the end of cluster 'c'. Extend the cluster, if the object cannot fit in.
 *
 * @param      c     pointer on cluster_t-type structure
 * @param[in]  obj   obj_t-type structure
 */
void append_cluster(struct cluster_t *c, struct obj_t obj);

/**
 * @brief      To cluster 'c1' adds objects from cluster 'c2'. Cluster 'c1' if need required will be extended. Objects
 *             in cluster 'c1' will be sort by decreasing their identification number. Cluster 'c2' will be unchanged.
 *
 * @param      c1    pointer on cluster_t-type structure
 * @param      c2    pointer on cluster_t-type structure
 *
 * @pre      c1 != NULL
 * @pre      c2 != NULL
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2);

/*@}*/

/**
 * @addtogroup G2
 * @{
 */

/**
 * @brief      Removes a cluster from array of clusters 'carr'. Array of clusters includes 'narr' entries (cluster).
 *             Cluster for remove is located on the index 'idx'. Function returns a new amount of clusters in array.
 *
 * @param      carr  pointer on array of cluster_t-type structures
 * @param[in]  narr  array size
 * @param[in]  idx   index in array representing a cluster for remove
 *
 * @pre      idx < narr
 * @pre      narr > 0
 *
 * @return     new array size
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx);

/**
 * @brief      Counts the Euclidean distance between two objects.
 *
 * @param      o1    pointer on obj_t-type structure
 * @param      o2    pointer on obj_t-type structure
 *
 * @pre      o1 != NULL
 * @pre      o2 != NULL
 *
 * @return     distance between two objects
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2);

/**
 * @brief      Counts the distance between two clusters. Distance is calculated on the basis of closest neighbour.
 *
 * @param      c1    pointer on cluster_t-type structure
 * @param      c2    pointer on cluster_t-type structure
 *
 * @pre      c1 != NULL
 * @pre      c2 != NULL
 * @pre      c1->size > 0
 * @pre      c2->size > 0
 *
 * @return     distance between two clusters
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2);

/**
 * @brief      The function finds two closest clusters in array of cluster_t-type structures 'carr' of size 'narr'.
 *             Function found two closest clusters identified by their indices in the 'carr' array. Indices will be
 *             saved to memory on adress of 'c1' and 'c2'.
 *
 * @param      carr  pointer on array of cluster_t-type structures
 * @param[in]  narr  array size
 * @param      c1    pointer on integer representing array index equal to position of 'c1'
 * @param      c2    pointer on integer representing array index equal to position of 'c2'
 *
 * @pre      narr > 0
 * @pre      carr != NULL
 * @pre      c1 != NULL
 * @pre      c2 != NULL
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2);

/**
 * @brief      Sorts objects in cluster 'c' by decreasing their identification number.
 *
 * @param      c     pointer on cluster_t-type structure
 */
void sort_cluster(struct cluster_t *c);

/**
 * @brief      Function prints cluster.
 *
 * @param      c     pointer on first object of array of cluster_t-type structures
 */
void print_cluster(struct cluster_t *c);

/**
 * @brief      Function from file named by 'filename' loads ojects. For every object makes cluster and save him to
 *             array of clusters. Allocates space for array of clusters and the pointer on first object of array of
 *             clusters saves to memory where 'arr' refers to. Function returns count of loaded clusters. In case of
 *             any error save to memory where 'arr' refers to NULL value.
 *
 * @param      filename  pointer on string presenting name of file
 * @param      arr       pointer on pointer on cluster_t-type structure
 *
 * @pre      arr != NULL
 *
 * @return     count of loaded clusters
 */
int load_clusters(char *filename, struct cluster_t **arr);

/**
 * @brief      Function prints array of cluster_t-type structures.
 *
 * @param      carr  pointer on first cluster_t-type structure in array of cluster_t-type structures
 * @param[in]  narr  array size
 */
void print_clusters(struct cluster_t *carr, int narr);

/*@}*/