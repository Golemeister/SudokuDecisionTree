#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    int** state;
    struct Node** pointer;
    int valid;
    int count;
}Node;

typedef struct Tree {
    Node* root;
}Tree;

typedef struct QNode {
    Node* item;
    struct QNode* next;
} QNode;

typedef struct Queue {
    QNode* front;
    QNode* rear;
}Queue;


void addInQueue(Queue* q,Node* item) {
    QNode* tmp = (QNode*)malloc(sizeof(QNode));
   
    tmp->item = item;
    tmp->next = NULL;

    if (q->rear == NULL) {
        q->front = q->rear = tmp;
    }
    else {
        q->rear->next = tmp;
        q->rear = tmp;
    }
}

Node* removeFromQueue(Queue* q) {
    
    if (q->front == NULL)
        return NULL;
    
    QNode* tmp = q->front;
    q->front = q->front->next;

    if (q->front == NULL)
        q->rear = NULL;

    Node* current = tmp->item;
    free(tmp);
    return current;
}


int** readSudokuBoard(const char* filename, int* size) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file.\n");
        return NULL;
    }

    fscanf(file, "%d\n", size);
    if ((*size) <= 0) {
        printf("Invalid board size.\n");
        fclose(file);
        return NULL;
    }

    int** board = (int**)malloc((*size) * sizeof(int*));
    for (int i = 0; i < (*size); i++) {
        board[i] = (int*)malloc((*size) * sizeof(int));
    }

    
    for (int i = 0; i < (*size); i++) {
        for (int j = 0; j < (*size); j++) {
            char c = fgetc(file);
            int tmp = c - '0';
            board[i][j] = tmp;
        }
        fgetc(file);
    }
    
    fclose(file);
    return board;
}

void printSudokuBoard(int** sudokuBoard,int size) {
    if (sudokuBoard) {
        printf("Sudoku board of size %d x %d:\n", size, size);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                printf("%d ", sudokuBoard[i][j]);
            }
            printf("\n");
        }
    }
}

int checkSudokuState(int** board) {
    
    for (int k = 0; k < 4; k++) {
        for (int i = 0; i < 4; i++) {
            for (int j = i+1 ; j < 4; j++) {
                if (board[k][i] == board[k][j] && board[k][i] != 0) return 0;
            }
        }
    }
   
    for (int k = 0; k < 4; k++) {
        for (int i = 0; i < 4; i++) {
            for (int j = i + 1; j < 4; j++) {
                if (board[i][k] == board[j][k] && board[i][k] != 0) return 0;
            }
        }
    }

    if (board[0][0] == board[1][1] && board[0][0] != 0) return 0;
    if (board[0][2] == board[1][3] && board[0][2] != 0) return 0;
    if (board[2][0] == board[3][1] && board[2][0] != 0) return 0;
    if (board[2][2] == board[3][3] && board[2][2] != 0) return 0;

    return 1;
}

int countLevels(int** board) {
    int levels = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (board[i][j] == 0) {
                levels++;
            }
        }
    }
    return levels;
}

Tree* initializeTree(Tree* tree,int** board) {
    Tree* current = malloc(1 * sizeof(Tree));

    current->root = malloc(1 * sizeof(Node));

    current->root->state = malloc(4 * sizeof(int*));

    for (int i = 0; i < 4; i++) {
        current->root->state[i] = malloc(4 * sizeof(int));
    }

    for (int i = 0; i < 4; i++) {
        memcpy(current->root->state[i], board[i], 4 * sizeof(int));
    }

    int levels = countLevels(board);
    

    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->front = NULL;
    q->rear = NULL;

    addInQueue(q,current->root);
   
    while (q->front != NULL) {
        Node* current = removeFromQueue(q);
        if (checkSudokuState(current->state) == 1) {
            current->valid = 1;
            levels = countLevels(current->state);
            current->pointer = (Node**)malloc(levels * 4 * sizeof(Node*));
            current->count = 0;
            int id = 0;
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if (current->state[i][j] == 0) {
                        for (int k = 1; k < 5; k++) {
                            Node* newItem = (Node*)malloc(1*sizeof(Node));
                            newItem->state = (int**)malloc(4 * sizeof(int*));
                            for (int p = 0; p < 4; p++) {
                                newItem->state[p] = malloc(4 * sizeof(int));
                            }
                            for (int p = 0; p < 4; p++) {
                                memcpy(newItem->state[p], current->state[p], 4 * sizeof(int));
                            }
                            newItem->state[i][j] = k;
                            current->pointer[id++] = newItem;
                            current->count++;
                            addInQueue(q,newItem);
                        }
                    }
                }
            }
        }
        else {
            current->valid = 0;
        }
    }


    return current;
}

void printTree(Tree* tree) {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->front = NULL;
    q->rear = NULL;
    int level = 0;

    addInQueue(q, tree->root);
    while (q->front != NULL) {
        Node* current = removeFromQueue(q);
        printSudokuBoard(current->state, 4);
        printf("Validnost: %d\n", current->valid);
        for (int i = 0; i < current->count; i++) {
            addInQueue(q, current->pointer[i]);
        }
        printf("\n");
    }
}

int isFull(int** board) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (board[i][j] == 0) {
                return 0;
            }
        }
    }
    return 1;
}

void printSolutions(Tree* tree) {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->front = NULL;
    q->rear = NULL;
    int level = 0;
    
    addInQueue(q, tree->root);
    while (q->front != NULL) {
        Node* current = removeFromQueue(q);
        if (isFull(current->state) == 1 && current->valid == 1 && q->front != NULL) {
            printSudokuBoard(current->state, 4);
            printf("\n");
        }
        for (int i = 0; i < current->count; i++) {
            addInQueue(q, current->pointer[i]);
        }
    }
}

int main() {
    char file[10];
    int size;
    int** sudokuBoard = NULL;
    int choice;
    Tree* tree = NULL;

    while (1) {
        printf("[1]Ucitaj iz terminala\n[2]Ucitaj iz fajla\n[3]Ispisi\n[4]Proveri ispravnost\n[5]Formiraj stablo\n[6]Obilazak stabla\n[7]Ispisi resenja sudokua\n");
        scanf("%d", &choice);
        switch (choice) {
            case 1: 
                break;
            case 2: 
                printf("Unesi ime fajla\n");
                scanf("%s", &file);
                sudokuBoard = readSudokuBoard(file, &size);
                break;
            case 3:
                if(sudokuBoard)
                printSudokuBoard(sudokuBoard, size);
                break;
            case 4:
                printf("%d\n",checkSudokuState(sudokuBoard));
                break;
            case 5:
                tree = initializeTree(tree, sudokuBoard);
                break;
            case 6:
                printTree(tree);
                break;
            case 7:
                printSolutions(tree);
                break;
        }

    }
    

    return 0;
}