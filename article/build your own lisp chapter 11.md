# 变量

## 不变性

在前面的章节中，我们已经在语言的基础上取得了相当大的进步。

我们已经可以做一些其他语言做不到的事情，比如把代码放到列表中。现在是时候开始添加一些特性，使我们的语言更加实用。首先就是变量。

它们被称为变量，但这是一个误导性的名称，因为我们的变量不会改变。我们的变量是不可变的。到目前为止，我们语言中的一切表现都表示它是不可变的。当我们计算一个表达式时，我们删除之前的东西，计算并返回新的东西。虽然在实现过程中，我们通常更希望重用之前的数据来构建新的东西，但从概念上讲，这是思考我们的语言如何工作的一种好方法。

所以实际上我们的变量只是一种命名值的方式。它们允许我们为一个值指定一个名称，然后在以后需要该值时获得该值的一个副本。

为了允许对值进行命名，我们需要创建一个结构来存储程序中所有命名的名称和值。我们称之为环境。当我们启动一个新的交互式提示符时，我们希望创建一个新的环境来配合它，在这个环境中，每一个新的输入位都会被求解。之后我们可以在编程时存储并且回调（重复调用）变量。

当我们重新给一个名字赋给新的东西时会发生什么? 这难道不像可变性吗？

在我们的 Lisp 中，当我们重新分配一个名称时，我们将删除旧的关联（名字到值的关联）并创建一个新的。这给人一种错觉，认为分配给该名称的事物已经改变，并且是可变的，但实际上我们已经删除了旧的事物，并分配给它一个新的事物。这与 c 语言不同，在 c 语言中，我们可以改变指针指向的数据或者存储在结构中的数据，而不需要删除它并创建一个新的结构。

## 符号语法

现在我们要考虑用户定义的变量，我们需要更新语法以使符号更加灵活。而不仅仅是我们的内置函数，它应该匹配任何可能的有效符号。与 c 语言不同的是，在 c 语言中，可以给出变量的名称是相当严格的，我们将允许在变量的名称中使用各种字符。

我们可以创建一个正则表达式来表示可用字符的范围，如下所示。

`/[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/`

乍一看，这看起来像我们刚刚猛击我们的手键盘。实际上，它是一个使用大范围说明符[]的正则表达式。范围内的特殊字符失去了它们的意义，但是其中的一些字符仍然需要用反斜杠进行转义。因为这是 c 字符串的一部分，所以我们需要放两个反斜杠来表示输入中的一个反斜杠字符。

这个规则允许符号是 c 标识符 a-zA-Z0-9_，算术运算符字符 +-*/，反斜杠字符，比较运算符字符 = < > ！或符号 & 。这将为我们定义新的和现有的符号提供所需的灵活性。

注意上面的`\\\\`代表的实际上是`\`，因为输入到规则中就变成了`\\`，再对应到用户输入中就变成了`\`

```c++
mpca_lang(MPCA_LANG_DEFAULT,
  "                                                     \
    number : /-?[0-9]+/ ;                               \
    symbol : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;         \
    sexpr  : '(' <expr>* ')' ;                          \
    qexpr  : '{' <expr>* '}' ;                          \
    expr   : <number> | <symbol> | <sexpr> | <qexpr> ;  \
    lispy  : /^/ <expr>* /$/ ;                          \
  ",
  Number, Symbol, Sexpr, Qexpr, Expr, Lispy);
```

## 函数指针

一旦我们引入了变量，在我们的语言中，符号将不再代表函数，而是代表一个名字，让我们查看环境，从中获取一些新的值。

因此，我们需要一个新的值来表示语言中的函数，一旦遇到内置符号之一，我们就可以返回这个值。为了创建这个新类型的 lval，我们将使用一个叫做函数指针的东西。

函数指针是 c 语言的一个重要特性，它可以让你存储和传递指向函数的指针。编辑这些指针指向的数据是没有意义的。相反，我们使用它们来调用它们指向的函数，就好像它是一个普通的函数。

在前面的章节中，我们的内建函数以一个 `lval*` 作为输入，并返回一个 `lval*` 作为输出。在本章中，我们的内置函数将采用一个额外的指向环境 `lenv*` 的指针作为输入。我们可以为这种类型的函数声明一个名为 `lbuiltin` 的函数指针类型，如下所示。

`typedef lval*(*lbuiltin)(lenv*, lval*);`

`lbuiltin` 指针指向接收 `lval*` 和 `lenv*` 作为输入，并且返回一个 `lval*` 的函数

循环类型

`lbuiltin` 类型引用 `lval` 类型和 `lenv` 类型。这意味着它们应该首先在源文件中声明。

但是我们想在 `lval` 结构中创建一个 `lbuiltin` 字段，这样我们就可以创建函数值。因此，我们的 `lbuiltin` 声明必须先于 `lval` 声明。这导致了所谓的循环类型依赖，其中两种类型相互依赖。

在 c 语言中，我们可以对类型做同样的事情。首先，我们声明两个没有体的结构类型。其次，我们将这些类型化为 `lval` 和 `lenv`。然后我们可以定义我们的 `lbuiltin` 函数指针类型。最后我们可以定义 `lval` 结构体的主体。现在我们所有的类型问题都解决了，编译器也不会再抱怨了。

```c++
/* Forward Declarations */

struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;

/* Lisp Value */

enum { LVAL_ERR, LVAL_NUM,   LVAL_SYM,
       LVAL_FUN, LVAL_SEXPR, LVAL_QEXPR };

typedef lval*(*lbuiltin)(lenv*, lval*);

struct lval {
  int type;

  long num;
  char* err;
  char* sym;
  lbuiltin fun;

  int count;
  lval** cell;
};
```

## 功能类型

因为我们已经用枚举 `LVAL_FUN` 添加了一个新的可能的 `LVAL` 类型。我们应该更新所有在 `lvals` 上工作的相关函数，以正确处理此更新。在大多数情况下，这只意味着在 switch 语句中插入新的 case。

我们可以先为这个类型创建一个新的构造函数。

```c++
lval* lval_fun(lbuiltin func) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_FUN;
  v->fun = func;
  return v;
}
```

在删除时，我们不需要对函数指针做任何特殊的处理。

`case LVAL_FUN: break;`

在打印时，我们只需打印一个字符串代表他是函数即可。

`case LVAL_FUN:   printf("<function>"); break;`

我们还要添加一个新函数来复制 `lval`。当我们把东西放进环境，把东西从环境中拿出来的时候，这就会派上用场。对于数字和函数，我们可以直接复制相关的字段。对于字符串，我们需要使用 `malloc` 和 `strcpy` 进行复制。要复制列表，我们需要分配正确的空间，然后单独复制每个元素。

```c++
lval* lval_copy(lval* v) {

  lval* x = malloc(sizeof(lval));
  x->type = v->type;    //分配整个结构的空间

  switch (v->type) {

    /* Copy Functions and Numbers Directly */
    case LVAL_FUN: x->fun = v->fun; break;
    case LVAL_NUM: x->num = v->num; break;

    /* Copy Strings using malloc and strcpy */
    case LVAL_ERR:
      x->err = malloc(strlen(v->err) + 1);
      strcpy(x->err, v->err); break;

    case LVAL_SYM:
      x->sym = malloc(strlen(v->sym) + 1);
      strcpy(x->sym, v->sym); break;

    /* Copy Lists by copying each sub-expression */
    case LVAL_SEXPR:
    case LVAL_QEXPR:
      x->count = v->count;
      x->cell = malloc(sizeof(lval*) * x->count);  //分配指针的空间
      for (int i = 0; i < x->count; i++) {
        x->cell[i] = lval_copy(v->cell[i]);
      }
    break;
  }

  return x;
}
```

## 环境

我们的环境结构必须对名称和值之间的关系列表进行编码。有很多方法可以建立一个结构，可以做这种事情。我们将采用最简单有效的方法。这是使用两个长度相等的列表。一个是 `lval*` 列表，另一个是 `char*` 列表。一个列表中的每个条目在另一个列表中的相同位置都有一个对应的条目。

我们已经前向声明了我们的 `lenv` 结构体，所以我们可以这样定义它。

```c++
struct lenv {
  int count;
  char** syms;
  lval** vals;
};
```

我们需要一些函数来创建和删除这个结构。这些都很简单。创建初始化结构字段，而删除迭代两个列表中的项，并删除或释放它们。

```c++
lenv* lenv_new(void) {
  lenv* e = malloc(sizeof(lenv));
  e->count = 0;
  e->syms = NULL;
  e->vals = NULL;
  return e;
}
```

```c++
void lenv_del(lenv* e) {
  for (int i = 0; i < e->count; i++) {
    free(e->syms[i]);
    lval_del(e->vals[i]);
  }
  free(e->syms);
  free(e->vals);
  free(e);
}
```

接下来，我们可以创建两个函数，它们可以从环境中获取值，也可以将值放入环境中。

为了从环境中获取值，我们循环遍历环境中的所有项，并检查给定的符号是否与任何存储的字符串匹配。如果找到匹配项，我们可以返回存储值的一个副本。如果没有找到匹配，我们应该返回一个错误。

向环境中添加新变量的函数要稍微复杂一些。首先，我们要检查具有相同名称的变量是否已经存在。如果是这样的话，我们应该用新的值替换它的值。为此，我们循环遍历环境中的所有现有变量并检查它们的名称。如果找到匹配项，我们删除存储在该位置的值，并存储输入值的副本。

如果找不到具有该名称的现有值，则需要分配更多空间将其放入。为此，我们可以使用 `realloc` ，并在新分配的位置存储 `lval` 及其名称的副本。

```c++
lval* lenv_get(lenv* e, lval* k) {

  /* Iterate over all items in environment */
  for (int i = 0; i < e->count; i++) {
    /* Check if the stored string matches the symbol string */
    /* If it does, return a copy of the value */
    if (strcmp(e->syms[i], k->sym) == 0) {
      return lval_copy(e->vals[i]);
    }
  }
  /* If no symbol found return error */
  return lval_err("unbound symbol!");
}
```

```c++
void lenv_put(lenv* e, lval* k, lval* v) {

  /* Iterate over all items in environment */
  /* This is to see if variable already exists */
  for (int i = 0; i < e->count; i++) {

    /* If variable is found delete item at that position */
    /* And replace with variable supplied by user */
    if (strcmp(e->syms[i], k->sym) == 0) {
      lval_del(e->vals[i]);
      e->vals[i] = lval_copy(v);
      return;
    }
  }

  /* If no existing entry found allocate space for new entry */
  e->count++;
  e->vals = realloc(e->vals, sizeof(lval*) * e->count);
  e->syms = realloc(e->syms, sizeof(char*) * e->count);

  /* Copy contents of lval and symbol string into new location */
  e->vals[e->count-1] = lval_copy(v);
  e->syms[e->count-1] = malloc(strlen(k->sym)+1);
  strcpy(e->syms[e->count-1], k->sym);
}
```

## 变量求值

我们的求值函数现在取决于环境。我们应该将其作为参数传入，并在遇到符号类型时使用它来获取值。因为我们的环境返回值的副本，所以我们要记得删除输入符号 `lval`。

```c++
lval* lval_eval(lenv* e, lval* v) {
  if (v->type == LVAL_SYM) {
    lval* x = lenv_get(e, v);
    lval_del(v);
    return x;
  }
  if (v->type == LVAL_SEXPR) { return lval_eval_sexpr(e, v); }
  return v;
}
```

因为我们添加了一个函数类型，所以我们对 S-表达式的计算也需要改变。与检查符号类型不同，我们希望确保它是函数类型。如果这个条件成立，我们可以使用与标准函数调用相同的符号调用 `lval` 的 `fun` 字段。

```c++
lval* lval_eval_sexpr(lenv* e, lval* v) {

  for (int i = 0; i < v->count; i++) {
    v->cell[i] = lval_eval(e, v->cell[i]);
  }

  for (int i = 0; i < v->count; i++) {
    if (v->cell[i]->type == LVAL_ERR) { return lval_take(v, i); }
  }

  if (v->count == 0) { return v; }
  if (v->count == 1) { return lval_take(v, 0); }

  /* Ensure first element is a function after evaluation */
  lval* f = lval_pop(v, 0);
  if (f->type != LVAL_FUN) {
    lval_del(v); lval_del(f);
    return lval_err("first element is not a function");
  }

  /* If so call function to get result */
  lval* result = f->fun(e, v);
  lval_del(f);
  return result;
}
//与上一章的代码相比，这里将基础的四则运算也看做了fun，即函数类型
```

## 内置物品

现在我们的求值依赖于新的函数类型，我们需要确保在启动交互提示符之前能够在环境中注册所有的内置函数。目前我们的内置函数不是正确的类型。我们需要更改它们的类型签名，以便在某些环境中使用它们，并在适当的地方更改它们，以便将这个环境传递到需要它的其他调用中。我不会为此发布代码，所以请自己修改 内置函数的类型签名，将 `lenv*` 作为它们的第一个参数。如果您感到困惑，可以查看本章的示例代码。

作为一个例子，我们可以利用我们的内建函数为我们的语言支持的每个数学函数定义单独的内置函数。

```c++
lval* builtin_add(lenv* e, lval* a) {
  return builtin_op(e, a, "+");
}

