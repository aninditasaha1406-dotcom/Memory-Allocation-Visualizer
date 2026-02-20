#include <stdio.h>

/* Function declarations */
void firstFit(int blocks[], int m, int processes[], int n);
void bestFit(int blocks[], int m, int processes[], int n);
void worstFit(int blocks[], int m, int processes[], int n);

void printAsciiMemory(int original[], int remaining[], int m);
int calculateInternalFragmentation(int original[], int remaining[], int m);

/* ========================= MAIN ========================= */
int main() {

    /* Disable output buffering to avoid black screen */
    setbuf(stdout, NULL);

    int m, n, choice;
    int i;

    printf("\n====================================================\n");
    printf("  SIMULATION OF CONTIGUOUS MEMORY ALLOCATION\n");
    printf("====================================================\n");

    /* STEP 1: MEMORY INPUT */
    printf("\nSTEP 1: ENTER MEMORY BLOCK DETAILS\n");
    printf("----------------------------------------\n");
    printf("Enter the TOTAL number of memory blocks: ");
    scanf("%d", &m);

    int blockSize[m];
    for (i = 0; i < m; i++) {
        printf("Enter size of Block %d (in KB): ", i + 1);
        scanf("%d", &blockSize[i]);
    }

    /* Save original blocks */
    int originalBlocks[m];
    for (i = 0; i < m; i++)
        originalBlocks[i] = blockSize[i];

    /* STEP 2: PROCESS INPUT */
    printf("\nSTEP 2: ENTER PROCESS DETAILS\n");
    printf("----------------------------------------\n");
    printf("Enter the TOTAL number of processes: ");
    scanf("%d", &n);

    int processSize[n];
    for (i = 0; i < n; i++) {
        printf("Enter memory required for Process %d (in KB): ", i + 1);
        scanf("%d", &processSize[i]);
    }

    /* STEP 3: CHOICE */
    printf("\nSTEP 3: SELECT ALLOCATION STRATEGY\n");
    printf("----------------------------------------\n");
    printf("1 -> First Fit\n");
    printf("2 -> Best Fit\n");
    printf("3 -> Worst Fit\n");
    printf("4 -> Compare ALL (First, Best, Worst)\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    printf("\n====================================================\n");

    if (choice == 1) {
        for (i = 0; i < m; i++) blockSize[i] = originalBlocks[i];
        printf("\n>>> FIRST FIT <<<\n");
        firstFit(blockSize, m, processSize, n);
    }
    else if (choice == 2) {
        for (i = 0; i < m; i++) blockSize[i] = originalBlocks[i];
        printf("\n>>> BEST FIT <<<\n");
        bestFit(blockSize, m, processSize, n);
    }
    else if (choice == 3) {
        for (i = 0; i < m; i++) blockSize[i] = originalBlocks[i];
        printf("\n>>> WORST FIT <<<\n");
        worstFit(blockSize, m, processSize, n);
    }
    else if (choice == 4) {

        for (i = 0; i < m; i++) blockSize[i] = originalBlocks[i];
        printf("\n>>> FIRST FIT <<<\n");
        firstFit(blockSize, m, processSize, n);

        for (i = 0; i < m; i++) blockSize[i] = originalBlocks[i];
        printf("\n>>> BEST FIT <<<\n");
        bestFit(blockSize, m, processSize, n);

        for (i = 0; i < m; i++) blockSize[i] = originalBlocks[i];
        printf("\n>>> WORST FIT <<<\n");
        worstFit(blockSize, m, processSize, n);
    }
    else {
        printf("Invalid choice! Restart the program.\n");
    }

    printf("\n============= END OF SIMULATION =============\n");
    printf("Press Enter to exit...");
    getchar();
    getchar();

    return 0;
}

/* ================= ASCII MEMORY MAP ================= */
void printAsciiMemory(int original[], int remaining[], int m) {
    int i;

    printf("\n---------------- MEMORY MAP (ASCII) ----------------\n");
    for (i = 0; i < m; i++) {
        if (remaining[i] < original[i]) {
            printf("| Block %d | Used: %d KB | Free: %d KB |\n",
                   i + 1,
                   original[i] - remaining[i],
                   remaining[i]);
        } else {
            printf("| Block %d | Free: %d KB |\n",
                   i + 1, original[i]);
        }
    }
    printf("----------------------------------------------------\n");
}

/* ========== INTERNAL FRAGMENTATION ========== */
int calculateInternalFragmentation(int original[], int remaining[], int m) {
    int i, frag = 0;
    for (i = 0; i < m; i++) {
        if (original[i] != remaining[i]) {
            frag += remaining[i];
        }
    }
    return frag;
}

/* ================= FIRST FIT ================= */
void firstFit(int blocks[], int m, int processes[], int n) {
    int allocation[n];
    int original[m];
    int i, j;

    for (i = 0; i < m; i++) original[i] = blocks[i];
    for (i = 0; i < n; i++) allocation[i] = -1;

    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            if (blocks[j] >= processes[i]) {
                allocation[i] = j;
                blocks[j] -= processes[i];
                break;
            }
        }
    }

    printf("\nPROCESS ALLOCATION (FIRST FIT)\n");
    for (i = 0; i < n; i++) {
        if (allocation[i] != -1)
            printf("Process %d (%d KB) -> Block %d\n",
                   i + 1, processes[i], allocation[i] + 1);
        else
            printf("Process %d (%d KB) -> Not Allocated\n",
                   i + 1, processes[i]);
    }

    printf("Internal Fragmentation = %d KB\n",
           calculateInternalFragmentation(original, blocks, m));
    printAsciiMemory(original, blocks, m);
}

