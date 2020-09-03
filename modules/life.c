#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#include "ADTMap.h"
#include "ADTList.h"
#include "life.h"

//Συνάρτηση compare_LifeCell για τη σύγκριση δύο μεταβλητών τύπου LifeCell, με βάση τις
//συντεταγμένες τους
int compare_LifeCell(Pointer a, Pointer b){
    LifeCellPtr cella = (LifeCellPtr)a;
    LifeCellPtr cellb = (LifeCellPtr)b;
    int ia = cella->x;
    int ja = cella->y;
    int ib = cellb->x;
    int jb = cellb->y;
    if(ia - ib){
        return(ia - ib);
    }else{
        return(ja - jb);
    }
}

//Δημιουργεί μία τιμή στην οποία θα δείχνει ένας δείκτης
int *create_int (int a) {
	int* n = malloc(sizeof(int));
	*n = a;
	return n;
}

// Δημιουργεί μια κατάσταση του παιχνιδιού όπου όλα τα κελιά είναι νεκρά.
LifeState life_create(){
    Map map = map_create(compare_LifeCell, free, free);
    return map;
}

// Δημιουργεί μία κατάσταση του παιχνιδιού με βάση τα δεδομένα του αρχείο file (RLE format).
LifeState life_create_from_rle(char* file){
    
    Map map = life_create();

    //Προσθέτουμε την κατάληξη ".rle" στο αρχείο με το όνομα "file".
    char* result = malloc((strlen(file)+5)*sizeof(char));
    strcpy(result, file);
    strcat(result, ".rle");
    
    FILE *fp = fopen(result, "r");

    char *str = malloc(3*sizeof(char));

    //Η μεταβλητή temp κρατά τους αριθμούς που υπάρχουν μέσα στο "file.rle".
    int temp = 1;
    //Η μεταβλητή i αντιπροσωπεύει τις γραμμές του θεωρητικού πίνακα (συντεγαγμένες y)
    int i = 0;
    //Η μεταβλητή j αντιπροσωπεύει τις στήλες του θεωρητικού πίνακα (συντεταγμένες x)
    int j = 0;
    //Η μεταβλητή flag μας δείχνει αν έχει διαβαστεί ένας αριθμός από το "file.rle" και έχει καταχωρηθεί στην temp.
    int flag = 0;

    //Διαβάζουμε έναν έναν τους χαρακτήρες από το αρχείο "file.rle"
    while(fgets(str, 2, fp) && strcmp(str, "!") != 0){

        //Αν ο χαρακτήρας είναι ο "$" αλλάζουμε γραμμή στο LifeState.
        if(strcmp(str, "$") == 0){
            for(int k = 0; k < temp; k++){
                i++;
            }
            j=0;
            temp = 1;
            flag = 0;
            continue;

        //Αν ο χαρακτήρας είναι ο "o" δημιουργούμε όσα ζωντανά κελιά χρειάζονται με βάση την temp.
        }else if(strcmp(str, "o") == 0){
            for(int k = j; k < j + temp; k++){
                LifeCellPtr lifecell = malloc(sizeof(lifecell));
                lifecell->x = i;
                lifecell->y = k;
                int *boolean = create_int(1);
                map_insert(map, lifecell, boolean);
                flag = 0;
            }

        //Αν ο χαρακτήρας είναι οτιδήποτε άλλο εκτός από "b" (και "$", "o") που έχουμε διαβάσει παραπάνω,
        //τότε θα είναι αριθμός. Αν διαβάζουμε αριθμητικό ψηφίο για πρώτη φορά, τότε ή flag==0 και η
        //temp==1, άρα απλώς εκχωρούμε στην temp την αριθμητική τιμή του χαρακτήρα που μόλςι διαβάσαμε. Αν
        //έχουμε ήδη διαβάσει τουλάχιστον ένα ψηφίο, τότε η flag!=0, άρα πολλαπλασιάζουμε με το 10 τον
        //υπάρχοντα αριθμό στην temp ώστε να μεταβληθεί κατά ένα η θέση του (μονάδες, δεκάδες, εκατοντάδες κλπ)
        //και προσθέτουμε την αριθμητική τιμή του χαρακτήρα που μόλις διαβάσαμε (στη θέση των μονάδων).
        }else if(strcmp(str, "b") != 0){
            if(temp == 1 && (flag == 0)){
                temp = atoi(str);
                flag = 1;
            }else{
                temp *= 10;
                temp += atoi(str);
            }
            continue;
        }
        j+=temp;
        temp = 1;
        flag = 0;
    }

    fclose(fp);
    
    //Αποδεσμεύουμε την μνήμη που δεσμεύθηκε δυναμικά.
    free(str);
    free(result);

    return map;
}