lval* builtin_sub(lenv* e, lval* a) {
  return builtin_op(e, a, "-");
}

lval* builtin_mul(lenv* e, lval* a) {
  return builtin_op(e, a, "*");
}

lval* builtin_div(lenv* e, lval* a) {
  return builtin_op(e, a, "/");
}
```

一旦我们将内建函数改为正确的类型，我们就可以创建一个函数，将所有内置物品注册到一个环境中。

对于每个内建，我们希望用给定的名称创建一个函数 `lval` 和符号 `lval`。然后，我们使用 `lenv_put` 在环境中注册这些。环境总是获取或返回值的副本，因此我们需要记住在注册后删除这两个 `lval`，因为我们不再需要它们了。

如果我们把这个任务分成两个函数，我们就可以整齐地注册所有的内置函数。

```c++
void lenv_add_builtin(lenv* e, char* name, lbuiltin func) {
  lval* k = lval_sym(name);
  lval* v = lval_fun(func);
  lenv_put(e, k, v);
  lval_del(k); lval_del(v);
}

void lenv_add_builtins(lenv* e) {
  /* List Functions */
  lenv_add_builtin(e, "list", builtin_list);
  lenv_add_builtin(e, "head", builtin_head);
  lenv_add_builtin(e, "tail", builtin_tail);
  lenv_add_builtin(e, "eval", builtin_eval);
  lenv_add_builtin(e, "join", builtin_join);

  /* Mathematical Functions */
  lenv_add_builtin(e, "+", builtin_add);
  lenv_add_builtin(e, "-", builtin_sub);
  lenv_add_builtin(e, "*", builtin_mul);
  lenv_add_builtin(e, "/", builtin_div);
}
```

最后一步是在创建交互式提示符之前调用这个函数。我们还需要记住，一旦程序结束我们要删除环境。

```c++
lenv* e = lenv_new();
lenv_add_builtins(e);

