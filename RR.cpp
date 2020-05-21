#include <stdio.h>
#include <malloc.h>


/*
    定义进程的结构体
*/
typedef struct Node{
    char name;          //进程的名字
    int pcbNum;         //进程的唯一标识符
    int atTime;         //进程的到达时间
    int runTime;        //进程需要的运行时间
    int status;         //进程状态，0-4五状态定义
    int isWait;         //进程是否有等待事件
    int waitTime;       //等待事件的长度
    int whenWait;       //何时出现等待事件
    int restTime;       //进程的剩余运行时间
}PCB;


/*
    定义五个状态的进程队列
*/
struct Node newPcb[10];
struct Node running[1];
struct Node ready[100];
struct Node stocked[100];
struct Node exit[10];


/*
    从控制台输入进程
*/
void create(int num){
    for(int i = 0; i < num; i++){
        PCB pcb;

        pcb.status = 0;
        pcb.name = 'A' + i;
        pcb.pcbNum = i + 19 * i;
        printf("input the %d program arrive time:\n",i+1);
        scanf("%d",&pcb.atTime);
        printf("input the %d program running time:\n",i+1);
        scanf("%d",&pcb.runTime);
        pcb.restTime = pcb.runTime;
        printf("input 0 or 1 to show is this program need to stocked(0 for not):\n");
        scanf("%d",&pcb.isWait);
        if(pcb.isWait == 1){
            printf("input the program wait time:\n");
            scanf("%d",&pcb.waitTime);
            printf("when will the io need waiting:\n");
            scanf("%d",&pcb.whenWait);
        }
        if(pcb.isWait == 0){
            pcb.waitTime = 0;
            pcb.whenWait = 0;
        }
        newPcb[i] = pcb;
    }
}


/*
    进程运行
*/
void Run(int time){
    running[0].restTime--;
    printf("this is the %d time, this is the \"%c\" progress, and it is the %d time to be running.\n",
                time,running[0].name,running[0].runTime - running[0].restTime);
}


/*
    每一次调度打印当前的队列的状态
*/
void myPrint(int readyFront,int readyTail,int stockedFront, int stockedTail, int exitFront, int exitTail){
    printf("ready queue: ");
    for(int i = readyFront; i < readyTail; i++){
        printf("name: %c status: %d ",ready[i].name,ready[i].status);
    }

    printf("\nstocked queue:");
    if(stockedTail - stockedFront == 0){
        printf("there is no progress in stocked queue.");
    }
    else{
        for(int i = stockedFront; i < stockedTail; i++){
            printf("name: %c status: %d ",stocked[i].name,stocked[i].status);
        }
    }

    printf("\nexit queue: ");
    if(exitTail - exitFront == 0){
        printf("there is no progress in exit queue.\n");
    }
    else{
        for(int i = exitFront; i < exitTail; i++){
            printf("name: %c status: %d ",exit[i].name,exit[i].status);
        }
        printf("\n");
    }
}

void RR(int time,int num){
    int readyFront = 0,readyTail = 0;
    int stockFront = 0,stockTail = 0;
    int exitFront = 0, exitTail = 0;

    for(int cpuTime = 1; cpuTime <= time; cpuTime++){
        /*
            判断new的进程是否到达，将到达的进程放入就绪队列
        */
        for(int i = 0; i < num; i++){
            if(newPcb[i].atTime == cpuTime){
                ready[readyTail] = newPcb[i];
                ready[readyTail].status = 1;
                readyTail++;
            }
        }

        /*
            判断阻塞态的进程是否结束等待事件，如果是，将其放入就绪队列中
        
        */
        if(stockTail - stockFront > 0){
            if(stocked[stockFront].waitTime == 0){
                ready[readyTail] = stocked[stockFront];
                ready[readyTail].status = 1;
                readyTail++;
                stockFront++;
            }
            else{
                stocked[stockFront].waitTime--;
            }
        }
        

        /*
            根据轮询将就绪队列中的进程调入
        */
        running[0] = ready[readyFront];
        running[0].status = 2;
        readyFront++;

        
        /*
            进程运行
        */
        Run(cpuTime);

        /*
            对各个队列进行输出
        */
        myPrint(readyFront,readyTail,stockFront,stockTail,exitFront,exitTail);

        /*
            该进程运行完后，时间片用完进入exit完成队列
        */
        if(running[0].restTime == 0){
            exit[exitTail] = running[0];
            exit[exitTail].status = 4;
            exitTail++;
        }

        /*
            该进程运行结束后，没有阻塞事件，仍未执行完毕，直接回到就绪队列
        */
        if(running[0].isWait == 0 && running[0].restTime > 0){
            ready[readyTail] = running[0];
            ready[readyTail].status = 2;
            readyTail++;
        }

        /*
            判断是否是有等待的进程，并且判断该进程是否进入阻塞队列
        */
        if(running[0].isWait == 1){
            if(running[0].whenWait == 1){
                stocked[stockTail] = running[0];
                stocked[stockTail].status = 4;
                stockTail++;
            }
            else{
                ready[readyTail] = running[0];
                ready[readyTail].status = 2;
                ready[readyTail].whenWait--;
                readyTail++;
            }
        }
    }
}


int main(void){
    int num = 0, time = 0;

    printf("how many progress do you want to test by RR:\n");
    scanf("%d",&num);

    create(num);
    for(int i= 0; i < num; i++){
        time = time + newPcb[i].runTime;
    }
    RR(time,num);

    return 0;
}

