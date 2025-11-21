#include <stdio.h>
#include <stdlib.h>
struct Pilot{
    char name[30];
    int id;
    char gender;
    int experience;
};
struct Flight{
    char id[10];
    char airline[30];
    char start[20];
    char destination[20];
    char date[10];
    char time[10];
    char duration[3];
    int seats;
    int economy;
    int luxury;
    int ep;
    int seats_left;
    int seats_occupied;
    int lp;
};
struct customer{
    char name[30];
    int age;
    char password[30];
    long long phone;
};
int main(){
    printf("===========================================\n");
    printf("            IIITB AIRPORT                   \n");
    printf("              WELCOME!                       \n");
    printf("============================================\n");
    printf("Choose an option: \n");
    printf("1.Continue as Customer\n");
    printf("2.Continue as Admin\n");
    printf("3.View flights\n");
    printf("4.Exit application\n");
    int choice;
    
    while(1){
        printf("enter choice: ");
        scanf("%d",&choice);
    if(choice==4){
        printf("THANK YOU FOR USING THE IIITB AIRPORT WEBSITE!\n ====PLEASE VISIT AGAIN!====\n");
        break;
    }
    else if(choice==1){
        printf("WELCOME!");
        printf("1.Login(existing customer)\n");
        printf("2.Sign up(new customer)\n");
        int customer_type;
        printf("enter choice: ");
        scanf("%d",&customer_type);
    }
}}