while (1) {

  char* input = readline("lispy> ");
  add_history(input);

  mpc_result_t r;
  if (mpc_parse("<stdin>", input, Lispy, &r)) {

    lval* x = lval_eval(e, lval_read(r.output));
    lval_println(x);
    lval_del(x);

    mpc_ast_delete(r.output);
  } else {
    mpc_err_print(r.error);
    mpc_err_delete(r.error);
  }

  free(input);

}

lenv_del(e);
```

## 定义功能

我们已经将我们的内置函数注册为了变量，但是我们仍然没有一种方法让用户定义他们自己的变量。

这实际上有点尴尬。我们需要让用户将一个符号传递给 name，以及要分配给它的值。但是符号不能单独出现。否则，计算函数将尝试从环境中检索它们的值。

我们传递符号而不计算它们的唯一方法是将它们放在引用表达式中的{}之间。所以我们要用这个技巧来定义函数。它将输入一个符号列表和一些其他值。然后，它将为每个符号分配每个值。

这个函数应该像其他内置函数一样工作。它首先检查错误情况，然后执行一些命令并返回一个值。在这个例子中，它首先检查输入参数是否是正确的类型。然后遍历每个符号和值，并将它们放入环境中。如果有错误，我们可以返回它，但如果成功，我们将返回空表达式()。

```c++
lval* builtin_def(lenv* e, lval* a) {
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
    "Function 'def' passed incorrect type!");

  /* First argument is symbol list */
  lval* syms = a->cell[0];

  /* Ensure all elements of first list are symbols */
  for (int i = 0; i < syms->count; i++) {
    LASSERT(a, syms->cell[i]->type == LVAL_SYM,
      "Function 'def' cannot define non-symbol");
  }

  /* Check correct number of symbols and values */
  LASSERT(a, syms->count == a->count-1,
    "Function 'def' cannot define incorrect "
    "number of values to symbols");

  /* Assign copies of values to symbols */
  for (int i = 0; i < syms->count; i++) {
    lenv_put(e, syms->cell[i], a->cell[i+1]);
  }

  lval_del(a);
  return lval_sexpr();
}
```

我们需要使用`lenv_add_builtins`来注册这个新的内置函数。

```c++
/* Variable Functions */
lenv_add_builtin(e, "def",  builtin_def);
```

现在我们应该能够支持用户定义的变量。因为我们的 def 函数接受一个符号列表，我们可以做一些很酷的事情，在列表中存储和操作符号，然后再将他们定义。运行一下程序，确保一切正常工作。你应该得到如下行为。找找还有什么其他复杂的方法可以定义或者求值。一旦我们开始定义函数，我们就会真正看到用这种方法可以实现的一些有用的东西。

```c++
lispy> def {x} 100
()
lispy> def {y} 200
()
lispy> x
100
lispy> y
200
lispy> + x y
300
lispy> def {a b} 5 6
()
lispy> + a b
11
lispy> def {arglist} {a b x y}
()
lispy> arglist
{a b x y}
lispy> def arglist 1 2 3 4
()
lispy> list a b x y
{1 2 3 4}
lispy>
```

## 错误报告

到目前为止，我们的错误报告工作不是很好。我们可以报告错误发生的时间，并给出一个模糊的概念，说明问题是什么，但是我们没有给用户提供太多的信息，说明到底哪里出错了。例如，如果有一个未绑定的符号，我们应该能够报告确切的符号是未绑定的。这可以帮助用户跟踪错误、拼写错误和其他琐碎的问题。

如果我们能够编写一个函数，能够以类似 printf 的方式报告错误，那该多好啊。如果我们能够传入字符串、整数和其他数据来丰富错误消息，那将是理想的。

`printf` 函数是 c 语言中的一个特殊函数，因为它接受的参数数量可变。我们可以创建自己的变量参数函数，这就是我们要做的，让我们的错误报告更好。

我们将修改 `lval_err`，以与 `printf` 相同的方式执行操作，接受一个格式化字符串，然后将可变数量的参数匹配到这个字符串中

要声明函数接受类型签名中的变量参数，可以使用特殊的省略号... 语法，它表示其余的参数。

```c++
lval* lval_err(char* fmt, ...);
```

然后，在函数内部有一些标准的库函数，我们可以使用它们来检查调用者传入的内容。

第一步是创建一个 `va_list` 结构，并用 `va_start` 初始化它，传入最后一个命名的参数。对于其他目的，可以使用 `va_ arg` 检查传递的每个参数，但是我们将把整个参数列表直接传递给 `vsnprintf` 函数。这个函数的作用类似于 `printf`，但是它会写入一个字符串并接受一个 `va_list` 。一旦我们完成了，我们应该调用 `va_end` 来清理所使用的任何资源。

函数输出到一个字符串，我们需要先分配一些。因为我们不知道这个字符串的大小，我们首先分配一个512字节的缓冲区，一旦我们写入完毕，就重新分配一个较小的缓冲区。如果一个错误消息超过512个字符，它就会被切断，但是希望这不会发生。

将它们放在一起，我们的新错误函数如下所示。

```c++
lval* lval_err(char* fmt, ...) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_ERR;

  /* Create a va list and initialize it */
  va_list va;
  va_start(va, fmt);

  /* Allocate 512 bytes of space */
  v->err = malloc(512);

  /* printf the error string with a maximum of 511 characters */
  vsnprintf(v->err, 511, fmt, va);

  /* Reallocate to number of bytes actually used */
  v->err = realloc(v->err, strlen(v->err)+1);

  /* Cleanup our va list */
  va_end(va);

  return v;
}
```

通过这个，我们可以开始添加一些更好的错误消息到我们的函数。作为一个例子，我们可以看看 `lenv_get`。如果找不到符号，而不是报告一般错误，我们可以实际报告未找到的名称。

```c++
return lval_err("Unbound Symbol '%s'", k->sym);
```

我们也可以调整我们的 `LASSERT` 宏，这样它也可以采取可变的参数。因为这是一个宏，而不是标准函数，所以语法略有不同。在定义的左边，我们再次使用省略号表示法，但在右边，我们使用一个特殊的变量 `__VA_ARGS__` 来粘贴所有其他参数的内容。

我们需要在这个特殊变量的前面加上两个哈希符号 `##` 。这可以确保在没有传递额外参数的情况下正确粘贴宏。本质上，这样做的目的是确保去掉前面的逗号，看起来好像没有传入任何额外的参数。

