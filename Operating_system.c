#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define RAM_TOTAL 2048
#define RAM_HIGHEST_PRIORITY 512
#define MAX_PROCESSES 100




typedef struct {
    char process_number[10];
    int arrival_time;
    int priority;
    int burst_time;
    int ram;
    int cpu_rate;
    bool isDone;
} Process;

typedef struct {
    Process* que1;
    Process* que2;
    Process* que3;
    Process* que4;
    int num_processes_que1;
    int num_processes_que2;
    int num_processes_que3;
    int num_processes_que4;
} CPU;

char last[10];

void clearOutputFile() {
    FILE* fPointer;
    fopen_s(&fPointer, "output.txt", "w");
    if (fPointer == NULL) {
        printf("Error opening file.\n");
        return;
    }
    fclose(fPointer);
}
void writeOutput(char message[]) {
    FILE* fPointer;
    fopen_s(&fPointer, "output.txt", "a");
    if (fPointer == NULL) {
        printf("Error opening file.\n");
        return;
    }
    fprintf(fPointer, "%s\n", message);
    fclose(fPointer);
}

void FCFS(CPU* cpu1, int index, int remainingMemoryForThisCpu) {
    char message[50];
    if (remainingMemoryForThisCpu < cpu1->que1[index].ram) {
        sprintf_s(message, sizeof(message), "Process %s is exceeding RAM for CPU-1.", cpu1->que1[index].process_number);
        writeOutput(message);
    }
    else {
        int initialMemory = remainingMemoryForThisCpu;
        sprintf_s(message, sizeof(message), "Process %s is assigned to CPU-1.", cpu1->que1[index].process_number);
        writeOutput(message);
        remainingMemoryForThisCpu -= cpu1->que1[index].ram;
        sprintf_s(message, sizeof(message), "Process %s is completed and terminated.\n", cpu1->que1[index].process_number);
        writeOutput(message);
        remainingMemoryForThisCpu = initialMemory;
        cpu1->que1[index].isDone = true;
    }
}

void SJF(CPU* cpu2, int* remainingMemoryForThisCpu) {
    // Sort the processes in the queue based on burst time and CPU rate (shortest job first)
    for (int i = 0; i < cpu2->num_processes_que2 - 1; i++) {
        for (int j = 0; j < cpu2->num_processes_que2 - i - 1; j++) {
            // Calculate the completion time for each process based on burst time and CPU rate
            float completionTime1 = (float)cpu2->que2[j].burst_time / cpu2->que2[j].cpu_rate;
            float completionTime2 = (float)cpu2->que2[j + 1].burst_time / cpu2->que2[j + 1].cpu_rate;

            // Compare completion times to determine the order
            if (completionTime1 > completionTime2) {
                // Swap the processes
                Process temp = cpu2->que2[j];
                cpu2->que2[j] = cpu2->que2[j + 1];
                cpu2->que2[j + 1] = temp;
            }
        }
    }
    // Execute the processes
    for (int i = 0; i < cpu2->num_processes_que2; i++) {
        if (!cpu2->que2[i].isDone) {
            // Check if there is enough remaining memory for the process
            if (*remainingMemoryForThisCpu >= cpu2->que2[i].ram) {
                char message[50];
                sprintf_s(message, sizeof(message), "Process %s is assigned to CPU-2.", cpu2->que2[i].process_number);
                writeOutput(message);

                // Simulate the execution by deducting burst time from remaining memory
                *remainingMemoryForThisCpu -= cpu2->que2[i].burst_time;
                sprintf_s(message, sizeof(message), "Process %s is completed and terminated.\n", cpu2->que2[i].process_number);
                writeOutput(message);

                // the process is done
                cpu2->que2[i].isDone = true;
                // Add the memory used by the process back to the remaining memory
                *remainingMemoryForThisCpu += cpu2->que2[i].ram;
            }
            else {
                char message[50];
                sprintf_s(message, sizeof(message), "Insufficient RAM for Process %s in CPU-2.\n", cpu2->que2[i].process_number);
                writeOutput(message);
            }
        }
    }
}


void Round_Robin(CPU* cpu, int* remainingMemoryForThisCpu, int quantum_time, int queue_num) {
    char message[100];
    int num_processes;
    Process* queue;

    switch (queue_num) {
    case 3:
        queue = cpu->que3;
        num_processes = cpu->num_processes_que3;
        break;
    case 4:
        queue = cpu->que4;
        num_processes = cpu->num_processes_que4;
        break;
    default:
        return; // Invalid queue number
    }

    for (int i = 0; i < num_processes; i++) {
        if (!queue[i].isDone) {
            if (queue[i].burst_time <= quantum_time) {
                sprintf_s(message, sizeof(message), "Process %s is assigned to CPU-2.", queue[i].process_number);
                writeOutput(message);
                *remainingMemoryForThisCpu -= queue[i].burst_time;
                sprintf_s(message, sizeof(message), "Process %s is completed and terminated.\n", queue[i].process_number);
                writeOutput(message);
                queue[i].isDone = true;
                *remainingMemoryForThisCpu += queue[i].ram;
            }
            else {
                sprintf_s(message, sizeof(message), "Process %s is assigned to CPU-2.", queue[i].process_number);
                writeOutput(message);
                queue[i].burst_time -= quantum_time;
                sprintf_s(message, sizeof(message), "Process %s is preempted.\n", queue[i].process_number);
                writeOutput(message);
            }
        }
    }
}


void runCpu1(CPU* cpu1, int remainingMemoryForThisCpu) {
    int counter = 0;
    while (counter < cpu1->num_processes_que1) {
        if (!cpu1->que1[counter].isDone) {
            FCFS(cpu1, counter, remainingMemoryForThisCpu);
        }
        counter++;
    }
}

