/* ... */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include <limits.h>

#include "spell.h"
#include "hashtbl.h"

#define NUM_LETTERS 26
#define HASH_TABLE_MULTIPLIER 10
#define EDIT_LIMIT 3

/**** FUNCTION DECLARATIONS ****/
int compute_edit_distance(char *word1, char *word2);
bool check_store_dist_1(HashTable *table, char *word, char **storage, int *num_strings);
void check_dist_1(HashTable *table, char *word, char **best_word, int *best_pos);
bool check_dist_2(HashTable *table, char **storage, int num_strings);
bool search_dict_dist_3(List *dictionary, char *word);

int min(int a1, int a2, int a3);
void delete_char(char *word, char *new_word, int n);
void substitution_cycle(char *word, int n);
void intial_insert(char *word, char *new_word, int n);
void hash_dictionary(HashTable *table, List *dictionary);
void check_word(HashTable *table, char *word, char **best_word, int *best_pos);
void store_string_copy(char **storage, char *word, int *num_strings);

// see Assignment Task 1: Computing edit distance
void print_edit_distance(char *word1, char *word2) {
	printf("%d\n", compute_edit_distance(word1, word2));
}

// see Assignment Task 2: Enumerating all possible edits
void print_all_edits(char *word) {
	// consider all possible deletions
	int del;
	// new word should be one char less than word, plus one for null byte
	char *new_word = malloc(sizeof(char)*(strlen(word)));
	// iterate through each char of word and print out a new word with that char deleted
	for(del = 0; del < strlen(word); del++) {
		delete_char(word, new_word, del);
		printf("%s\n", new_word);
	}
	free(new_word);

	// consider all possible substitutions
	int sub, i;
	// new word should have same number of chars as word, plus one for null byte
	new_word = malloc(sizeof(char)*(strlen(word)+1));
	// iterate through each char of word, and consider each of 26 substitutions available for that char
	strcpy(new_word, word);
	for(sub = 0; sub < strlen(word); sub++) {
		for(i = 0; i < NUM_LETTERS; i++) {
			substitution_cycle(new_word, sub);
			printf("%s\n", new_word);
		}
	}
	free(new_word);

	// consider all possible insertions
	int ins;
	// new word should have one more char than word, plus one for null byte
	new_word = malloc(sizeof(char)*(strlen(word)+2));
	// iterate through each spacing between chars of word, and consider each of 26 insertions available for that spacing
	for(ins = 0; ins < strlen(word) + 1; ins++) {
		intial_insert(word, new_word, ins);
		printf("%s\n", new_word);
		// perform the substitution cycle on the inserted letter to check the other 25 cases
		for(i = 0; i < NUM_LETTERS - 1; i++) {
			substitution_cycle(new_word, ins);
			printf("%s\n", new_word);
		}
	}
	free(new_word);
}

// see Assignment Task 3: Spell checking
void print_checked(List *dictionary, List *document) {
	clock_t start_t, end_t;
	start_t = clock();
	// initialise hash table
	HashTable *table = new_hash_table(dictionary->size*HASH_TABLE_MULTIPLIER);
	// hash words in dictionary to hash table
	hash_dictionary(table, dictionary);
	// try to find words in document in hash table
	Node *curr_node = document->head;
	while(curr_node != NULL) {
		char *word = curr_node->data;
		if(hash_table_has(table, word)) {
			printf("%s\n", word);
		} else {
			printf("%s?\n", word);
		}
		curr_node = curr_node->next;
	}
	end_t = clock();
	printf("Time taken: %f\n", (double) (end_t - start_t)/CLOCKS_PER_SEC);
}

