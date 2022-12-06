#include <stdlib.h>
#include <stdio.h>

struct linkedNode {
    int val;
    struct linkedNode* next;
};

struct doublyLinkedNode {
    int val;
    struct doublyLinkedNode* prev;
    struct doublyLinkedNode* next;
};

struct linkedNode* initAscendingLinkedList(int n) {
    if (!n)
        return NULL;

    struct linkedNode* head = malloc(sizeof(struct linkedNode));
    head->val = 0;

    struct linkedNode* currentNode = head;

    for (int i=1; i<n; i++) {
        currentNode->next = malloc(sizeof(struct linkedNode));
        currentNode = currentNode->next;
        currentNode->val = i;
    }

    currentNode->next = NULL;

    return head;
}

int sizeOfLinkedList(struct linkedNode* head) {
    int size = 0;
    while(head) {
        size++;
        head = head->next;
    }
    return size;
}

void displayLinkedList(struct linkedNode* head) {
    while(head) {
        printf("%p: %d\n", head, head->val);
        head=head->next;
    }
}

struct linkedNode* removeHead(struct linkedNode** headPtr) {
    if (*headPtr == NULL)
        return NULL;
    struct linkedNode* popedNode = *headPtr;
    *headPtr = (*headPtr)->next;
    return popedNode;
}

struct linkedNode* removeTail(struct linkedNode** headPtr) {
    if (*headPtr == NULL)
        return NULL;
    struct linkedNode* tmp1 = *headPtr;
    if ((*headPtr)->next == NULL) {
        (*headPtr) = NULL;
        return tmp1;
    }
    while(tmp1) {
        if (tmp1->next->next == NULL) {
            struct linkedNode* tmp2 = tmp1->next;
            tmp1->next = NULL;
            return tmp2;
        }
        tmp1 = tmp1->next;
    }
    return NULL;
}

void addHead(struct linkedNode** headPtr, int val) {
    struct linkedNode* newHead = malloc(sizeof(struct linkedNode));
    newHead->val = val;
    if (*headPtr == NULL)
        newHead->next = NULL;
    else
        newHead->next = *headPtr;
    *headPtr = newHead;
}

void addTail(struct linkedNode** headPtr, int val) {
    struct linkedNode* newTail = malloc(sizeof(struct linkedNode));
    newTail->val=val, newTail->next = NULL;
    if (*headPtr == NULL)
        *headPtr = newTail;
    else {
        struct linkedNode* tmp = *headPtr;
        while(tmp) {
            if (tmp->next == NULL) {
                tmp->next = newTail;
                break;
            }
            tmp = tmp->next;
        }
    }
}

void concatLinkedList(struct linkedNode* baseHead, struct linkedNode* addedHead) {
    while (baseHead) {
        if (baseHead->next == NULL) {
            baseHead->next = addedHead;
            break;
        }
        baseHead = baseHead->next;
    }
}

struct linkedNode* applyFunctionToLinkedList(struct linkedNode* head, int (*fun_ptr)(int)) {
    struct linkedNode *newHead = malloc(sizeof(struct linkedNode)), *currentNode = newHead;
    while (head) {
        currentNode->val = fun_ptr(head->val);
        if (head->next == NULL)
            currentNode->next = NULL;
        else
            currentNode->next = malloc(sizeof(struct linkedNode));
        currentNode=currentNode->next, head=head->next;
    }
    return newHead;
}

struct doublyLinkedNode* initAscendingCircularDoublyLinkedList(int n) {
    if (!n)
        return NULL;

    struct doublyLinkedNode* head = malloc(sizeof(struct doublyLinkedNode));
    head->val = 0;

    struct doublyLinkedNode* currentNode = head;

    for (int i=1; i<n; i++) {
        currentNode->next = malloc(sizeof(struct doublyLinkedNode));
        currentNode->next->prev = currentNode;
        currentNode = currentNode->next;
        currentNode->val = i;
    }

    currentNode->next = head;
    head->prev = currentNode;

    return head;
}

int sizeOfCircularDoublyLinkedList(struct doublyLinkedNode* head) {
    int size = 0;
    struct doublyLinkedNode* savedHead = head;
    do {
        size++;
        head = head->next;
    } while(head!=savedHead);
    return size;
}

void displayCircularDoublyLinkedList(struct doublyLinkedNode* head) {
    struct doublyLinkedNode* savedHead = head;
    if (head!=NULL) {
        printf("%p: %d\n", head, head->val);
        head = head->next;
    }
    while(head!=savedHead) {
        printf("%p: %d\n", head, head->val);
        head=head->next;
    }
}

struct doublyLinkedNode* removeCircularHead(struct doublyLinkedNode** headPtr) {
    if (*headPtr == NULL)
        return NULL;
    struct doublyLinkedNode* popedNode = *headPtr;
    (*headPtr)->prev->next = (*headPtr)->next;
    *headPtr = (*headPtr)->next;
    return popedNode;
}

struct doublyLinkedNode* removeCircularTail(struct doublyLinkedNode** headPtr) {
    if (*headPtr == NULL)
        return NULL;
    struct doublyLinkedNode* tmp1 = *headPtr;
    if ((*headPtr)->next == *headPtr) {
        (*headPtr) = NULL;
        return tmp1;
    }
    struct doublyLinkedNode* savedHead = *headPtr;
    do {
        if (tmp1->next->next == savedHead) {
            struct doublyLinkedNode* tmp2 = tmp1->next;
            tmp1->next = savedHead;
            savedHead->prev = tmp1;
            return tmp2;
        }
        tmp1 = tmp1->next;
    } while(tmp1!=savedHead);
    return NULL;
}

void addCircularHead(struct doublyLinkedNode** headPtr, int val) {
    struct doublyLinkedNode* newHead = malloc(sizeof(struct doublyLinkedNode));
    newHead->val = val;
    if (*headPtr == NULL)
        newHead->next = newHead, newHead->prev=newHead;
    else
        newHead->next = *headPtr, newHead->prev = (*headPtr)->prev;
    (*headPtr)->prev->next = newHead, (*headPtr)->prev = newHead;
    *headPtr = newHead;
}

void addCircularTail(struct doublyLinkedNode** headPtr, int val) {
    struct doublyLinkedNode* newTail = malloc(sizeof(struct doublyLinkedNode));
    newTail->val=val;
    if (*headPtr == NULL) {
        *headPtr = newTail;
        newTail->next = newTail, newTail->prev = newTail;
    }
    else {
        struct doublyLinkedNode *tmp = *headPtr, *savedHead = *headPtr;
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

void concatCircularDoublyLinkedList(struct doublyLinkedNode* baseHead, struct doublyLinkedNode* addedHead) {
    struct doublyLinkedNode *savedHead = baseHead;
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

struct doublyLinkedNode* applyFunctionToCircularDoublyLinkedList(struct doublyLinkedNode* head, int (*fun_ptr)(int)) {
    struct doublyLinkedNode *newHead = malloc(sizeof(struct doublyLinkedNode)), *currentNode = newHead, *savedHead = head;
    do {
        currentNode->val = fun_ptr(head->val);
        if (head->next == savedHead) {
            currentNode->next = newHead, newHead->prev = currentNode;
            break;
        }
        currentNode->next = malloc(sizeof(struct doublyLinkedNode));
        currentNode->next->prev = currentNode, currentNode=currentNode->next, head=head->next;
    } while(head != savedHead);
    return newHead;
}