// Αποθηκεύει την κατάσταση state στο αρχείο file (RLE format)
void life_save_to_rle(LifeState state, char* file){
    
    //Ορίζουμε δύο σταθερές μεταβλητές που θα δείχνουν τα όρια ενός θεωρητικού διασδιάστατου πίνακα
    //που θα προκύψει με βάση τις συντεταγμένες των ζωντανών κελιών. Υπ' όψιν ότι στο map κρατάμε
    //μόνο τα ζωντανά κελιά, τα νεκρά κελιά αγνοούνται.
    int i_const = 0, j_const = 0;
    
    //Διασχίζουμε το map και βρίσκουμε τις max συντεταγμένες στους δύο άξονες, βρίσκοντας τις μεγαλύτερες
    //τιμές από το map.
    for(MapNode node = map_first(state); node != MAP_EOF; node = map_next(state, node)){
        LifeCellPtr temp_struct = map_node_key(state, node);
        if(temp_struct->x > i_const){
            i_const = temp_struct->x;
        }
        if(temp_struct->y > j_const){
            j_const = temp_struct->y;
        }
    }

    //Για να διατρέξουμε τον πίνακα ως δισδιάστατο, αυξάνουμε την τιμή των δύο παραπάνω σταθερών μεταβλητών κατά
    //ένα, ώστε η for() να διατρέχει από 0 έως < i_const/j_const και όχι <=.
    i_const++;
    j_const++;

    //Δεσμεύουμε την απαραίτητη μνήμη για τη δημιουργία ενός θεωρητικού δισδιάστατου πίνακα που θα περιέχει μόνο τιμές 0 και 1.
    int **arr = malloc((i_const)*sizeof(int*));
    for(int i = 0; i < i_const; i++){
        arr[i] = malloc((j_const)*sizeof(int));
    }

    //Αρχικοποιούμε όλα τα κελιά του θεωρητικού πίνακα με 0.
    for(int i = 0; i < i_const; i++){
        for(int j = 0; j < j_const; j++){
            arr[i][j] = 0;
        }
    }

    //Αν κάποιο κελί περιέχεται στα ζωντανά του map, τότε αλλάζουμε την τιμή του στον πίνακα σε 1.
    for(MapNode node = map_first(state); node != MAP_EOF; node = map_next(state, node)){
        LifeCellPtr temp_struct = map_node_key(state, node);
        int i = temp_struct->x;
        int j = temp_struct->y;
        arr[i][j] = 1;
    }

    //Προσθέτουμε την κατάληξη ".rle" στο αρχείο "file" στο οποίο θα αποθηκεύσουμε τις πληροφορίες.
    char* result = malloc((strlen(file)+5)*sizeof(char));
    strcpy(result, file);
    strcat(result, ".rle");

    FILE *fp = fopen(result, "w");

    //Διατρέχουμε τον θεωρητικό δισδιάστατο ώστε να καταγράψουμε τις πληροφορίες στο RLE.    
    for(int i = 0; i < i_const; i++){

        int flag_change_line = 0;

        //Κάθε φορά που φτάνουμε σε κάποιο στοιχείο της γραμμής i, ελέγχουμε πόσα από τα επόμενά του
        //είναι ίδια, ώστε να τα συμπεριλάβουμε όλα μαζί σε μια μορφή AB(A ακέραιος, B κατάσταση "b" ή "o").
        int sum = 1;
        for(int j = 0; j < j_const; j++){
            if(j+1 < j_const && arr[i][j+1] == arr[i][j]){
                sum++;
                continue;

            //Αν το επόμενο στοιχείο δεν είναι ίδιο, τότε προσθέτουμε στο αρχείο τον χαρακτήρα που διαβάσαμε
            //προηγουμένως μαζί με τον τελεστή του (sum).
            }else{

                //Αν ο τελεστής είναι 1, τότε δεν τον εμφανίζουμε στο αρχείο.
                if(sum == 1){

                    //Αν στον θεωρητικό πίνακα η τιμή του κελιού είναι 0, τότε είναι νεκρό. 
                    if(arr[i][j] == 0){

                        //Ελέγχουμε αν μετά το νεκρό αυτό κελί υπάρχει κάποιο ζωντανό. Αν δεν υπάρχει, τότε δεν το
                        //προσθέτουμε στο αρχείο γιατί ακολουθούμε τη λογική να μην υπάρχουν νεκρά κελιά στο τέλος
                        //κάθε γραμμής στο RLE.
                        int flag = 0;
                        for(int k = j+1; k < j_const; k++){
                            if(arr[i][k] == 1){
                                flag = 1;
                                break;
                            }
                        }
                        if(flag == 1){
                            fprintf(fp, "b");
                            flag_change_line = 1;
                        }
                    
                    //Αν η τιμή του κελιού στον θεωρητικό πίνακα δεν είναι 0, τότε θα είναι ζωντανό.
                    }else{
                        fprintf(fp, "o");
                        flag_change_line = 1;
                    }

                //Αν ο τελεστής δεν είναι 1, τότε πρέπει να τον εμφανίσουμε στο αρχείο.
                }else{

                    //Αν στον θεωρητικό πίνακα η τιμή του κελιού είναι 0, τότε είναι νεκρό, όπως και τα
                    //sum προηγούμενά του. 
                    if(arr[i][j] == 0){
                        
                        //Ελέγχουμε αν μετά το νεκρό αυτό κελί υπάρχει κάποιο ζωντανό. Αν δεν υπάρχει, τότε δεν το
                        //προσθέτουμε στο αρχείο γιατί ακολουθούμε τη λογική να μην υπάρχουν νεκρά κελιά στο τέλος
                        //κάθε γραμμής στο RLE.
                        int flag = 0;
                        for(int k = j+1; k < j_const; k++){
                            flag = 1;
                            break;
                        }
                        if(flag == 1){
                            fprintf(fp, "%db", sum);
                            flag_change_line = 1;
                        }

                    //Αν η τιμή του κελιού στον θεωρητικό πίνακα δεν είναι 0, τότε θα είναι ζωντανό, όπως και
                    //τα sum προηγούμενά του.
                    }else{
                        fprintf(fp, "%do", sum);
                        flag_change_line = 1;
                    }
                    sum = 1;
                }
            }
        }

        //Ελέγχουμε για κενές γραμμές ή αλλαγή γραμμής. Δεν εμφανίζουμε "$" στην τελευταία γραμμή, οπότε
        //ελέγχουμε ότι i!=i_const. Ελέγχουμε, επίσης, ότι έχει εκτυπωθεί οτιδήποτε στην παρούσα γραμμή,
        //δηλαδή ότι δεν είναι κενή, μέσω της flag_change_line η οποία παίρνει τιμές 0 και 1. Αν η γραμμή
        //είναι κενή, βρίσκουμε και πόσες από τις επόμενες γραμμές είναι κενές και αποθηκεύουμε το πλήθος
        //στην μεταβλητή sum_empty_lines.
        if(i != i_const-1 && flag_change_line == 1){

            int sum_empty_lines = 0;
            for(int m = i+1; m < i_const; m++){
                int flag = 0;
                for(int n = 0; n < j_const; n++){
                    if(arr[m][n] == 1){
                        flag = 1;
                        break;
                    }
                }
                if(flag == 0){
                    sum_empty_lines++;
                    i++;
                }else{
                    break;
                }
            }

            //sum_empty_lines==0 σημαίνει ότι απλώς αλλάζουμε γραμμή χωρίς να αφήσουμε κενή, ενώ sum_empty_lines!=0
            //σημαίνει ότι παρεμβάλλονται sum_empty_lines κενές γραμμές, συν η μία που αλλάζουμε τώρα.
            if(sum_empty_lines != 0){
                fprintf(fp, "%d$", sum_empty_lines+1);
            }else{
                fprintf(fp, "$");
            }

        }
    }
    fprintf(fp, "!");

    fclose(fp);

    //Αποδεσμεύουμε την μνήμη που δεσμεύθηκε δυναμικά.
    free(result);
    for(int i = 0; i < i_const; i++){
        free(arr[i]);
    }
    free(arr);
}

