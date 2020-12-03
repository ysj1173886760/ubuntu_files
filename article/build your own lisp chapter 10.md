# Q-表达式

## 内置函数(Builtin Functions)

我们现在可以读取Q-表达式，但是他们还是没什么实际作用。我们需要一些方法来操作他们。

由于这个原因我们可以定义一些内置操作符，选择一个简明的操作符集合是很重要的。如果我们实现了一些基本的操作符，接下来我们就可以用这些来定义新的操作并且不需要增加新的C代码。这里有一些方法可以选择这些基础操作符，我选择了对我们最好的一个集合，他们的定义如下。

+ `list` 输入一个或多个参数并且返回一个新的带参数的Q-表达式

+ `head` 输入一个Q-表达式并返回一个只含有第一个元素的Q-表达式

+ `tail` 输入一个Q-表达式并返回一个除去第一个元素的Q-表达式

+ `join` 输入一个或多个Q-表达式并返回他们连接起来的Q-表达式

+ `eval` 输入一个Q-表达式并作为S-表达式求值

就像是我们的数学运算符号一样，我们应该把这些操作符也加入到符号(symbol)中，然后我们就可以尝试在`builtin_op`中定义他们了

```c++
mpca_lang(MPCA_LANG_DEFAULT,
  "                                                        \
    number : /-?[0-9]+/ ;                                  \
    symbol : \"list\" | \"head\" | \"tail\"                \
           | \"join\" | \"eval\" | '+' | '-' | '*' | '/' ; \
    sexpr  : '(' <expr>* ')' ;                             \
    qexpr  : '{' <expr>* '}' ;                             \
    expr   : <number> | <symbol> | <sexpr> | <qexpr> ;     \
    lispy  : /^/ <expr>* /$/ ;                             \
  ",
  Number, Symbol, Sexpr, Qexpr, Expr, Lispy)
```

## 第一次尝试

我们的内置函数应该和 `builtin_op` 具有相同的接口。这意味着参数应该被打包成一个S-表达式并且我们的函数应该使用并最终删除他。我们的函数应该返回一个新的 `lval*` 作为求值的结果

对Q-表达式进行 `head` 或者 `tail` 应该不是对我们很困难的事情。我们可以利用当前为S-表达式已经写好的函数，例如 `lval_take` 和 `lval_pop` 。 但是对于 `buildin_op` 我们仍然需要去检查输入的参数是有效的。

首先看 `head` 和 `tail` ，这些函数有若干种他们无法进行操作的情况，我们必须确保他们只被传入了一个参数，并且那个参数是一个Q-表达式。我们还需要确保那个Q-表达式不是空的并且有一些元素在里面。

`head` 函数可以一直在1处进行pop并且删除他们，直到列表里没有其他的元素。

`tail` 函数就更简单了，在0处进行pop和删除，剩下的就是结果了。最开始的写法就像这样。

```c++
lval* builtin_head(lval* a) {
  /* Check Error Conditions */
  if (a->count != 1) {
    lval_del(a);
    return lval_err("Function 'head' passed too many arguments!");
  }

  if (a->cell[0]->type != LVAL_QEXPR) {
    lval_del(a);
    return lval_err("Function 'head' passed incorrect types!");
  }

  if (a->cell[0]->count == 0) {
    lval_del(a);
    return lval_err("Function 'head' passed {}!");
  }

  /* Otherwise take first argument */
  lval* v = lval_take(a, 0);

  /* Delete all elements that are not head and return */
  while (v->count > 1) { lval_del(lval_pop(v, 1)); }
  return v;
}
```

```c++
lval* builtin_tail(lval* a) {
  /* Check Error Conditions */
  if (a->count != 1) {
    lval_del(a);
    return lval_err("Function 'tail' passed too many arguments!");
  }

  if (a->cell[0]->type != LVAL_QEXPR) {
    lval_del(a);
    return lval_err("Function 'tail' passed incorrect types!");
  }

  if (a->cell[0]->count == 0) {
    lval_del(a);
    return lval_err("Function 'tail' passed {}!");
  }

  /* Take first argument */
  lval* v = lval_take(a, 0);

  /* Delete first element and return */
  lval_del(lval_pop(v, 0));
  return v;
}
```

