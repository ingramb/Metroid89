// C Source File
// Created 7/12/02; 1:48:07 AM

#define USE_TI89              // Produce .89z File

#define RETURN_VALUE          // Return Pushed Expression

#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

// #define SAVE_SCREEN        // Save/Restore LCD Contents

#include <tigcclib.h>         // Include All Header Files

#define OP_TOTAL 4

long process_dice(char *dice)
{
	register long i, j;
	short length = strlen(dice);
	short first_loc = 0;
	short token_number = 0;
	short operator_number = 0;
	short *token_start;
	short *token_end;
	long *token_values;
	char *operators;
	long *new_values;
	char *new_operators;
	long *temp1;
	char *temp2;
	char token[30];
	short current_operator;
	long total;
	
	void *kbq = kbd_queue ();
	unsigned short key;


	
	token_start = malloc(length * 2 * sizeof(short) + length * 2 * sizeof(long) + length * 2 * sizeof(char));
	token_end = token_start + length;
	token_values = (long *)(token_end + length);
	new_values = token_values + length;
	
	operators = (char*)(new_values + length);
	new_operators = operators + length;
	
	for(i = 0 ; i < length ; i++) {
		switch(*(dice + i)) {
			case '+':
			case '-':
			case '*':
			case 'd':
			case 'D':
				token_start[token_number] = first_loc;
				token_end[token_number] = i;
				operators[token_number] = *(dice + i);
				first_loc = i + 1;
				token_number++;
				operator_number++;
				break;
		}
	}
	
	token_start[token_number] = first_loc;
	token_end[token_number] = i;
	operators[token_number] = 0;
	token_number++;
	
	if(operator_number == token_number) return 0;
	
	for(i = 0 ; i < token_number ; i++) {
		for(j = token_start[i] ; j < token_end[i] ; j++)
			token[j - token_start[i]] = *(dice + j);
		token[token_end[i] - token_start[i]] = 0;
		token_values[i] = atol(token);
	}
	
	current_operator = 0;
	
	while(token_number > 1) {
		for(i = 0 ; i < token_number ; i++) {
			if((current_operator == 0 && operators[i] == 'd') ||
				 (current_operator == 1 && operators[i] == '*') ||
				 (current_operator == 2 && (operators[i] == '+' || operators[i] == '-'))) break;
		}
		
		if(i == token_number) {
			current_operator++;
			continue;
		}
		
		for(j = 0 ; j < i ; j++) {
			new_values[j] = token_values[j];
			new_operators[j] = operators[j];
		}
		
		switch(operators[i]) {
			case '+':
				new_values[i] = token_values[i] + token_values[i + 1];
				break;
			case '-':
				new_values[i] = token_values[i] - token_values[i + 1];
				break;
			case '*':
				new_values[i] = token_values[i] * token_values[i + 1];
				break;
			case 'd':
				total = 0;
				for(j = 0 ; j < token_values[i] ; j++) {
					total += random(token_values[i + 1]) + 1;
					if(!OSdequeue (&key, kbq)) return 0;
				}
				new_values[i] = total;
				break;
		}
		
		for(j = i + 1; j < token_number ; j++)
			new_operators[j - 1] = operators[j];
			
		for(j = i + 2; j < token_number ; j++)
			new_values[j - 1] = token_values[j];
			
		token_number--;
		
		temp1 = token_values;
		temp2 = operators;
		
		token_values = new_values;
		operators = new_operators;
		
		new_values = temp1;
		new_operators = temp2;
	}
	
	total = token_values[0];
	
	free(token_start);
	
	return total;
}
		
				


void error(const char *msg)
{
	while(GetArgType(top_estack) != END_TAG)
		top_estack = next_expression_index(top_estack);
	
	top_estack--;
	push_ANSI_string((char*)msg);
}

// Main Function
void _main(void)
{
	ESI argptr;
  short arg_number;
  short arg_type;
  long result;
  char *dice;
  
	arg_number = ArgCount();
	
	if(arg_number != 1) {
		error("ERROR");
		return;
	}
  
  InitArgPtr(argptr);
  
  arg_type = GetArgType(argptr);
  
  if(arg_type != STR_TAG) {
  	error("ERROR");
		return;
	}
	
	dice = GetStrnArg(argptr);
	
	GKeyFlush();
	
	result = process_dice(dice);
	
	while(GetArgType(top_estack) != END_TAG)
		top_estack = next_expression_index(top_estack);
	
	top_estack--;
	push_longint((long)result);
}
