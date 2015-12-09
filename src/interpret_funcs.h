#ifndef __INTERPRET_FUNCS_H_INCLUDED
#define __INTERPRET_FUNCS_H_INCLUDED

#include "stable.h"

/**
  * @brief Interprets call of 'cin' function
  * @details Reads one integer/double/word from
  *          standard input and saves it into
  *          passed variable structure.
  *
  * @param var Pointer to variable structure
*/
void interpret_cin(stable_variable_t *var);

/**
  * @brief Interprets call of 'cout' function
  * @detail Prints value from variable structure
  *         in appropriate format.
  *
  * @param var Pointer to variable structure
*/
void interpret_cout(stable_variable_t *var);

#endif
