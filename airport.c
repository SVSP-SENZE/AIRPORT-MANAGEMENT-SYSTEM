#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Pilot{
    char id[5];
    char name[30];
    char gender[2];
    char experience[10];
    int age;
    char airline[20];
};

struct Flight{
    char id[10];
    char type[20];
    int eprice;
    int lprice;
    char dest[20];
    struct Flight *next;
};

struct Flight *head = NULL;

struct Admins{
    char id[10];
    char name[30];
    char password[10];
};
void cancel(name){
    FILE *f=fopen("bookings.txt","r");
    FILE *t=fopen("t.txt","w");
    char n[30];char id[20];
    int p;int c=0;
      while (fscanf(f, "%s %s %d",n,id,&p)!=EOF) {
        if (strcmp(n,name) ==0) {
            c=1;
            printf("Flight tickets for flight id: %s cancelled\n",id);
        } else {
            fprintf(t, "%s %s %d\n",n,id,p);
        }
    }

    fclose(f);
    fclose(t);

    remove("bookings.txt");
    rename("temp.txt", "bookings.txt");

    if (!c)
        printf("No such ticket found to cancel\n");

     
}
int check(char *name, char *pass){
    FILE *f = fopen("customerdata.txt", "r");
    if(!f) return 0;

    char name2[30], pass2[30];
    int age;
    long long ph;

    while(fscanf(f,
        "Name: %s\nAge: %d\nPhone: %lld\nPassword: %s\n---------------------\n",
        name2, &age, &ph, pass2) == 4)
    {
        if(strcmp(name2, name)==0 && strcmp(pass2, pass)==0){
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0;
}

void view_flights(){
    struct Flight *p = head;

    printf("***AVAILABLE FLIGHTS***\n");
    while(p != NULL){
        printf("\nFlight ID: %s\n", p->id);
        printf("Aircraft: %s\n", p->type);
        printf("Destination: %s\n", p->dest);
        printf("Economy Price: %d\n", p->eprice);
        printf("Luxury Price: %d\n", p->lprice);
        printf("-------------------------\n");
        p = p->next;
    }
}

void book_ticket(char name[30]){
    char id[10];
    int c;

    printf("Enter flight id you want to book: \n");
    scanf("%s", id);

    struct Flight *p = head;
    while(p != NULL && strcmp(p->id, id) != 0)
        p = p->next;

    if(p == NULL){
        printf("Invalid flight ID!\n");
        return;
    }

    printf("1.Economy\n2.Luxury\nEnter choice: ");
    scanf("%d", &c);

    int price = (c == 1) ? p->eprice : p->lprice;

    FILE *fp = fopen("bookings.txt", "a");
    fprintf(fp, "%s %s %d\n", name, p->id, price);
    fclose(fp);

    printf("Ticket booking successful!\n");
}

void customer_login(){
    printf("****NEW CUSTOMER LOGIN!****\n");

    char name[30];
    int age;
    long long ph;
    char pass[20];

    printf("Enter name: ");
    scanf("%s",name);
    printf("Enter age: ");
    scanf("%d", &age);
    printf("Enter phone number: ");
    scanf("%lld", &ph);
    printf("Enter a valid password: ");
    scanf("%s", pass);

    FILE *f = fopen("customerdata.txt", "a");
    if(f){
    fprintf(f, "Name: %s\nAge: %d\nPhone: %lld\nPassword: %s\n",
            name, age, ph, pass);
    fprintf(f, "---------------------\n");
    fclose(f);}

    printf("CUSTOMER REGISTRATION SAVED SUCCESSFULLY!\n");
}

void customer_dashboard(char *name){
    while(1){
        int choice;

        printf("1.Book Ticket\n");
        printf("2.Cancel tickets\n");
        printf("3.Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if(choice == 3){
            printf("Logging out....\n");
            return;
        }
        else if(choice == 1){
            book_ticket(name);
        }
        else if(choice ==2){
            cancel(name);
        }
    }
}

void customer_menu(){
    printf("WELCOME!\n");

    while(1){
        int customer_type;

        printf("1.Login(existing customer)\n");
        printf("2.Sign up(new customer)\n");
        printf("3.Return to menu\n");
        printf("enter choice: ");
        scanf("%d", &customer_type);

        if(customer_type == 2){
            customer_login();
        }
        else if(customer_type == 1){
            char name[30], pass[20];

            printf("Please enter your name: ");
            scanf("%s", name);

            printf("Please enter your password: ");
            scanf("%s", pass);

            if(check(name, pass)){
                printf("Login successful!\n");
                customer_dashboard(name);
            }
            else{
                printf("Invalid credentials\n");
            }
        }
        else if(customer_type == 3){
            return;
        }
        else{
            printf("Invalid option\n");
        }
    }
}

int main(){
    printf("===========================================\n");
    printf("            IIITB AIRPORT                   \n");
    printf("              WELCOME!                      \n");
    printf("============================================\n");

    printf("Choose an option: \n");
    printf("1.Continue as Customer\n");
    printf("2.Continue as Admin\n");
    printf("3.View flights\n");
    printf("4.Exit application\n");

    int choice;

    while(1){
        printf("enter choice: ");
        scanf("%d", &choice);

        if(choice == 4){
            printf("THANK YOU FOR USING THE IIITB AIRPORT WEBSITE!\n====PLEASE VISIT AGAIN!====\n");
            break;
        }
        else if(choice == 1){
            customer_menu();
        }
        else if(choice == 3){
            view_flights();
        }
    }
}