因为我们可能在构造错误消息时使用 `args`，所以我们需要确保在创建错误值之前不删除它。

```c++
#define LASSERT(args, cond, fmt, ...) \
  if (!(cond)) { \
    lval* err = lval_err(fmt, ##__VA_ARGS__); \
    lval_del(args); \
    return err; \
  }
```

现在，我们可以更新一些错误消息，使其信息更加丰富。例如，如果传递的参数数目不正确，我们可以指定需要多少个参数以及给定了多少个参数。

```c++
LASSERT(a, a->count == 1,
  "Function 'head' passed too many arguments. "
  "Got %i, Expected %i.",
  a->count, 1);
```

我们还可以改进类型错误的错误报告。我们应该尝试报告函数期望的类型以及它实际得到的类型。在我们这样做之前，最好有一个函数作为输入某种类型的枚举并返回该类型的字符串表示形式。

```c++
char* ltype_name(int t) {
  switch(t) {
    case LVAL_FUN: return "Function";
    case LVAL_NUM: return "Number";
    case LVAL_ERR: return "Error";
    case LVAL_SYM: return "Symbol";
    case LVAL_SEXPR: return "S-Expression";
    case LVAL_QEXPR: return "Q-Expression";
    default: return "Unknown";
  }
}
```

```c++
LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
  "Function 'head' passed incorrect type for argument 0. "
  "Got %s, Expected %s.",
  ltype_name(a->cell[0]->type), ltype_name(LVAL_QEXPR));
```

继续并使用 LASSERT 在整个代码中更深入地报告错误。当我们开始使用新的语言编写复杂的代码时，这将使下一阶段的许多调试变得更加容易。看看是否可以使用宏保存键入信息，并自动生成常见错误报告方法的代码。

