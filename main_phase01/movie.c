/**/
/* Created by  Manolis Petrakis on 8/11/23.*/
/**/
#include "streaming_service.h"
#include "stdio.h"
#include "stdlib.h"



struct movie *Categories[6] ;


char *movieCategoryStr[6];

struct new_movie *new_movies_head = NULL;

struct user *userhead = NULL;


/*----------------------------------- EVENT R  --------------------------------------------*/

/*create user*/
struct user *createuser(int uid)
{
    struct user *newuser = (struct user *)malloc(sizeof(struct user));
    newuser->uid = uid;
    newuser->suggestedHead = NULL;
    newuser->suggestedTail = NULL;
    newuser->watchHistory = NULL;
    newuser->next = NULL;
    return newuser;
}

/*insert guard node*/
void insertGuard(struct user ** head) {
    struct user *newuser = createuser(-1);
    newuser->next = *head;
    *head = newuser;
}

/*look up user by id*/
struct user* userIdLookUp(int uid){
    struct user * current = userhead;
    while(current->uid != -1){
        if(current->uid == uid){
            return current;
        }
        current = current->next;
    }
    return current;

}

/*insert user at the beginning of the list*/
void insertFirst(struct user **head,int uid){
        struct user *new_user = createuser(uid);
        new_user->next = *head;
        *head = new_user;
        /*printf("User %d registered\n",uid);*/

}

/*print user list*/
void print_userR(struct user *head){
    struct user * current = head;
    printf("R %d \n" ,current->uid);
    printf("Users = ");
    while(current->uid != -1){
        printf(" %d ,",current->uid);
        current = current->next;

    }

    printf("\n  DONE \n");

}

/*register user*/

int register_user(int uid){
    if(userIdLookUp(uid)->uid == uid){
        printf("User %d already exists\n",uid);
        return -1;
    }
    else {
        insertFirst(&userhead, uid);
        print_userR(userhead);
        return 0;
    }

}
/*----------------------------------- EVENT U  --------------------------------------------*/

void print_userU(struct user *head){
    struct user * current = head;
    printf("Users = ");
    while(current->uid != -1){
        printf("<%d>,",current->uid);
        current = current->next;

    }

    printf("\n DONE \n");

}

void unregister_user(int uid){
    struct user *current = userhead;
    struct user *prev = NULL;
    while(current->uid != -1){
        if(current->uid == uid){

            /*CLEAR WATCHHISTORY STACK*/
            struct movie *temp = current->watchHistory;
            while(temp != NULL){
                struct movie *temp2 = temp;
                temp = temp->next;
                free(temp2);
            }

            /*CLEAR SUGGESTED MOVIES LIST*/
            struct suggested_movie *temp3 = current->suggestedHead;
            while(temp3 != NULL){
                struct suggested_movie *temp4 = temp3;
                temp3 = temp3->next;
                free(temp4);
            }

            if(prev == NULL){
                userhead = current->next;
            }
            else{
                prev->next = current->next;
            }
            free(current);
            printf(" U %d \n" , uid);
            print_userU(userhead);
            return;

        }

        prev = current;
        current = current->next;

    }
    printf("User %d does not exist\n",uid);
}


/*----------------------------------- EVENT A  --------------------------------------------*/


/*create a new movie*/
struct new_movie *createNew_Movie(unsigned mid,movieCategory_t category, unsigned year){
    struct new_movie *newMovie = (struct new_movie*)malloc(sizeof(struct new_movie));
    newMovie->info.mid = mid;
    newMovie->info.year = year;
    newMovie->category = category;
    newMovie->next = NULL;
    return newMovie;
}

