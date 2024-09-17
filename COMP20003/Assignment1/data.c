/* ---------------------------------------------------------------------------
Written By Max Chivers. 
..Project: COMP20003 Assignment 1
    data.c :
            = the implementation of data module for Assignment 1
--------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"

// Define Suburb
struct Suburb {
	int id;                // 0 - The record IDs added by the COMP20003 teaching team (integer)
	int suburb_id;         // 1 - The suburb ID (integer)
	char *suburb_name;     // 2 - The name of the suburb (string)
	int year;              // 3 - The year the information was recorded for (integer)
	char *state_ids;       // 4 - The IDs of the corresponding states (string)
	char *state_names;     // 5 - The names of the corresponding states (string [of integer])
	char *local_gov_ids;   // 6 - The IDs of the corresponding local government areas (string [of integer])
	char *local_gov_names; // 7 - The names of the corresponding local government areas (string)
	double y;              // 8 - The latitude (y) of the suburb centre (double)
	double x;              // 9 - The longitude (x) of the suburb centre (double)	
};

// Encapsulated ColumnInfo array
static char* headers[MAX_HEADERS];  // Indexes for header see above
static int num_headers = 0; // To keep track of the number of headers

/* OR!!! 
static const char *headers[] = {
    "COMP20003 Code",
    "Official Code Suburb",
    "Official Name Suburb",
    "Year",
    "Official Code State",
    "Official Name State",
    "Official Code Local Government Area",
    "Official Name Local Government Area",
    "Latitude",
    "Longitude"
};
static const int num_headers = 10; // Adjust this according to the number of columns */


/* Prints the headers of the CSV file turned struct suburb 
    - Assumes num_headers, and headers are appropriate for suburb */
void print_headers(FILE* f) {
    for(int i = 0; i < num_headers; i++) {
        fprintf(f, "%s", headers[i]);
        if (i != num_headers - 1) {
            fprintf(f, ",");
        } else {
            fprintf(f, "\n");
        }
    }
}

/* Prints the suburb data without headers */
void print_suburb(FILE* f, void *suburb_void) {
    suburb_t *suburb = (suburb_t *)suburb_void;
    assert(suburb != NULL);
    for(int i = 0; i < num_headers; i++) {
        print_suburb_component_csv(f, suburb, i);
        if (i != num_headers - 1) {
            fprintf(f, ",");
        } else {
            fprintf(f, "\n");
        }
    }
}

/* Prints the suburb with its associated header infront 
    - Assumes num_headers, and headers are appropriate for suburb */
void print_with_headers(FILE* f, void *suburb_void) {
    suburb_t *suburb = (suburb_t *)suburb_void;
    assert(suburb != NULL);
        
    for(int i = 0; i < num_headers; i++) {
        fprintf(f, "%s: ", headers[i]);
        print_suburb_component(f, suburb, i);
        
        if (i != num_headers - 1) {
            fprintf(f, ", ");
        } else {
            fprintf(f, "\n");
        }
    }
}

/* Prints the suburb value from the struct based on its index (aka column)*/
void print_suburb_component(FILE* f, suburb_t *suburb, int index) {
    assert(suburb);
    switch (index) {
        case 0:
            fprintf(f, "%d", suburb->id);
            break;
        case 1:
            fprintf(f, "%d", suburb->suburb_id);
            break;
        case 2:
            fprintf(f, "%s", suburb->suburb_name);
            break;
        case 3:
            fprintf(f, "%d", suburb->year);
            break;
        case 4:
            fprintf(f, "%s", suburb->state_ids);
            break;
        case 5:
            fprintf(f, "%s", suburb->state_names);
            break;
        case 6:
            fprintf(f, "%s", suburb->local_gov_ids);
            break;
        case 7:
            fprintf(f, "%s", suburb->local_gov_names);
            break;
        case 8:
            fprintf(f, "%.7lf", suburb->y);
            break;
        case 9:
            fprintf(f, "%.7lf", suburb->x);
            break;
        default:
            fprintf(f, "Invalid index.");
            break;
    }
}

/* Prints the suburb value from the struct based on its index (aka column) for csv format */
void print_suburb_component_csv(FILE* f, suburb_t *suburb, int index) {
    assert(suburb);
    switch (index) {
        case 0:
            fprintf(f, "%d", suburb->id);
            break;
        case 1:
            fprintf(f, "%d", suburb->suburb_id);
            break;
        case 2:
            fprintf(f, "%s", suburb->suburb_name);
            break;
        case 3:
            fprintf(f, "%d", suburb->year);
            break;
        case 4:
            if (strchr(suburb->state_ids, ',') == NULL) { 
                fprintf(f, "%s", suburb->state_ids);
            } else {
                fprintf(f, "\"%s\"", suburb->state_ids);
            }
            break;
        case 5:
            if (strchr(suburb->state_names, ',') == NULL) { 
                fprintf(f, "%s", suburb->state_names);
            } else {
                fprintf(f, "\"%s\"", suburb->state_names);
            }
            break;
        case 6:
            if (strchr(suburb->local_gov_ids, ',') == NULL) { 
                fprintf(f, "%s", suburb->local_gov_ids);
            } else {
                fprintf(f, "\"%s\"", suburb->local_gov_ids);
            }
            break;
        case 7:
            if (strchr(suburb->local_gov_names, ',') == NULL) { 
                fprintf(f, "%s", suburb->local_gov_names);
            } else {
                fprintf(f, "\"%s\"", suburb->local_gov_names);
            }
            break;
        case 8:
            fprintf(f, "%.7lf", suburb->y);
            break;
        case 9:
            fprintf(f, "%.7lf", suburb->x);
            break;
        default:
            fprintf(f, "Invalid index.");
            break;
    }
}

