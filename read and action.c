#include <stdio.h>
#include "socket.h"
#include <stdlib.h>

#define CHECK 1
#define CALL  2
#define RAISE 3
#define ALLIN 4
#define FOLD  5

void get_word(char **msg, int fd)
{
    char tmp;
    bool flag = 0;
    *msg = 0;
    while(1){
        read(fd, tmp, sizeof(tmp));
        if(tmp == ' ' || tmp == '\n'){
            if(flag == 1)break;
        }else strcat(*msg, tmp), flag = 1;
    }
}

int change_to_num(char *msg, int *number)// return -1: not number    return 0: is number
{
    int l = strlen(msg), flag = 1;
    char tmp[l + 5];
    strcpy(tmp, msg);
    int num = 0, i;
    for(i = 0; i < l; i++){
        if(i == 0 && tmp[i] == '-')flag = -1;
        else if(tmp[i] >= '0' && tmp[i] <= '9'){
            num *= 10;
            num += tmp[i] - 48;
        }else return -1;
    }
    *number = num * flag;
    return 0;
}

void action(int x, int num, int fd)//1:check  2:call  3:raise num  4:all_in  5:fold
{
    char action_msg[25] = {0};
    if(x == CHECK){
        strcpy(action_msg, "check \n");
    }else if(x == CALL){
        strcpy(action_msg, "call \n");
    }else if(x == RAISE){
        char num1[10] = {0};
        while(num > 0){
            int ret = num%10;
            strcat(num1, ret + 48);
            num /= 10;    
        }
        strrev(num1);
        strcpy(action_msg, "raise ");
        strcat(action_msg, num1);
        strcat(action_msg, " \n");
    }else if(x == ALLIN){
        strcpy(action_msg, "all_in \n");
    }else if(x == FOLD){
        strcpy(action_msg, "fold \n");
    }
    write(fd, action_msg, sizeof(action_msg));
}

  //action(kind, bet number, socket);
	//get_word(&a_word_name, socket);
	//change_to_num(a_word_name, &a_number_name);