## 宏

`head` 和 `tail` 函数是正确的，但是代码并不是那么的显而易见，并且长。这里有太多的错误检查导致了函数本身的功能很难看到。有一个处理这种情况的方法就是使用宏

宏是一个预处理指令，用来创建一些类似函数的，在编译之前就进行求值的东西。宏可以用来做很多事情，其中之一就是我们当前要做的——整理代码。

宏接收一些参数（参数几乎可以是任何东西），复制并按照一些给定的模式进行粘贴。通过改变这些模式或者参数我们可以改变被宏生成的代码。定义宏我们需要使用 `#define` 预处理器指令，然后我们写下宏的名字，接着在后面小括号中写下参数的名字。接下来我们指定模式，来确定通过给出的参数来生成什么样的代码。

我们可以设计一个 `LASSERT` 宏来帮助我们处理错误情况，宏常常用全大写的名字用来区分正常的C函数。这个宏需要3个参数，`args` `cond` 和 `err` 。宏按照右手边的方式来生成代码，参数会被粘贴到和他们相同名字的地方，这是一个很棒的模式可以处理我们所有的错误情况

```c++
#define LASSERT(args, cond, err) \
  if (!(cond)) { lval_del(args); return lval_err(err); }
```

我们可以用这个来改变刚才函数的写法，但是实际生成的代码是不变的。这个对程序员来说提高了可读性，并且节省了一些代码量。有了这个宏，剩下的错误情况的处理也会变得简单起来。

## Head & Tail

现在的函数就变的更加清晰了

```c++
lval* builtin_head(lval* a) {
  LASSERT(a, a->count == 1,
    "Function 'head' passed too many arguments!");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
    "Function 'head' passed incorrect type!");
  LASSERT(a, a->cell[0]->count != 0,
    "Function 'head' passed {}!");

  lval* v = lval_take(a, 0);
  while (v->count > 1) { lval_del(lval_pop(v, 1)); }
  return v;
}
```

```c++
lval* builtin_tail(lval* a) {
  LASSERT(a, a->count == 1,
    "Function 'tail' passed too many arguments!");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
    "Function 'tail' passed incorrect type!");
  LASSERT(a, a->cell[0]->count != 0,
    "Function 'tail' passed {}!");

  lval* v = lval_take(a, 0);
  lval_del(lval_pop(v, 0));
  return v;
}
```

## List & Eval

`list` 函数比较简单，就是把输入的S-表达式转换成了Q-表达式

`eval` 函数是将Q-表达式转换成S-表达式，求值就使用 `lval_eval` 

```c++
lval* builtin_list(lval* a) {
  a->type = LVAL_QEXPR;
  return a;
}
```

```c++
lval* builtin_eval(lval* a) {
  LASSERT(a, a->count == 1,
    "Function 'eval' passed too many arguments!");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
    "Function 'eval' passed incorrect type!");

  lval* x = lval_take(a, 0);
  x->type = LVAL_SEXPR;
  return lval_eval(x);
}
```

## Join

`join` 函数是最后一个了。不像是其他的函数，这个可以获得多个参数，所以他的结构就像是 `builtin_op` 一样。首先我们检查是否所有的参数都是Q-表达式，然后我们把他们用 `lval_join` 连接到一起。原理是重复的从 `y` pop元素并加到 `x` 后面直到 `y` 为空，然后删除 `y` 并返回 `x`。

```c++
lval* builtin_join(lval* a) {

  for (int i = 0; i < a->count; i++) {
    LASSERT(a, a->cell[i]->type == LVAL_QEXPR,
      "Function 'join' passed incorrect type.");
  }

  lval* x = lval_pop(a, 0);

  while (a->count) {
    x = lval_join(x, lval_pop(a, 0));
  }

  lval_del(a);
  return x;
}
```