// see Assignment Task 4: Spelling correction
void print_corrected(List *dictionary, List *document) {
	// initialise hash table
	HashTable *table = new_hash_table(dictionary->size*HASH_TABLE_MULTIPLIER);
	// hash all dictionary words into table
	hash_dictionary(table, dictionary);
	// try to find each document word in hash table, or else a close match
	Node *curr_node = document->head;
	while(curr_node != NULL) {
		bool found = false;
		char *curr_word = curr_node->data;

		// check if the word appears in the dictionary
		if(hash_table_has(table, curr_word)) {
			printf("%s\n", curr_word);
			found = true;
		}

		// otherwise look for a close match in the dictionary that has edit distance 1
		// same algorithm as task 2 is used
		// initialise array to store strings with edit distance 1
		// note there are 53*length + 26 strings produced using this algorithm
		char **storage = malloc(sizeof(char*)*(53*strlen(curr_word)+26));
		int num_strings = 0;

		if(!found) {
			found = check_store_dist_1(table, curr_word, storage, &num_strings);
		}

		// if still no match found, look for a close match with edit distance 2
		if(!found) {
			found = check_dist_2(table, storage, num_strings);
		}

		// if still no match found, look for a close match with edit distance 3
		if(!found) {
			found = search_dict_dist_3(dictionary, curr_word);
		}

		// if still no match found, give up
		if(!found) {
			printf("%s?\n", curr_word);
		}

		// move on to the next word
		curr_node = curr_node->next;
	}

}

// finds the Levenshtein edit distance between two words
int compute_edit_distance(char *word1, char *word2) {
	int i = 0, j = 0;
	int length1 = strlen(word1), length2 = strlen(word2);
	int option1 = 0, option2 = 0, option3 = 0;
	int edit_dist = 0;

	// initialise our two dimensional array
	int **array = malloc(sizeof(int*)*(length1+1));
	for(i = 0; i <= length1; i++) {
		array[i] = malloc(sizeof(int)*(length2+1));
	}

	// initialise values for the first column and row
	for(i = 0; i <= length1; i++) {
		array[i][0] = i;
	}
	for(j = 1; j <= length2; j++) {
		array[0][j] = j;
	}

	// fill in edit distance table
	for(j = 1; j <= length2; j++) {
		for(i = 1; i <= length1; i++) {
			// either we insert a letter from word2
			option1 = array[i][j-1] + 1;
			// or we insert a letter from word1
			option2 = array[i-1][j] + 1;
			// or we add both letters at the same time
			if(word1[i-1] == word2[j-1]) {
				// then no edit is made
				option3 = array[i-1][j-1];
			} else {
				// then a substitution is made
				option3 = array[i-1][j-1] + 1;
			}
			array[i][j] = min(option1, option2, option3);
		}
	}

	// our final edit distance is at the bottom right corner of the table
	edit_dist = array[length1][length2];

	// clean up
	for(i = 0; i < length1 + 1; i++) {
		free(array[i]);
	}
	free(array);

	return edit_dist;

}

