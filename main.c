/******************************************************************/
// Muhammed Furkan Yağbasan
// 2099505
/******************************************************************/
#include "do_not_submit.h"
#include <pthread.h>
#include <semaphore.h>

sem_t mutexes[GRIDSIZE][GRIDSIZE]; // for cell locking
pthread_cond_t *conds; // sleeping condition varibles
sem_t *mtxForDraw; // for drawWindow

typedef struct coordinate {
    int x;
    int y;
} coordinate;

typedef enum state {idle, withFood, sleeping, tired, sleepingWithFood} state;

typedef struct ant {
    coordinate pos;
    state antState;
    int id;
} ant;

char getStateChar(state s) {
    switch(s) {
        case idle:
            return '1';
        case withFood:
            return 'P';
        case sleeping:
            return 'S';
        case tired:
            return '1';
        case sleepingWithFood:
            return '$';
    }
}

// checks neighbor cells rondomly
// if there is a food returns its coordinates as result
// if there is no food result will be (-1, -1)
// while checking a cell, it is being locked and unlocked
// if the function returns, cell will be unlocked in antThread function
// if the result retuned with (-1, -1), it means all cells already unlocked here
void checkNeighborsForFood(coordinate pos, coordinate* result) {
    int tmp;
    int tmps[8] = {0,0,0,0,0,0,0,0};
    while((tmps[0] + tmps[1] + tmps[2] + tmps[3] + tmps[4] + tmps[5] + tmps[6] + tmps[7]) != 8) {

        tmp = rand() % 8;
        if(tmps[tmp] == 1)
            continue;
        else {
            tmps[tmp] = 1;
            switch(tmp){
                case 0:
                    if(pos.x >0 && pos.y >0) {
                        sem_wait(&mutexes[pos.y-1][pos.x-1]);
                        if(lookCharAt(pos.x - 1, pos.y - 1) == 'o'){
                            (*result).x = pos.x - 1;
                            (*result).y = pos.y - 1;
                            return;
                        }
                        sem_post(&mutexes[pos.y-1][pos.x-1]);
                    }
                    break;
                case 1:
                    if(pos.y >0) {
                        sem_wait(&mutexes[pos.y-1][pos.x]);
                        if(lookCharAt(pos.x, pos.y - 1) == 'o'){
                            (*result).x = pos.x;
                            (*result).y = pos.y - 1;
                            return;
                        }
                        sem_post(&mutexes[pos.y-1][pos.x]);
                    }
                    break;
                case 2:
                    if(pos.x < GRIDSIZE-1 && pos.y >0) {
                        sem_wait(&mutexes[pos.y-1][pos.x+1]);
                        if(lookCharAt(pos.x + 1, pos.y - 1) == 'o'){
                            (*result).x = pos.x + 1;
                            (*result).y = pos.y - 1;
                            return;
                        }
                        sem_post(&mutexes[pos.y-1][pos.x+1]);
                    }
                    break;
                case 3:
                    if(pos.x < GRIDSIZE-1) {
                        sem_wait(&mutexes[pos.y][pos.x+1]);
                        if(lookCharAt(pos.x + 1, pos.y) == 'o'){
                            (*result).x = pos.x + 1;
                            (*result).y = pos.y;
                            return;
                        }
                        sem_post(&mutexes[pos.y][pos.x+1]);
                    }
                    break;
                case 4:
                    if(pos.x < GRIDSIZE-1 && pos.y < GRIDSIZE-1) {
                        sem_wait(&mutexes[pos.y+1][pos.x+1]);
                        if(lookCharAt(pos.x + 1, pos.y + 1) == 'o'){
                            (*result).x = pos.x + 1;
                            (*result).y = pos.y + 1;
                            return;
                        }
                        sem_post(&mutexes[pos.y+1][pos.x+1]);
                    }
                    break;
                case 5:
                    if(pos.y < GRIDSIZE-1) {
                        sem_wait(&mutexes[pos.y+1][pos.x]);
                        if(lookCharAt(pos.x, pos.y + 1) == 'o'){
                            (*result).x = pos.x;
                            (*result).y = pos.y + 1;
                            return;
                        }
                        sem_post(&mutexes[pos.y+1][pos.x]);
                    }
                    break;
                case 6:
                    if(pos.x >0 && pos.y <GRIDSIZE-1) {
                        sem_wait(&mutexes[pos.y+1][pos.x-1]);
                        if(lookCharAt(pos.x - 1, pos.y + 1) == 'o'){
                            (*result).x = pos.x - 1;
                            (*result).y = pos.y + 1;
                            return;
                        }
                        sem_post(&mutexes[pos.y+1][pos.x-1]);
                    }
                    break;
                case 7:
                    if(pos.x >0) {
                        sem_wait(&mutexes[pos.y][pos.x-1]);
                        if(lookCharAt(pos.x - 1, pos.y) == 'o'){
                            (*result).x = pos.x - 1;
                            (*result).y = pos.y;
                            return;
                        }
                        sem_post(&mutexes[pos.y][pos.x-1]);
                    }
                    break;
            }
        }
    }
    (*result).x = -1;
    (*result).y = -1;
    return;
}