```c++
lval* lval_join(lval* x, lval* y) {

  /* For each cell in 'y' add it to 'x' */
  while (y->count) {
    x = lval_add(x, lval_pop(y, 0));
  }

  /* Delete the empty 'y' and return 'x' */
  lval_del(y);
  return x;
}
```

## Builtins总览

我们现在把所有的内置函数都定义好了，我们现在需要一个函数来通过符号正确的调用对应的函数，我们可以通过 `strcmp` 和 `strstr` 实现这一点

```c++
lval* builtin(lval* a, char* func) {
  if (strcmp("list", func) == 0) { return builtin_list(a); }
  if (strcmp("head", func) == 0) { return builtin_head(a); }
  if (strcmp("tail", func) == 0) { return builtin_tail(a); }
  if (strcmp("join", func) == 0) { return builtin_join(a); }
  if (strcmp("eval", func) == 0) { return builtin_eval(a); }
  if (strstr("+-/*", func)) { return builtin_op(a, func); }
  lval_del(a);
  return lval_err("Unknown Function!");
}
```

然后我们就可以在 `lval_eval_sexpr` 中的 `builtin_op` 换成新定义的 `builtin` 了。

```c++
/* Call builtin with operator */
lval* result = builtin(v, f->sym);
lval_del(f);
return result;
```

最终我们的语言就可以支持Q-表达式了，编译并运行最新的版本来看一看你能做些什么。 尝试用不同的方法进行求值，在S-表达式中放入Q-表达式是一个很棒的事情。这代表我们可以像对待数据一样对待代码，这就是lisp语言的标志性特性。

```c++
lispy> list 1 2 3 4
{1 2 3 4}
lispy> {head (list 1 2 3 4)}
{head (list 1 2 3 4)}
lispy> eval {head (list 1 2 3 4)}
{1}
lispy> tail {tail tail tail}
{tail tail}
lispy> eval (tail {tail tail {5 6 7}})
{6 7}
lispy> eval (head {(+ 1 2) (+ 10 20)})
3
```

总体代码