// checks and stores every string with edit distance 1 to word in the hashtable
// if at least one word is found, the one with the highest position in the dictionary is printed
// returns true if a word is found
bool check_store_dist_1(HashTable *table, char *word, char **storage, int *num_strings) {
	// consider all possible deletions
	int del;
	char *new_word;
	// record the word found with the lowest dictionary position value
	char *best_word = NULL;
	int best_pos = INT_MAX;
	// iterate through each char of word and print out a new word with that char deleted
	for(del = 0; del < strlen(word); del++) {
		// new word should be one char less than curr_word, plus one for null byte
		new_word = malloc(sizeof(char)*(strlen(word)));
		delete_char(word, new_word, del);
		check_word(table, new_word, &best_word, &best_pos);
		store_string_copy(storage, new_word, num_strings);
		free(new_word);
	}
			
	// consider all possible substitutions
	int sub, i;
	// new word should have same number of chars as word, plus one for null byte
	new_word = malloc(sizeof(char)*(strlen(word)+1));
	// iterate through each char of word, and consider each of 26 substitutions available for that char
	strcpy(new_word, word);
	for(sub = 0; sub < strlen(word); sub++) {
		for(i = 0; i < NUM_LETTERS; i++) {
			substitution_cycle(new_word, sub);
			check_word(table, new_word, &best_word, &best_pos);
			store_string_copy(storage, new_word, num_strings);
		}
	}
	free(new_word);

	// consider all possible insertions
	int ins;
	// new word should have one more char than word, plus one for null byte
	new_word = malloc(sizeof(char)*(strlen(word)+2));
	// iterate through each spacing between chars of word, and consider each of 26 insertions available for that spacing
	for(ins = 0; ins < strlen(word) + 1; ins++) {
		intial_insert(word, new_word, ins);
		check_word(table, new_word, &best_word, &best_pos);
		store_string_copy(storage, new_word, num_strings);
		// perform the substitution cycle on the inserted letter to check the other 25 cases
		for(i = 0; i < NUM_LETTERS - 1; i++) {
			substitution_cycle(new_word, ins);
			check_word(table, new_word, &best_word, &best_pos);
			store_string_copy(storage, new_word, num_strings);
		}
	}
	free(new_word);

	// print out the best word we've found, if it exists
	if(best_word != NULL) {
		printf("%s\n", best_word);
		free(best_word);
		return true;
	} else {
		return false;
	}
}

// checks every string with edit distance 1 to word in the hashtable
// if the word appears in the table, check if it appears higher up in the dictionary than the current best_word
// if so, make this the new best_word and concurrently update best_pos
void check_dist_1(HashTable *table, char *word, char **best_word, int *best_pos) {
	// consider all possible deletions
	int del;
	char *new_word;
	
	// iterate through each char of word and print out a new word with that char deleted
	for(del = 0; del < strlen(word); del++) {
		// new word should be one char less than curr_word, plus one for null byte
		new_word = malloc(sizeof(char)*(strlen(word)));
		delete_char(word, new_word, del);
		check_word(table, new_word, best_word, best_pos);
		free(new_word);
	}
			
	// consider all possible substitutions
	int sub, i;
	// new word should have same number of chars as word, plus one for null byte
	new_word = malloc(sizeof(char)*(strlen(word)+1));
	// iterate through each char of word, and consider each of 26 substitutions available for that char
	strcpy(new_word, word);
	for(sub = 0; sub < strlen(word); sub++) {
		for(i = 0; i < NUM_LETTERS; i++) {
			substitution_cycle(new_word, sub);
			check_word(table, new_word, best_word, best_pos);
		}
	}
	free(new_word);

	// consider all possible insertions
	int ins;
	// new word should have one more char than word, plus one for null byte
	new_word = malloc(sizeof(char)*(strlen(word)+2));
	// iterate through each spacing between chars of word, and consider each of 26 insertions available for that spacing
	for(ins = 0; ins < strlen(word) + 1; ins++) {
		intial_insert(word, new_word, ins);
		check_word(table, new_word, best_word, best_pos);
		// perform the substitution cycle on the inserted letter to check the other 25 cases
		for(i = 0; i < NUM_LETTERS - 1; i++) {
			substitution_cycle(new_word, ins);
			check_word(table, new_word, best_word, best_pos);
		}
	}
	free(new_word);

}

// checks every string with edit distance 2 to curr_word in the hashtable
// functions by iterating over an array of stored string of edit distance 1 from curr_word
// if at least one word is found, the one with the highest position in the dictionary is printed
// returns true if a word is found
bool check_dist_2(HashTable *table, char **storage, int num_strings) {
	int i;
	// record the word found with the lowest dictionary position value
	char *best_word = NULL;
	int best_pos = INT_MAX;
	for(i = 0; i < num_strings; i++) {
		check_dist_1(table, storage[i], &best_word, &best_pos);
	}

	// print out the best word we've found, if it exists
	if(best_word != NULL) {
		printf("%s\n", best_word);
		free(best_word);
		return true;
	} else {
		return false;
	}
}

