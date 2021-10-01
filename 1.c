#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define MAX_SIZE 100
#define MAX_PRIO 256

struct Process {
	char* task;
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
		if (qList->minPrio = -1 || p->prio < qList->minPrio) {
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

void clean(struct QueueList* qList) {
	for (int i = 0; i < MAX_PRIO; i++) {
		for (int j = 0; j < MAX_SIZE; j++) {
			free(qList->qList[i].pList[j]);
		}
	}
}

void getFront(struct Loc* loc, struct QueueList* qList) {
	loc->qNum = qList->minPrio;
	loc->qIdx = loc->qNum == -1 ? -1 : qList->qList[loc->qNum].front;
}

void init(struct QueueList* qList) {
	qList->minPrio = -1;
	for (int i = 0; i < MAX_PRIO; i++) {
		qList->qList[i].front = qList->qList[i].back = -1;
	}
}

int main() {
	int active = 0;
	struct QueueList qLists[2];
	init(&qLists[0]); init(&qLists[1]);

    struct Process* p = malloc(sizeof(struct Process));
	p->task = "Woohoo!"; p->prio = 100; 
	push(p, &qLists[active]);

    struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 5e8; // .5s

	while (1) {
		struct Loc loc;
		getFront(&loc, &qLists[active]);
		while (loc.qNum != -1 && loc.qIdx != -1) {
			struct Process* p = pop(&loc, &qLists[active]);
			printf("%s\n", p->task);
			fflush(stdout);
			nanosleep(&ts, NULL);
			push(p, &qLists[1 - active]);
			getFront(&loc, &qLists[active]);
		}
		active = 1 - active;
	}

	clean(&qLists[0]);
	clean(&qLists[1]);

	return 0;
}