```c++
lispy> + 1 {5 6 7}
Error: Function '+' passed incorrect type for argument 1. Got Q-Expression, Expected Number.
lispy> head {1 2 3} {4 5 6}
Error: Function 'head' passed incorrect number of arguments. Got 2, Expected 1.
lispy>
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

/* Forward Declarations */

struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;

/* Lisp Value */

enum { LVAL_ERR, LVAL_NUM,   LVAL_SYM, 
       LVAL_FUN, LVAL_SEXPR, LVAL_QEXPR };

typedef lval*(*lbuiltin)(lenv*, lval*);

struct lval {
  int type;
  long num;
  char* err;
  char* sym;
  lbuiltin fun;
  int count;
  lval** cell;
};

lval* lval_num(long x) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_NUM;
  v->num = x;
  return v;
}

lval* lval_err(char* fmt, ...) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_ERR;
  
  /* Create a va list and initialize it */
  va_list va;
  va_start(va, fmt);
  
  /* Allocate 512 bytes of space */
  v->err = malloc(512);
  
  /* printf the error string with a maximum of 511 characters */
  vsnprintf(v->err, 511, fmt, va);
  
  /* Reallocate to number of bytes actually used */
  v->err = realloc(v->err, strlen(v->err)+1);
  
  /* Cleanup our va list */
  va_end(va);
  
  return v;
}

lval* lval_sym(char* s) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_SYM;
  v->sym = malloc(strlen(s) + 1);
  strcpy(v->sym, s);
  return v;
}

lval* lval_fun(lbuiltin func) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_FUN;
  v->fun = func;
  return v;
}

lval* lval_sexpr(void) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_SEXPR;
  v->count = 0;
  v->cell = NULL;
  return v;
}

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
    case LVAL_FUN: break;
    case LVAL_ERR: free(v->err); break;
    case LVAL_SYM: free(v->sym); break;
    case LVAL_QEXPR:
    case LVAL_SEXPR:
      for (int i = 0; i < v->count; i++) {
        lval_del(v->cell[i]);
      }
      free(v->cell);
    break;
  }
  
  free(v);
}

lval* lval_copy(lval* v) {

  lval* x = malloc(sizeof(lval));
  x->type = v->type;
  
  switch (v->type) {
    
    /* Copy Functions and Numbers Directly */
    case LVAL_FUN: x->fun = v->fun; break;
    case LVAL_NUM: x->num = v->num; break;
    
    /* Copy Strings using malloc and strcpy */
    case LVAL_ERR:
      x->err = malloc(strlen(v->err) + 1);
      strcpy(x->err, v->err); break;
      
    case LVAL_SYM:
      x->sym = malloc(strlen(v->sym) + 1);
      strcpy(x->sym, v->sym); break;
    
    /* Copy Lists by copying each sub-expression */
    case LVAL_SEXPR:
    case LVAL_QEXPR:
      x->count = v->count;
      x->cell = malloc(sizeof(lval*) * x->count);
      for (int i = 0; i < x->count; i++) {
        x->cell[i] = lval_copy(v->cell[i]);
      }
    break;
  }
  
  return x;
}

lval* lval_add(lval* v, lval* x) {
  v->count++;
  v->cell = realloc(v->cell, sizeof(lval*) * v->count);
  v->cell[v->count-1] = x;
  return v;
}

lval* lval_join(lval* x, lval* y) {  
  for (int i = 0; i < y->count; i++) {
    x = lval_add(x, y->cell[i]);
  }
  free(y->cell);
  free(y);  
  return x;
}

lval* lval_pop(lval* v, int i) {
  lval* x = v->cell[i];  
  memmove(&v->cell[i], &v->cell[i+1],
    sizeof(lval*) * (v->count-i-1));  
  v->count--;  
  v->cell = realloc(v->cell, sizeof(lval*) * v->count);
  return x;
}

lval* lval_take(lval* v, int i) {
  lval* x = lval_pop(v, i);
  lval_del(v);
  return x;
}

void lval_print(lval* v);

void lval_print_expr(lval* v, char open, char close) {
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
    case LVAL_FUN:   printf("<function>"); break;
    case LVAL_NUM:   printf("%li", v->num); break;
    case LVAL_ERR:   printf("Error: %s", v->err); break;
    case LVAL_SYM:   printf("%s", v->sym); break;
    case LVAL_SEXPR: lval_print_expr(v, '(', ')'); break;
    case LVAL_QEXPR: lval_print_expr(v, '{', '}'); break;
  }
}

void lval_println(lval* v) { lval_print(v); putchar('\n'); }

char* ltype_name(int t) {
  switch(t) {
    case LVAL_FUN: return "Function";
    case LVAL_NUM: return "Number";
    case LVAL_ERR: return "Error";
    case LVAL_SYM: return "Symbol";
    case LVAL_SEXPR: return "S-Expression";
    case LVAL_QEXPR: return "Q-Expression";
    default: return "Unknown";
  }
}

/* Lisp Environment */

struct lenv {
  int count;
  char** syms;
  lval** vals;
};

lenv* lenv_new(void) {

  /* Initialize struct */
  lenv* e = malloc(sizeof(lenv));
  e->count = 0;
  e->syms = NULL;
  e->vals = NULL;
  return e;
  
}

void lenv_del(lenv* e) {
  
  /* Iterate over all items in environment deleting them */
  for (int i = 0; i < e->count; i++) {
    free(e->syms[i]);
    lval_del(e->vals[i]);
  }
  
  /* Free allocated memory for lists */
  free(e->syms);
  free(e->vals);
  free(e);
}

lval* lenv_get(lenv* e, lval* k) {
  
  /* Iterate over all items in environment */
  for (int i = 0; i < e->count; i++) {
    /* Check if the stored string matches the symbol string */
    /* If it does, return a copy of the value */
    if (strcmp(e->syms[i], k->sym) == 0) {
      return lval_copy(e->vals[i]);
    }
  }
  /* If no symbol found return error */
  return lval_err("Unbound Symbol '%s'", k->sym);
}

void lenv_put(lenv* e, lval* k, lval* v) {
  
  /* Iterate over all items in environment */
  /* This is to see if variable already exists */
  for (int i = 0; i < e->count; i++) {
  
    /* If variable is found delete item at that position */
    /* And replace with variable supplied by user */
    if (strcmp(e->syms[i], k->sym) == 0) {
      lval_del(e->vals[i]);
      e->vals[i] = lval_copy(v);
      return;
    }
  }
  
  /* If no existing entry found allocate space for new entry */
  e->count++;
  e->vals = realloc(e->vals, sizeof(lval*) * e->count);
  e->syms = realloc(e->syms, sizeof(char*) * e->count);
  
  /* Copy contents of lval and symbol string into new location */
  e->vals[e->count-1] = lval_copy(v);
  e->syms[e->count-1] = malloc(strlen(k->sym)+1);
  strcpy(e->syms[e->count-1], k->sym);
}

/* Builtins */

#define LASSERT(args, cond, fmt, ...) \
  if (!(cond)) { lval* err = lval_err(fmt, ##__VA_ARGS__); lval_del(args); return err; }

#define LASSERT_TYPE(func, args, index, expect) \
  LASSERT(args, args->cell[index]->type == expect, \
    "Function '%s' passed incorrect type for argument %i. Got %s, Expected %s.", \
    func, index, ltype_name(args->cell[index]->type), ltype_name(expect))

#define LASSERT_NUM(func, args, num) \
  LASSERT(args, args->count == num, \
    "Function '%s' passed incorrect number of arguments. Got %i, Expected %i.", \
    func, args->count, num)

#define LASSERT_NOT_EMPTY(func, args, index) \
  LASSERT(args, args->cell[index]->count != 0, \
    "Function '%s' passed {} for argument %i.", func, index);


lval* lval_eval(lenv* e, lval* v);

lval* builtin_list(lenv* e, lval* a) {
  a->type = LVAL_QEXPR;
  return a;
}

lval* builtin_head(lenv* e, lval* a) {
  LASSERT_NUM("head", a, 1);
  LASSERT_TYPE("head", a, 0, LVAL_QEXPR);
  LASSERT_NOT_EMPTY("head", a, 0);
  
  lval* v = lval_take(a, 0);  
  while (v->count > 1) { lval_del(lval_pop(v, 1)); }
  return v;
}

lval* builtin_tail(lenv* e, lval* a) {
  LASSERT_NUM("tail", a, 1);
  LASSERT_TYPE("tail", a, 0, LVAL_QEXPR);
  LASSERT_NOT_EMPTY("tail", a, 0);

  lval* v = lval_take(a, 0);  
  lval_del(lval_pop(v, 0));
  return v;
}

lval* builtin_eval(lenv* e, lval* a) {
  LASSERT_NUM("eval", a, 1);
  LASSERT_TYPE("eval", a, 0, LVAL_QEXPR);
  
  lval* x = lval_take(a, 0);
  x->type = LVAL_SEXPR;
  return lval_eval(e, x);
}

lval* builtin_join(lenv* e, lval* a) {
  
  for (int i = 0; i < a->count; i++) {
    LASSERT_TYPE("join", a, i, LVAL_QEXPR);
  }
  
  lval* x = lval_pop(a, 0);
  
  while (a->count) {
    lval* y = lval_pop(a, 0);
    x = lval_join(x, y);
  }
  
  lval_del(a);
  return x;
}

lval* builtin_op(lenv* e, lval* a, char* op) {
  
  for (int i = 0; i < a->count; i++) {
    LASSERT_TYPE(op, a, i, LVAL_NUM);
  }
  
  lval* x = lval_pop(a, 0);
  
  if ((strcmp(op, "-") == 0) && a->count == 0) {
    x->num = -x->num;
  }
  
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

lval* builtin_add(lenv* e, lval* a) {
  return builtin_op(e, a, "+");
}

lval* builtin_sub(lenv* e, lval* a) {
  return builtin_op(e, a, "-");
}

lval* builtin_mul(lenv* e, lval* a) {
  return builtin_op(e, a, "*");
}

lval* builtin_div(lenv* e, lval* a) {
  return builtin_op(e, a, "/");
}

lval* builtin_def(lenv* e, lval* a) {

  LASSERT_TYPE("def", a, 0, LVAL_QEXPR);
  
  /* First argument is symbol list */
  lval* syms = a->cell[0];
  
  /* Ensure all elements of first list are symbols */
  for (int i = 0; i < syms->count; i++) {
    LASSERT(a, (syms->cell[i]->type == LVAL_SYM),
      "Function 'def' cannot define non-symbol. "
      "Got %s, Expected %s.",
      ltype_name(syms->cell[i]->type), ltype_name(LVAL_SYM));
  }
  
  /* Check correct number of symbols and values */
  LASSERT(a, (syms->count == a->count-1),
    "Function 'def' passed too many arguments for symbols. "
    "Got %i, Expected %i.",
    syms->count, a->count-1);
  
  /* Assign copies of values to symbols */
  for (int i = 0; i < syms->count; i++) {
    lenv_put(e, syms->cell[i], a->cell[i+1]);
  }
  
  lval_del(a);
  return lval_sexpr();
}

void lenv_add_builtin(lenv* e, char* name, lbuiltin func) {
  lval* k = lval_sym(name);
  lval* v = lval_fun(func);
  lenv_put(e, k, v);
  lval_del(k); lval_del(v);
}

void lenv_add_builtins(lenv* e) {
  /* Variable Functions */
  lenv_add_builtin(e, "def", builtin_def);
  
  /* List Functions */
  lenv_add_builtin(e, "list", builtin_list);
  lenv_add_builtin(e, "head", builtin_head);
  lenv_add_builtin(e, "tail", builtin_tail);
  lenv_add_builtin(e, "eval", builtin_eval);
  lenv_add_builtin(e, "join", builtin_join);
  
  /* Mathematical Functions */
  lenv_add_builtin(e, "+", builtin_add);
  lenv_add_builtin(e, "-", builtin_sub);
  lenv_add_builtin(e, "*", builtin_mul);
  lenv_add_builtin(e, "/", builtin_div);
}

/* Evaluation */

lval* lval_eval_sexpr(lenv* e, lval* v) {
  
  for (int i = 0; i < v->count; i++) {
    v->cell[i] = lval_eval(e, v->cell[i]);
  }
  
  for (int i = 0; i < v->count; i++) {
    if (v->cell[i]->type == LVAL_ERR) { return lval_take(v, i); }
  }
  
  if (v->count == 0) { return v; }  
  if (v->count == 1) { return lval_take(v, 0); }
  
  /* Ensure first element is a function after evaluation */
  lval* f = lval_pop(v, 0);
  if (f->type != LVAL_FUN) {
    lval* err = lval_err(
      "S-Expression starts with incorrect type. "
      "Got %s, Expected %s.",
      ltype_name(f->type), ltype_name(LVAL_FUN));
    lval_del(f); lval_del(v);
    return err;
  }
  
  /* If so call function to get result */
  lval* result = f->fun(e, v);
  lval_del(f);
  return result;
}

lval* lval_eval(lenv* e, lval* v) {
  if (v->type == LVAL_SYM) {
    lval* x = lenv_get(e, v);
    lval_del(v);
    return x;
  }
  if (v->type == LVAL_SEXPR) { return lval_eval_sexpr(e, v); }
  return v;
}

/* Reading */

lval* lval_read_num(mpc_ast_t* t) {
  errno = 0;
  long x = strtol(t->contents, NULL, 10);
  return errno != ERANGE ? lval_num(x) : lval_err("Invalid Number.");
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

/* Main */

int main(int argc, char** argv) {
  
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Symbol = mpc_new("symbol");
  mpc_parser_t* Sexpr  = mpc_new("sexpr");
  mpc_parser_t* Qexpr  = mpc_new("qexpr");
  mpc_parser_t* Expr   = mpc_new("expr");
  mpc_parser_t* Lispy  = mpc_new("lispy");
  
  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                     \
      number : /-?[0-9]+/ ;                               \
      symbol : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;         \
      sexpr  : '(' <expr>* ')' ;                          \
      qexpr  : '{' <expr>* '}' ;                          \
      expr   : <number> | <symbol> | <sexpr> | <qexpr> ;  \
      lispy  : /^/ <expr>* /$/ ;                          \
    ",
    Number, Symbol, Sexpr, Qexpr, Expr, Lispy);
  
  puts("Lispy Version 0.0.0.0.7");
  puts("Press Ctrl+c to Exit\n");
  
  lenv* e = lenv_new();
  lenv_add_builtins(e);
  
  while (1) {
  
    char* input = readline("lispy> ");
    add_history(input);
    
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      lval* x = lval_eval(e, lval_read(r.output));
      lval_println(x);
      lval_del(x);
      mpc_ast_delete(r.output);
    } else {    
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    
    free(input);
    
  }
  
  lenv_del(e);
  
  mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Lispy);
  
  return 0;
}
```