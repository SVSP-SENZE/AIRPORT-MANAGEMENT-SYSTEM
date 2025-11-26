#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void save_flight();
struct Pilot{
    char id[5];
    char name[30];
    char gender[2];
    char experience[10];
    int age;
    char airline[20];
    struct Pilot* next;
};

struct Flight{
    char id[10];
    char type[20];
    int eprice;
    int lprice;
    char dest[20];
    char pilot[30];
    struct Flight *next;
};

struct Flight *head = NULL;
struct Pilot *phead=NULL;
struct Admins{
    char id[10];
    char name[30];
    char password[10];
};
void assign_pilot(){
    char fid[10], pid[10];
    printf("Enter Flight ID: ");
    scanf("%s", fid);
    printf("Enter Pilot ID: ");
    scanf("%s", pid);

    struct Flight *f = head;
    while(f && strcmp(f->id, fid) != 0)
        f = f->next;

    if(!f){
        printf("Flight not in database\n");
        return;
    }

    struct Pilot *p = phead;
    while(p && strcmp(p->id, pid) != 0)
        p = p->next;

    if(!p){
        printf("Pilot not in database\n");
        return;
    }

    strcpy(f->pilot, p->name);
    save_flight();
    printf("Pilot %s has been assigned to flight %s\n", p->name, fid);
}
void load_pilots(){
    FILE *f=fopen("pilots.txt","r");
    if(!f) return;
    struct Pilot *p;
    while(1){
        p=(struct Pilot*)malloc(sizeof(struct Pilot));
        if(fscanf(f,"%s %s %s %s %d %s",
                  p->id,p->name,p->gender,p->experience,&p->age,p->airline)!=6){
            free(p);
            break;
        }
        p->next=phead;
        phead=p;
    }
    fclose(f);
}
void savepilot(){
    FILE *f=fopen("pilots.txt","w");
    struct Pilot *p=phead;
    while(p){
        fprintf(f,"%s %s %s %s %d %s\n",
                p->id,p->name,p->gender,p->experience,p->age,p->airline);
        p=p->next;
    }
    fclose(f);
}
void addpilot(){
    struct Pilot *p=(struct Pilot*)malloc(sizeof(struct Pilot));

    printf("Enter pilot id: ");
    scanf("%s",p->id);
    printf("Enter name: ");
    scanf("%s",p->name);
    printf("Enter gender(M/F): ");
    scanf("%s",p->gender);
    printf("Enter experience: ");
    scanf("%s",p->experience);
    printf("Enter age: ");
    scanf("%d",&p->age);
    printf("Enter airline: ");
    scanf("%s",p->airline);

    p->next=phead;
    phead=p;
    savepilot();
    printf("New pilot added\n");

}
void view_pilots(){
    struct Pilot *p=phead;
    printf("***PILOT INFORMATION LOG***\n");
    while(p){
        printf("ID: %s\n",p->id);
        printf("Name: %s\n",p->name);
        printf("Gender: %s\n",p->gender);
        printf("Experience: %s\n",p->experience);
        printf("Age: %d\n",p->age);
        printf("Airline: %s\n",p->airline);
        printf("-------------------\n");
        p=p->next;
    }
}
void editpilot(){
       char id[10];
    printf("Enter pilot ID to edit: ");
    scanf("%s",id);

    struct Pilot *p=phead;
    while(p && strcmp(p->id,id)!=0)
        p=p->next;

    if(!p){
        printf("No such pilot exists\n");
        return;
    }

    printf("Enter new name: ");
    scanf("%s",p->name);
    printf("Enter new gender(M/F): ");
    scanf("%s",p->gender);
    printf("Enter new experience: ");
    scanf("%s",p->experience);
    printf("Enter new age: ");
    scanf("%d",&p->age);
    printf("Enter new airline: ");
    scanf("%s",p->airline);

    savepilot();
    printf("Pilot details updated\n");
}
void cancel(char name[]){
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
    rename("t.txt", "bookings.txt");

    if (!c)
        printf("No such ticket found to cancel\n");

     
}
void load_flights(){
    FILE *f = fopen("flights.txt","r");
    if(!f) return;

    struct Flight *p;
    while(1){
        p = (struct Flight*)malloc(sizeof(struct Flight));
        if(fscanf(f, "%s %s %s %d %d %s",
            p->id, p->type, p->dest, &p->eprice, &p->lprice,p->pilot) != 6){
            free(p);
            break;
        }
        p->next = head;
        head = p;
    }
    fclose(f);
}
void save_flight(){
    FILE *f = fopen("flights.txt","w");
    struct Flight *p = head;
    while(p){
        fprintf(f, "%s %s %s %d %d %s\n",p->id, p->type, p->dest, p->eprice, p->lprice,p->pilot);
        p = p->next;
    }
    fclose(f);
}
void edit(){
    char id[20];
    printf("Enter flight ID to edit: ");
    scanf("%s",id);

    struct Flight *p = head;
    while(p&&strcmp(p->id,id)!=0)
        p = p->next;

    if(!p){
        printf("No such flight exists\n");
        return;
    }

    printf("Enter new aircraft: ");
    scanf("%s",p->type);
    printf("Enter new destination: ");
    scanf("%s",p->dest);
    printf("Enter new economy price: ");
    scanf("%d",&p->eprice);
    printf("Enter new luxury price: ");
    scanf("%d",&p->lprice);
    save_flight();
    printf("Flight details updated\n");

}
void add_flight(){
    struct Flight *p = (struct Flight*)malloc(sizeof(struct Flight));

    printf("Enter flight id: ");
    scanf("%s", p->id);
    printf("Enter aircraft: ");
    scanf("%s", p->type);
    printf("Enter destination: ");
    scanf("%s", p->dest);
    printf("Enter economy price: ");
    scanf("%d", &p->eprice);
    printf("Enter luxury price: ");
    scanf("%d", &p->lprice);
    strcpy(p->pilot, "NONE");
    p->next = head;
    head = p;
    save_flight();
    printf("New flight details added\n");
}
int check(char *name, char *pass){
      FILE *f = fopen("customerdata.txt", "r");
    if(!f) return 0;

    char name2[30], pass2[30];
    int age;
    long long ph;
    char line[200];

    while(fgets(line, sizeof(line), f)){
        if(sscanf(line, "Name: %s", name2) == 1 &&
           fgets(line, sizeof(line), f) &&
           sscanf(line, "Age: %d", &age) == 1 &&
           fgets(line, sizeof(line), f) &&
           sscanf(line, "Phone: %lld", &ph) == 1 &&
           fgets(line, sizeof(line), f) &&
           sscanf(line, "Password: %s", pass2) == 1)
        {
            fgets(line, sizeof(line), f);

            if(strcmp(name2, name)==0 && strcmp(pass2, pass)==0){
                fclose(f);
                return 1;
            }
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
    view_flights();
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
void admin_dashboard(){
    printf("Enter admin code: \n");
    char c[20];
    scanf("%s",c);
    if(strcmp("omkarsidshubhan",c)!=0){
        printf("WRONG CODE!\n");
        return;
    }
    while(1){
        printf("****ADMIN COMMANDS****\n");
        printf("1.Add flights\n2.View flights\n3.Edit flights\n4.Homescreen\n5.Add pilots\n6.View pilots\n7.Edit pilots\n8.Assign pilot to a flight\nEnter option: ");
        int o;
        scanf("%d",&o);
        if(o==2){
            view_flights();
        }
        else if(o==4){
            printf("Going back...\n");
            return;
        }
        else if(o==1){
            add_flight();
        }
        else if(o==3){
            edit();
        }
        else if(o==5){
            addpilot();
        }
        else if(o==6){
            view_pilots();
        }
        else if(o==7){
            editpilot();
        }
        else if(o==8){
            assign_pilot();
        }
        else{
            printf("Invalid option\n");
        }
}}

int main(){
    printf("===========================================\n");
    printf("            IIITB AIRPORT                   \n");
    printf("              WELCOME!                      \n");
    printf("============================================\n");
    load_flights();
    load_pilots();
    int choice;
    while(1){
        printf("Choose an option: \n");
        printf("1.Continue as Customer\n");
        printf("2.Continue as Admin\n");
        printf("3.View flights\n");
        printf("4.Exit application\n");
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
        else if(choice ==2){
            admin_dashboard();       
        }
    }
}
