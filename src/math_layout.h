#ifndef C21_MATH_LAYOUT_H
#define C21_MATH_LAYOUT_H
#include "engine.h"
/* Layout only: no expression evaluation, simplification or symbolic solving. */
typedef enum { M_TEXT,M_ROW,M_FRACTION,M_ROOT,M_SCRIPT,M_PARENS,M_VARIABLE,M_STACK } MathKind;
typedef enum { M_CONSTANT,M_INPUT,M_OUTPUT } MathRole;
typedef struct MathNode {
 MathKind kind; const char *text; const struct MathNode *a,*b,*c; int gap;
 VariableId variable; MathRole role;
} MathNode;
typedef struct { int width,above,below; } MathBox;
#define MATH_CAPACITY 96
typedef struct { MathNode nodes[MATH_CAPACITY]; unsigned used; int overflow,interactive,formula_id; } MathArena;
typedef struct { int x,y,width,height; VariableId variable; MathRole role; } MathFieldRect;
#define MATH_FIELD_RECTS 16
typedef struct { MathFieldRect fields[MATH_FIELD_RECTS]; unsigned count; VariableId active; } MathInteraction;
typedef enum { FORMULA_Z=CALC_COUNT,FORMULA_DELTA_V,FORMULA_COUNT } FormulaId;
void math_arena_init(MathArena *arena);
const MathNode *math_text(MathArena *arena,const char *utf8);
const MathNode *math_stack(MathArena *arena,const MathNode *top,const MathNode *bottom,int gap);
const MathNode *math_row(MathArena *arena,const MathNode *a,const MathNode *b,int gap);
const MathNode *math_fraction(MathArena *arena,const MathNode *numerator,const MathNode *denominator);
const MathNode *math_root(MathArena *arena,const MathNode *radicand);
const MathNode *math_script(MathArena *arena,const MathNode *base,const MathNode *sup,const MathNode *sub);
const MathNode *math_parens(MathArena *arena,const MathNode *inside);
MathBox math_measure(const MathNode *node);
void math_draw(const MathNode *node,int x,int baseline,int ink);
const MathNode *formula_build(MathArena *arena,int formula_id);
const MathNode *formula_build_interactive(MathArena *arena,CalcId id);
int formula_interactive(CalcId id,VariableId active,int x,int y,int width,int height,MathInteraction *interaction);
int formula_interactive_fits(CalcId id,int width,int height);
const MathNode *formula_build_values(MathArena *arena,int formula_id,const char *const *values);
/* Center a measured expression in a viewport without scaling the bitmap font. */
int formula_draw(int formula_id,int x,int y,int width,int height);
int formula_draw_primary(int formula_id,int x,int y,int width,int height);
int formula_draw_values(int formula_id,const double *si_values,int x,int y,int width,int height);
#endif
