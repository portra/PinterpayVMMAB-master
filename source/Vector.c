#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "global.h"

void vector_init(VECTOR *vector) {
  // initialize size and capacity
  vector->size = 0;
  vector->capacity = VECTOR_INITIAL_CAPACITY;

  // allocate memory for vector->data
  vector->data = malloc(sizeof(int) * vector->capacity);
}

void vector_append(VECTOR *vector, MenuItem value) {
  // make sure there's room to expand into
  vector_add_capacity_if_full(vector);

  // append the value and increment vector->size
  vector->data[vector->size++] = value;
}

MenuItem vector_get(VECTOR *vector, int index) {
  if (index >= vector->size || index < 0) {
    printf("Index %d out of bounds for vector of size %d\n", index, vector->size);
    exit(1);
  }
  return vector->data[index];
}

MenuItem * vector_get_array(VECTOR *vector) {
  MenuItem * ret=&vector->data[counterPackage];
  memcpy(ret, &vector->data[0], counterPackage*sizeof(*vector->data));  
  return ret;
}


void vector_set(VECTOR *vector, int index, MenuItem value) {
  // zero fill the vector up to the desired index
  while (index >= vector->size) {
  	MenuItem itm;
    vector_append(vector, itm);
  }

  // set the value at the desired index
  vector->data[index] = value;
}

void vector_add_capacity_if_full(VECTOR *vector) {
  if (vector->size >= vector->capacity) {
    // double vector->capacity and resize the allocated memory accordingly
    vector->capacity += 1;
    vector->data = realloc(vector->data, sizeof(int) * vector->capacity);
  }
}

void vector_free(VECTOR *vector) {
  free(vector->data);
}
