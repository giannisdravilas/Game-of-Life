//////////////////////////////////////////////////////////////////
//
// Test για το life module
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing

#include "life.h"

//Δημιουργεί ένα άδειο state, αφού όλα τα κελιά του θα πρέπει να είναι νεκρά.
void test_life_create(void){
    
    LifeState state = life_create();
    TEST_CHECK(map_size(state) == 0);
    
    life_destroy(state);
}

//Δημιουργεί ένα LifeState από το αρχείο file.rle και ελέγχει μέσω της life_get_cell() ότι
//κάθε κελί έχει την κατάσταση (ζωντανό, νεκρό) που πρέπει. Το pattern που ελέγχεται είναι
//ένα glider, μέσω του RLE: bo$2bo$3o!
void test_life_create_from_rle(void){
    
    LifeState state = life_create_from_rle("file");
    LifeCell lifecell;

    lifecell.x = 0;
    lifecell.y = 0;
    TEST_CHECK(life_get_cell(state, lifecell) == false);

    lifecell.x = 0;
    lifecell.y = 1;
    TEST_CHECK(life_get_cell(state, lifecell) == true);

    lifecell.x = 0;
    lifecell.y = 2;
    TEST_CHECK(life_get_cell(state, lifecell) == false);

    for(int i = 0; i < 2; i++){
        lifecell.x = 1;
        lifecell.y = i;
        TEST_CHECK(life_get_cell(state, lifecell) == false);
    }

    lifecell.x = 1;
    lifecell.y = 2;
    TEST_CHECK(life_get_cell(state, lifecell) == true);

    for(int i = 0; i < 3; i++){
        lifecell.x = 2;
        lifecell.y = i;
        TEST_CHECK(life_get_cell(state, lifecell) == true);
    }
    life_destroy(state);
}

//Δημιουργεί ένα LifeState από το αρχείο file.rle. Έπειτα αποθηκεύει το LifeState σε μορφή RLE
//στο αρχείο file.rle. Ελέγχει ότι το string που αποθηκεύτηκε στο αρχείο είναι το σωστό για ένα
//glider, δηλαδή το "bo$2bo$3o!".
void test_life_save_to_rle(void){
    
    LifeState state = life_create_from_rle("file");
    life_save_to_rle(state, "output");
    
    FILE *fp = fopen("output.rle", "r");
    char *arr = malloc(11*sizeof(char));
    fgets(arr, 11, fp);
    TEST_CHECK(strcmp(arr, "bo$2bo$3o!") == 0);
    fclose(fp);
    
    free(arr);
    life_destroy(state);
}

//Δημιουργεί ένα LifeState από το αρχείο file.rle και έπειτα καλεί για τις συντεταγμένες των ζωντανών
//κελιών που περιέχει το LifeState και των γειτονικών τους νεκρών κελιών την life_get_cell, η οποία
//πρέπει να επιστρέφει τις σωστές τιμές. Το pattern που ελέγχεται είναι ένα glider και το file.rle
//πρέπει να περιέχει το RLE: bo$2bo$3o!. 
void test_life_get_cell(void){
    LifeState state = life_create_from_rle("file");
    LifeCell lifecell;

    lifecell.x = 0;
    lifecell.y = 0;
    TEST_CHECK(life_get_cell(state, lifecell) == false);

    lifecell.x = 0;
    lifecell.y = 1;
    TEST_CHECK(life_get_cell(state, lifecell) == true);

    lifecell.x = 0;
    lifecell.y = 2;
    TEST_CHECK(life_get_cell(state, lifecell) == false);

    for(int i = 0; i < 2; i++){
        lifecell.x = 1;
        lifecell.y = i;
        TEST_CHECK(life_get_cell(state, lifecell) == false);
    }

    lifecell.x = 1;
    lifecell.y = 2;
    TEST_CHECK(life_get_cell(state, lifecell) == true);

    for(int i = 0; i < 3; i++){
        lifecell.x = 2;
        lifecell.y = i;
        TEST_CHECK(life_get_cell(state, lifecell) == true);
    }

    life_destroy(state);
}

//Δημιουργούμε ένα LifeState από το αρχείο file.rle. Μεταβάλλουμε ένα κελί που γνωρίζουμε ότι είναι
//νεκρό σε ζωνταντό και αντίστροφα. Έπειτα, καλούμε τη life_get_cell() για να ελέγξουμε ότι η αλλαγή
//έγινε σωστά. Το αρχείο file.rle θα πρέπει να περιέχει το RLE για ένα glider: bo$2bo$3o!.
void test_life_set_cell(void){

    LifeState state = life_create_from_rle("file");
    LifeCell lifecell;

    lifecell.x = 1;
    lifecell.y = 1;
    life_set_cell(state, lifecell, true);
    TEST_CHECK(life_get_cell(state, lifecell) == true);

    lifecell.x = 1;
    lifecell.y = 2;
    life_set_cell(state, lifecell, false);
    TEST_CHECK(life_get_cell(state, lifecell) == false);
    
    life_destroy(state);
}

//Δημιουργούμε ένα LifeState από το αρχείο file.rle, το οποίο πρέπει να περιέχει το RLE για ένα glider,
//δηλαδή bo$2bo$3o!. Έπειτα, καλούμε την life_evolve() κι ελέγχουμε ότι στο καινούριο LifeState περιέχονται
//τα σωστά ζωντανά κελιά, και ότι τα γειτονικά τους που δεν ανήκουν στο LifeState είναι νεκρά.
void test_life_evolve(void){
    LifeState state = life_create_from_rle("file");
    LifeState state_new = life_evolve(state);
    LifeCell lifecell;

    for(int j = 0; j < 5; j++){
        lifecell.x = 0;
        lifecell.y = j;
        TEST_CHECK(life_get_cell(state_new, lifecell) == false);
    }

    for(int j = 0; j < 4; j+=2){
        lifecell.x = 1;
        lifecell.y = j;
        TEST_CHECK(life_get_cell(state_new, lifecell) == true);
    }

    for(int i = 1; i < 4; i++){
        lifecell.x = i;
        lifecell.y = 3;
        TEST_CHECK(life_get_cell(state_new, lifecell) == false);
    }

    for(int i = 2; i < 4; i++){
        for(int j = 0; j < 3; j++){
            lifecell.x = i;
            lifecell.y = j;
            if(j == 1 || (i == 2 && j == 2)){
                TEST_CHECK(life_get_cell(state_new, lifecell) == true);
            }else{
                TEST_CHECK(life_get_cell(state_new, lifecell) == false);
            }
        }
    }

    for(int j = 0; j < 4; j++){
        lifecell.x = 4;
        lifecell.y = j;
        TEST_CHECK(life_get_cell(state_new, lifecell) == false);
    }

    life_destroy(state);
    life_destroy(state_new);
}

//life_destroy()
//
//Η life_destroy() καλείται σε όλα τα tests και για να βρούμε αν δουλεύει σωστά ελέγχουμε με valgrind

//Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "test_life_create", test_life_create },
    { "test_life_create_from_rle", test_life_create_from_rle },
    { "test_life_save_to_rle", test_life_save_to_rle },
    { "test_life_get_cell", test_life_get_cell },
    { "test_life_set_cell", test_life_set_cell },
    { "test_life_evolve", test_life_evolve },

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};