int lookup_new_movie(struct new_movie *head, unsigned mid){
    struct new_movie *temp = head;
    while(temp != NULL){
        if(temp->info.mid == mid){
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

/*insert new_movies in sorted linked list if doesnt exist*/

int insert_new_movie(struct new_movie **head,unsigned mid,movieCategory_t category, unsigned year){
    struct new_movie *newMovie = createNew_Movie(mid,category,year);
    struct new_movie *temp = new_movies_head;
    struct new_movie *prev = NULL;
    if(lookup_new_movie(new_movies_head,mid) == 0) {
        if (new_movies_head == NULL) {
            new_movies_head = newMovie;
        } else {
            while (temp != NULL && temp->info.mid < mid) {
                prev = temp;
                temp = temp->next;
            }
            if (temp == NULL) {
                prev->next = newMovie;
            } else {
                if (prev == NULL) {
                    newMovie->next = new_movies_head;
                    new_movies_head = newMovie;
                } else {
                    prev->next = newMovie;
                    newMovie->next = temp;
                }
            }
        }
    }
    else{
        printf("movie  already exists\n");
        return 0;
    }
    return 1;
}

/*delete new_movie from list*/
int delete_new_movie(struct new_movie **head, unsigned mid){
    struct new_movie *temp = *head;
    struct new_movie *prev = NULL;
    if(lookup_new_movie(*head,mid) == 0){
        printf("Movie doesn't exist to be deleted\n");
        return -1;
    }
    if(temp != NULL && temp->info.mid == mid){
        *head = temp->next;
        free(temp);
        return 1;
    }
    while(temp != NULL && temp->info.mid != mid){
        prev = temp;
        temp = temp->next;
    }
    if(temp == NULL){
        return 0;
    }
    prev->next = temp->next;
    free(temp);
    return 1;
}

/*print new_movies list*/
void print_new_movies(struct new_movie *head){
    struct new_movie *temp = head;
    printf("New movies = ");
    while(temp != NULL){
        printf("( mid = %d , category = %d, year = %d ),",temp->info.mid,temp->category,temp->info.year);
        temp = temp->next;
    }
    printf("\n");
    printf("    DONE\n");

}

/*Add new movie - Event A*/
int add_new_movie(unsigned mid, movieCategory_t category, unsigned year){
    int k ;
    k = insert_new_movie(&new_movies_head,mid,category,year);
    if(k == 1){
        /*A <mid> <category> <year>*/
        printf("A %d %d %d\n",mid,category,year);
        print_new_movies(new_movies_head);
        return 0;
        }
    else
        return -1;
}

/*----------------------------------- EVENT D --------------------------------------------*/


/*insert a new_movie in the categories list in sorted order*/
void insert_movies(struct movie **head,unsigned mid, unsigned year){
    struct movie *newMovie = (struct movie*)malloc(sizeof(struct movie));
    newMovie->info.mid = mid;
    newMovie->info.year = year;
    newMovie->next = NULL;
    struct movie *temp = *head;
    struct movie *prev = NULL;
    if(*head == NULL){
        *head = newMovie;
    }
    else{
        while(temp != NULL && temp->info.mid < mid){
            prev = temp;
            temp = temp->next;
        }
        if(temp == NULL){
            prev->next = newMovie;
        }
        else{
            if(prev == NULL){
                newMovie->next = *head;
                *head = newMovie;
            }
            else{
                prev->next = newMovie;
                newMovie->next = temp;
            }
        }
    }
}

/*traverse the list of new_movies and insert them in the categories list
 * and after delete them from the new_movies list*/
void move_new_movies(struct new_movie **head){
    struct new_movie *temp = *head;
    struct new_movie *prev = NULL;
    while(temp != NULL){
        insert_movies(&Categories[temp->category],temp->info.mid,temp->info.year);
        prev = temp;
        temp = temp->next;
        delete_new_movie(head,prev->info.mid);
    }
}

/*print all the categories list*/
void print_movie_categories(){
    int i;
    printf("Categorized Movies:\n");
    for( i = 0; i < 6; i++){
        struct movie *temp = Categories[i];
        printf("%s: ",movieCategoryStr[i]);
        while(temp != NULL){
            printf("( mid = %d, year = %d ),",temp->info.mid,temp->info.year);
            temp = temp->next;
        }
        printf("\n");
    }

}

int lookup_movie(struct movie *head, unsigned mid){
    struct movie *temp = head;
    while(temp != NULL){
        if(temp->info.mid == mid){
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

/*Distribute new movies - Event D*/
void distribute_new_movies(void){
    move_new_movies(&new_movies_head);
    printf("D \n");
    print_movie_categories();
    printf("    DONE\n");

}


/*----------------------------------- EVENT W --------------------------------------------*/



/*stack inserted in the watch history stuck*/

struct movie* top_watch_history(struct user *user){
    return user->watchHistory;
}

void insert_watch_history(struct user *user, unsigned mid, unsigned year){
    struct movie *newMovie = (struct movie*)malloc(sizeof(struct movie));
    newMovie->info.mid = mid;
    newMovie->info.year = year;
    newMovie->next = NULL;
    if(user->watchHistory == NULL){
        user->watchHistory = newMovie;
    }
    else{
        newMovie->next = user->watchHistory;
        user->watchHistory = newMovie;
    }
}

void print_watch_history(struct user *user){
    struct movie *temp = user->watchHistory;
    printf("W %d %d \n" ,user->uid, user->watchHistory->info.mid);
    printf("User %d Watch History = ",user->uid);
    if(temp == NULL){
        printf("Empty\n");
        return;
    }
    while(temp != NULL){
        printf("mid = %d,",temp->info.mid);
        temp = temp->next;
    }
    printf("\n  DONE\n");
}

int delete_watch_history(struct user *user, unsigned mid){
    struct movie *temp = user->watchHistory;
    struct movie *prev = NULL;
    if(temp != NULL && temp->info.mid == mid){
        user->watchHistory = temp->next;
        free(temp);
        return 1;
    }
    while(temp != NULL && temp->info.mid != mid){
        prev = temp;
        temp = temp->next;
    }
    if(temp == NULL){
        return 0;
    }
    prev->next = temp->next;
    free(temp);
    return 1;
}

/*delete first element of the stuck and return the item*/
int pop(struct user *user){
    if(user->watchHistory == NULL){
        return -1;
    }
    int mid = user->watchHistory->info.mid;
    struct movie *temp = user->watchHistory;
    user->watchHistory = user->watchHistory->next;
    free(temp);
    return mid;

}

int watch_movie(int uid, unsigned mid) {
    /*if the user doesn't exist*/
    struct user *tempUser = userIdLookUp(uid);
    if(tempUser -> uid != uid){
        return -1;
    }
    /*and the movie exist in the categories
    search in each category*/
    int flag = 0;
    int i;
    for (i = 0; i < 6; i++) {
        if (lookup_movie(Categories[i], mid) == 1) {
            /*make a copie of the movie
            search the movie in the category we find it
            once we find it we make a copie of it*/
            struct movie *temp = Categories[i];
            while (temp != NULL) {
                if (temp->info.mid == mid) {
                    struct movie *newMovie = (struct movie *) malloc(sizeof(struct movie));
                    newMovie->info.mid = temp->info.mid;
                    newMovie->info.year = temp->info.year;
                    newMovie->next = NULL;
                    flag = 1;
                    /*we pass the movie into the user stack*/
                    insert_watch_history(tempUser, newMovie->info.mid, newMovie->info.year);


                }
                temp = temp->next;
            }
        }


    }
    if(flag == 0){
        printf("Movie not found\n");
        return -1;
    }
    else{
        print_watch_history(tempUser);
        return 0;
    }


}


/*----------------------------  S  ---------------------------*/
void insertAlternately(struct user* tempuser, struct suggested_movie* newMovie,int insertDirection,struct suggested_movie** last){
    /*1 front insert 0 back insert*/

    if(insertDirection){
        if(tempuser->suggestedHead == NULL){
            tempuser->suggestedHead = newMovie;
            *last = newMovie;
        }
        else{
            (*last)->next = newMovie;
            newMovie->prev = *last;
            *last = newMovie;

        }
    }
    else
    {
        if(tempuser->suggestedTail == NULL){
            tempuser->suggestedTail = newMovie;
            *last = newMovie;
        }
        else{
            (*last)->prev = newMovie;
            newMovie->next = *last;
            *last = newMovie;

        }
    }



}



void print_suggested_movies(int uid){
    struct user *tempuser = userIdLookUp(uid);
    if(tempuser->uid != uid){
        printf("User doesn't exist\n");
        return;
    }
    printf("S %d\n",uid);
    printf("User %d Suggested Movies = ",tempuser->uid);
    struct suggested_movie *temp = tempuser->suggestedHead;
    if(temp == NULL){
        printf("No suggested movies\n");
        return;
    }
    while(temp != NULL){
        printf("mid = %d ,",temp->info.mid);
        temp = temp->next;
    }
    printf("\n");
}

int suggest_movies(int uid){
    /*check if the user exist*/
    struct user *tempuser = userIdLookUp(uid);
    struct suggested_movie *lastRight = NULL;
    struct suggested_movie *lastLeft = NULL;
    int direction = 1;
    int k;
    if(tempuser->uid != uid){
        printf("User doesn't exist\n");
        return -1;
    }
    /*for every user in the user list we take the first movie of the watch history
    traverse the users*/
    struct user *temp = userhead;
    while(temp->uid != -1){
        if(!(temp->watchHistory == NULL || temp->uid == uid)){
            /*print the user we take the data
            printf("WE ARE TAKING THE DATA OF THIS USER %d\n",temp->uid);
            take the first movie and insert it at the end of the suggested list*/
            struct suggested_movie *newMovie = (struct suggested_movie*)malloc(sizeof(struct suggested_movie));

            newMovie->info = top_watch_history(temp)->info;
            k = pop(temp);
            newMovie->next = newMovie->prev = NULL;
            insertAlternately(tempuser,newMovie,direction,(direction)?&lastLeft:&lastRight);
            direction = !direction;


        }
        temp = temp->next;

    }

    if(lastLeft != NULL){
        lastLeft->next = lastRight;
    }
    if(lastRight != NULL){
        lastRight->prev = lastLeft;
    }
    else{
        tempuser->suggestedTail = lastLeft;
    }
    /*print the suggested movies*/
    print_suggested_movies(uid);
    printf("\n  DONE\n");

    return 0;

}


/*----------------------------  T ---------------------------*/

int delete_suggested_movie(struct user* tempuser, unsigned mid){
    if (tempuser == NULL) {
        return 0; /* Return 0 if tempuser is NULL*/
    }

    struct suggested_movie *temp = tempuser->suggestedHead;
    while(temp != NULL){
        if(temp->info.mid == mid){

            if(temp == tempuser->suggestedHead){
                tempuser->suggestedHead = temp->next;
                if (tempuser->suggestedHead != NULL) {
                    tempuser->suggestedHead->prev = NULL;
                } else {

                    tempuser->suggestedTail = NULL;
                }
                free(temp);
                return 1;
            }
            else if(temp == tempuser->suggestedTail){
                tempuser->suggestedTail = temp->prev;
                tempuser->suggestedTail->next = NULL;
                free(temp);
                return 1;
            }
            else{
                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;
                free(temp);
                return 1;
            }
        }
        temp = temp->next;
    }
    return 0;
}


int delete_movie(struct movie **head, int mid) {
    /*Check if the list is empty*/
    if (*head == NULL) {
        return 0;
    }

    struct movie *temp = *head;
    struct movie *prev = NULL;

    /*Check if head node itself holds the mid to be deleted*/
    if (temp != NULL && temp->info.mid == mid) {
        *head = temp->next;  /*Changed head*/
        free(temp);          /*Free old head*/
        return 1;
    }

    /* Search for the mid to be deleted, keep track of the previous node*/
    while (temp != NULL && temp->info.mid != mid) {
        prev = temp;
        temp = temp->next;
    }

    /*If mid was not present in the list*/
    if (temp == NULL) {
        return 0;
    }

    /*Unlink the node from the linked list*/
    prev->next = temp->next;

    free(temp);  /* Free memory*/
    return 1;
}



void take_off_movie(unsigned mid){
    printf("T %d \n",mid);
    int userflag = 0;
    int flag = 0;
    int i;
    for (i = 0; i < 6; i++) {
        if (lookup_movie(Categories[i], mid) == 1) {
            /*delete the movie from each user's suggested list*/
            struct user *temp = userhead;
            while(temp->uid != -1){

                    userflag = delete_suggested_movie(temp,mid);
                    if(userflag == 1){
                       printf("mid = %d removed from user %d \n",mid,temp->uid);
                    }
                    temp = temp->next;


            }
            /*delete the movie from the category*/
            delete_movie(&Categories[i], mid);
            printf("mid = %d removed from category %s \n",mid,movieCategoryStr[i]);
            flag = 1;
            printf("   DONE\n");
        }
    }
    if(flag == 0){
        printf("Movie not found\n");
    }



}

/*----------------------------  F ---------------------------*/

struct suggested_movie *insert_sorted(struct suggested_movie **head, struct suggested_movie *new_node) {
    if (!*head || new_node->info.year < (*head)->info.year) {
        new_node->next = *head;
        if (*head) {
            (*head)->prev = new_node;
        }
        *head = new_node;
    } else {
        struct suggested_movie *current = *head;
        while (current->next && current->next->info.year < new_node->info.year) {
            current = current->next;
        }
        new_node->next = current->next;
        if (current->next) {
            current->next->prev = new_node;
        }
        new_node->prev = current;
        current->next = new_node;
    }

    /* Move to the last node (the tail)*/
    struct suggested_movie *tail = *head;
    while (tail && tail->next) {
        tail = tail->next;
    }
    return tail;
}


int filtered_movie_search(int uid, movieCategory_t category1,movieCategory_t category2, unsigned year) {
    /*traverse the movies in the category1*/
    struct movie *temp = Categories[category1];
    /*cretate a head of suggested movies*/
    struct suggested_movie *tempHead = NULL;
    struct suggested_movie *tempTail = NULL;
    while(temp != NULL){
        if(temp->info.year >= year){
            /*insert the movie in the suggested list of the user*/
            struct suggested_movie *newMovie = (struct suggested_movie*)malloc(sizeof(struct suggested_movie));
            newMovie->info = temp->info;
            newMovie->next = newMovie->prev = NULL;
            tempTail = insert_sorted(&tempHead,newMovie);
        }
        temp = temp->next;
    }

    struct movie *temp2 = Categories[category2];

    while(temp2 != NULL){
        if(temp2->info.year >= year){
            /*insert the movie in the suggested list of the user*/
            struct suggested_movie *newMovie = (struct suggested_movie*)malloc(sizeof(struct suggested_movie));
            newMovie->info = temp2->info;
            newMovie->next = newMovie->prev = NULL;
            tempTail = insert_sorted(&tempHead,newMovie);
        }
        temp2 = temp2->next;
    }


    /*if users suggested list is empty head is tempHead and tail is tempTail*/
    if(userhead->suggestedHead == NULL){
        userhead->suggestedHead = tempHead;
        userhead->suggestedTail = tempTail;
    }
    else{
        /*if users suggested list is not empty
        insert the suggested movies in the end of the list*/
        userhead->suggestedTail->next = tempHead;
        tempHead->prev = userhead->suggestedTail;
        userhead->suggestedTail = tempTail;
    }

    printf("F %d %s %s %d\n",uid,movieCategoryStr[category1],movieCategoryStr[category2],year);
    printf(" User %d Suggested movies = ",uid);
    struct suggested_movie *temp3 = userhead->suggestedHead;
    while(temp3 != NULL){
        printf(" %d  ",temp3->info.mid);
        temp3 = temp3->next;
    }

    printf("\n  DONE\n");

    return 0;

}


/*----------------------------  M  ---------------------------*/

void print_movies(void){
    printf("M \n");
    print_movie_categories();
    printf("    DONE\n");


}

/*----------------------------  P ---------------------------*/

void print_users(void){
    struct user *temp = userhead;
    printf("P \n");
    printf("Users:\n");
    while(temp->uid != -1){


        printf("  User %d\n",temp->uid);
        printf("    Suggested movies = ");
        /*print suggeste movies*/
        struct suggested_movie *tempSuggested = temp->suggestedHead;
        if(tempSuggested == NULL){
            printf("No suggested movies");

        }
        while(tempSuggested != NULL){
            printf("mid = %d ,",tempSuggested->info.mid);
            tempSuggested = tempSuggested->next;

        }

        printf("\n");

        printf("    Watch History = ");
        /*print watch history*/
        struct movie *tempWatchHistory = temp->watchHistory;
        if(tempWatchHistory == NULL){
            printf("No watch history");

        }
        while(tempWatchHistory != NULL){
            printf("mid = %d ,",tempWatchHistory->info.mid);
            tempWatchHistory = tempWatchHistory->next;
        }
        printf("\n");



        temp = temp->next;
    }
    printf("    DONE\n");
}


/*----------------------------  FREE ---------------------------*/


void free_suggested_movies(struct suggested_movie *head) {
    struct suggested_movie *current = head;
    while (current) {
        struct suggested_movie *temp = current;
        current = current->next;
        free(temp);
    }
}



void free_movies(struct movie *head) {
    struct movie *current = head;
    while (current) {
        struct movie *temp = current;
        current = current->next;
        free(temp);
    }
}

void free_users(struct user *head) {
    struct user *current = head;
    while (current) {
        struct user *temp = current;
        free_suggested_movies(current->suggestedHead);
        free_movies(current->watchHistory);
        current = current->next;
        free(temp);
    }
}

void free_new_movies(struct new_movie *head) {
    struct new_movie *current = head;
    while (current) {
        struct new_movie *temp = current;
        current = current->next;
        free(temp);
    }
}


void freeAll(){
    /*delete movies*/
    int i;
    for(i = 0; i < 5; i++){
        free_movies(Categories[i]);
    }
    /*delete users8*/
    free_users(userhead);
    /*delete new movies*/
    free_new_movies(new_movies_head);

    printf(" Free complete\n");

}













