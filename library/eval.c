// EVAL.C - libgoetia
// Basic script evaluation implementation.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include <eval.h>

#include <script.h>

#include <assert.h>

#define bind 	goetia_script_BindValue
#define unbind 	goetia_script_UnbindValue

void print_context(Context* c, List* l) {
	printf("CONTEXT: %s %i\n", l->file ? goetia_GetListFilename(l->file) : "idk", l->line);
	for (int i = 0; i < c->value_count; i++) {
		printf("name: %s value: ", goetia_atom_ToString(c->values[i].name));
		goetia_value_Print(c->values[i].value);
		printf("\n");
	}
	printf("\n");
}

int find_bind(Context* c, atom atom){
	for (int i = 0; i < c->value_count; i++) {
		if (c->values[i].name == atom) {
			return i;
		}
	}
	
	return -1;
}

Value eval(Context* context, Value expr);
Value apply(Context* context, Value func, List* expr);

/// Prints error message and returns error value.
Value goetia_script_error(const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	vprintf(msg, args);
	va_end(args);
	
	Value val;
	
	val.type = VALUE_ATOM;
	val.atom = ATOM_ERROR;
	
	return val;
}

/// Same as goetia_script_error(const char*, ...), but also prints file and line.
Value goetia_script_error2(List* list, const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	vprintf(msg, args);
	va_end(args);

	if (list->file) {
		printf("Occured on line %i in file %s\n", 
			list->line, goetia_GetListFilename(list->file));
	}
	
	return goetia_value_MakeAtom(ATOM_ERROR);
}

// temporary; factor out
static Value error(const char* msg, ...) {
	va_list args;
	va_start(args, msg);
	vprintf(msg, args);
	va_end(args);
	
	Value val;
	
	val.type = VALUE_ATOM;
	val.atom = ATOM_ERROR;
	
	return val;
}

/// Strips listiness from a value.
static Value unlist(Context* c, List* element) {
	if (!element) {
		return error("Unlist subrotutine got poutine!\n");
	} else {
		return eval(c, element->data);
	}
}
	
// -1 lt, 0 eq, 1 gt, -2 error
static int compare(Value v1, Value v2) {	
	float num1 = NAN;
	float num2 = NAN;
	
	//printf("\n%f %f\n", num1, num2);
	
	if (v1.type == VALUE_INT) num1 = v1.int32;
	if (v2.type == VALUE_INT) num2 = v2.int32;
	if (v1.type == VALUE_FLOAT) num1 = v1.float32;
	if (v2.type == VALUE_FLOAT) num2 = v2.float32;

	//printf("%f %f\n", num1, num2);
	//printf("%i %i %i", num1 != num1, num2 != num2, num1 != num1 || num2 != num2);
	
	if (num1 != num1 || num2 != num2) return -2;
	
	if (num1 == num2) return 0;
	
	if (num1 < num2) return -1;
	if (num1 > num2) return 1;
}

static const Value ok = {ATOM_OK, VALUE_ATOM};
static const Value true = {ATOM_TRUE, VALUE_ATOM};
static const Value false = {ATOM_FALSE, VALUE_ATOM};

/* ************************************************************************** */
/*                                                                            */
/*                                 ARITHMETIC                                 */
/*                                                                            */
/* ************************************************************************** */

static Value addition(Context* c, List* expr) {
	//printf("in addition\n");
	
	//printf("value: %i\n", expr->data.int32);
	
	float sum = 0.0f;
	do {
		//printf("unlisting\n");
		Value val = unlist(c, expr);
		
		//printf("summing\n");
		if (val.type == VALUE_INT) {
			sum += val.int32;
		} else if (val.type == VALUE_FLOAT) {
			sum += val.float32;
		} else {
			return error("Can't use %s type in addition!\n", 
				goetia_value_TypeToString(val.type)
			);
		}
		
	} while (expr = expr->next);

	return goetia_value_MakeNumeric(sum);
}

