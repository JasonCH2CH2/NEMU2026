#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ,

	/* TODO: Add more token types */
	TK_NUM,   // 代表数字
  	TK_NEQ,   // 代表不等于 !=
 	TK_AND,   // 代表逻辑与 &&
	TK_OR,     // 代表逻辑或 ||
	TK_HEX,  
  	TK_REG
};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				// spaces
	{"\\+", '+'},					// plus
	{"==", EQ},						// equal
	{"!=", TK_NEQ},       // 不等于
  	{"&&", TK_AND},       // 逻辑与
  	{"\\|\\|", TK_OR},    // 逻辑或
  	{"\\-", '-'},         // 减号
  	{"\\*", '*'},         // 乘号
  	{"/", '/'},           // 除号
  	{"\\(", '('},         // 左括号
  	{"\\)", ')'},         // 右括号
	{"0x[0-9a-fA-F]+", TK_HEX}, //任务5
  	{"\\$[a-zA-Z]+", TK_REG},//十六进制在十进制前面
  	{"[0-9]+", TK_NUM}    // 十进制数字 (0到9的连续组合)
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
                                                // 如果是空格直接跳过
                                                break;
                                        default:
                                                // 记录下这个符号的类型
                                                tokens[nr_token].type = rules[i].token_type;
                                                
                                                // 把数字的字符串记下来
                                                if (rules[i].token_type == TK_NUM|| rules[i].token_type == TK_HEX) {
                                                        // 限制长度
                                                        int len = substr_len < 32 ? substr_len : 31;
                                                        strncpy(tokens[nr_token].str, substr_start, len);
                                                        tokens[nr_token].str[len] = '\0';
                                                }
                                                // 记好一个之后，把计数器加 1，准备记下一个
                                                nr_token++;
                                                break;
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
// 1. 检查括号是否匹配的函数
static bool check_parentheses(int p, int q) {
    if (tokens[p].type != '(' || tokens[q].type != ')') return false;
    int count = 0;
    for (int i = p; i <= q; i++) {
        if (tokens[i].type == '(') count++;
        else if (tokens[i].type == ')') count--;
        if (count == 0 && i < q) return false; 
    }
    return count == 0;
}

// 2. 寻找主运算符（最后才运算的那个符号）的函数
static int dominant_operator(int p, int q) {
    int op = -1;
    int level = 0;
    int min_prec = 100; // 记录最低优先级

    for (int i = p; i <= q; i++) {
        if (tokens[i].type == '(') level++;
        else if (tokens[i].type == ')') level--;
        else if (level == 0) { // 只看括号外面的运算符
            int prec = 0;
            if (tokens[i].type == TK_OR) prec = 1;
            else if (tokens[i].type == TK_AND) prec = 2;
            else if (tokens[i].type == EQ || tokens[i].type == TK_NEQ) prec = 3;
            else if (tokens[i].type == '+' || tokens[i].type == '-') prec = 4;
            else if (tokens[i].type == '*' || tokens[i].type == '/') prec = 5;
            else continue; // 不是运算符

            // 优先级越小，说明越要在最后算。遇到同级的，选最右边的
            if (prec <= min_prec) {
                min_prec = prec;
                op = i;
            }
        }
    }
    return op;
}

// 3. 递归求值的核心函数
static uint32_t eval(int p, int q) {
    if (p > q) {
        return 0; // 表达式有问题
    }
    else if (p == q) {
	uint32_t val = 0;
        if (tokens[p].type == TK_NUM) {
            sscanf(tokens[p].str, "%d", &val);
        } else if (tokens[p].type == TK_HEX) {
            sscanf(tokens[p].str, "%x", &val);
        } else if (tokens[p].type == TK_REG) {
            if (strcmp(tokens[p].str, "$eax") == 0) val = cpu.eax;
            else if (strcmp(tokens[p].str, "$ecx") == 0) val = cpu.ecx;
            else if (strcmp(tokens[p].str, "$edx") == 0) val = cpu.edx;
            else if (strcmp(tokens[p].str, "$ebx") == 0) val = cpu.ebx;
            else if (strcmp(tokens[p].str, "$esp") == 0) val = cpu.esp;
            else if (strcmp(tokens[p].str, "$ebp") == 0) val = cpu.ebp;
            else if (strcmp(tokens[p].str, "$esi") == 0) val = cpu.esi;
            else if (strcmp(tokens[p].str, "$edi") == 0) val = cpu.edi;
            else if (strcmp(tokens[p].str, "$eip") == 0) val = cpu.eip;
        }
        return val;
    }
    else if (check_parentheses(p, q) == true) {
        // 被一对括号包围，把皮剥掉，算里面的
        return eval(p + 1, q - 1);
    }
    else {
        // 找到主运算符的位置
        int op = dominant_operator(p, q);
        // 递归算左边和右边
        uint32_t val1 = eval(p, op - 1);
        uint32_t val2 = eval(op + 1, q);

        switch (tokens[op].type) {
            case '+': return val1 + val2;
            case '-': return val1 - val2;
            case '*': return val1 * val2;
            case '/': return val1 / val2;
            case EQ: return val1 == val2;
            case TK_NEQ: return val1 != val2;
            case TK_AND: return val1 && val2;
            case TK_OR: return val1 || val2;
            default: return 0;
        }
    }
}
uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
    	*success = true;
    	return eval(0, nr_token - 1);
}