// chooses a random neigbor cell which is empty(-)
// and ruturns that cell's coordinate as result
// if there is no empty cell, result will be (-1, -1)
// while checking a cell, it is being locked and unlocked
// if the function returns, cell will be unlocked in antThread function
// if the result retuned with (-1, -1), it means all cells already unlocked here
void randomMove(coordinate pos, coordinate* result) {
    int tmp;
    int tmps[8] = {0,0,0,0,0,0,0,0};
    while((tmps[0] + tmps[1] + tmps[2] + tmps[3] + tmps[4] + tmps[5] + tmps[6] + tmps[7]) != 8) {
        tmp = rand() % 8;
        if(tmps[tmp] == 1)
            continue;
        else {
            tmps[tmp] = 1;
            switch(tmp){
                case 0:
                    if(pos.x >0 && pos.y >0) {
                        sem_wait(&mutexes[pos.y-1][pos.x-1]);
                        if(lookCharAt(pos.x - 1, pos.y - 1) == '-'){
                            (*result).x = pos.x - 1;
                            (*result).y = pos.y - 1;
                            return;
                        }
                        sem_post(&mutexes[pos.y-1][pos.x-1]);
                    }
                    break;
                case 1:
                    if(pos.y >0) {
                        sem_wait(&mutexes[pos.y-1][pos.x]);
                        if(lookCharAt(pos.x, pos.y - 1) == '-') {
                            (*result).x = pos.x;
                            (*result).y = pos.y - 1;
                            return;
                        }
                        sem_post(&mutexes[pos.y-1][pos.x]);
                    }
                    break;
                case 2:
                    if(pos.x < GRIDSIZE-1 && pos.y >0) {
                        sem_wait(&mutexes[pos.y-1][pos.x+1]);
                        if(lookCharAt(pos.x + 1, pos.y - 1) == '-') {
                            (*result).x = pos.x + 1;
                            (*result).y = pos.y - 1;
                            return;
                        }
                        sem_post(&mutexes[pos.y-1][pos.x+1]);
                    }
                    break;
                case 3:
                    if(pos.x < GRIDSIZE-1) {
                        sem_wait(&mutexes[pos.y][pos.x+1]);
                        if(lookCharAt(pos.x + 1, pos.y) == '-') {
                            (*result).x = pos.x + 1;
                            (*result).y = pos.y;
                            return;
                        }
                        sem_post(&mutexes[pos.y][pos.x+1]);
                    }
                    break;
                case 4:
                    if(pos.x < GRIDSIZE-1 && pos.y < GRIDSIZE-1) {
                        sem_wait(&mutexes[pos.y+1][pos.x+1]);
                        if(lookCharAt(pos.x + 1, pos.y + 1) == '-') {
                            (*result).x = pos.x + 1;
                            (*result).y = pos.y + 1;
                            return;
                        }
                        sem_post(&mutexes[pos.y+1][pos.x+1]);
                    }
                    break;
                case 5:
                    if(pos.y < GRIDSIZE-1) {
                        sem_wait(&mutexes[pos.y+1][pos.x]);
                        if(lookCharAt(pos.x, pos.y + 1) == '-') {
                            (*result).x = pos.x;
                            (*result).y = pos.y + 1;
                            return;
                        }
                        sem_post(&mutexes[pos.y+1][pos.x]);
                    }
                    break;
                case 6:
                    if(pos.x >0 && pos.y <GRIDSIZE-1) {
                        sem_wait(&mutexes[pos.y+1][pos.x-1]);
                        if(lookCharAt(pos.x - 1, pos.y + 1) == '-') {
                            (*result).x = pos.x - 1;
                            (*result).y = pos.y + 1;
                            return;
                        }
                        sem_post(&mutexes[pos.y+1][pos.x-1]);
                    }
                    break;
                case 7:
                    if(pos.x >0) {
                        sem_wait(&mutexes[pos.y][pos.x-1]);
                        if(lookCharAt(pos.x - 1, pos.y) == '-') {
                            (*result).x = pos.x - 1;
                            (*result).y = pos.y;
                            return;
                        }
                        sem_post(&mutexes[pos.y][pos.x-1]);
                    }
                    break;
            }
        }
    }
    (*result).x = -1;
    (*result).y = -1;
    return;
}