static Value subtraction(Context* c, List* expr) {
	if (!expr) return error("Nothing to subtract!");
	
	Value val = unlist(c, expr);
	
	float dif;
	
	if (val.type == VALUE_INT) {
		dif = val.int32;
	} else if (val.type == VALUE_FLOAT) {
		dif = val.float32;
	} else {
		return error("Can't use %s type in subtraction!\n", 
			goetia_value_TypeToString(val.type)
		);
	}
	
	expr = expr->next;
	
	if (!expr) return goetia_value_MakeNumeric(-dif);
	
	do {
		Value val = unlist(c, expr);
		
		if (val.type == VALUE_INT) {
			dif -= val.int32;
		} else if (val.type == VALUE_FLOAT) {
			dif -= val.float32;
		} else {
			return error("Can't use %s type in subtraction!\n", 
				goetia_value_TypeToString(val.type)
			);
		}
		
	} while (expr = expr->next);

	return goetia_value_MakeNumeric(dif);
}

static Value multiplication(Context* c, List* expr) {
	//printf("mutliply list: ");
	//goetia_PrintList(expr);
	//printf("\n");
	
	//print_context(c);
	
	float prd = 1.0f;
	do {
		
		//printf("param uneval: "); goetia_value_Print(expr->data); printf("\n");
		
		Value val = unlist(c, expr);
		
		//printf("param evaled: "); goetia_value_Print(val); printf("\n");
		
		if (val.type == VALUE_INT) {
			prd *= val.int32;
		} else if (val.type == VALUE_FLOAT) {
			prd *= val.float32;
		} else {
			return error("Can't use %s type in multiplication!\n", 
				goetia_value_TypeToString(val.type)
			);
		}
		
	} while (expr = expr->next);

	return goetia_value_MakeNumeric(prd);
}

static Value division(Context* c, List* expr) {
	if (!expr) return error("Nothing to divide!");
	
	Value val = unlist(c, expr);
	
	float div;
	
	if (val.type == VALUE_INT) {
		div = val.int32;
	} else if (val.type == VALUE_FLOAT) {
		div = val.float32;
	} else {
		return error("Can't use %s type in division!\n", 
			goetia_value_TypeToString(val.type)
		);
	}
	
	expr = expr->next;
	
	if (!expr) return goetia_value_MakeNumeric(1.0f / div);
	
	do {
		Value val = unlist(c, expr);
		
		if (val.type == VALUE_INT) {
			div /= val.int32;
		} else if (val.type == VALUE_FLOAT) {
			div /= val.float32;
		} else {
			return error("Can't use %s type in subtraction!\n", 
				goetia_value_TypeToString(val.type)
			);
		}
		
	} while (expr = expr->next);

	return goetia_value_MakeNumeric(div);
}

// -1 <, 0 ==, 1 >
static Value arithmetic_compare(Context* c, List* expr, int sign) {
	Value prev = unlist(c, expr);
	
	while (expr = expr->next) {
		Value next = unlist(c, expr);
		
		int comp = compare(prev, next);
		
		if (comp == -2) return error("Can't compare %s and %s!",
			goetia_value_TypeToString(prev.type),
			goetia_value_TypeToString(next.type)
		);
		
		if (comp != sign) return false;
		
		prev = next;
	}
	
	return true;
}

// -1 not, 0 or, 1 and
static Value logic_operation(Context* c, List* expr, int op) {
	do {
		Value next_val = unlist(c, expr);
		if (!goetia_value_Compare(next_val, true) &&
			!goetia_value_Compare(next_val, false)
		) error("Invalid logic expr!\n");
	
		int next = goetia_value_Compare(next_val, true);
		
		if (op == -1) {
			if (next) {
				return false;
			} else {
				return true;
			}
		}
		
		if (op == 0 && next) return true;
		if (op == 1 && !next) return false;
	} while (expr = expr->next);
	
	if (op == -1) error("Mothing to not!\n");
	if (op == 0) return false;
	if (op == 1) return true;
}

/* ************************************************************************** */
/*                                                                            */
/*                            OTHER LANGUAGE STUFF                            */
/*                                                                            */
/* ************************************************************************** */

static Value print(Context* c, List* expr) {
	do {
		Value val = unlist(c, expr);
		
		if (val.type == VALUE_STRING) {
			printf("%s ", val.str);
		} else {
			goetia_value_Print(val);
			printf(" ");
		}
	} while (expr = expr->next);
	
	printf("\n");
	
	return ok;
}

