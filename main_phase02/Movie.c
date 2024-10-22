/*****************************************************
 * @file   Movie.c                                   *
 * @author Paterakis Giorgos <geopat@csd.uoc.gr>     *
 *                                                   *
 * @brief Implementation for Movie.h 				 *
 * Project: Winter 2023						         *
 *****************************************************/
#include "Movie.h"


/*-------------------------functions for hashing---------------------------------*/
int a;
int b;
int prime_num;

/*function to check if a number it s prime*/
int is_prime(int num) {
    int i;

    if (num <= 1) {
        return 0;
    }

    for (i = 2; i * i <= num; ++i) {
        if (num % i == 0) {
            return 0;
        }
    }

    return 1;
}

void calculate_hashtable_size(int users) {
    hashtable_size = (int)(1.3 * users) + 1;

    while (!is_prime(hashtable_size)) {
        ++hashtable_size;
    }
}

int find_next_prime(int num) {
    int prime;
    int i;
    for(i = 0 ; i < 170 ;++i){
        if(primes_g[i] > num){
            prime = primes_g[i];
        }
    }
    return prime;

}

void init_hash_table(){
    int i;
    user_hashtable_p = malloc(hashtable_size * sizeof(user_t*));
    for(i = 0; i < hashtable_size; ++i){
        user_hashtable_p[i] = NULL;
    }
}

void init_hash(int maxid,int maxusers){

    prime_num = find_next_prime(maxid);
    a = rand() % prime_num;
    b = rand() % prime_num;
    calculate_hashtable_size(maxusers);
    init_hash_table();

}

int Universal_Hash_Function(int key){
    return (   (((a*key)+b)% prime_num )% hashtable_size   );
}