```c++
#include "mpc.h"

#ifdef _WIN32

static char buffer[2048];

char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

void add_history(char* unused) {}

#else
#include <editline/readline.h>
#include <editline/history.h>
#endif

/* Add QEXPR as possible lval type */
enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR };

typedef struct lval {
  int type;
  long num;
  char* err;
  char* sym;
  int count;
  struct lval** cell;
} lval;

lval* lval_num(long x) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_NUM;
  v->num = x;
  return v;
}

lval* lval_err(char* m) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_ERR;
  v->err = malloc(strlen(m) + 1);
  strcpy(v->err, m);
  return v;
}

lval* lval_sym(char* s) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_SYM;
  v->sym = malloc(strlen(s) + 1);
  strcpy(v->sym, s);
  return v;
}

lval* lval_sexpr(void) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_SEXPR;
  v->count = 0;
  v->cell = NULL;
  return v;
}

/* A pointer to a new empty Qexpr lval */
lval* lval_qexpr(void) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_QEXPR;
  v->count = 0;
  v->cell = NULL;
  return v;
}

void lval_del(lval* v) {

  switch (v->type) {
    case LVAL_NUM: break;
    case LVAL_ERR: free(v->err); break;
    case LVAL_SYM: free(v->sym); break;
    
    /* If Qexpr or Sexpr then delete all elements inside */
    case LVAL_QEXPR:
    case LVAL_SEXPR:
      for (int i = 0; i < v->count; i++) {
        lval_del(v->cell[i]);
      }
      /* Also free the memory allocated to contain the pointers */
      free(v->cell);
    break;
  }
  
  free(v);
}

lval* lval_add(lval* v, lval* x) {
  v->count++;
  v->cell = realloc(v->cell, sizeof(lval*) * v->count);
  v->cell[v->count-1] = x;
  return v;
}

lval* lval_pop(lval* v, int i) {
  lval* x = v->cell[i];
  memmove(&v->cell[i], &v->cell[i+1],
    sizeof(lval*) * (v->count-i-1));
  v->count--;
  v->cell = realloc(v->cell, sizeof(lval*) * v->count);
  return x;
}

lval* lval_join(lval* x, lval* y) {

  while (y->count) {
    x = lval_add(x, lval_pop(y, 0));
  }

  lval_del(y);  
  return x;
}

lval* lval_take(lval* v, int i) {
  lval* x = lval_pop(v, i);
  lval_del(v);
  return x;
}

void lval_print(lval* v);

void lval_expr_print(lval* v, char open, char close) {
  putchar(open);
  for (int i = 0; i < v->count; i++) {
    
    lval_print(v->cell[i]);
    
    if (i != (v->count-1)) {
      putchar(' ');
    }
  }
  putchar(close);
}

void lval_print(lval* v) {
  switch (v->type) {
    case LVAL_NUM:   printf("%li", v->num); break;
    case LVAL_ERR:   printf("Error: %s", v->err); break;
    case LVAL_SYM:   printf("%s", v->sym); break;
    case LVAL_SEXPR: lval_expr_print(v, '(', ')'); break;
    case LVAL_QEXPR: lval_expr_print(v, '{', '}'); break;
  }
}

void lval_println(lval* v) { lval_print(v); putchar('\n'); }

#define LASSERT(args, cond, err) \
  if (!(cond)) { lval_del(args); return lval_err(err); }
  
lval* lval_eval(lval* v);

lval* builtin_list(lval* a) {
  a->type = LVAL_QEXPR;
  return a;
}

lval* builtin_head(lval* a) {
  LASSERT(a, a->count == 1,
    "Function 'head' passed too many arguments.");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
    "Function 'head' passed incorrect type.");
  LASSERT(a, a->cell[0]->count != 0,
    "Function 'head' passed {}.");
  
  lval* v = lval_take(a, 0);  
  while (v->count > 1) { lval_del(lval_pop(v, 1)); }
  return v;
}

lval* builtin_tail(lval* a) {
  LASSERT(a, a->count == 1,
    "Function 'tail' passed too many arguments.");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
    "Function 'tail' passed incorrect type.");
  LASSERT(a, a->cell[0]->count != 0,
    "Function 'tail' passed {}.");

  lval* v = lval_take(a, 0);  
  lval_del(lval_pop(v, 0));
  return v;
}

lval* builtin_eval(lval* a) {
  LASSERT(a, a->count == 1,
    "Function 'eval' passed too many arguments.");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
    "Function 'eval' passed incorrect type.");
  
  lval* x = lval_take(a, 0);
  x->type = LVAL_SEXPR;
  return lval_eval(x);
}

lval* builtin_join(lval* a) {

  for (int i = 0; i < a->count; i++) {
    LASSERT(a, a->cell[i]->type == LVAL_QEXPR,
      "Function 'join' passed incorrect type.");
  }
  
  lval* x = lval_pop(a, 0);
  
  while (a->count) {
    x = lval_join(x, lval_pop(a, 0));
  }
  
  lval_del(a);
  return x;
}

lval* builtin_op(lval* a, char* op) {
  
  for (int i = 0; i < a->count; i++) {
    if (a->cell[i]->type != LVAL_NUM) {
      lval_del(a);
      return lval_err("Cannot operate on non-number!");
    }
  }
  
  lval* x = lval_pop(a, 0);
  if ((strcmp(op, "-") == 0) && a->count == 0) { x->num = -x->num; }
  
  while (a->count > 0) {
  
    lval* y = lval_pop(a, 0);
    
    if (strcmp(op, "+") == 0) { x->num += y->num; }
    if (strcmp(op, "-") == 0) { x->num -= y->num; }
    if (strcmp(op, "*") == 0) { x->num *= y->num; }
    if (strcmp(op, "/") == 0) {
      if (y->num == 0) {
        lval_del(x); lval_del(y);
        x = lval_err("Division By Zero.");
        break;
      }
      x->num /= y->num;
    }
    
    lval_del(y);
  }
  
  lval_del(a);
  return x;
}

lval* builtin(lval* a, char* func) {
  if (strcmp("list", func) == 0) { return builtin_list(a); }
  if (strcmp("head", func) == 0) { return builtin_head(a); }
  if (strcmp("tail", func) == 0) { return builtin_tail(a); }
  if (strcmp("join", func) == 0) { return builtin_join(a); }
  if (strcmp("eval", func) == 0) { return builtin_eval(a); }
  if (strstr("+-/*", func)) { return builtin_op(a, func); }
  lval_del(a);
  return lval_err("Unknown Function!");
}

lval* lval_eval_sexpr(lval* v) {
  
  for (int i = 0; i < v->count; i++) {
    v->cell[i] = lval_eval(v->cell[i]);
  }
  
  for (int i = 0; i < v->count; i++) {
    if (v->cell[i]->type == LVAL_ERR) { return lval_take(v, i); }
  }
  
  if (v->count == 0) { return v; }
  
  if (v->count == 1) { return lval_take(v, 0); }
  
  lval* f = lval_pop(v, 0);
  if (f->type != LVAL_SYM) {
    lval_del(f); lval_del(v);
    return lval_err("S-expression Does not start with symbol.");
  }
  
  /* Call builtin with operator */
  lval* result = builtin(v, f->sym);
  lval_del(f);
  return result;
}

lval* lval_eval(lval* v) {
  if (v->type == LVAL_SEXPR) { return lval_eval_sexpr(v); }
  return v;
}

lval* lval_read_num(mpc_ast_t* t) {
  errno = 0;
  long x = strtol(t->contents, NULL, 10);
  return errno != ERANGE ? lval_num(x) : lval_err("invalid number");
}

lval* lval_read(mpc_ast_t* t) {
  
  if (strstr(t->tag, "number")) { return lval_read_num(t); }
  if (strstr(t->tag, "symbol")) { return lval_sym(t->contents); }
  
  lval* x = NULL;
  if (strcmp(t->tag, ">") == 0) { x = lval_sexpr(); } 
  if (strstr(t->tag, "sexpr"))  { x = lval_sexpr(); }
  if (strstr(t->tag, "qexpr"))  { x = lval_qexpr(); }
  
  for (int i = 0; i < t->children_num; i++) {
    if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
    if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
    if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
    if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
    if (strcmp(t->children[i]->tag,  "regex") == 0) { continue; }
    x = lval_add(x, lval_read(t->children[i]));
  }
  
  return x;
}

int main(int argc, char** argv) {
  
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Symbol = mpc_new("symbol");
  mpc_parser_t* Sexpr  = mpc_new("sexpr");
  mpc_parser_t* Qexpr  = mpc_new("qexpr");
  mpc_parser_t* Expr   = mpc_new("expr");
  mpc_parser_t* Lispy  = mpc_new("lispy");
  
  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                    \
      number : /-?[0-9]+/ ;                              \
      symbol : \"list\" | \"head\" | \"tail\" | \"eval\" \
             | \"join\" | '+' | '-' | '*' | '/' ;        \
      sexpr  : '(' <expr>* ')' ;                         \
      qexpr  : '{' <expr>* '}' ;                         \
      expr   : <number> | <symbol> | <sexpr> | <qexpr> ; \
      lispy  : /^/ <expr>* /$/ ;                         \
    ",
    Number, Symbol, Sexpr, Qexpr, Expr, Lispy);
  
  puts("Lispy Version 0.0.0.0.6");
  puts("Press Ctrl+c to Exit\n");
  
  while (1) {
  
    char* input = readline("lispy> ");
    add_history(input);
    
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      lval* x = lval_eval(lval_read(r.output));
      lval_println(x);
      lval_del(x);
      mpc_ast_delete(r.output);
    } else {    
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    
    free(input);
    
  }
  
  mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Lispy);
  
  return 0;
}
```