// THREAD FUNCTION
void *antThread(void* ant)
{   
    struct ant *theAnt = (struct ant*) ant;
    enum state currentState; // to keep ant state before sleeping

    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

    while(TRUE) {

        // check if the ant should be sleeping
        if((*theAnt).id < getSleeperN()) {
            currentState = (*theAnt).antState;
            if((*theAnt).antState == withFood)
                (*theAnt).antState = sleepingWithFood;
            else if((*theAnt).antState == idle || (*theAnt).antState == tired)
                (*theAnt).antState = sleeping;
            sem_wait(&mutexes[(*theAnt).pos.y][(*theAnt).pos.x]);
            putCharTo((*theAnt).pos.x, (*theAnt).pos.y, getStateChar((*theAnt).antState));
            sem_post(&mutexes[(*theAnt).pos.y][(*theAnt).pos.x]);
        } 

        // check if the ant should wake up
        if((*theAnt).antState == sleeping || (*theAnt).antState == sleepingWithFood) {
            pthread_mutex_lock(&lock);
            pthread_cond_wait(&conds[(*theAnt).id], &lock);
            pthread_mutex_unlock(&lock);

            (*theAnt).antState = currentState;
            sem_wait(&mutexes[(*theAnt).pos.y][(*theAnt).pos.x]);
            putCharTo((*theAnt).pos.x, (*theAnt).pos.y, getStateChar((*theAnt).antState));
            sem_post(&mutexes[(*theAnt).pos.y][(*theAnt).pos.x]);
        }

        usleep(getDelay() * 1000 + (rand() % 9000));

        // check neighbors for food
        coordinate result;
        checkNeighborsForFood((*theAnt).pos, &result);

        // a food found, if the ant does not have already,take it
        // otherwise, drop the food to the current position and move randomly to an empty cell
        if(result.x != -1 && (*theAnt).antState != tired) {
            if((*theAnt).antState == withFood){
                
                sem_post(&mutexes[result.y][result.x]);

                coordinate moveTo;

                randomMove((*theAnt).pos, &moveTo);
                if(moveTo.x != -1) {
                    (*theAnt).antState = tired;

                    sem_wait(&mtxForDraw[(*theAnt).id]);

                    putCharTo(moveTo.x, moveTo.y, getStateChar((*theAnt).antState));
                    sem_post(&mutexes[moveTo.y][moveTo.x]);
                    sem_wait(&mutexes[(*theAnt).pos.y][(*theAnt).pos.x]);
                    putCharTo((*theAnt).pos.x, (*theAnt).pos.y, 'o');
                    sem_post(&mutexes[(*theAnt).pos.y][(*theAnt).pos.x]);

                    sem_post(&mtxForDraw[(*theAnt).id]);

                    (*theAnt).pos.x = moveTo.x;
                    (*theAnt).pos.y = moveTo.y;

                    
                }

            } else {
                sem_wait(&mtxForDraw[(*theAnt).id]);

                putCharTo(result.x, result.y, 'P');
                sem_post(&mutexes[result.y][result.x]);
                sem_wait(&mutexes[(*theAnt).pos.y][(*theAnt).pos.x]);
                putCharTo((*theAnt).pos.x, (*theAnt).pos.y, '-');
                sem_post(&mutexes[(*theAnt).pos.y][(*theAnt).pos.x]);

                sem_post(&mtxForDraw[(*theAnt).id]);
                (*theAnt).antState = withFood;
                (*theAnt).pos.x = result.x;
                (*theAnt).pos.y = result.y;
                
            }

        } else { // no food in neighbor cells, move randomly to an empty cell
            if(result.x != -1)
                sem_post(&mutexes[result.y][result.x]);
            
            coordinate moveTo;

            randomMove((*theAnt).pos, &moveTo);

            if(moveTo.x != -1) {
                if((*theAnt).antState == tired)
                    (*theAnt).antState = idle;

                sem_wait(&mtxForDraw[(*theAnt).id]);

                putCharTo(moveTo.x, moveTo.y, getStateChar((*theAnt).antState));
                sem_post(&mutexes[moveTo.y][moveTo.x]);
                sem_wait(&mutexes[(*theAnt).pos.y][(*theAnt).pos.x]);
                putCharTo((*theAnt).pos.x, (*theAnt).pos.y, '-');
                sem_post(&mutexes[(*theAnt).pos.y][(*theAnt).pos.x]);

                sem_post(&mtxForDraw[(*theAnt).id]);
                (*theAnt).pos.x = moveTo.x;
                (*theAnt).pos.y = moveTo.y;
 
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    pthread_t tid;
    int antNum = atoi(argv[1]);
    int foodNum = atoi(argv[2]);
    int progTime = atoi(argv[3]);
    int* progTimePtr = &progTime;
    mtxForDraw = malloc(sizeof(sem_t)*antNum);

    conds = malloc(sizeof(pthread_cond_t)*antNum);

    ant* antsArr[antNum];

    int i,j;

    // init sleepinf condition varibles
    for(i=0; i<antNum; i++){
        pthread_cond_init(&conds[i], NULL); 
        sem_init(&mtxForDraw[i], 0, 1);
    }

    // init cell semophores
    for(i=0; i<GRIDSIZE; i++){
        for(j=0; j<GRIDSIZE; j++){
            sem_init(&mutexes[i][j], 0, 1); 
        }
    }

    for (i = 0; i < GRIDSIZE; i++) {
        for (j = 0; j < GRIDSIZE; j++) {
            putCharTo(i, j, '-');
        }
    }
    int a,b;

    for (i = 0; i < foodNum; i++) {
        do {
            a = rand() % GRIDSIZE;
            b = rand() % GRIDSIZE;
        }while (lookCharAt(a,b) != '-');
        putCharTo(a, b, 'o');
    }

    // put ants to map and create ant pointers for threads
    for (i = 0; i < antNum; i++) {
        do {
            a = rand() % GRIDSIZE;
            b = rand() % GRIDSIZE;
        }while (lookCharAt(a,b) != '-');

        struct ant *ant;
        ant = malloc(sizeof(struct ant));
        (*ant).pos.x = a;
        (*ant).pos.y = b;
        (*ant).antState = idle;
        (*ant).id = i;
        antsArr[i] = ant;

        putCharTo(a, b, '1');
        
    }

    // create ant threads
    for(i=0; i<antNum; i++){
        pthread_create(&tid, NULL, antThread, (void *)antsArr[i]);
    }

    time_t start = time(NULL);

    startCurses();
    
    char c;
    while (TRUE) {
        if (time(NULL) - start >= progTime)
            break;

        for(i=0; i<antNum; i++) {
            sem_wait(&mtxForDraw[i]);
        }
        drawWindow();
        for(i=0; i<antNum; i++) {
            sem_post(&mtxForDraw[i]);
        }
        
        c = 0;
        c = getch();

        if (c == 'q' || c == ESC) break;
        if (c == '+') {
            setDelay(getDelay()+10);
        }
        if (c == '-') {
            setDelay(getDelay()-10);
        }
        if (c == '*') {
            if(getSleeperN() < antNum)
                setSleeperN(getSleeperN()+1);
        }
        if (c == '/') {
            setSleeperN(getSleeperN()-1);
            pthread_cond_signal(&conds[getSleeperN()]); // wake up last slept ant
        }
        usleep(DRAWDELAY);
        
    }
    
    endCurses();
    
    for(i=0; i<GRIDSIZE; i++){
        for(j=0; j<GRIDSIZE; j++){
            sem_destroy(&mutexes[i][j]);
        }
    }
    for(i=0; i<antNum; i++) {
        free(antsArr[i]);
    }
    free(mtxForDraw);
    free(conds);
    return 0;
}
