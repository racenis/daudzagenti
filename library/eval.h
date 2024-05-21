// EVAL.H - libgoetia
// Basic script evaluation.

#ifndef EVAL_H
#define EVAL_H

#include <list.h>
#include <context.h>

Value goetia_script_Evaluate(Context* context, List* expression);
Value goetia_script_EvaluateValue(Context* context, Value value);

Value goetia_script_error(const char* msg, ...);
Value goetia_script_error2(List* list, const char* msg, ...);


#endif // EVAL_H