// Επιστρέφει την τιμή του κελιού cell στην κατάσταση state (true: ζωντανό, false: νεκρό)
bool life_get_cell(LifeState state, LifeCell cell){

    //Διατρέχουμε το map
    for(MapNode node = map_first(state); node != MAP_EOF; node = map_next(state, node)){

        //Δημιουργούμε μια μεταβλητή που δείχνει σε struct τύπου LifeCell, το οποίο
        //περιέχει τις συντεταγμένες του τρέχοντα κόμβου του map.
        LifeCellPtr temp_struct = map_node_key(state, node);

        //Αν το LifeCell που αναζητούμε ισούται με το LifeCell του τρέχοντα κόμβου του map, τότε
        //το κελί είναι ζωνταντό.
        if(cell.x == temp_struct->x && cell.y == temp_struct->y){
            return true;
        }
    }

    //Αν το κελί δεν βρεθεί στο map, αυτό σημαίνει πως είναι νεκρό.
    return false;
}

// Αλλάζει την τιμή του κελιού cell στην κατάσταση state
void life_set_cell(LifeState state, LifeCell cell, bool value){
    bool flag = false;

    //Δημιουργούμε έναν δείκτη στο LifeCell που δόθηκε.
    LifeCellPtr lifecell = malloc(sizeof(LifeCellPtr));
    lifecell->x = cell.x;
    lifecell->y = cell.y;

    //Αν το LifeCell είναι ζωντανό θα βρεθεί στο map και αφαιρείται από αυτό, ώστε να γίνει νεκρό.
    flag = map_remove(state, lifecell);

    //Αν η flag επιστρέψει false, τότε το LifeCell δεν βρίσκεται στο map, άρα είναι νεκρό. Το προσθέτουμε
    //στο map, ώστε να γίνει ζωντανό.
    if(flag == false){
        int *value = create_int(1);
        map_insert(state, lifecell, value);

    //Αν το LifeCell ήταν ζωντανό και έγινε νεκρό, τότε αποδεσμεύουμε τον δείκτη σε αυτό που δεσμεύσαμε, αφού
    //δεν μας χρειάζεται πλέον.
    }else{
        free(lifecell);
    }
}

