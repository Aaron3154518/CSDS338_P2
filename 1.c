#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_SIZE 100
#define MAX_PRIO 256
#define TIMESLICE 1000 // ns

struct Process {
    char task[255];
	long tLeft; // ns
    int prio;
};

struct Queue {
    struct Process* pList[MAX_SIZE];
    int front, back;
};

struct QueueList {
    struct Queue qList[MAX_PRIO];
    int minPrio;
};

struct Loc {
    int qNum;
    int qIdx;
};

void push(struct Process* p, struct QueueList* qList) {
    if (p->prio < 0) { p->prio = 0; }
    if (p->prio >= MAX_PRIO) { p->prio = MAX_PRIO - 1; }
    struct Queue* q = &qList->qList[p->prio];
    // Queue is not full
    if (q->front != (q->back + 1) % MAX_SIZE) {
        if (q->back == -1) {
            q->front = q->back = 0;
        } else {
            q->back = (q->back + 1) % MAX_SIZE;
        }
        q->pList[q->back] = p;
        if (qList->minPrio == -1 || p->prio < qList->minPrio) {
            qList->minPrio = p->prio;
        }
    }
}

struct Process* pop(struct Loc* loc, struct QueueList* qList) {
    struct Process* res = NULL;
    if (loc->qNum != -1 && loc->qIdx != -1) {
        struct Queue* q = &qList->qList[loc->qNum];
        // Make sure idx is in range
        int idx = loc->qIdx;
        if ((q->front <= q->back) ? (idx >= q->front && idx <= q->back)
                : (idx >= q->front || idx <= q->back)) {
            res = q->pList[idx];
            while (idx != q->back) {
                int nextIdx = (idx + 1) % MAX_SIZE;
                q->pList[idx] = q->pList[nextIdx];
                idx = nextIdx;
            }
            q->pList[q->back] = NULL;
            if (q->front == q->back) {
                q->front = q->back = -1;
            } else {
                q->back = (q->back - 1) % MAX_SIZE;
            }

            // Adjust minPrio
            if (loc->qNum == qList->minPrio) {
                while (qList->minPrio < MAX_PRIO && qList->qList[qList->minPrio].front == -1) {
                    ++qList->minPrio;
                }
                if (qList->minPrio >= MAX_PRIO) {
                    qList->minPrio = -1;
                }
            }
        }
    }
    return res;
}

void getFront(struct Loc* loc, struct QueueList* qList) {
    loc->qNum = qList->minPrio;
    loc->qIdx = loc->qNum == -1 ? -1 : qList->qList[loc->qNum].front;
}

void initQueue(struct Queue* q) {
    q->front = q->back = -1;
    for (int i = 0; i < MAX_SIZE; i++) {
        q->pList[i] = NULL;
    }
}

void init(struct QueueList* qList) {
    qList->minPrio = -1;
    for (int i = 0; i < MAX_PRIO; i++) {
        initQueue(&qList->qList[i]);
    }
}

void clean(struct QueueList* qList) {
    for (int i = 0; i < MAX_PRIO; i++) {
        for (int j = 0; j < MAX_SIZE; j++) {
            free(qList->qList[i].pList[j]);
        }
    }
}

struct Process* newProcess(char* task, long tLeft, int prio) {
	struct Process* res = malloc(sizeof(struct Process));
	strcpy(res->task, task);
	res->tLeft = tLeft;
	res->prio = prio;
	return res;
}

// Using insertion sort because I want to maintain original order amongst pTimes values
void sortSchedule(int pTimes[], struct Process* pSched[], int len) {
	int tmp;
	struct Process* pTmp;
	for (int i = 1; i < len; i++) {
		int j = i;
		while (j >= 1 && pTimes[j] < pTimes[j - 1]) {
			tmp = pTimes[j];
			pTimes[j] = pTimes[j - 1];
			pTimes[j - 1] = tmp;

			pTmp = pSched[j];
			pSched[j] = pSched[j - 1];
			pSched[j - 1] = pTmp;
			j--;
		}
	}
}

int main(int argc, char* argv[]) {
	struct Process* pSched[256];
	int pTimes[256];
	int schedLen = 0;
	if (argc >= 2) {
		FILE* inFile = fopen(argv[1], "r");
		if (inFile != NULL) {
			char pTask[255];
			int pTime = 0, pPrio = 0;
			long pDur = 0;
			fscanf(inFile, "%d %s %d %d", &pTime, pTask, &pDur, &pPrio);
			while (schedLen < 256 && !ferror(inFile) && !feof(inFile)) {
				pSched[schedLen] = newProcess(pTask, pDur, pPrio);
				pTimes[schedLen] = pTime;
				++schedLen;
				fscanf(inFile, "%d %s %d %d", &pTime, pTask, &pDur, &pPrio);
			}
		}
	}
	sortSchedule(pTimes, pSched, schedLen);
	printf("Processe Insertion Schedule:\n");
	for (int i = 0; i < schedLen; i++) {
		if (i == 0 || pTimes[i] != pTimes[i - 1]) {
			printf("Time = %d\n", pTimes[i]);
		}
		printf("\t%s: Duration = %dns, Priority = %d\n",
				pSched[i]->task, pSched[i]->tLeft, pSched[i]->prio);
	}
	printf("\n");

    int active = 1;
    struct QueueList qLists[2];
    init(&qLists[0]);
    init(&qLists[1]);

	int schedIdx = 0;
    struct timespec dt={0, 100};
    long tLeft = 0;
    struct Loc loc = {-1, -1};
    struct Process* p = NULL;
    int cntr = -1;
    while (++cntr < 10000) {
        // Run current task
        if (p != NULL && tLeft > 0) {
            nanosleep(&dt, NULL);
            fflush(stdout);
            tLeft -= dt.tv_nsec;
			p->tLeft -= dt.tv_nsec;
        }
        // No process or timeslice expired
        if (p == NULL || tLeft <= 0) {
            // Preempt current task
            if (p != NULL) {
				if (p->tLeft <= 0) {
					printf("Task Finished: %s\n", p->task);
					free(p);
				} else {
                    push(p, &qLists[1 - active]);
				}
            }
			// Get location of next
			getFront(&loc, &qLists[active]);
            // Check if we should switch queues
            if (loc.qNum == -1 || loc.qIdx == -1) {
                active = 1 - active;
                getFront(&loc, &qLists[active]);
            }
			// Get next process to run
            p = pop(&loc, &qLists[active]);
			tLeft = TIMESLICE;
        }
		// Simulate new processes
		while (schedIdx < schedLen && pTimes[schedIdx] == cntr) {
			push(pSched[schedIdx++], &qLists[active]);
		}
    }

    clean(&qLists[0]);
    clean(&qLists[1]);
	while (schedIdx < schedLen) {
		free(pSched[schedIdx++]);
	}

    return 0;
}
