#include "bmp.h"
#include "gif.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "life.h"
#include "ADTList.h"

int main(int argc, char *argv[]) {

	//Αποθηκεύουμε σε κατάλληλες μεταβλητές τα ορίσματα που πήραμε από την γραμμή εντολών.
	int len = strlen(argv[1]);
	char *file = malloc((len+1)*sizeof(char));
	strcpy(file, argv[1]);
	int top = atoi(argv[2]);
	int left = atoi(argv[3]);
	int bottom = atoi(argv[4]);
	int right = atoi(argv[5]);
	int frames = atoi(argv[6]);
	int speed = atoi(argv[8]);
	int delay = atoi(argv[9]);
	len = strlen(argv[10]);
	char *name = malloc((len+1)*sizeof(char));
	name = strcpy(name, argv[10]);

	//Έλεγχος εγκυρότητας
	if(frames < 1){
		printf("Frames should be >=1");
		return -1;
	}

	if(speed < 1){
		printf("Speed should be >=1");
		return -1;
	}

	//Το size του gif θα εξαρτάται από τις συντεταγμένες που θα δώσει ο χρήστης.
	int size;
	if(bottom > right){
		size = bottom;
	}else{
		size = right;
	}

	// Δημιουργία ενός GIF και ενός bitmap στη μνήμη
	GIF* gif = gif_create(size, size);
	Bitmap* bitmap = bm_create(size, size);

	// Ορίζουμε τα χρώματα που χρησιμοποιούνται στο GIF (αλλιώς παίρνει αυτά που υπάρχουν στο πρώτο frame)
	unsigned int palette[] = { 0xFF000000, 0xFFFFFFFF }; // black, white
	gif_set_palette(gif, palette, 2);

	// Default καθυστέρηση μεταξύ των frames, σε εκατοστά του δευτερολέπτου
	gif->default_delay = delay;

	//Δημιουργία state από το όνομα του αρχείο που έδωσε ο χρήστης.
	LifeState state = life_create_from_rle(file);
	
	//Δημιουργία επόμενων εξελίξεων.
	ListNode *loop = malloc(sizeof(loop));
	*loop = NULL;
	List list = life_evolve_many(state, frames, loop);

	// Δημιουργούμε ενα animation για κάθε LifeState που υπάρχει στη list.

	//Μετρητές frames και επαναλήψεων αντίστοιχα.
	int i = 0;
	int m = -1;
	for(ListNode node = list_first(list); (node != LIST_EOF && i < frames); node = list_next(list, node)){
		
		m++;

		//Αν speed > 1, τότε προσπερνάμε όσα frames δείχνει το speed.
		if(speed > 1 && m > 0){
			if((m % speed) != 0){
				continue;
			}
		}

		i++;
		LifeState state_from_list = list_node_value(list, node);
		
		// Σε κάθε frame, πρώτα μαυρίζουμε ολόκληρο το bitmap
		bm_set_color(bitmap, bm_atoi("black"));
		bm_clear(bitmap);
				
		//Χρωματίζουμε με άσπρο όσα κελιά υπάρχουν στο LifeState, δηλαδή είναι ζωντανά.
		for(int k = top; k < bottom; k++){
			for(int j = left; j < right; j++){
				LifeCell temp_struct;
				temp_struct.x = k;
				temp_struct.y = j;

				//Επειδή στο LifeCell τα x, y χρησιμοποιούνται ως i, j ενός θεωρητικού δισδιάστατου πίνακα
				//αντίστοιχα (βλ. README), στην ουσία το x αντιπροσωπεύει τον κατακόρυφο άξονα (i του πίνακα),
				//ενώ το y αντιπροσωπεύει τον οριζόντιο άξονα (j του πίνακα). Γι' αυτό, τα χρησιμοποιούμε
				//αντίστροφα στη bm_fillrect().
				if(life_get_cell(state_from_list, temp_struct) == true){
					bm_set_color(bitmap, bm_atoi("white"));
					bm_fillrect(bitmap, j, k, j, k);
				}
			}
		}

		// Τέλος προσθέτουμε το bitmap σαν frame στο GIF (τα περιεχόμενα αντιγράφονται)
		gif_add_frame(gif, bitmap);
	}
	
	//Ελέγχουμε αν έχει βρεθεί επανάληψη στην life_evolve_many() και συνεχίζουμε το animation.
	if(*loop != NULL){
		while(i < frames){
			for(ListNode node = *loop; (node != LIST_EOF && i < frames); node = list_next(list, node)){
				
				m++;

				//Αν speed > 1, τότε προσπερνάμε όσα frames δείχνει το speed.
				if(speed > 1 && m > 0){
					if((m % speed) != 0){
						continue;
					}
				}

				i++;
				LifeState state_from_list = list_node_value(list, node);
				
				// Σε κάθε frame, πρώτα μαυρίζουμε ολόκληρο το bitmap
				bm_set_color(bitmap, bm_atoi("black"));
				bm_clear(bitmap);
		
				//Χρωματίζουμε με άσπρο όσα κελιά υπάρχουν στο LifeState, δηλαδή είναι ζωντανά.
				for(int k = top; k < bottom; k++){
					for(int j = left; j < right; j++){
						LifeCell temp_struct;
						temp_struct.x = k;
						temp_struct.y = j;

						//Επειδή στο LifeCell τα x, y χρησιμοποιούνται ως i, j ενός θεωρητικού δισδιάστατου πίνακα
						//αντίστοιχα (βλ. README), στην ουσία το x αντιπροσωπεύει τον κατακόρυφο άξονα (i του πίνακα),
						//ενώ το y αντιπροσωπεύει τον οριζόντιο άξονα (j του πίνακα). Γι' αυτό, τα χρησιμοποιούμε
						//αντίστροφα στη bm_fillrect().
						if(life_get_cell(state_from_list, temp_struct) == true){
							bm_set_color(bitmap, bm_atoi("white"));
							bm_fillrect(bitmap, j, k, j, k);
						}
					}
				}

				// Τέλος προσθέτουμε το bitmap σαν frame στο GIF (τα περιεχόμενα αντιγράφονται)
				gif_add_frame(gif, bitmap);
			}
		}
	}

	// Αποθήκευση σε αρχείο
	char *name_format = malloc((len+5)*sizeof(char));
	strcpy(name_format, name);
    strcat(name_format, ".gif");
	gif_save(gif, name_format);

	// Αποδέσμευση μνήμης
	free(file);
	free(name);
	free(name_format);
	free(loop);
	list_destroy(list);
	bm_free(bitmap);
	gif_free(gif);
}