//Βοηθητική συνάρτηση για τη life_evolve. Λαμβάνει τις συντεταγμένες ενός γειτονικού κελιού στον θεωρητικό
//δισδιάστατο πίνακα και δύο μεταβλητές alive, dead, που αντιπροσωπεύουν τα ζωντανά και νεκρά κελιά του
//κελιού το οποίο γειτνιάζει με αυτό του οποίου έχουμε τις συντεταγμένες. Η συνάρτηση μεταβάλλει το περιεχόμενο
//των alive, dead ανάλογα.
void check_alive_dead(int **arr, int i, int j, int *alive, int *dead){
    if(arr[i][j]==0){
        (*(int*)dead)++;
    }else{
        (*(int*)alive)++;
    }
}

// Παράγει και επιστρέφει μια νέα κατάσταση που προκύπτει από την εξέλιξη της κατάστασης state.
// Η ίδια η state δε μεταβάλλεται (ούτε καταστρέφεται).
LifeState life_evolve(LifeState state){

    //Ορίζουμε δύο σταθερές μεταβλητές που θα δείχνουν τα όρια ενός θεωρητικού διασδιάστατου πίνακα
    //που θα προκύψει με βάση τις συντεταγμένες των ζωντανών κελιών. Υπ' όψιν ότι στο map κρατάμε
    //μόνο τα ζωντανά κελιά, τα νεκρά κελιά αγνοούνται.
    int i_const = 0, j_const = 0;

    //Διασχίζουμε το map και βρίσκουμε τις max συντεταγμένες στους δύο άξονες, βρίσκοντας τις μεγαλύτερες
    //τιμές από το map.
    for(MapNode node = map_first(state); node != MAP_EOF; node = map_next(state, node)){
        LifeCellPtr temp_struct = map_node_key(state, node);
        if(temp_struct->x > i_const){
            i_const = temp_struct->x;
        }
        if(temp_struct->y > j_const){
            j_const = temp_struct->y;
        }
    }

    //Για να διατρέξουμε τον πίνακα ως δισδιάστατο, αυξάνουμε την τιμή των δύο παραπάνω σταθερών μεταβλητών κατά
    //ένα, ώστε η for() να διατρέχει από 0 έως < i_const/j_const και όχι <=. Επίσης, αυξάνουμε τις ίδιες μεταβλητές
    //κατά δύο μονάδες ακόμη, ώστε να επεκτείνουμε τον θεωρητικό πίνακα κατά ένα προς όλες τις κατευθύνσεις. Κατ'
    //αυτόν τον τρόπο, καλύπτουμε την πιθανότητα να ζωντανέψει κάποιο κελί εκτός του αρχικού θεωρητικού δισδιάστατου.
    //Κελιά με συντεταγμένες πέρα από τις αρχικές διαστάσεις+1 δεν μεταβάλλουν την κατάστασή τους, αφού έχουν 8 νεκρούς
    //γείτονες.
    i_const+=3;
    j_const+=3;

    //Δεσμεύουμε την απαραίτητη μνήμη για τη δημιουργία του θεωρητικού δισδιάστατου πίνακα που θα περιέχει μόνο τιμές 0 και 1,
    //καθώς και ενός ακόμη κι ενός ακόμη θεωρητικού δισδιάστατου πίνακα που θα περιέχει τις καινούριες τιμές μετά το evolve.
    int **arr = malloc((i_const)*sizeof(int*));
    int **arr_new = malloc((i_const)*sizeof(int*));
    for(int i = 0; i < i_const; i++){
        arr[i] = malloc((j_const)*sizeof(int));
        arr_new[i] = malloc((j_const)*sizeof(int));
    }

    //Αρχικοποιούμε όλα τα κελιά του θεωρητικού πίνακα με 0.
    for(int i = 0; i < i_const; i++){
        for(int j = 0; j < j_const; j++){
            arr[i][j] = 0;
        }
    }

    //Αν κάποιο κελί περιέχεται στα ζωντανά του map, τότε αλλάζουμε την τιμή του στον πίνακα σε 1,
    //λαμβάνοντας υπ' όψιν την επέκτασή του κατά μία μονάδα προς κάθε κατεύθυνση.
    for(MapNode node = map_first(state); node != MAP_EOF; node = map_next(state, node)){
        LifeCellPtr temp_struct = map_node_key(state, node);
        int i = temp_struct->x;
        int j = temp_struct->y;
        arr[i+1][j+1] = 1;
    }

    //Διασχίζουμε κάθε κελί του θεωρητικού δισδιάστατου πίνακα και, ανάλογα με τη θέση του, ελέγχουμε
    //τους αντίστοιχους γείτονες μέσω της συνάρτησης check_alive_dead(). Αν χρειαστεί, προσθέτουμε τα
    //θεωρητικά νεκρά κελιά που δεν καλύπτονται από τον θεωρητικό δισδιάστατο.
    for(int i = 0; i < i_const; i++){
        for(int j = 0; j < j_const; j++){
            int alive = 0;
            int dead = 0;
            if(i == 0){
                if(j == 0){
                    check_alive_dead(arr, 0, 1, &alive, &dead);
                    check_alive_dead(arr, 1, 1, &alive, &dead);
                    check_alive_dead(arr, 1, 0, &alive, &dead);
                    dead += 5;
                }else if(j == j_const-1){
                    check_alive_dead(arr, 0, j-1, &alive, &dead);
                    check_alive_dead(arr, 1, j-1, &alive, &dead);
                    check_alive_dead(arr, 1, j, &alive, &dead);
                    dead += 5;
                }else{
                    check_alive_dead(arr, 0, j-1, &alive, &dead);
                    check_alive_dead(arr, 1, j-1, &alive, &dead);
                    check_alive_dead(arr, 1, j, &alive, &dead);
                    check_alive_dead(arr, 1, j+1, &alive, &dead);
                    check_alive_dead(arr, 0, j+1, &alive, &dead);
                    dead += 3;
                }
            }else if(i == i_const-1){
                if(j == 0){
                    check_alive_dead(arr, i-1, j, &alive, &dead);
                    check_alive_dead(arr, i-1, j+1, &alive, &dead);
                    check_alive_dead(arr, i, j+1, &alive, &dead);
                    dead += 5;
                }else if(j == j_const-1){
                    check_alive_dead(arr, i-1, j, &alive, &dead);
                    check_alive_dead(arr, i-1, j-1, &alive, &dead);
                    check_alive_dead(arr, i, j-1, &alive, &dead);
                    dead += 5;
                }else{
                    check_alive_dead(arr, i, j-1, &alive, &dead);
                    check_alive_dead(arr, i-1, j-1, &alive, &dead);
                    check_alive_dead(arr, i-1, j, &alive, &dead);
                    check_alive_dead(arr, i-1, j+1, &alive, &dead);
                    check_alive_dead(arr, i, j+1, &alive, &dead);
                    dead += 3;
                }
            }else if(j == 0){
                check_alive_dead(arr, i-1, j, &alive, &dead);
                check_alive_dead(arr, i-1, j+1, &alive, &dead);
                check_alive_dead(arr, i, j+1, &alive, &dead);
                check_alive_dead(arr, i+1, j+1, &alive, &dead);
                check_alive_dead(arr, i+1, j, &alive, &dead);
                dead += 3;
            }else if(j == j_const-1){
                check_alive_dead(arr, i-1, j, &alive, &dead);
                check_alive_dead(arr, i-1, j-1, &alive, &dead);
                check_alive_dead(arr, i, j-1, &alive, &dead);
                check_alive_dead(arr, i+1, j-1, &alive, &dead);
                check_alive_dead(arr, i+1, j, &alive, &dead);
                dead += 3;
            }else{
                check_alive_dead(arr, i-1, j-1, &alive, &dead);
                check_alive_dead(arr, i-1, j, &alive, &dead);
                check_alive_dead(arr, i-1, j+1, &alive, &dead);
                check_alive_dead(arr, i, j+1, &alive, &dead);
                check_alive_dead(arr, i+1, j+1, &alive, &dead);
                check_alive_dead(arr, i+1, j, &alive, &dead);
                check_alive_dead(arr, i+1, j-1, &alive, &dead);
                check_alive_dead(arr, i, j-1, &alive, &dead);
            }

            //Εκχωρούμε στον νέο θεωρητικό δισδιάστο πίνακα την νέα τιμή του κελιού, σύμφωνα
            //με τους κανόνες του Game Of Life.
            if(arr[i][j] == 1 && (alive < 2 || alive > 3)){
                arr_new[i][j] = 0;
            }else if(arr[i][j] == 0 && alive == 3){
                arr_new[i][j] = 1;
            }else{
                arr_new[i][j] = arr[i][j];
            }
        }
    }

    //Επειδή κάθε φορά επεκτείνουμε τον θεωρητικό δισδιάστατο πίνακα προς όλες τις κατευθύνσεις
    //και συνεπώς όλο το pattern θα μετατοπίζεται προς τα κάτω και δεξιά, αν τα άκρα αριστερά
    //και άκρα πάνω κελιά είναι νεκρά, μετατοπίζουμε αντίστοιχα όλο το pattern.
    int left = 0, top = 0;
    for(int i = 0; i < i_const; i++){
        if(arr_new[i][0] == 1){
            left = 1;
        }

    }
    for(int j = 0; j < j_const; j++){
        if(arr_new[0][j] == 1){
            top = 1;
        }
    }
    if(left == 0 && top == 0){
        for(int i = 0; i < i_const-1; i++){
            for(int j = 0; j < j_const-1; j++){
                arr_new[i][j] = arr_new[i+1][j+1];
            }
        }
    }else if(left == 0){
        for(int i = 0; i < i_const-1; i++){
            for(int j = 0; j < j_const-1; j++){
                arr_new[i][j] = arr_new[i][j+1];
            }
        }
    }
    else if(top == 0){
        for(int i = 0; i < i_const-1; i++){
            for(int j = 0; j < j_const-1; j++){
                arr_new[i][j] = arr_new[i+1][j];
            }
        }
    }

    //Δημιουργούμε ένα καινούριο LifeState στο οποίο εκχωρούμε τις συντεταγμένες των ζωντανών
    //κελιών μετα το evolve.
    Map map_new = life_create();
    for(int i = 0; i < i_const-1; i++){
        for(int j = 0; j < j_const-1; j++){
            if(arr_new[i][j] == 1){
                LifeCellPtr lifecell = malloc(sizeof(LifeCellPtr));
                lifecell->x = i;
                lifecell->y = j;
                int *boolean = create_int(1);
                map_insert(map_new, lifecell, boolean);
            }
        }
    }

    //Αποδεσμεύουμε τη μνήμη που έχει δεσμευθεί δυναμικά.
    for(int i = 0; i < i_const; i++){
        free(arr[i]);
        free(arr_new[i]);
    }
    free(arr);
    free(arr_new);

    return map_new;
}

