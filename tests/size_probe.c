/* ARM object symbol sizes expose target ABI struct sizes, without executing device code. */
#include "ui.h"
#include "math_layout.h"
char probe_app[sizeof(App)];
char probe_arena[sizeof(MathArena)];
char probe_interaction[sizeof(MathInteraction)];