void print_user_list(void){
    int i;
    user_t *current;
    for(i = 0; i < hashtable_size; ++i){
        printf("Chain %d of Users\n",i);
        current = user_hashtable_p[i];
        while(current != NULL){
            printf("    User %d\n",current->userID);
            current = current->next;
        }

    }
    printf("DONE\n");

}
/*returns a pointer of the user if exists*/
user_t *user_lookup(int userID){
    int hash_value;
    user_t *current;

    hash_value = Universal_Hash_Function(userID);
    current = user_hashtable_p[hash_value];

    while(current != NULL){
        if(current->userID == userID){
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/**
 * @brief Creates a new user.
 * Creates a new user with userID as its identification.
 *
 * @param userID The new user's identification
 *
 * @return 1 on success
 *         0 on failure
 */

int register_user(int userID){
    int hash_value;
    user_t *new_user;
    user_t *current;
    printf("R %d\n", userID);

    if(user_lookup(userID) == NULL) {
         hash_value = Universal_Hash_Function(userID);
        new_user = malloc(sizeof(user_t));
        new_user->userID = userID;
        new_user->history = NULL;
        new_user->next = NULL;
        if (user_hashtable_p[hash_value] == NULL) {
            user_hashtable_p[hash_value] = new_user;
        } else {
            current = user_hashtable_p[hash_value];
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = new_user;
        }
        print_user_list();
        return 1;

    }
    else {
        printf("User %d already exists\n", userID);
        return 0;
    }
}

void delete_history_tree(userMovie_t *root){
    if(root != NULL){
        delete_history_tree(root->lc);
        delete_history_tree(root->rc);
        free(root);
    }
}
 
/**
 * @brief Deletes a user.
 * Deletes a user with userID from the system, along with users' history tree.
 *
 * @param userID The new user's identification
 *
 * @return 1 on success
 *         0 on failure
 */

int unregister_user(int userID){
    user_t *current;
    user_t *previous;
    if(user_lookup(userID) == NULL){
        printf("User %d does not exist\n",userID);
        return 0;
    }
    printf("U %d\n", userID);
    int hash_value = Universal_Hash_Function(userID);
    current = user_hashtable_p[hash_value];
    previous = NULL;
    while(current != NULL){
        if(current->userID == userID){
            if(previous == NULL){
                user_hashtable_p[hash_value] = current->next;
            }
            else{
                previous->next = current->next;
            }
            //delte history tree
            delete_history_tree(current->history);
            free(current);
            print_user_list();
            return 1;
        }
        previous = current;
        current = current->next;
    }
    print_user_list();
    return 1;
}


/*-------------------------functions for new_movies---------------------------------*/

struct new_movie* create_new_movie(int movieID, int category, int year){
    new_movie_t *movie = (new_movie_t *)malloc(sizeof(new_movie_t));
    movie->category = category;
    movie->movieID = movieID;
    movie->year = year;
    movie->watchedCounter = 0;
    movie->sumScore = 0;
    movie->lc = NULL;
    movie->rc = NULL;
    return movie;
}

new_movie_t* lookup_new_movie(new_movie_t *root, int movieID) {
    while (root != NULL) {
        if (root->movieID == movieID) {
            return root;
        } else if (root->movieID > movieID) {
            root = root->lc;
        } else {
            root = root->rc;
        }
    }
    return NULL; // Movie not found
}

void print_new_movies_inorder(new_movie_t *root){
    if(root != NULL){
        print_new_movies_inorder(root->lc);
        printf("%d, ", root->movieID );
        print_new_movies_inorder(root->rc);
    }
}

 /**
 * @brief Add new movie to new release binary tree.
 * Create a node movie and insert it in 'new release' binary tree.
 *
 * @param movieID The new movie identifier
 * @param category The category of the movie
 * @param year The year movie released
 *
 * @return 1 on success
 *         0 on failure
 */

 int add_new_movie(int movieID, int category, int year){
     new_movie_t *new_movie;
     new_movie_t *current;
     printf("A %d %d %d\n", movieID, category, year);
     if(lookup_new_movie(new_releases, movieID) != NULL){
         printf("Movie %d already exists\n", movieID);
         return 0;
     }
     new_movie = create_new_movie(movieID, category, year);

     printf("New releases Tree: \n");
     printf("   <new_releases>: ");
     if(new_releases == NULL){

         new_releases = new_movie;

         print_new_movies_inorder(new_releases);
         printf("\n");
         printf("DONE\n");
         return 1;
     }
     current = new_releases;
     while(1){


         if(current->movieID > movieID){
             if(current->lc == NULL){

                 current->lc = new_movie;
                 print_new_movies_inorder(new_releases);
                 printf("\n");
                    printf("DONE\n");
                 return 1;
             }
             current = current->lc;
         }
         else{
             if(current->rc == NULL){

                 current->rc = new_movie;
                 print_new_movies_inorder(new_releases);
                 printf("\n");
                 printf("DONE\n");

                 return 1;
             }
             current = current->rc;
         }
     }

 }

/*--------------------MOVIES----------------------------------------*/

int add_movie(int category,int movieID ,int year){
    movie_t *current;
    movie_t *newmovie;
  current = categoryArray[category]->movie;
    if(lookup_movie(current,movieID) != NULL){
        printf("Movie %d already exists\n", movieID);
        return 0;
    }

    newmovie = create_movie(movieID,year);
    if(current == NULL){
        //printf("mphke gia insert\n");
        current = newmovie;
        return 1;
    }

    while(1){
        if(current->movieID > movieID){
            if(current->lc == NULL){
                current->lc = newmovie;
                return 1;
            }
            current = current->lc;
        }
        else{
            if(current->rc == NULL){
                current->rc = newmovie;
                return 1;
            }
            current = current->rc;
        }
    }
}

void insert_movie(movie_t **root, movie_t *newMovie) {
    if (*root == NULL || *root == sentinel) {
        *root = newMovie;
    } else {
        if (newMovie->movieID < (*root)->movieID) {
            insert_movie(&(*root)->lc, newMovie);
        } else {
            insert_movie(&(*root)->rc, newMovie);
        }
    }
}

new_movie_t* delete_newmovie(new_movie_t *root, int movieID) {
    new_movie_t *temp;
    if (root == NULL) {
        return root;
    }

    if (movieID < root->movieID) {
        root->lc = delete_newmovie(root->lc, movieID);
    } else if (movieID > root->movieID) {
        root->rc = delete_newmovie(root->rc, movieID);
    } else {
        if (root->lc == NULL) {
            temp = root->rc;
            free(root);
            return temp;
        } else if (root->rc == NULL) {
            temp = root->lc;
            free(root);
            return temp;
        }
        temp = root->rc;
        while (temp->lc != NULL) {
            temp = temp->lc;
        }
        root->movieID = temp->movieID;
        root->category = temp->category;
        root->rc = delete_newmovie(root->rc, temp->movieID);
    }
    return root;
}

//calculate how many new movies have the category i want
//create a new array with new movie pointers size of numMovies
//inorder traversal of new releases tree and insert the new movies in the array

void calculate_moviesincat_inorder(new_movie_t *root, int *numMovies,int category) {
    if (root != NULL) {
        calculate_moviesincat_inorder(root->lc, numMovies ,category);
        if(root->category == category){
            (*numMovies)++;
        }
        calculate_moviesincat_inorder(root->rc, numMovies, category);
    }
}

void insert_newmovies_inorder(new_movie_t *root, new_movie_t **array, int *index,int category){
    if (root != NULL) {
        insert_newmovies_inorder(root->lc, array, index,category);
        if(root->category == category){
            array[*index] = root;
            (*index)++;
        }
        insert_newmovies_inorder(root->rc, array, index,category);
    }
}
movie_t* sortedArrayToBST(new_movie_t *arr[], int start, int end)
{
    /* Base Case */
    if (start > end)
        return sentinel;

    /* Get the middle element and make it root */
    int mid = (start + end) / 2;
    movie_t* root = create_movie(arr[mid]->movieID, arr[mid]->year);




    /* Recursively construct the left subtree
    and make it left child of root */
    root->lc = sortedArrayToBST(arr, start, mid - 1);

    /* Recursively construct the right subtree
    and make it right child of root */
    root->rc = sortedArrayToBST(arr, mid + 1, end);

    return root;
}


void delete_all_new_movies(new_movie_t **root) {
    int i;
    int numMovies[6] = {0, 0, 0, 0, 0, 0};
    int index[6] = {0, 0, 0, 0, 0, 0};
    new_movie_t **array[6];


    for(i = 0; i < 6; i++){
        calculate_moviesincat_inorder(*root, &numMovies[i], i);
        array[i] = (new_movie_t **)malloc(numMovies[i] * sizeof(new_movie_t*));
        insert_newmovies_inorder(*root, array[i], &index[i], i);

    }
    for(i = 0; i < 6; i++){
        categoryArray[i]->movie = sortedArrayToBST(array[i], 0, numMovies[i] - 1);
    }



}

struct movie* create_movie(int movieID, int year) {
    struct movie *movie = (movie_t *) malloc(sizeof(movie_t));
    movie->movieID = movieID;
    movie->year = year;
    movie->watchedCounter = 0;
    movie->sumScore = 0;
    movie->lc = sentinel;
    movie->rc = sentinel;
    return movie;
}

struct movie *lookup_movie(struct movie *root, int movieID) {
    sentinel->movieID = movieID;

    while (root -> movieID != movieID) {
        //printf("Movie id : %d\n", root->movieID);
        if (root->movieID > movieID) {
            root = root->lc;
        } else {
            root = root->rc;
        }
    }
    //printf("Movie id : %d\n", root->movieID);
    sentinel->movieID = -1;
    if (root == sentinel) {
        return NULL;
    }
    return root;

}

void print_movies_inorder(struct movie *root) {
    if (root != NULL && root != sentinel) {
        print_movies_inorder(root->lc);
        printf("%d ", root->movieID);
        print_movies_inorder(root->rc);
    }
}

void print_movie_category() {
    for(int i = 0; i < 6; i++){
        printf("    Category %s : ", movieCategoryStr[i]);
        print_movies_inorder(categoryArray[i]->movie);
        printf("\n");
    }
}

 /**
 * @brief Distribute the movies from new release binary tree to the array of categories.
 *
 * @return 0 on success
 *         1 on failure
 */

 int distribute_movies(void){

     delete_all_new_movies(&new_releases);
     printf("Movie Categories Array: \n");
     print_movie_category();
     printf("DONE\n");

     return 1;
 }





/*-------------------- USERS HISTORY ----------------------------------------*/

void insertLeafOriented(userMovie_t** root, int movieID, int category, int score) {
    userMovie_t* cur = *root, *prev = NULL, *newNode, *newRoot;

    if (*root == NULL) {
        *root = malloc(sizeof(userMovie_t));
        (*root)->movieID = movieID;
        (*root)->category = category;
        (*root)->score = score;
        (*root)->parent = NULL;
        (*root)->lc = NULL;
        (*root)->rc = NULL;
        return;
    }

    while (cur->lc != NULL && cur->rc != NULL) {
        prev = cur;
        if (cur->movieID >= movieID)
            cur = cur->lc;
        else
            cur = cur->rc;
    }

    newNode = malloc(sizeof(userMovie_t));
    newNode->movieID = movieID;
    newNode->category = category;
    newNode->score = score;
    newNode->parent = NULL;
    newNode->lc = NULL;
    newNode->rc = NULL;

    newRoot = malloc(sizeof(userMovie_t));
    if (movieID < cur->movieID) {
        newRoot->movieID = movieID;
        newRoot->category = category;
        newRoot->score = score;
        newRoot->parent = NULL;
        newRoot->lc = newNode;
        newRoot->rc = cur;
    } else {
        newRoot->movieID = cur->movieID;
        newRoot->category = cur->category;
        newRoot->score = cur->score;
        newRoot->parent = NULL;
        newRoot->lc = cur;
        newRoot->rc = newNode;
    }

    if (prev == NULL)
        *root = newRoot;
    else if (prev->lc == cur)
        prev->lc = newRoot;
    else
        prev->rc = newRoot;
}


void FindNextLeaf_Print(userMovie_t *root)
{
    /*if node is null, return*/
    if (!root)
        return;

    /*if node is leaf node, print its data*/
    if (!root->lc && !root->rc)
    {
        printf("    < %d, %d >\n", root->movieID, root->score);
        return;
    }

    /*if left child exists, check for leaf
    recursively*/
    if (root->lc)
        FindNextLeaf_Print(root->lc);

    /*if right child exists, check for leaf
    recursively*/
    if (root->rc)
        FindNextLeaf_Print(root->rc);
}

/**
 * @brief User rates the movie with identification movieID with score
 *
 * @param userID The identifier of the user
 * @param category The Category of the movie
 * @param movieID The identifier of the movie
 * @param score The score that user rates the movie with id movieID
 *
 * @return 1 on success
 *         0 on failure
 */

int watch_movie(int userID,int category, int movieID, int score){
        movie_t *movie;
        struct user *temp_user;
        printf("W %d %d %d %d\n", userID, category, movieID, score);
       movie = lookup_movie(categoryArray[category]->movie, movieID);
        if(movie == NULL){
            printf("Movie %d does not exist\n", movieID);
            return 0;
        }
        temp_user = user_lookup(userID);
        if(temp_user == NULL){
            printf("User %d does not exist\n", userID);
            return 0;
        }
        movie->watchedCounter++;
        movie->sumScore += score;
        insertLeafOriented(&temp_user->history, movieID, category, score);

        printf("History of user %d : \n", userID);
        FindNextLeaf_Print(temp_user->history);
        printf("DONE\n");


	 return 1;
 }







void heapify(movie_t arr[], int n, int i) {
     int largest = i;
     int left = 2 * i + 1;
     int right = 2 * i + 2;

     if ( arr[left].watchedCounter > 0 && arr[largest].watchedCounter > 0 && left < n && (arr[left].sumScore/arr[left].watchedCounter) > (arr[largest].sumScore/arr[largest].watchedCounter) )
         largest = left;

     if (arr[right].watchedCounter > 0 && arr[largest].watchedCounter > 0 && right < n && (arr[right].sumScore/arr[right].watchedCounter) > (arr[largest].sumScore/arr[largest].watchedCounter) )
         largest = right;

     if (largest != i) {
         /*Swap*/
         movie_t temp = arr[i];
         arr[i] = arr[largest];
         arr[largest] = temp;

         /*Recursively heapify the affected sub-tree*/
         heapify(arr, n, largest);
     }
 }

void heapSort(movie_t arr[], int n) {
    /*Build max heap*/
    movie_t temp;
    int i;
    for (i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    /* Extract elements one by one from the heap*/
    for (i = n - 1; i > 0; i--) {
        /*Move current root to end*/
        temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;

        /*Call max heapify on the reduced heap*/
        heapify(arr, i, 0);
    }
}

void calculate_numMovies_inorder(struct movie *root, int *numMovies,int score) {
    if (root != NULL && root->movieID != -1) {
        calculate_numMovies_inorder(root->lc, numMovies, score);

        if(  root->watchedCounter != 0 && (  (root->sumScore) / (root->watchedCounter)  ) > score ){
            (*numMovies)++;
        }
        calculate_numMovies_inorder(root->rc, numMovies, score);
    }
}


void insert_movies_inorder(struct movie *root, struct movie *array, int score, int *index) {
    if (root != NULL && root->movieID != -1) {
        insert_movies_inorder(root->lc, array,  score,index);

        if( root->watchedCounter != 0 && (  (root->sumScore) / (root->watchedCounter)  ) > score ){
            //insert it in the array
            array[(*index)] = *root;
            (*index)++;
        }
        insert_movies_inorder(root->rc, array, score,index);
    }
}

/*event F*/
 int filter_movies(int userID, int score){

     int numMovies = 0;
     int numMovies2 = 0;
     struct movie *array;
     int index ;
     struct user *temp_user;
     int i;
     temp_user = user_lookup(userID);

     if(temp_user == NULL){
         printf("User %d does not exist\n", userID);
         return 0;
     }

     for(int i = 0; i < 6; i++){
            struct movie *root = categoryArray[i]->movie;
            calculate_numMovies_inorder(root, &numMovies, score);
     }
     printf("   Movies found: %d\n", numMovies);
     if(numMovies == 0){
         printf("   No movies found\n");
         printf("DONE\n");
         return 1;
     }

     array = (struct movie *)malloc(numMovies * sizeof(struct movie));
     index = 0;
     for(int i = 0; i < 6; i++){
         struct movie *root = categoryArray[i]->movie;
         insert_movies_inorder( root,array, score,&index);
     }

     heapSort(array, numMovies);
     printf("   ");
     for(i = 0; i < numMovies; i++){
         if(array[i].watchedCounter != 0) {
             printf("< %d , %d > ,", array[i].movieID, array[i].sumScore / array[i].watchedCounter);
         }
     }
     printf("\n");




     printf("DONE\n");
     return 1;
 }



 /*Returns the sum of the scores of the movies in the tree*/
 int FindNextLeaf_SumScore(userMovie_t *root,int *sum,int *counter){
    // if node is null, return
    if (!root)
        return 0;

    /* if node is leaf node, print its data*/
    if (!root->lc && !root->rc)
    {
        *sum += root->score;
        *counter += 1;
        return 1;
    }

    /*if left child exists, check for leaf
    recursively*/
    if (root->lc)
        FindNextLeaf_SumScore(root->lc,sum,counter);

    /* if right child exists, check for leaf
     recursively*/
    if (root->rc)
        FindNextLeaf_SumScore(root->rc,sum,counter);
    return 1;


 }
/**
 * @brief Find movies from categories withn median_score >= score t
 *
 * @param userID The identifier of the user
 * @param category Array with the categories to search.
 * @param score The minimum score the movies we want to have
 *
 * @return 1 on success
 *         0 on failure
 */

 int user_stats(int userID){
	 //we find the user
        struct user *temp_user = user_lookup(userID);
        int scoresum = 0;
        int counter = 0;
        int totalscore = 0;
        if(temp_user == NULL){
            printf("User %d does not exist\n", userID);
            return 0;
        }
        userMovie_t *temp = temp_user->history;

        if(temp == NULL){
            printf("Q %d %d \n", userID, totalscore);
            printf("User %d has not watched any movies\n", userID);
            return 1;
        }
        FindNextLeaf_SumScore(temp,&scoresum,&counter);
        totalscore = scoresum/counter;
        printf("Q %d %d \n", userID, totalscore);
        printf("    User with ID %d has watched %d movies and score %d\n", userID, counter, totalscore);
        printf("DONE\n");

        return 1;

 }
 
/**
 * @brief Search for a movie with identification movieID in a specific category.
 *
 * @param movieID The identifier of the movie
 * @param category The category of the movie
 *
 * @return 1 on success
 *         0 on failure
 */

 int search_movie(int movieID, int category){
    struct movie *temp;
     temp = lookup_movie(categoryArray[category]->movie, movieID);
        if (temp != NULL) {
            printf("I movieID %d , movieCategory %s, year %d\n", temp->movieID, movieCategoryStr[category], temp->year);
        } else {
            printf("Movie not found\n");
        }
	 return 1;
 }
 
 /**
 * @brief Prints the movies in movies categories array.
 * @return 1 on success
 *         0 on failure
 */

 int print_movies(void){
    printf("Event M \n");
    printf("Movie Categories Array: \n");
    print_movie_category();
    printf("DONE\n");
     return 1;
 }
 
  /**
 * @brief Prints the users hashtable.
 * @return 1 on success
 *         0 on failure
 */

 int print_users(void){
     /*take each chain of the hashtable*/
        int i;
      struct user *temp;
        for(i=0; i<hashtable_size; i++){
            temp = user_hashtable_p[i];
            printf("Chain %d of Users\n", i);
            while(temp != NULL){
                printf("  User ID: %d\n", temp->userID);
                printf("    History Tree: \n");

                FindNextLeaf_Print(temp->history);

                temp = temp->next;
            }

        }
        printf("DONE\n");

      return 1;
 }
 
