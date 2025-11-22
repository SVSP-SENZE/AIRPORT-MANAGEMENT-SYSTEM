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
    char duration[20];
    int seats;
    int economy;
    int luxury;
    int ep;
    int lp;
    int seats_left;
    int seats_occupied;
};
struct customer{
    char name[30];
    int age;
    char password[30];
    long long phone;
};
struct booking{
    char username[30];
    char flight_id[10];
    int seat_number;};
void displaySeatLayout(int occupied[8][4]) {
    char seatLabels[4] = {'A', 'B', 'C', 'D'};

    printf("\n================ AIRPLANE SEATING ================\n");
    printf("                     [COCKPIT]\n\n");

    for (int row = 0; row < 8; row++) {

        // Show which class the row belongs to
        if (row == 0)
            printf("---- LUXURY CLASS ----\n");
        if (row == 2)
            printf("\n---- ECONOMY CLASS ----\n");

        printf(" 🪟  ");

        // Left side (A, B)
        for (int col = 0; col < 2; col++) {
            if (occupied[row][col] == 1)
                printf(" X   ");
            else
                printf("%c%d  ", seatLabels[col], row + 1);
        }

        printf(" || ");

        // Right side (C, D)
        for (int col = 2; col < 4; col++) {
            if (occupied[row][col] == 1)
                printf(" X   ");
            else
                printf("%c%d  ", seatLabels[col], row + 1);
        }

        printf(" 🪟\n");
    }

    printf("=================================================\n");
    printf("🪟 = Window   || = Aisle   X = Occupied\n");
}
void bookFlight(char username[]) {
    // Requires:
    // - displaySeatLayout(int occupied[8][4]) to be defined
    // - flights.txt and bookings.txt in the program folder (flights.txt must exist)
    // - flights.txt format per spec:
    //   id airline start destination date time duration seats economy luxury ep lp seats_left seats_occupied
    //
    // Date format in file and input: "dd-mm-yy"
    // Time format: "HH:MM" (24-hour)
    //
    // Seat layout: 8 rows (1-2 luxury, 3-8 economy), 4 columns (A,B | C,D)
    // Seat codes: A1..D8

    printf("\n--- BOOK A FLIGHT ---\n");

    // --- 1) Read and validate user date input (dd-mm-yy) ---
    char date_in[16];
    printf("Enter travel date in format dd-mm-yy (example: 27-11-25): ");
    if (scanf("%15s", date_in) != 1) {
        while (getchar() != '\n');
        printf("Please enter a valid date in format dd-mm-yy.\n");
        return;
    }

    int d, m, yy;
    if (sscanf(date_in, "%d-%d-%d", &d, &m, &yy) != 3) {
        printf("Please enter a valid date in format dd-mm-yy.\n");
        return;
    }
    // convert two-digit year to full year (assume 2000+)
    int year_full = (yy < 100) ? (2000 + yy) : yy;

    // build input tm at midnight
    struct tm input_tm = {0};
    input_tm.tm_mday = d;
    input_tm.tm_mon  = m - 1;
    input_tm.tm_year = year_full - 1900;
    input_tm.tm_hour = 0;
    input_tm.tm_min  = 0;
    input_tm.tm_sec  = 1; // avoid boundary issues
    time_t input_time = mktime(&input_tm);
    if (input_time == (time_t)-1) {
        printf("Please enter a valid calendar date.\n");
        return;
    }

    // get today's midnight
    time_t now = time(NULL);
    struct tm now_tm = *localtime(&now);
    now_tm.tm_hour = 0; now_tm.tm_min = 0; now_tm.tm_sec = 0;
    time_t today_midnight = mktime(&now_tm);

    const int seconds_per_day = 24 * 60 * 60;
    time_t first_allowed = today_midnight + seconds_per_day;         // tomorrow
    time_t last_allowed  = today_midnight + 5 * seconds_per_day;     // today + 5

    if (difftime(input_time, today_midnight) < 0.0) {
        printf("Invalid date — cannot book for a past date.\n");
        return;
    }
    if (difftime(input_time, first_allowed) < 0.0) {
        printf("Bookings open starting from tomorrow only. Please pick a future date.\n");
        return;
    }
    if (difftime(input_time, last_allowed) > 0.0) {
        printf("Bookings for this date are not open yet. You can book only up to 5 days from now.\n");
        return;
    }

    // --- 2) Destination selection (fixed 10) ---
    const char *destinations[10] = {
        "Mumbai", "Delhi", "Kolkata", "Chennai", "Hyderabad",
        "Goa", "Ahmedabad", "Pune", "Jaipur", "Kochi"
    };
    printf("\nSelect destination:\n");
    for (int i = 0; i < 10; ++i) {
        printf("%2d. %s\n", i + 1, destinations[i]);
    }
    printf("Enter choice (1-10): ");
    int dchoice;
    if (scanf("%d", &dchoice) != 1) {
        while (getchar() != '\n');
        printf("Invalid choice input.\n");
        return;
    }
    if (dchoice < 1 || dchoice > 10) {
        printf("No flights to this location.\n");
        return;
    }
    const char *destination = destinations[dchoice - 1];

    // --- 3) Read flights.txt and collect matches ---
    FILE *ff = fopen("flights.txt", "r");
    if (!ff) {
        printf("Flight database not found (flights.txt). Ask your teammate to provide the file.\n");
        return;
    }

    // local struct to hold flight record for processing
    typedef struct {
        char id[16];
        char airline[64];
        char start[32];
        char dest[32];
        char date[16];
        char time_s[16];
        char duration[32];
        int seats;
        int economy;
        int luxury;
        int ep;
        int lp;
        int seats_left;
        int seats_occupied;
    } FlightRec;

    FlightRec allFlights[512];
    int totalFlights = 0;
    char line[512];

    while (fgets(line, sizeof(line), ff)) {
        if (line[0] == '\n' || line[0] == '\r') continue;
        FlightRec fr;
        // Parse according to confirmed order:
        // id airline start destination date time duration seats economy luxury ep lp seats_left seats_occupied
        int scanned = sscanf(line, "%15s %63s %31s %31s %15s %15s %31s %d %d %d %d %d %d",
                             fr.id, fr.airline, fr.start, fr.dest, fr.date, fr.time_s, fr.duration,
                             &fr.seats, &fr.economy, &fr.luxury, &fr.ep, &fr.lp, &fr.seats_left, &fr.seats_occupied);
        if (scanned < 14) {
            // malformed: skip line
            continue;
        }
        allFlights[totalFlights++] = fr;
        if (totalFlights >= (int)(sizeof(allFlights)/sizeof(allFlights[0]))) break;
    }
    fclose(ff);

    // Filter matches: start must be Bangalore, dest matches and date matches (dd-mm-yy)
    FlightRec matches[128];
    int match_count = 0;
    for (int i = 0; i < totalFlights; ++i) {
        if (strcmp(allFlights[i].start, "Bangalore") != 0) continue;
        if (strcmp(allFlights[i].dest, destination) != 0) continue;
        if (strcmp(allFlights[i].date, date_in) != 0) continue;
        // seats_left should be >0
        if (allFlights[i].seats_left <= 0) continue;
        matches[match_count++] = allFlights[i];
    }

    if (match_count == 0) {
        printf("No matching flights found for Bangalore -> %s on %s\n", destination, date_in);
        return;
    }

    // --- 4) Display matched flights and choose one ---
    printf("\nFound %d flight(s):\n", match_count);
    for (int i = 0; i < match_count; ++i) {
        printf("\nFlight %d:\n", i + 1);
        printf("  Flight ID: %s\n", matches[i].id);
        printf("  Airline: %s\n", matches[i].airline);
        printf("  Date: %s\n", matches[i].date);
        printf("  Time: %s\n", matches[i].time_s);
        printf("  From: %s\n", matches[i].start);
        printf("  To: %s\n", matches[i].dest);
        printf("  Seats left: %d  (occupied: %d)\n", matches[i].seats_left, matches[i].seats_occupied);
        printf("  Economy seats: %d  Luxury seats: %d\n", matches[i].economy, matches[i].luxury);
        printf("  Prices (EP/LP): %d / %d\n", matches[i].ep, matches[i].lp);
    }

    printf("\nChoose a flight number to continue (1 - %d): ", match_count);
    int fchoice;
    if (scanf("%d", &fchoice) != 1) {
        while (getchar() != '\n');
        printf("Invalid input.\n");
        return;
    }
    if (fchoice < 1 || fchoice > match_count) {
        printf("Invalid flight selection.\n");
        return;
    }

    FlightRec chosen = matches[fchoice - 1];
    char chosen_id[16];
    strcpy(chosen_id, chosen.id);

    // --- 5) Build occupied seat map for the chosen flight using bookings.txt ---
    int occupied[8][4] = {0}; // rows 0..7, cols 0..3 (A,B,C,D)
    FILE *bf = fopen("bookings.txt", "r");
    if (bf) {
        char buser[64], bseat[8], bfid[16];
        while (fscanf(bf, "%63s %15s %7s", buser, bfid, bseat) == 3) {
            if (strcmp(bfid, chosen_id) != 0) continue; // only for chosen flight
            // parse seat like "A3"
            char sl = bseat[0];
            int srow = atoi(bseat + 1); // 1..8
            if (srow < 1 || srow > 8) continue;
            int scol = -1;
            if (sl == 'A') scol = 0;
            else if (sl == 'B') scol = 1;
            else if (sl == 'C') scol = 2;
            else if (sl == 'D') scol = 3;
            if (scol >= 0) occupied[srow - 1][scol] = 1;
        }
        fclose(bf);
    } // if bookings.txt missing -> no occupied seats

    // --- 6) Display seat layout using your function ---
    displaySeatLayout(occupied);

    // --- 7) Seat selection ---
    char seatChoice[8];
    printf("\nEnter desired seat (e.g. A3). Rows: 1..8, Columns: A,B,C,D: ");
    if (scanf("%7s", seatChoice) != 1) {
        while (getchar() != '\n');
        printf("Invalid seat input.\n");
        return;
    }

    char sl = seatChoice[0];
    int srow = atoi(seatChoice + 1);
    int scol = -1;
    if (sl == 'A') scol = 0;
    else if (sl == 'B') scol = 1;
    else if (sl == 'C') scol = 2;
    else if (sl == 'D') scol = 3;
    else {
        printf("Invalid seat choice (invalid letter). Use A-D.\n");
        return;
    }
    if (srow < 1 || srow > 8) {
        printf("Invalid seat choice (row out of range). Use 1-8.\n");
        return;
    }
    if (occupied[srow - 1][scol]) {
        printf("Seat already occupied!\n");
        return;
    }

    // --- 8) Save booking: append to bookings.txt ---
    FILE *bfw = fopen("bookings.txt", "a");
    if (!bfw) {
        printf("Warning: could not open bookings file for writing. Booking will not be saved.\n");
    } else {
        fprintf(bfw, "%s %s %s\n", username, chosen_id, seatChoice);
        fclose(bfw);
    }

    // --- 9) Update flights array (allFlights) and rewrite flights.txt to persist seats_left/seats_occupied ---
    // Find the flight in allFlights with matching id and update it
    int foundIndex = -1;
    for (int i = 0; i < totalFlights; ++i) {
        if (strcmp(allFlights[i].id, chosen_id) == 0) {
            foundIndex = i;
            break;
        }
    }
    if (foundIndex == -1) {
        printf("Internal error: chosen flight not found in memory.\n");
        // still has booking appended; we stop here
    } else {
        // decrement seats_left, increment seats_occupied
        if (allFlights[foundIndex].seats_left > 0) {
            allFlights[foundIndex].seats_left -= 1;
            allFlights[foundIndex].seats_occupied += 1;
        } else {
            // race condition - someone else might have taken the last seat between reading and booking
            printf("Warning: flight reported no seats left while booking. Your booking has been recorded in bookings.txt, but the flight DB was not updated.\n");
        }

        // rewrite flights.txt (safe rewrite using temp file)
        FILE *fw = fopen("flights.tmp", "w");
        if (!fw) {
            printf("Error: could not open temporary file to update flights database. DB not updated.\n");
        } else {
            for (int i = 0; i < totalFlights; ++i) {
                FlightRec *fr = &allFlights[i];
                // Write in the same order and formatting as expected
                // id airline start destination date time duration seats economy luxury ep lp seats_left seats_occupied
                fprintf(fw, "%s %s %s %s %s %s %s %d %d %d %d %d %d %d\n",
                        fr->id, fr->airline, fr->start, fr->dest, fr->date, fr->time_s, fr->duration,
                        fr->seats, fr->economy, fr->luxury, fr->ep, fr->lp, fr->seats_left, fr->seats_occupied);
            }
            fclose(fw);
            // replace original
            if (remove("flights.txt") != 0) {
                // try to still rename (may overwrite)
                rename("flights.tmp", "flights.txt"); // attempt best-effort
            } else {
                rename("flights.tmp", "flights.txt");
            }
        }
    }

    // --- 10) Final ticket ---
    // Print ticket using selected flight info
    printf("\nBooking Successful!\n");
    printf("----- TICKET -----\n");
    printf("Passenger: %s\n", username);
    printf("Flight ID: %s\n", chosen.id);
    printf("Airline: %s\n", chosen.airline);
    printf("Date: %s\n", chosen.date);
    printf("Time: %s\n", chosen.time_s);
    printf("From: %s\n", chosen.start);
    printf("To: %s\n", chosen.dest);
    printf("Seat: %s\n", seatChoice);
    // choose price: simple heuristic: if row 1-2 -> luxury else economy
    int price = 0;
    if (srow <= 2) price = chosen.lp;
    else price = chosen.ep;
    printf("Price: Rs.%d\n", price);
    printf("------------------\n");

    return;
}
void cancelFlight(char username[]) {
    printf("\n--- CANCEL A BOOKING ---\n");

    // 1) Load all bookings and filter only this user's bookings
    FILE *bf = fopen("bookings.txt", "r");
    if (!bf) {
        printf("No bookings found (bookings.txt missing or empty).\n");
        return;
    }

    typedef struct {
        char user[64];
        char flight_id[16];
        char seat[8];
    } Booking;

    Booking allBookings[2000];
    int totalBookings = 0;

    Booking userBookings[2000];
    int userCount = 0;

    char buser[64], bfid[16], bseat[8];
    while (fscanf(bf, "%63s %15s %7s", buser, bfid, bseat) == 3) {
        if (totalBookings < (int)(sizeof(allBookings)/sizeof(allBookings[0]))) {
            strcpy(allBookings[totalBookings].user, buser);
            strcpy(allBookings[totalBookings].flight_id, bfid);
            strcpy(allBookings[totalBookings].seat, bseat);
            totalBookings++;
        }
        if (strcmp(buser, username) == 0) {
            if (userCount < (int)(sizeof(userBookings)/sizeof(userBookings[0]))) {
                strcpy(userBookings[userCount].user, buser);
                strcpy(userBookings[userCount].flight_id, bfid);
                strcpy(userBookings[userCount].seat, bseat);
                userCount++;
            }
        }
    }
    fclose(bf);

    if (userCount == 0) {
        printf("You have no active bookings.\n");
        return;
    }

    // 2) Show user's bookings only
    printf("\nYour bookings:\n");
    for (int i = 0; i < userCount; ++i) {
        printf("%d) Flight %s   Seat %s\n", i + 1,
               userBookings[i].flight_id, userBookings[i].seat);
    }
    printf("Enter the booking number to cancel (0 to abort): ");
    int choice = 0;
    if (scanf("%d", &choice) != 1) {
        while (getchar() != '\n');
        printf("Invalid input.\n");
        return;
    }
    if (choice == 0) {
        printf("Cancellation aborted.\n");
        return;
    }
    if (choice < 1 || choice > userCount) {
        printf("Invalid choice.\n");
        return;
    }

    Booking toCancel = userBookings[choice - 1];

    // 3) Password verification: read customers file and lookup password
    char custName[64], custPass[64];
    int custAge = 0;
    long long custPhone = 0;
    int verified = 0;

    FILE *cf = fopen("customers.txt", "r");
    if (!cf) {
        printf("Customer database not found (customers.txt).\n");
        return;
    }

    // customers file format as confirmed: name age password phone
    char file_name[64], file_pass[64];
    int file_age;
    long long file_phone;
    while (fscanf(cf, "%63s %d %63s %lld", file_name, &file_age, file_pass, &file_phone) == 4) {
        if (strcmp(file_name, username) == 0) {
            // found user record
            strcpy(custName, file_name);
            strcpy(custPass, file_pass);
            custAge = file_age;
            custPhone = file_phone;
            verified = -1; // mark as found but not yet password-checked
            break;
        }
    }
    fclose(cf);

    if (verified == 0) {
        printf("User record not found in customers.txt.\n");
        return;
    }

    // ask for password
    char inputPass[64];
    printf("Enter your password to confirm cancellation: ");
    scanf("%63s", inputPass);

    if (strcmp(inputPass, custPass) != 0) {
        printf("Password incorrect. Cancellation aborted.\n");
        return;
    }

    // 4) Find flight info for the toCancel.flight_id (to compute refund & update seats)
    typedef struct {
        char id[16];
        char airline[64];
        char start[32];
        char dest[32];
        char date[16];
        char time_s[16];
        char duration[32];
        int seats;
        int economy;
        int luxury;
        int ep;
        int lp;
        int seats_left;
        int seats_occupied;
    } FlightRec;

    FlightRec flights[1024];
    int totalFlights = 0;
    FILE *ff = fopen("flights.txt", "r");
    if (!ff) {
        printf("Flight database not found (flights.txt).\n");
        return;
    }

    // read all flights into memory
    while (1) {
        FlightRec fr;
        int scanned = fscanf(ff,
            "%15s %63s %31s %31s %15s %15s %31s %d %d %d %d %d %d %d",
            fr.id, fr.airline, fr.start, fr.dest, fr.date, fr.time_s, fr.duration,
            &fr.seats, &fr.economy, &fr.luxury, &fr.ep, &fr.lp, &fr.seats_left, &fr.seats_occupied
        );
        if (scanned == EOF || scanned < 14) break;
        if (totalFlights < (int)(sizeof(flights)/sizeof(flights[0]))) {
            flights[totalFlights++] = fr;
        } else {
            break;
        }
    }
    fclose(ff);

    int flightIndex = -1;
    for (int i = 0; i < totalFlights; ++i) {
        if (strcmp(flights[i].id, toCancel.flight_id) == 0) {
            flightIndex = i;
            break;
        }
    }
    if (flightIndex == -1) {
        printf("Flight record not found for ID %s. Cancellation aborted.\n", toCancel.flight_id);
        return;
    }

    FlightRec *chosen = &flights[flightIndex];

    // 5) Compute refund based on time to takeoff
    // parse chosen->date (dd-mm-yy) and chosen->time_s (HH:MM)
    int dd, mm, yy;
    if (sscanf(chosen->date, "%d-%d-%d", &dd, &mm, &yy) != 3) {
        printf("Flight date has invalid format. Cannot compute refund precisely.\n");
    }
    int year_full = (yy < 100) ? (2000 + yy) : yy;
    int hh, min;
    if (sscanf(chosen->time_s, "%d:%d", &hh, &min) != 2) {
        printf("Flight time has invalid format. Cannot compute refund precisely.\n");
    }

    struct tm ft = {0};
    ft.tm_mday = dd;
    ft.tm_mon  = mm - 1;
    ft.tm_year = year_full - 1900;
    ft.tm_hour = hh;
    ft.tm_min  = min;
    ft.tm_sec  = 0;
    time_t flight_time = mktime(&ft);
    time_t now = time(NULL);
    double seconds_left = difftime(flight_time, now);
    double hours_left = seconds_left / 3600.0;

    double refund_percent = 0.0;
    if (hours_left > 48.0) refund_percent = 0.80;
    else if (hours_left > 24.0) refund_percent = 0.50;
    else if (hours_left > 6.0) refund_percent = 0.25;
    else refund_percent = 0.0;

    // determine seat class and base price
    char seatLetter = toCancel.seat[0];
    int seatRow = atoi(toCancel.seat + 1); // 1..8
    int basePrice = 0;
    if (seatRow >= 1 && seatRow <= 2) basePrice = chosen->lp; // luxury
    else basePrice = chosen->ep; // economy

    double refundAmount = basePrice * refund_percent;

    // 6) Remove booking from bookings list (rewrite bookings.txt without the cancelled booking)
    FILE *bw = fopen("bookings.tmp", "w");
    if (!bw) {
        printf("Error: could not open temporary bookings file for writing.\n");
        return;
    }
    int removed = 0;
    for (int i = 0; i < totalBookings; ++i) {
        if (strcmp(allBookings[i].user, toCancel.user) == 0 &&
            strcmp(allBookings[i].flight_id, toCancel.flight_id) == 0 &&
            strcmp(allBookings[i].seat, toCancel.seat) == 0 && removed == 0) {
            // skip this one (the first matching booking)
            removed = 1;
            continue;
        }
        fprintf(bw, "%s %s %s\n",
                allBookings[i].user, allBookings[i].flight_id, allBookings[i].seat);
    }
    fclose(bw);

    // replace bookings file safely
    if (remove("bookings.txt") != 0) {
        // if removal failed, still attempt rename (best-effort)
    }
    rename("bookings.tmp", "bookings.txt");

    if (!removed) {
        printf("Internal error: booking to cancel was not found while rewriting bookings file.\n");
        return;
    }

    // 7) Update flights array and rewrite flights.txt (seats_left++, seats_occupied--)
    if (chosen->seats_left < chosen->seats) chosen->seats_left += 1;
    chosen->seats_occupied -= 1;
    if (chosen->seats_occupied < 0) chosen->seats_occupied = 0;

    FILE *fw = fopen("flights.tmp", "w");
    if (!fw) {
        printf("Error: could not open temp file to update flights database.\n");
        return;
    }
    for (int i = 0; i < totalFlights; ++i) {
        FlightRec *fr = &flights[i];
        fprintf(fw, "%s %s %s %s %s %s %s %d %d %d %d %d %d %d\n",
                fr->id, fr->airline, fr->start, fr->dest, fr->date, fr->time_s, fr->duration,
                fr->seats, fr->economy, fr->luxury, fr->ep, fr->lp, fr->seats_left, fr->seats_occupied);
    }
    fclose(fw);
    if (remove("flights.txt") != 0) {
        // fallthrough: attempt rename anyway
    }
    rename("flights.tmp", "flights.txt");

    // 8) Print cancellation receipt with refund details
    printf("\n=== BOOKING CANCELLED SUCCESSFULLY ===\n");
    printf("Passenger: %s\n", username);
    printf("Flight ID: %s\n", toCancel.flight_id);
    printf("Airline: %s\n", chosen->airline);
    printf("Seat: %s\n", toCancel.seat);
    printf("Original price (approx): Rs %d\n", basePrice);
    printf("Time until takeoff: %.1f hours\n", hours_left > 0 ? hours_left : 0.0);
    printf("Refund percent: %.0f%%\n", refund_percent * 100.0);
    printf("Refund amount: Rs %.2f\n", refundAmount);
    printf("======================================\n");
}
struct flights{
     char id[10],airline[20], date[20],time[10],from[30],to[30];
    int price;
    float duration;
};
typedef struct flights Flights;
//To add a flight
void addFlight(){
    FILE *fp= fopen("flights.txt","a");
    if(fp==NULL){
       printf("\nCannot open flights.txt\n");
       return;
   }
    Flights f;
    printf("\n===ADD A FLIGHT===\n");
    printf("Add Flight id (eg-Fl007): ");
    scanf("%s",f.id);
    printf("Add airline: ");
    scanf("%s",f.airline);
    printf("Add date: ");
    scanf("%s", f.date);
    printf("Enter time: ");
    scanf("%s", f.time);
    printf("Enter start: ");
    scanf("%s",f.from);
    printf("Enter end: ");
    scanf("%s",f.to);
    printf("Enter price: ");
    scanf("%d",f.price);
    printf("Enter duration in hours: ");
    scanf("%f",f.duration);
    
    //adding the file in data base
    fprintf(fp,"%s" "%s" "%s" "%s" "%s" "%s" "%d" "%.1f\n",f.id,f.airline,f.date,f.time,f.from,f.to,f.price,f.duration);
    fclose(fp);
    printf("\nFlight added successfully!\n");
    
    FILE *sf=fopen("seatFile.txt")
    
}