/* Takes the first line of the csv and associates the headers to the headers 
struct */
void determine_headers(char *header_line) {
    num_headers = 0;
    char* header_line_cpy = strdup(header_line);
    // Remove newline character if it exists
    size_t len = strcspn(header_line_cpy, "\n");
    header_line_cpy[len] = '\0';

    char* token = strtok(header_line_cpy, ",");
    while (token && num_headers < MAX_HEADERS) {
        // Store the header
        headers[num_headers] = strdup(token);
        assert(headers[num_headers]);

        num_headers++;
        // Continue tokenising same string
        token = strtok(NULL, ","); 
    }

    free(header_line_cpy);
}

/* Compares two suburbs by their name and returns:
    - a negative value if key is less than a's name
    - a positive value if key is more than a's name
    - 0 if they are the same */
int cmpSuburbName(void* key, void *a) {
    // Assert that the pointers are valid
    assert(a != NULL);

    suburb_t *sub = (suburb_t *)a;

    return strcmp((char*)key, sub->suburb_name);
}

/* Frees the suburb */
void free_suburb(void *suburb) {
    assert(suburb != NULL);
    suburb_t *to_free = (suburb_t*)suburb;

    // Free dynamic data from suburb:
        // suburb_name, state_ids, state_names, local_gov_ids, and local_gov_names
    free(to_free->suburb_name);
    free(to_free->state_ids);
    free(to_free->state_names);
    free(to_free->local_gov_ids);
    free(to_free->local_gov_names);

    free(to_free);
}

/* CSV Row parse to Suburb data structure function
    - Assumes input is a csv row of the correct format
    -  any field containing a comma will begin with a double quotation mark (") 
    and end with a quotation mark ("), eg “this, and this,, ”,*/
suburb_t *parseRowToSuburb(char *row) {
    char* row_cpy = strdup(row);
    // Retrieve each field as a string
    char *fields[MAX_HEADERS];
    int field_count = 0;
    char *token = strtok(row_cpy, ",");

    while (token && field_count < MAX_HEADERS) {
        // Handle quoted fields (fields that contain commas)
        if (token[0] == '"') {
            token++; // Move to next char
            char *field = strdup(token);
            char *end = token + strlen(token) - 1;
            while ((token = strtok(NULL, ",")) && *end != '"' ) {
                // Check if the token ends with a closing quote
                end = token + strlen(token) - 1;
                if (*end == '"') {
                    // Remove the trailing quote
                    *end = '\0';  // Set the end of the string to null terminator
                    field = (char *)realloc(field, strlen(field) + strlen(token) + 2); // +2 for comma and null terminator
                    assert(field);
                    strcat(field, ",");
                    strcat(field, token);
                    token = strtok(NULL, ",");
                    break;
                }
                field = (char *)realloc(field, strlen(field) + strlen(token) + 2); // +2 for comma and null terminator
                assert(field);
                strcat(field, ",");
                strcat(field, token);
            }
            fields[field_count++] = field;
            continue;
        }
        fields[field_count++] = strdup(token);
        token = strtok(NULL, ",");
    }

    // Add components to suburb
    suburb_t *suburb = (suburb_t *)malloc(sizeof(suburb_t));
    assert(suburb);
    suburb->id = atoi(fields[0]);
    suburb->suburb_id = atoi(fields[1]);
    suburb->suburb_name = strdup(fields[2]);
    suburb->year = atoi(fields[3]);
    suburb->state_ids = strdup(fields[4]);
    suburb->state_names = strdup(fields[5]);
    suburb->local_gov_ids = strdup(fields[6]);
    suburb->local_gov_names = strdup(fields[7]);
    suburb->y = atof(fields[8]);
    suburb->x = atof(fields[9]);

    // Free allocated fields
    for (int i = 0; i < field_count; i++) {
        free(fields[i]);  // Free duplicated tokens
    }
    free(row_cpy);  // Free the copy of the original row

    return suburb;
}

/* Creates a deep copy of the suburb specific */
void *cpySuburb(void *s) {
    assert(s != NULL);

    // Cast the input void pointer to a Suburb pointer
    suburb_t *suburb = (suburb_t *)s;

    // Allocate memory for the new Suburb struct
    suburb_t *copy = malloc(sizeof(suburb_t));
    assert(copy != NULL);

    // Copy the simple data types (ints and doubles)
    copy->id = suburb->id;
    copy->suburb_id = suburb->suburb_id;
    copy->year = suburb->year;
    copy->y = suburb->y;
    copy->x = suburb->x;

    // Deep copy the strings
    if (suburb->suburb_name != NULL) {
        copy->suburb_name = strdup(suburb->suburb_name);
        assert(copy->suburb_name != NULL);
    } else {
        copy->suburb_name = NULL;
    }

    if (suburb->state_ids != NULL) {
        copy->state_ids = strdup(suburb->state_ids);
        assert(copy->state_ids != NULL);
    } else {
        copy->state_ids = NULL;
    }

    if (suburb->state_names != NULL) {
        copy->state_names = strdup(suburb->state_names);
        assert(copy->state_names != NULL);
    } else {
        copy->state_names = NULL;
    }

    if (suburb->local_gov_ids != NULL) {
        copy->local_gov_ids = strdup(suburb->local_gov_ids);
        assert(copy->local_gov_ids != NULL);
    } else {
        copy->local_gov_ids = NULL;
    }

    if (suburb->local_gov_names != NULL) {
        copy->local_gov_names = strdup(suburb->local_gov_names);
        assert(copy->local_gov_names != NULL);
    } else {
        copy->local_gov_names = NULL;
    }

    return copy;
}

/* Frees the dynamically determined static array of headers */
void free_headers(void) {
    for (int i = 0; i < num_headers; i++) {
        free(headers[i]);
    }
}


