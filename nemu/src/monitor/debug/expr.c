#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>

enum {
	NOTYPE = 256, 
	EQ,

	/* TODO: Add more token types */
    NEQ,
    AND,
    NUM,
    HEX,
    REG,
    NOT,
    NEG,
    DEREF
};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

 {" +", NOTYPE},

    {"0[xX][0-9a-fA-F]+", HEX},
    {"\\$[a-zA-Z]+", REG},
    {"[0-9]+", NUM},

    {"==", EQ},
    {"!=", NEQ},
    {"&&", AND},
	{"!", NOT},

    {"\\+", '+'},
    {"-", '-'},
    {"\\*", '*'},
    {"/", '/'},

    {"\\(", '('},
    {"\\)", ')'}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				
				switch(rules[i].token_type) {
				case NOTYPE:
					break;

				case NUM:
				case HEX:
				case REG:
				case EQ:
				case NEQ:
				case AND:
				case NOT:
				case NEG:
				case DEREF:
				case '+':
				case '-':
				case '*':
				case '/':
				case '(':
				case ')':
					tokens[nr_token].type = rules[i].token_type;

					if (substr_len >= sizeof(tokens[nr_token].str)) {
						printf("token too long\n");
						return false;
					}

					strncpy(tokens[nr_token].str, substr_start, substr_len);
					tokens[nr_token].str[substr_len] = '\0';

					nr_token++;

					if (tokens[nr_token - 1].type == '-' &&
						(nr_token == 1 ||
						tokens[nr_token - 2].type == '(' ||
						tokens[nr_token - 2].type == '+' ||
						tokens[nr_token - 2].type == '-' ||
						tokens[nr_token - 2].type == '*' ||
						tokens[nr_token - 2].type == '/' ||
						tokens[nr_token - 2].type == EQ ||
						tokens[nr_token - 2].type == NEQ ||
						tokens[nr_token - 2].type == AND ||
						tokens[nr_token - 2].type == NOT)) {
						tokens[nr_token - 1].type = NEG;
					}

					break;

				default:
					panic("please implement me");
			}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

static bool check_parentheses(int p, int q) {
    if (tokens[p].type != '(' || tokens[q].type != ')') {
        return false;
    }

    int balance = 0;
    int i;

    for (i = p; i <= q; i++) {
        if (tokens[i].type == '(') {
            balance++;
        } else if (tokens[i].type == ')') {
            balance--;
            if (balance == 0 && i < q) {
                return false;
            }
        }

        if (balance < 0) {
            return false;
        }
    }

    return balance == 0;
}

static uint32_t eval(int p, int q, bool *success) {
    // 只有一个 token
    if (p == q) {
        if (tokens[p].type == NUM) {
            return strtoul(tokens[p].str, NULL, 10);
        }

        if (tokens[p].type == HEX) {
            return strtoul(tokens[p].str, NULL, 16);
        }

        if (tokens[p].type == REG) {
            int i;

            for (i = R_EAX; i <= R_EDI; i++) {
                if (strcmp(tokens[p].str + 1, regsl[i]) == 0) {
                    return reg_l(i);
                }
            }

            *success = false;
            return 0;
        }

        *success = false;
        return 0;
    }

    // 整个表达式被一对括号包住
    if (check_parentheses(p, q)) {
        return eval(p + 1, q - 1, success);
    }

    // 找主运算符
    int op = -1;
    int balance = 0;
    int i;
	int lowest_prec = 100;

   for (i = p; i <= q; i++) {
    int prec = -1;

    if (tokens[i].type == '(') {
        balance++;
        continue;
    }

    if (tokens[i].type == ')') {
        balance--;
        continue;
    }

    if (balance != 0) {
        continue;
    }

    if (tokens[i].type == AND) {
        prec = 1;
    }
    else if (tokens[i].type == EQ ||
             tokens[i].type == NEQ) {
        prec = 2;
    }
    else if (tokens[i].type == '+' ||
             tokens[i].type == '-') {
        prec = 3;
    }
    else if (tokens[i].type == '*' ||
             tokens[i].type == '/') {
        prec = 4;
    }

    if (prec != -1 && prec <= lowest_prec) {
        lowest_prec = prec;
        op = i;
    }
}

if (op == -1) {
    if (tokens[p].type == NOT) {
        uint32_t val = eval(p + 1, q, success);

        if (!*success) {
            return 0;
        }

        return !val;
    }

    *success = false;
    return 0;
}

    uint32_t val1 = eval(p, op - 1, success);
    if (!*success) {
        return 0;
    }

    uint32_t val2 = eval(op + 1, q, success);
    if (!*success) {
        return 0;
    }
	if (tokens[op].type == AND) return (val1 != 0) && (val2 != 0);
	if (tokens[op].type == EQ) return val1 == val2;
	if (tokens[op].type == NEQ) return val1 != val2;
    if (tokens[op].type == '+') {
        return val1 + val2;
    }

    if (tokens[op].type == '-') {
        return val1 - val2;
    }

	if (tokens[op].type == '*') {
    return val1 * val2;
	}

	if (tokens[op].type == '/') {
		if (val2 == 0) {
			*success = false;
			return 0;
		}

		return val1 / val2;
	}

    *success = false;
    return 0;
}

uint32_t expr(char *e, bool *success) {
    if (!make_token(e)) {
        *success = false;
        return 0;
    }

    *success = true;

    if (nr_token == 0) {
        *success = false;
        return 0;
    }

    return eval(0, nr_token - 1, success);
}

