#include <stdlib.h>
#include <stdio.h>


struct LinkedNode {
    int val;
    struct LinkedNode* next;
};


struct DoublyLinkedNode {
    int val;
    struct DoublyLinkedNode* prev;
    struct DoublyLinkedNode* next;
};


// Question 1

struct LinkedNode* initAscendingLinkedList(int n) {
    if (!n)
        return NULL;

    struct LinkedNode* head = malloc(sizeof(struct LinkedNode));
    head->val = 0;

    struct LinkedNode* currentNode = head;

    for (int i=1; i<n; i++) {
        currentNode->next = malloc(sizeof(struct LinkedNode));
        currentNode = currentNode->next;
        currentNode->val = i;
    }

    currentNode->next = NULL;

    return head;
}


// Question 2

int sizeOfLinkedList(struct LinkedNode* head) {
    int size = 0;
    while(head) {
        size++;
        head = head->next;
    }
    return size;
}


// Question 3

void displayLinkedList(struct LinkedNode* head) {
    while(head) {
        printf("%p: %d\n", head, head->val);
        head=head->next;
    }
}


// Question 4

struct LinkedNode* removeHead(struct LinkedNode** headPtr) {
    if (*headPtr == NULL)
        return NULL;
    struct LinkedNode* poppedNode = *headPtr;
    *headPtr = (*headPtr)->next;
    return poppedNode;
}


// Question 5

struct LinkedNode* removeTail(struct LinkedNode** headPtr) {
    if (*headPtr == NULL)
        return NULL;
    struct LinkedNode* tmp1 = *headPtr;
    if ((*headPtr)->next == NULL) {
        (*headPtr) = NULL;
        return tmp1;
    }
    while(tmp1) {
        if (tmp1->next->next == NULL) {
            struct LinkedNode* tmp2 = tmp1->next;
            tmp1->next = NULL;
            return tmp2;
        }
        tmp1 = tmp1->next;
    }
    return NULL;
}


// Question 6

void addHead(struct LinkedNode** headPtr, int val) {
    struct LinkedNode* newHead = malloc(sizeof(struct LinkedNode));
    newHead->val = val;
    if (*headPtr == NULL)
        newHead->next = NULL;
    else
        newHead->next = *headPtr;
    *headPtr = newHead;
}


// Question 7

void addTail(struct LinkedNode** headPtr, int val) {
    struct LinkedNode* newTail = malloc(sizeof(struct LinkedNode));
    newTail->val=val, newTail->next = NULL;
    if (*headPtr == NULL)
        *headPtr = newTail;
    else {
        struct LinkedNode* tmp = *headPtr;
        while(tmp) {
            if (tmp->next == NULL) {
                tmp->next = newTail;
                break;
            }
            tmp = tmp->next;
        }
    }
}


// Question 8

void concatLinkedLists(struct LinkedNode* baseHead, struct LinkedNode* addedHead) {
    while (baseHead) {
        if (baseHead->next == NULL) {
            baseHead->next = addedHead;
            break;
        }
        baseHead = baseHead->next;
    }
}


// Question 9

struct LinkedNode* applyFunctionToLinkedList(struct LinkedNode* head, int (*fun_ptr)(int)) {
    struct LinkedNode *newHead = malloc(sizeof(struct LinkedNode)), *currentNode = newHead;
    while (head) {
        currentNode->val = fun_ptr(head->val);
        if (head->next == NULL)
            currentNode->next = NULL;
        else
            currentNode->next = malloc(sizeof(struct LinkedNode));
        currentNode=currentNode->next, head=head->next;
    }
    return newHead;
}


// Question 10

struct DoublyLinkedNode* ConvertLinkedNodetoDoublyLinkedNode(struct LinkedNode* head) {
    struct DoublyLinkedNode *newHead = malloc(sizeof(struct DoublyLinkedNode)), *currentNode = newHead;
    while (head) {
        currentNode->val = head->val;
        if (head->next == NULL)
            currentNode->next = NULL;
        else
            currentNode->next = malloc(sizeof(struct DoublyLinkedNode));
        currentNode->next->prev = currentNode;
        currentNode=currentNode->next, head=head->next;
    }
    return newHead;
}

// Question 11

struct DoublyLinkedNode* initAscendingCircularDoublyLinkedList(int n) {
    if (!n)
        return NULL;

    struct DoublyLinkedNode* head = malloc(sizeof(struct DoublyLinkedNode));
    head->val = 0;

    struct DoublyLinkedNode* currentNode = head;

