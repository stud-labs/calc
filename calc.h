#include <string>

class ast;
class symlist;

class symbol {
public:
    std::string name;
    double value;
    ast *func;
    symlist *syms;
    symbol *next;
    symbol(std::string name);
    static symbol *head;
    static symbol *lookup( char *sym);
};

class symlist {
public:
    symbol *sym;
    symlist *next;
    symlist(symbol *sym, symlist *next);
    ~symlist();
};

enum bifs {
  B_sqrt = 1,
  B_exp,
  B_log,
  B_print
};

class ast {
public:
    int nodetype;
    ast(int nodetype):nodetype(nodetype){}
    virtual ~ast(){}
    virtual double eval(void)=0;
    virtual void print(int level)=0;
    void print_indent(int level);
};

class binop : public ast
{
public:
    ast *l;
    ast *r;
    binop( int nodetype, ast *l, ast *r);
    ~binop();
    double eval(void);
    void print(int level);
};

class unop : public ast
{
public:
    ast *r;
    unop(int nodetype, ast *r);
    ~unop();
    double eval(void);
    void print(int level);
};

class fncall : public ast
{
public:
    ast *l;
    bifs functype;
    fncall(bifs functype, ast *l);
    ~fncall();
    double eval(void);
    void print(int level);
};

class ufncall : public ast
{
public:
    ast *l;
    symbol *s;
    ufncall(symbol *s, ast *l);
    ~ufncall();
    double eval(void);
    void print(int level);
};

class flow : public ast
{
public:
    ast *cond;
    ast *tl;
    ast *el;
    flow(int nodetype, ast *cond, ast *tl, ast *el);
    ~flow();
    double eval(void);
    void print(int level);
};

class numval : public ast
{
public:
    double number;
    numval(double d);
    ~numval();
    double eval(void);
    void print(int level);
};

class symref : public ast
{
public:
    symbol *s;
    symref(symbol *s);
    ~symref();
    double eval(void);
    void print(int level);
};

class symasgn : public ast
{
public:
    symbol *s;
    ast *v;
    symasgn(symbol *s, ast *v);
    ~symasgn();
    double eval(void);
    void print(int level);
};


/* define a function */
void dodef(struct symbol *name, struct symlist *syms, struct ast *stmts);


    