// Καταστρέφει την κατάσταση ελευθερώντας οποιαδήποτε μνήμη έχει δεσμευτεί
void life_destroy(LifeState state){
    map_destroy(state);
}

//Συνάρτηση compare_strings για χρήση στο map της life_evolve_many στο οποίο περιέχονται
//οι διάφορες καταστάσεις σε μορφή RLE.
int compare_strings(Pointer a, Pointer b){
    return strcmp(a, b);
}

//Συνάρτηση compare_lifestates που συγκρίνει δύο τύπους LifeState, αποθηκεύοντας τις δύο καταστάσεις
//σε δύο αρχεία RLE και συγκρίνοντας, έπειτα, τα δύο strings που περιέχονται στα αρχεία RLE.
//Διαβάζουμε το string κάθε αρχείου δύο φορές, μία για να αντλήσουμε το μέγεθός του και να μάθουμε
//τι μνήμη χρειάζεται να δεσμεύσουμε για αυτό, και μία για να το αποθηκεύσουμε σε κατάλληλη μεταβλητή
//ώστε στο μέλλον να το συγκρίνουμε.
int compare_lifestates(Pointer a, Pointer b){
    
    life_save_to_rle(a, "state1");
    life_save_to_rle(b, "state2");
    
    //Πρώτο άνοιγμα πρώτου αρχείου.
    FILE *fp = fopen("state1.rle", "r");
    char* arr;
    int i = 1;
    while((arr = malloc(2*sizeof(char))) && (fgets(arr, 2, fp)) && (*arr != EOF)){
        i++;
    }
    fclose(fp);
    
    //Δεύτερο άνοιγμα πρώτου αρχείου.
    fp = fopen("state1.rle", "r");
    char* state1_rle = malloc(i*sizeof(char));
    fscanf(fp, "%s", state1_rle);
    fclose(fp);

    //Πρώτο άνοιγμα δεύτερου αρχείου.
    fp = fopen("state2.rle", "r");
    i = 1;
    while((arr = malloc(2*sizeof(char))) && (fgets(arr, 2, fp)) && (*arr != EOF)){
        i++;
    }
    fclose(fp);
    
    //Δεύτερο άνοιγμα δεύτερου αρχείου.
    fp = fopen("state2.rle", "r");
    char* state2_rle = malloc(i*sizeof(char));
    fscanf(fp, "%s", state2_rle);
    
    fclose(fp);

    int result = strcmp(state1_rle, state2_rle);

    free(arr);
    free(state1_rle);
    free(state2_rle);
    return result;
}

