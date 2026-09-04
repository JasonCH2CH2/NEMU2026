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
	TK_OR     // 代表逻辑或 ||
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
                                                if (rules[i].token_type == TK_NUM) {
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

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	panic("please implement me");
	return 0;
}