static Value if_expr(Context* c, List* expr) {
	Value logic_val = unlist(c, expr);
	if (!goetia_value_Compare(logic_val, true) &&
		!goetia_value_Compare(logic_val, false)
	) error("Invalid logic expr!\n");
	
	if(!(expr = expr->next)) return error("Missing if statment \n");
	
	if (goetia_value_Compare(logic_val, true)) {
		return unlist(c, expr);
	}

	if (expr->next) {
		return unlist(c, expr->next);
	} else {
		return ok;
	}
}

static Value set(Context* c, List* expr) {
	Value set_var = expr->data;
	
	if (set_var.type != VALUE_ATOM) return error("Can only set atoms!\n");
	
	expr = expr->next;
	
	Value set_val = unlist(c, expr);
	
	if (set_val.type == VALUE_ATOM && set_val.atom == ATOM_NULL) {
		unbind(c, set_var.atom);
	} else {
		bind(c, set_var.atom, set_val);
	}
	
	return ok;
}

static Value quote(Context* c, List* expr) {
	return expr->data;
}

static Value list(Context* c, List* expr) {	
	if (expr->data.type == VALUE_ATOM) switch (expr->data.atom) {
		case ATOM_HEAD:
			Value list = eval(c, expr->next->data);
			assert(list.type == VALUE_LIST);
			return list.list->data;
		case ATOM_TAIL:
			/*Value*/ list = eval(c, expr->next->data);
			assert(list.type == VALUE_LIST);
		
			if (!list.list->next) {
				return goetia_value_MakeAtom(ATOM_NULL);
			} else {
				Value val;
				val.type = VALUE_LIST;
				val.list = list.list->next;
				return val;
			}
		case ATOM_SIZE:
			/*Value*/ list = eval(c, expr->next->data);
			assert(list.type == VALUE_LIST);
		
			int count = 0;
			for (List* l = list.list; l; l = l->next) {
				count++;
			}
			
			return goetia_value_MakeInt(count);
		case ATOM_DROP:
			/*Value*/ list = eval(c, expr->next->data);
			assert(list.type == VALUE_LIST);
		
			List* before_last = list.list;
		
			while (before_last->next->next) before_last = before_last->next;
			
			before_last->next = NULL;
			
			return list;
		case ATOM_PUSH:
			/*Value*/ list = eval(c, expr->next->data);
			assert(list.type == VALUE_LIST);
			
			Value val = eval(c, expr->next->next->data);
			
			List* new_elem = goetia_NewListElement();
			
			new_elem->next = list.list;
			new_elem->data = val;
			
			Value new_list;
			new_list.type = VALUE_LIST;
			new_list.list = new_elem;
			return new_list;
		case ATOM_POP:
			/*Value*/ list = eval(c, expr->next->data);
			assert(list.type == VALUE_LIST);
		
			/*List**/ before_last = list.list;
		
			while (before_last->next->next) before_last = before_last->next;
			
			List* last = before_last->next;
			
			before_last->next = NULL;
			
			return last->data;
		case ATOM_LAST:
			/*Value*/ list = eval(c, expr->next->data);
			assert(list.type == VALUE_LIST);
		
			/*List**/ last = list.list;
		
			while (last->next) last = last->next;

			return last->data;
		default:
			break;
	}
	
	
	List* new_list = goetia_NewListElement();
	
	// TODO: add error checking
	
	for (List* elem = new_list, *left = expr; left; left = left->next) {
		if (left != expr) {
			List* new_elem = goetia_NewListElement();
			elem->next = new_elem;
			elem = new_elem;
		}
		
		elem->data = unlist(c, left);
	}
	
	Value val;
	val.type = VALUE_LIST;
	val.list = new_list;
	
	return val;
}

static Value eval_func(Context* c, List* expr) {
	assert(expr);
	
	Value val = eval(c, expr->data);
	
	
	//return val;
	return eval(c, val);
	/*
	Value val = unlist(c, expr);
	
	
	
	if (val.type != VALUE_LIST) return error("can only eval list");
	
	return goetia_script_Evaluate(c, val.list);*/
}

// idk if we even need 'apply' in this language
static Value apply_func(Context* c, List* expr) {
	Value func = unlist(c, expr);
	Value list = unlist(c, expr->next);
	
	//if (func.type != VALUE_ATOM) return error("Can only apply atom!\n");
	if (list.type != VALUE_LIST) return error("can only apply list");
	
	return apply(c, func, list.list);
}

