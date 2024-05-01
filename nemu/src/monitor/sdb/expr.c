#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 *
 */
#include <regex.h>

enum {
  TK_NOTYPE,TK_LEFT_BRACKET, TK_RIGHT_BRACKET, TK_DECIMAL_INT, TK_HEX_INT,
  TK_REG = 32, TK_MUL, TK_DIVI = 128, TK_EQ, TK_NON_EQ, TK_AND

  /* TODO: Add more token types */

};  

static struct rule { 
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},						// space
  {"\\(", TK_LEFT_BRACKET},					//left bracket
  {"\\)", TK_RIGHT_BRACKET},				//right bracket
  {"0x[0-9a-f]+", TK_HEX_INT},				//
  {"[0-9]+", TK_DECIMAL_INT},				//decimal integer
												
  {"\\$[a-z]+[0-9]*", TK_REG},
//  {"0x", TK_HEX},
  {"\\+", '+'},								// plus
  {"-", '-'},								//minus
  {"\\*", TK_MUL},							//multiply
  {"/", TK_DIVI},							//divide

  {"==", TK_EQ},							//equal
  {"!=", TK_NON_EQ},							//non_equal
  {"&&", TK_AND},							//and
											//

};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token { 
  int type;
  char str[32];
  uint32_t level;
} Token;
#define TOK_NUM 512
static Token tokens[TOK_NUM] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;



static bool make_token(char *e) {
  int position = 0;
  int i = 0;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if ((regexec(&re[i], e + position, 1, &pmatch, 0) == 0) && (pmatch.rm_so == 0)) {
		//int regexec(const regex_t *preg, const char *string, size_t nmatch
		//		,regmatch_t pmatch[], int eflags);
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        //Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
         //   i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
		if (nr_token > sizeof(tokens) / sizeof(tokens[0])){
			printf("The length of tokens is %ld, and the  exp has out of the tokens", sizeof(tokens) / sizeof(tokens[0]));
			return false;
		}

		if (rules[i].token_type != TK_NOTYPE){
			tokens[nr_token].type = rules[i].token_type;
			if (substr_len < 32){
				strncpy(tokens[nr_token].str, substr_start, substr_len);
			}else{
				strncpy(tokens[nr_token].str, substr_start, 31);
				tokens[nr_token].str[31] = '\0';
				printf("The token <%s> is too long\n", tokens[nr_token].str);
				return false;
			}
			tokens[nr_token].str[substr_len] = '\0';
			nr_token++;
		}
		break;
    	} 
		}
	

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
		for (int i = 0; i < nr_token; i++ ){
			printf("type:%d, str:%s\n", tokens[i].type, tokens[i].str);
		}
	  return false;
	}
  }
  //printf("That has matched %d tokens\n", nr_token);
 /* for (int i = 0; i < nr_token; i++ ){
	printf("%s ", tokens[i].str);
  }
  printf("\n");*/
  return true;
}


bool judge_parentheses(){ 
	int flag = 0;
	int op_type;
	for (int i = 0;i < nr_token;i ++){
		op_type = tokens[i].type;
		switch (op_type){
			case TK_LEFT_BRACKET:
				flag++;
				break;
			case TK_RIGHT_BRACKET:
				flag--;
				break;
			default:
				break;
		}
		if (flag < 0){
//			printf("The pattern of expression is invalid.\n");
			return 1;
		}
	}
	if (flag != 0){
//		printf("The pattern of expression is invalid.\n");
		return 1;
	}
	return 0;
}

bool check_parentheses(int p, int q){
	if ((tokens[p].type == TK_LEFT_BRACKET) && (tokens[q].type == TK_RIGHT_BRACKET)){
		return true;
	}
	return false;
}

int scan_main_operator(int p, int q){ 
	int token;
	int flag = 0;
	int level = 0;
	int op = -1;

	for (int i = q; i >= p; i --){
		token = tokens[i].type;
		switch (token){ 

			case '+':{
						 if (flag == 0){
							 if ((i == p) || (tokens[i-1].type >= 32  && tokens[i].type < 128)){
								 if (level < 2){
									level = 2;
									op = i;
			 					 }
							 }else{
								 if (level < 4){
									level = 4;
									op = i;
			 					 }
							 }
						 }
						 break;
		 			 }
			case '-':{
						 if (flag == 0){
							 if ((i == p) || (tokens[i-1].type >= 32  && tokens[i].type < 128)){
								 if (level < 2){
									level = 2;
									op = i;
								 }
							 }else{
								 if (level < 4){
									level = 4;
									op = i;
								 }
							 }
						 }
						 break;
					 }
			case TK_MUL:{
						 if (flag == 0){
							 if ((i == p) || (tokens[i-1].type >= 32  && tokens[i].type < 128)){
								 if (level < 2){
									level = 2;
									op = i;
								 }
							 }else{
								 if (level < 3){
									level = 3;
									op = i;
								 }
							 }
						 }
						 break;
					 }
			case TK_DIVI:{
						 if (flag == 0){
							 if(level < 3){
								 level = 3;
								 op = i;
							 }
						 }
						 break;
					 }
			case TK_LEFT_BRACKET:{
						 flag--;
						 break;
					 }
			case TK_RIGHT_BRACKET:{
						 flag++;
						 break;
					 }
			case TK_EQ:{
						  if (flag == 0){
							 if(level < 7){
								 level = 7;
								 op = i;
							 }
						  }
						  break;
					   }
			case TK_NON_EQ:	{
						  if (flag == 0){
							 if(level < 7){
								 level = 7;
								 op = i;
							 }
						  }
						  break;
					   }

			case TK_AND:{
						  if (flag == 0){
							 if(level < 11){
								 level = 11;
								 op = i;
							 }
						  }
						  break;
					   }
			default:
						break;

		} 
	}
	if (op == -1) {
		return 0;
	}
	return op;
}