    for (int i=1; i<n; i++) {
        currentNode->next = malloc(sizeof(struct DoublyLinkedNode));
        currentNode->next->prev = currentNode;
        currentNode = currentNode->next;
        currentNode->val = i;
    }

    currentNode->next = head;
    head->prev = currentNode;

    return head;
}

int sizeOfCircularDoublyLinkedList(struct DoublyLinkedNode* head) {
    int size = 0;
    struct DoublyLinkedNode* savedHead = head;
    do {
        size++;
        head = head->next;
    } while(head!=savedHead);
    return size;
}

void displayCircularDoublyLinkedList(struct DoublyLinkedNode* head) {
    struct DoublyLinkedNode* savedHead = head;
    if (head!=NULL) {
        printf("%p: %d\n", head, head->val);
        head = head->next;
    }
    while(head!=savedHead) {
        printf("%p: %d\n", head, head->val);
        head=head->next;
    }
}

struct DoublyLinkedNode* removeCircularHead(struct DoublyLinkedNode** headPtr) {
    if (*headPtr == NULL)
        return NULL;
    struct DoublyLinkedNode* popedNode = *headPtr;
    (*headPtr)->prev->next = (*headPtr)->next;
    *headPtr = (*headPtr)->next;
    return popedNode;
}

struct DoublyLinkedNode* removeCircularTail(struct DoublyLinkedNode** headPtr) {
    if (*headPtr == NULL)
        return NULL;
    struct DoublyLinkedNode* tmp1 = *headPtr;
    if ((*headPtr)->next == *headPtr) {
        (*headPtr) = NULL;
        return tmp1;
    }
    struct DoublyLinkedNode* savedHead = *headPtr;
    do {
        if (tmp1->next->next == savedHead) {
            struct DoublyLinkedNode* tmp2 = tmp1->next;
            tmp1->next = savedHead;
            savedHead->prev = tmp1;
            return tmp2;
        }
        tmp1 = tmp1->next;
    } while(tmp1!=savedHead);
    return NULL;
}

void addCircularHead(struct DoublyLinkedNode** headPtr, int val) {
    struct DoublyLinkedNode* newHead = malloc(sizeof(struct DoublyLinkedNode));
    newHead->val = val;
    if (*headPtr == NULL)
        newHead->next = newHead, newHead->prev=newHead;
    else
        newHead->next = *headPtr, newHead->prev = (*headPtr)->prev;
    (*headPtr)->prev->next = newHead, (*headPtr)->prev = newHead;
    *headPtr = newHead;
}

void addCircularTail(struct DoublyLinkedNode** headPtr, int val) {
    struct DoublyLinkedNode* newTail = malloc(sizeof(struct DoublyLinkedNode));
    newTail->val=val;
    if (*headPtr == NULL) {
        *headPtr = newTail;
        newTail->next = newTail, newTail->prev = newTail;
    }
    else {
        struct DoublyLinkedNode *tmp = *headPtr, *savedHead = *headPtr;
        do {
            if (tmp->next == savedHead) {
                tmp->next = newTail, newTail->prev = tmp;
                newTail->next = savedHead, savedHead->prev = newTail;
                break;
            }
            tmp = tmp->next;
        } while(tmp != savedHead);
    }
}

void concatCircularDoublyLinkedList(struct DoublyLinkedNode* baseHead, struct DoublyLinkedNode* addedHead) {
    struct DoublyLinkedNode *savedHead = baseHead;
    do {
        if (baseHead->next == addedHead) {
            baseHead->next = savedHead, savedHead->prev = baseHead;
            break;
        }
        if (baseHead->next == savedHead) {
            baseHead->next = addedHead, addedHead->prev = baseHead;
        }
        baseHead = baseHead->next;
    } while(baseHead != savedHead);
}

struct DoublyLinkedNode* applyFunctionToCircularDoublyLinkedList(struct DoublyLinkedNode* head, int (*fun_ptr)(int)) {
    struct DoublyLinkedNode *newHead = malloc(sizeof(struct DoublyLinkedNode)), *currentNode = newHead, *savedHead = head;
    do {
        currentNode->val = fun_ptr(head->val);
        if (head->next == savedHead) {
            currentNode->next = newHead, newHead->prev = currentNode;
            break;
        }
        currentNode->next = malloc(sizeof(struct DoublyLinkedNode));
        currentNode->next->prev = currentNode, currentNode=currentNode->next, head=head->next;
    } while(head != savedHead);
    return newHead;
}