/* ================= BEST FIT ================= */
void bestFit(int blocks[], int m, int processes[], int n) {
    int allocation[n];
    int original[m];
    int i, j, bestIdx;

    for (i = 0; i < m; i++) original[i] = blocks[i];
    for (i = 0; i < n; i++) allocation[i] = -1;

    for (i = 0; i < n; i++) {
        bestIdx = -1;
        for (j = 0; j < m; j++) {
            if (blocks[j] >= processes[i]) {
                if (bestIdx == -1 || blocks[j] < blocks[bestIdx])
                    bestIdx = j;
            }
        }
        if (bestIdx != -1) {
            allocation[i] = bestIdx;
            blocks[bestIdx] -= processes[i];
        }
    }

    printf("\nPROCESS ALLOCATION (BEST FIT)\n");
    for (i = 0; i < n; i++) {
        if (allocation[i] != -1)
            printf("Process %d (%d KB) -> Block %d\n",
                   i + 1, processes[i], allocation[i] + 1);
        else
            printf("Process %d (%d KB) -> Not Allocated\n",
                   i + 1, processes[i]);
    }

    printf("Internal Fragmentation = %d KB\n",
           calculateInternalFragmentation(original, blocks, m));
    printAsciiMemory(original, blocks, m);
}

/* ================= WORST FIT ================= */
void worstFit(int blocks[], int m, int processes[], int n) {
    int allocation[n];
    int original[m];
    int i, j, worstIdx;

    for (i = 0; i < m; i++) original[i] = blocks[i];
    for (i = 0; i < n; i++) allocation[i] = -1;

    for (i = 0; i < n; i++) {
        worstIdx = -1;
        for (j = 0; j < m; j++) {
            if (blocks[j] >= processes[i]) {
                if (worstIdx == -1 || blocks[j] > blocks[worstIdx])
                    worstIdx = j;
            }
        }
        if (worstIdx != -1) {
            allocation[i] = worstIdx;
            blocks[worstIdx] -= processes[i];
        }
    }

    printf("\nPROCESS ALLOCATION (WORST FIT)\n");
    for (i = 0; i < n; i++) {
        if (allocation[i] != -1)
            printf("Process %d (%d KB) -> Block %d\n",
                   i + 1, processes[i], allocation[i] + 1);
        else
            printf("Process %d (%d KB) -> Not Allocated\n",
                   i + 1, processes[i]);
    }

    printf("Internal Fragmentation = %d KB\n",
           calculateInternalFragmentation(original, blocks, m));
    printAsciiMemory(original, blocks, m);
}

