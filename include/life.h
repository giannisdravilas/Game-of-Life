///////////////////////////////////////////////////////////////////
//
// pq_sort
//
// Ταξινόμηση μέσω ουράς προτεραιότητας
//
///////////////////////////////////////////////////////////////////

#pragma once // #include το πολύ μία φορά

#include "ADTMap.h"
#include "ADTList.h"

typedef Map LifeState;

typedef struct {
	int x, y;
} LifeCell;

typedef LifeCell *LifeCellPtr;

// Δημιουργεί μια κατάσταση του παιχνιδιού όπου όλα τα κελιά είναι νεκρά.
LifeState life_create();

// Δημιουργεί μία κατάσταση του παιχνιδιού με βάση τα δεδομένα του αρχείο file (RLE format)
LifeState life_create_from_rle(char* file);

// Αποθηκεύει την κατάσταση state στο αρχείο file (RLE format)
void life_save_to_rle(LifeState state, char* file);

// Επιστρέφει την τιμή του κελιού cell στην κατάσταση state (true: ζωντανό, false: νεκρό)
bool life_get_cell(LifeState state, LifeCell cell);

// Αλλάζει την τιμή του κελιού cell στην κατάσταση state
void life_set_cell(LifeState state, LifeCell cell, bool value);

// Παράγει και επιστρέφει μια νέα κατάσταση που προκύπτει από την εξέλιξη της κατάστασης state.
// Η ίδια η state δε μεταβάλλεται (ούτε καταστρέφεται).
LifeState life_evolve(LifeState state);

// Καταστρέφει την κατάσταση ελευθερώντας οποιαδήποτε μνήμη έχει δεσμευτεί
void life_destroy(LifeState state);

// Επιστρέφει μία λίστα από το πολύ steps εξελίξεις, ξεκινώνας από την κατάσταση
// state. Αν βρεθεί επανάληψη τότε στο *loop αποθηκεύεται ο κόμβος της λίστας στον
// οποίο συνεχίζεται η εξέλιξη μετά τον τελευταίο κόμβο, διαφορετικά NULL.
List life_evolve_many(LifeState state, int steps, ListNode* loop);