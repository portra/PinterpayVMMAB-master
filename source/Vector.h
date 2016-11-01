#include "appLib.h"
#define VECTOR_INITIAL_CAPACITY 1

// Define a vector type
typedef struct {
  int size;      // slots used so far
  int capacity;  // total available slots
  MenuItem *data;     // array of integers we're storing
}VECTOR;

 void vector_init(VECTOR *vector);

 void vector_append(VECTOR *vector, MenuItem value);

 MenuItem vector_get(VECTOR *vector, int index);

 MenuItem * vector_get_array(VECTOR *vector);


 void vector_set(VECTOR *vector, int index, MenuItem value);

 void vector_add_capacity_if_full(VECTOR *vector);

 void vector_free(VECTOR *vector);

