#include <stdio.h>
#include <string.h>

typedef enum reg_status{
    REG_DISABLE,
    REG_INIT,
    REG_REGING,
    REG_UP,
    REG_UNREGING,
    REG_QUIES,
    REG_ERR
}reg_status_e;

struct voipRegState{
    reg_status_e status;
    char        desc[20];
};


struct voipRegState regState[] = 
{
    REG_DISABLE,        "Disabled",
    REG_INIT,           "Initializing",
    REG_REGING,         "Registering",
    REG_UP,             "Up",
    REG_UNREGING,       "Unregistering",
    REG_QUIES,          "Quiescent",
    REG_ERR,            "Error"
};


int main(int argc, char const *argv[])
{
    int i;
    char *str = "pass";
    char *array[]= {"test","hello","we","world","end"};
    for(i=0;i<sizeof(array);i++){
        if(strcmp(str,array[i]) == 0){
            return;
        }
    }
    strcpy(array[i],str);

    for(i=0;i<sizeof(array);i++){
        printf("%s\n",array[i]);
    }

    return 0;
}