static Value eval_list(Context* c, List* expr) {
	while (expr) {
		Value val = eval(c, expr->data);
		
		if (val.type == VALUE_ATOM && val.atom == ATOM_ERROR) {
			return val;
		}
		
		/*if (val.type != ATOM_LIST && val.atom != ATOM_OK) {
			return val;
		}*/
		
		expr = expr->next;
	}
	
	return ok;
}

static Value pattern(Context* c, List* expr) {
	// TODO: error checking
	
	List* patrn = expr->data.list;
	List* value = expr->next->data.list;
	
	do {
		bind(c, patrn->data.atom, value->data);
	} while ((patrn = patrn->next) && (value = value->next));
	
	return ok;
}

static Value is(Context* c, List* expr) {
	Value val1 = unlist(c, expr);
	Value val2 = unlist(c, expr->next);
	
	if (val1.type == VALUE_ATOM && val2.type == VALUE_ATOM) {
		//printf("comparing! ");
		//goetia_value_Print(val1);
		//goetia_value_Print(val2);
		//printf("\n");
		
		return val1.atom == val2.atom ? true : false;
	} else {
		return error("Can't 'is' type %s and %s.\n",
			goetia_value_TypeToString(val1.type),
			goetia_value_TypeToString(val2.type)
		);
	}
}

/* ************************************************************************** */
/*                                                                            */
/*                                    EVAL                                    */
/*                                                                            */
/* ************************************************************************** */

Value apply(Context* context, Value func, List* expr) {
	// need to function eval in case something is bound to it
	func = eval(context, func);
	
	// otherwise we assume that it is a built-in function
	if (func.type != VALUE_ATOM) return error("Can only apply atom!\n");
	
	switch (func.atom) {
		case ATOM_ADD:		return addition(context, expr);
		case ATOM_SUB:		return subtraction(context, expr);
		case ATOM_MULT:		return multiplication(context, expr);
		case ATOM_DIV:		return division(context, expr);
		case ATOM_EQ:		return arithmetic_compare(context, expr, 0);
		case ATOM_GT:		return arithmetic_compare(context, expr, 1);
		case ATOM_LT:		return arithmetic_compare(context, expr, -1);
		case ATOM_AND:		return logic_operation(context, expr, 1);
		case ATOM_OR:		return logic_operation(context, expr, 0);
		case ATOM_NOT:		return logic_operation(context, expr, -1);
		case ATOM_PRINT:	return print(context, expr);
		case ATOM_IF:		return if_expr(context, expr);
		case ATOM_SET:		return set(context, expr);
		case ATOM_QUOTE:	return quote(context, expr);
		case ATOM_LIST:		return list(context, expr);
		case ATOM_EVAL:		return eval_func(context, expr);
		case ATOM_APPLY:	return apply_func(context, expr);
		case ATOM_CONTEXT:	print_context(context, expr); return ok; 
		case ATOM_DO:		return eval_list(context, expr);
		case ATOM_PATTERN:	return pattern(context, expr);
		case ATOM_IS:		return is(context, expr);
		
		default:		break;
	}
	
	// this file is getting kinda long, let's continue in another one
	return goetia_script_SimulationFunction(context, func.atom, expr);
}

Value eval(Context* context, Value expr) {
	// if evaluation an atom, first check if it is bound to something
	if (expr.type == VALUE_ATOM) {
		int bind = find_bind(context, expr.atom);
		
		if (bind == -1) {
			return expr;
		} else {
			return context->values[bind].value;
		}
	}
	
	// otherwise, if not list, then return expression as-is
	if (expr.type != VALUE_LIST) return expr;
	
	// if list has only a single element, then evaluate just that element
	// I don't remember why?
	if (expr.list->next == NULL) return eval(context, expr.list->data);
	
	// find the function
	Value func = eval(context, expr.list->data);
	
	// apply function to its parameters
	return apply(context, func, expr.list->next);
}

/// Evaluates a script.
Value goetia_script_Evaluate(Context* context, List* expr) {
	Value val;
	val.type = VALUE_LIST;
	val.list = expr;
	
	return eval(context, val);
}

/// Evaluates a value.
Value goetia_script_EvaluateValue(Context* context, Value val) {
	return eval(context, val);
}