// computes the edit distance between a queried word and each word in the dictionary
// the word with the lowest edit distance, if it is less than or equal to 3, is printed
// if there is a tie in edit distance, the word with the highest position in the dictionary is printed
// returns true if a word is found
bool search_dict_dist_3(List *dictionary, char *word) {
	Node *curr_node = dictionary->head;
	char *best_word = NULL;
	int best_edit_dist = INT_MAX;
	int new_edit_dist = 0;
	while(curr_node) {
		char *word2 = curr_node->data;
		new_edit_dist = compute_edit_distance(word, word2);
		if(new_edit_dist < best_edit_dist) {
			// we've found a new best word
			best_edit_dist = new_edit_dist;
			best_word = word2;
		}
		curr_node = curr_node->next;
	}
	
	// print out the best word we've found, if it exists
	if(best_edit_dist <= EDIT_LIMIT) {
		printf("%s\n", best_word);
		return true;
	} else {
		return false;
	}
}

/**** HELPFUL FUNCTIONS ****/
// assign new_word to be word with character at index n deleted
void delete_char(char *word, char *new_word, int n) {
	assert(word != NULL);

	int i = 0, j = 0;

	for(i = 0; i < strlen(word); i++) {
		// ignore the char at index n
		if(i != n) {
			new_word[j] = word[i];
			j++;
		}
	}
	new_word[j] = '\0';

}

// change the char at index n of word to the next alphabetical letter, with wrap around
void substitution_cycle(char *word, int n) {
	assert(word != NULL);
	if(word[n] == 'z') {
		word[n] = 'a';
	} else {
		word[n]++;
	}
}

// assign new word to be the same as word but with an 'a' inserted at index n
void intial_insert(char *word, char *new_word, int n) {
	assert(word != NULL);

	int i = 0, j = 0;

	for(i = 0; i < strlen(word) + 1; i++) {
		if(i == n) {
			// make an insertion
			new_word[i] = 'a';
		} else {
			// copy the next letter from word
			new_word[i] = word[j];
			j++;
		}
	}
	new_word[i] = '\0';

}

// hashes all words in a dictionary into a table, recording the word's position in the dictionary as the hash value
void hash_dictionary(HashTable *table, List *dictionary) {
	char *curr_word;
	Node *curr_node;
	int pos = 1;

	curr_node = dictionary->head;
	while(curr_node != NULL) {
		// copy the word
		curr_word = malloc(sizeof(char)*(strlen(curr_node->data)+1));
		strcpy(curr_word, curr_node->data);
		// insert into the table
		hash_table_put(table, curr_word, pos);
		pos++;
		curr_node = curr_node->next;
		free(curr_word);
	}

}

// check if the word is in the hashtable, and then check if it is higher up than the current best matched word
void check_word(HashTable *table, char *word, char **best_word, int *best_pos) {
	assert(word != NULL);
	if(hash_table_has(table, word)) {
		if(hash_table_get(table, word) < *best_pos) {
			// found a word that is higher up the dictionary
			// store a copy of that word
			if(*best_word != NULL) {
				free(*best_word);
			}
			*best_word = malloc(sizeof(char)*(strlen(word)+1));
			strcpy(*best_word, word);
			*best_pos = hash_table_get(table, word);
		}
	}
}

// store a copy of word into a storage array of strings, updating the counter for storage as well
void store_string_copy(char **storage, char *word, int *num_strings) {
	storage[*num_strings] = malloc(sizeof(char)*(strlen(word)+1));
	strcpy(storage[*num_strings], word);
	(*num_strings)++;
}

// return the minimum of three integer values
int min(int a1, int a2, int a3) {
	if(a1 < a2) {
		if(a1 < a3) {
			return a1;
		} else {
			return a3;
		}
	} else {
		if(a2 < a3) {
			return a2;
		} else {
			return a3;
		}
	}
}