static bool FLAG = false;

uint32_t eval(int p , int q){
	bool *success = success;
	char *reg = NULL;
	uint32_t result;
	if (p > q) {
		//选用闭区间，所以这里到达不了
		printf("Invalid pattern\n");
		FLAG = true;
		return 0;
	}
	else if (p == q){
		//递归结束条件
		if (tokens[p].type == TK_HEX_INT){
			return (uint32_t) strtoul(&(tokens[p].str[2]), NULL, 16);
		}else if (tokens[p].type == TK_REG){
			reg = &(tokens[q].str[1]);	
			return isa_reg_str2val(reg, success);	
			
		}else{
			return (uint32_t) strtoul(tokens[p].str, NULL, 10);
		}
	}
	else if (check_parentheses(p, q) == true){
		//判断表达式两端是不是括号
		return eval(p + 1, q -1);
	}
	else {

		int op = scan_main_operator(p, q);//寻找主运算符位置
		if (FLAG) { return 0;}
//		printf("the main op is %s - %d\n", tokens[op].str, op);
		int op_type = tokens[op].type;
		if (op == p && tokens[op].type >= 128){
			printf("The %s can't be at the beginning\n", tokens[op].str);
			FLAG = true;
			return 0;
		}
		if ((op == p) || (tokens[op-1].type >= 32 && tokens[op].type < 128)){
			//单目运算符的情况
			result = (uint32_t) strtoul(tokens[q].str, NULL, 10);
			uint32_t temp_result = result;
			uint32_t *temp_ptr;
			temp_ptr = NULL;
			for (int i = q-1;i >= p;i --){
				//单目运算符有着最高优先级，此处ｐ－ｑ之间必然全是运算符和一个操作数
				op_type = tokens[i].type;
				switch (op_type){
					case '+':
						temp_result = (uint32_t) result;
						result = +temp_result;
						break;
					case '-':
						temp_result = (uint32_t) result;
						result = -temp_result;
						break;
					case TK_MUL:
						temp_ptr = (uint32_t*)(intptr_t) result;
						result = *temp_ptr;
						break;
					case TK_REG:
						break;
				}
			}
			return result;
		}

		uint32_t val1 = eval(p , op -1);
		uint32_t val2 = eval(op + 1, q);
		
		if (FLAG){
			return 0;
		}
		//unit of computing
		switch(op_type){
			case '+':
				return result = val1 + val2;
				break;
			case '-':
				return result = val1 - val2;
				break;
			case TK_MUL:
				return result = val1 * val2;
				break;
			case TK_DIVI: 
				if (val2 == 0){
					printf("The divider equal zero,invalid.\n");
					FLAG = true;
					return 0;
				}else{
					return result = val1 / val2;
					break;
				}		
			case TK_EQ:
				return result = (val1 == val2);
				break;
			case TK_NON_EQ:
				return result = (val1 != val2);
				break;
			case TK_AND:
				return result = (val1 && val2);
				break;
			default:
				assert(0);
		}
	}
	return 0;
}


word_t expr(char *e, bool *success) {

  if (!make_token(e)) {
	  //paser the sentence and store the token to tokens
    *success = false;
		return 1;
  }
  FLAG = false;//判断是否，发生出错
  if  (judge_parentheses()){
		printf("The pattern of expression is invalid.\n");
		return 1;
  }

  //evaluate the result 
  uint32_t result = eval(0, nr_token - 1);
  /* TODO: Insert codes to evaluate the expression. */
  //TODO();//标记函数，表示此处有需要完善的功能函数
  if (FLAG){
	  printf("The sentence %s is invaild\n", e);
	  return 1;
  }

	for (int i = 0; i < nr_token; i++ ){
	printf("%s ", tokens[i].str);
	}
	printf("= %u\n", result);
	
  return result;
}