// Επιστρέφει μία λίστα από το πολύ steps εξελίξεις, ξεκινώνας από την κατάσταση
// state. Αν βρεθεί επανάληψη τότε στο *loop αποθηκεύεται ο κόμβος της λίστας στον
// οποίο συνεχίζεται η εξέλιξη μετά τον τελευταίο κόμβο, διαφορετικά NULL.
List life_evolve_many(LifeState state, int steps, ListNode *loop){

    //Δημιουργούμε ένα map που θα περιέχει τα rle κάθε LifeState για εύκολη σύγκριση των τελευταίων
    //και μια λίστα που θα περιέχει τα LifeStates.
    Map map = map_create(compare_strings, free, NULL);
    List list = list_create((DestroyFunc)life_destroy);

    //Εισάγουμε το πρώτο LifeState, που είναι αυτό που μας δίνεται ως παράμετρος με την κλήση της
    //συνάρτησης.
    ListNode node = LIST_BOF;
    list_insert_next(list, node, state);
    
    life_save_to_rle(state, "temp1");

    //Πρώτο διάβασμα του temp1.rle για να μάθουμε το μέγεθος του string που περιέχει.
    FILE *fp = fopen("temp1.rle", "r");
    char* arr;
    int k = 1;
    while((arr = malloc(2*sizeof(char))) && (fgets(arr, 2, fp)) && (*arr != EOF)){
        k++;
        free(arr);
    }
    free(arr);

    fclose(fp);
    
    //Δεύτερο διάβασμα του temp1.rle για να αντλήσουμε το string που περιέχει.
    fp = fopen("temp1.rle", "r");
    char *str = malloc(k*sizeof(char));
    fscanf(fp, "%s", str);
    fclose(fp);

    //Εισάγουμε το string που αντλήσαμε στο map.
    map_insert(map, str, NULL);
    
    //Για να μπορέσει να γίνει σωστό evolve και στην πρώτη κλήση της life_evolve μέσω του παρακάτω βρόχου.
    LifeState state_new = state;
    
    //Παράγουμε το πολύ steps LifeStates.
    for(int i = 0; i < steps; i++){

        node = list_last(list);
        state_new = life_evolve(state_new);
        
        life_save_to_rle(state_new, "temp1");

        //Πρώτο διάβασμα του temp1.rle για να μάθουμε το μέγεθος του string το οποίο περιέχει.
        fp = fopen("temp1.rle", "r");
        k = 1;
        while((arr = malloc(2*sizeof(char))) && (fgets(arr, 2, fp)) && (*arr != EOF)){
            k++;
            free(arr);
        }
        free(arr);
        fclose(fp);

        //Δεύτερο διάβασμα του temp1.rle για να αντλήσουμε το string που περιέχει.
        fp = fopen("temp1.rle", "r");
        char *str = malloc(k*sizeof(char));
        fscanf(fp, "%s", str);
        fclose(fp);

        //Βρίσκουμε αν το string που διαβάσαμε υπάρχει ήδη στο map, δηλαδή έχουμε παράξει ξανά
        //το συγκεκριμένο state. Αν δεν το έχουμε διαβάσει ξανά, τότε εισάγουμε το rle στο map
        //και το state στη λίστα, αλλιώς εκχωρούμε στο *loop τη διεύθυνση στην οποία βρίσκεται
        //το state που έχουμε παράξει και στο παρελθόν και επιστρέφουμε τη λίστα.
        if(*str!=EOF){
            int *sum = map_find(map, str);
            if(sum == NULL){
                map_insert(map, str, NULL);
                list_insert_next(list, node, state_new);
                *loop = NULL;
            }else{
                *loop = list_find_node(list, state_new, compare_lifestates);
                return list;
            }
        }
    }

    //Αποδεσμεύουμε το map στο οποίο έχουμε αποθηκευμένα τα rle, αφού δεν χρειάζεται πλέον.
    map_destroy(map);
    return list;
}