//To view flights
void viewflights(){
    printf("Select '1' to view a particular flights\n");
    printf("Select '2' to view all flights\n");
    int op;
    scanf("%d",&op);
    FILE *vf= fopen("flights.txt","r");
    Flights VF;
     fscanf(vf, "%s" "%s" "%s" "%s" "%s" "%s" "%d" "%.1f\n",VF.id,VF.airline,VF.date,VF.time,VF.from,VF.to,VF.price,VF.duration);
     
     //display a particular flight 
    if(op==1){
     Flights f1;
    printf("Enter flight id: ");
    scanf("%s",f1.id);
    if(vf==NULL){
        printf("\n flights.txt not found\n");
        return;
    }
    int check=0;
     while( fscanf(vf, "%s" "%s" "%s" "%s" "%s" "%s" "%d" "%.1f\n",VF.id,VF.airline,VF.date,VF.time,VF.from,VF.to,VF.price,VF.duration)1=EOF){
    if(strcmp(fl.id,id)==0) {
        check = check+1;
        printf("\nFlight Found!\n");
        printf("Flight ID: %s\n",VF.id);
        printf("Airline: \n",VF.airline);
        printf("Date: \n",VF.date);
        printf("Time: \n",VF.time);
        printf("From: \n",VF.from);
        printf("To: \n",VF.to);
        printf("Price: \n",VF.price);
        printf("Duration: \n",VF.duration);
        fclose(vf);
        break;
    }
    }
        if(check==0 ){
         printf("\nFlight not found :(\n")
        printf("Enter correct flight id!\n");
        }
    } //option 1 ends 
    
    Flights cf;
    elif(op==2){
        printf("\n=====AVAILABLE FLIGHTS=====");
        while( fscanf(vf, "%s" "%s" "%s" "%s" "%s" "%s" "%d" "%.1f\n",cf.id,cf.airline,cf.date,cf.time,cf.from,cf.to,cf.price,cf.duration)!=EOF){
            printf("Flight ID: %s\n",id);
        printf("Airline: \n",cf.airline);
        printf("Date: \n",cf.date);
        printf("Time: \n",cf.time);
        printf("From: \n",cf.from);
        printf("To: \n",cf.to);
        printf("Price: \n",cf.price);
        printf("Duration: \n",cf.duration);
        printf("\n---------------------\n");
        }
        fclose(vf);}
    }
    //delete flight
    void deleteflight() {
    Flights f2;
    Flights currentFlight;

    printf("Enter ID of flight to be removed: ");
    scanf("%s", f2.id);

    FILE *originalFile = fopen("flights.txt", "r");
    FILE *tempFile = fopen("temp_flights.txt", "w"); 

    if (originalFile == NULL || tempFile == NULL) {
        perror("Error opening file");
        if (originalFile) fclose(originalFile);
        if (tempFile) fclose(tempFile);
        return;
    }

    int flightDeleted = 0;
    
    while (fscanf(originalFile, "%s %s %s %s %s %s %d %f\n", 
                  currentFlight.id, currentFlight.airline, currentFlight.date, 
                  currentFlight.time, currentFlight.from, currentFlight.to, 
                  &currentFlight.price, &currentFlight.duration) != EOF) {

        if (strcmp(f2.id, currentFlight.id) == 0) {
            flightDeleted = 1;
            printf("Flight with ID %s deleted successfully.\n", f2.id);
        } else {
            fprintf(tempFile, "%s %s %s %s %s %s %d %.1f\n",
                    currentFlight.id, currentFlight.airline, currentFlight.date, 
                    currentFlight.time, currentFlight.from, currentFlight.to, 
                    currentFlight.price, currentFlight.duration);
        }
    }

    fclose(originalFile);
    fclose(tempFile);
    
    if (flightDeleted) {
        if (remove("flights.txt") == 0) {
            if (rename("temp_flights.txt", "flights.txt") == 0) {
                
            } else {
                perror("Error renaming temporary file");
            }
        } else {
            perror("Error deleting original file");
        }
    } else {
        printf("Flight with ID %s not found. No record deleted.\n", f2.id);
        remove("temp_flights.txt");
    }
}
void admin()
{
    int choice;
    char password[20];

    printf("\n===== ADMIN LOGIN =====\n");
    printf("Enter Admin Password: ");
    scanf("%s", password);

    if (strcmp(password, "cproject") != 0)  
    {
        printf("\n Wrong Password! Returning to Main Menu...\n");
        return;
    }

    while (1)
    {
        printf("\n===== ADMIN PANEL =====\n");
        printf("1. Add Flight\n");
        printf("2. View All Flights\n");
        printf("3. Delete Flight\n");
        printf("4. Back to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1: addFlight(); break;
            case 2: viewFlights(); break;
            case 3: deleteFlight(); break;
            case 4: return;
            default: printf("\nInvalid Choice! Try Again.\n");
        }
    }
}

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