void runCpu2(CPU* cpu2, int* remainingMemoryForThisCpu) {
    SJF(cpu2, remainingMemoryForThisCpu);
    Round_Robin(cpu2, remainingMemoryForThisCpu, 8, 3); //8 quatum time
    Round_Robin(cpu2, remainingMemoryForThisCpu, 16, 4); //16 quatum time
}

void printSummary(CPU* cpu1, CPU* cpu2) {
    FILE* fPointer;
    fopen_s(&fPointer, "output.txt", "a");
    if (fPointer == NULL) {
        printf("Error opening file.\n");
        return;
    }

    int queue1Size = cpu1->num_processes_que1;
    int queue2Size = cpu2->num_processes_que2;
    int queue3Size = cpu2->num_processes_que3;
    int queue4Size = cpu2->num_processes_que4;

    fprintf(fPointer, "CPU-1 que1(priority-0) (FCFS) = ");
    for (int i = 0; i < queue1Size; i++) {
        fprintf(fPointer, "%s, ", cpu1->que1[i].process_number);
    }
    fprintf(fPointer, "...\n");

    fprintf(fPointer, "CPU-2 que2(priority-1) (SJF) = ");
    for (int i = 0; i < queue2Size; i++) {
        fprintf(fPointer, "%s, ", cpu2->que2[i].process_number);
    }
    fprintf(fPointer, "...\n");

    fprintf(fPointer, "CPU-2 que3(priority-2) (RR-q8) = ");
    for (int i = 0; i < queue3Size; i++) {
        fprintf(fPointer, "%s, ", cpu2->que3[i].process_number);
    }
    fprintf(fPointer, "...\n");

    fprintf(fPointer, "CPU-2 que4(priority-3) (RR-q16) = ");
    for (int i = 0; i < queue4Size; i++) {
        fprintf(fPointer, "%s, ", cpu2->que4[i].process_number);
    }
    fprintf(fPointer, "...\n");

    fclose(fPointer);
}


void readFile(CPU* cpu1, CPU* cpu2) {
    int ramForCpu1 = RAM_HIGHEST_PRIORITY;
    int ramForCpu2 = RAM_TOTAL - RAM_HIGHEST_PRIORITY;

    FILE* fPointer;
    fopen_s(&fPointer, "input.txt", "r");
    if (fPointer == NULL) {
        printf("Error opening file.\n");
        return;
    }

    char singleLine[150];
    int initTime = 0;

    while (fgets(singleLine, sizeof(singleLine), fPointer) != NULL) {
        Process process;
        process.isDone = false;
        sscanf_s(singleLine, "%[^,],%d,%d,%d,%d,%d",
            process.process_number, (unsigned)_countof(process.process_number),
            &process.arrival_time,
            &process.priority,
            &process.burst_time,
            &process.ram,
            &process.cpu_rate);

        if (initTime != process.arrival_time) {
            runCpu1(cpu1, ramForCpu1);
            initTime = process.arrival_time;
            runCpu2(cpu2, &ramForCpu2); // Execute CPU 2 tasks
        }

        strcpy_s(last, sizeof(last), process.process_number);

        // Enqueue processes into the appropriate queue based on priority status
        if (process.priority == 0) {
            cpu1->que1[cpu1->num_processes_que1++] = process;
            char message[90] = "";
            sprintf_s(message, sizeof(message), "Process %s is queued to be assigned to CPU-1.\n", process.process_number);
            writeOutput(message);
        }
        else {
            switch (process.priority) {
            case 1:
                cpu2->que2[cpu2->num_processes_que2++] = process;
                break;
            case 2:
                cpu2->que3[cpu2->num_processes_que3++] = process;
                break;
            case 3:
                cpu2->que4[cpu2->num_processes_que4++] = process;
                break;
            }
            char message[90] = "";

            sprintf_s(message, sizeof(message), "Process %s is queued to be assigned to CPU-2.\n", process.process_number);
            writeOutput(message);
        }

    }

    fopen_s(&fPointer, "output.txt", "a");
    if (fPointer == NULL) {
        printf("Error opening file.\n");
        return;
    }
    char message[50];
    sprintf_s(message, sizeof(message), "Process %s is assigned to CPU-1.", last);
    fprintf(fPointer, "%s\n", message);

    sprintf_s(message, sizeof(message), "Process %s is completed and terminated.\n", last);
    fprintf(fPointer, "%s\n", message);
    fclose(fPointer);



}

int main() {
    clearOutputFile();
    CPU cpu1;
    CPU cpu2;

    cpu1.que1 = (Process*)malloc(MAX_PROCESSES * sizeof(Process));
    cpu2.que2 = (Process*)malloc(MAX_PROCESSES * sizeof(Process));
    cpu2.que3 = (Process*)malloc(MAX_PROCESSES * sizeof(Process));
    cpu2.que4 = (Process*)malloc(MAX_PROCESSES * sizeof(Process));

    cpu1.num_processes_que1 = 0;
    cpu2.num_processes_que2 = 0;
    cpu2.num_processes_que3 = 0;
    cpu2.num_processes_que4 = 0;

    if (cpu1.que1 == NULL || cpu2.que2 == NULL || cpu2.que3 == NULL || cpu2.que4 == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    readFile(&cpu1, &cpu2);
    printSummary(&cpu1, &cpu2);

    free(cpu1.que1);
    free(cpu2.que2);
    free(cpu2.que3);
    free(cpu2.que4);


    return 0;
}
