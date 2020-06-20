#include "calc.h"
#include <stdio.h>
#include <cmath>
#include <stdarg.h>

symbol *symbol::head = NULL;

symbol::symbol(std::string name):
name(name){
    value = 0.0;
    func = NULL;
    syms = NULL;
    
    next = head;
    head = this;
}

symbol* symbol::lookup( char *sym_char){
    std::string sym = sym_char;
    
    symbol *s = head;
    while(s){
        if(s->name.compare(sym)==0){
            return s;
        }
        s = s->next;
    }
    // didn't find the symbol
    // allocate a new one
    return new symbol(sym);
}

symlist::symlist(symbol *sym, symlist *next):
sym(sym),
next(next){}

symlist::~symlist()
{
    if(next) delete next;
}

void ast::print_indent(int level){
    printf("%*.s",2*level,"");
}

binop::binop(int nodetype, ast *l, ast *r):
ast(nodetype),
l(l),
r(r){}

binop::~binop(){
    delete l;
    delete r;
}

double binop::eval(void){
    switch(nodetype){
        case '+':
            return l->eval() + r->eval();
        case '-':
            return l->eval() - r->eval();
        case '*':
            return l->eval() * r->eval();
        case '/':
            return l->eval() / r->eval();
        case 'L':
            l->eval();
            return r->eval();
        case '1':
            return (l->eval() > r->eval())? 1.0 : 0.0;
        case '2':
            return (l->eval() < r->eval())? 1.0 : 0.0;
        case '3':
            return (l->eval() != r->eval())? 1.0 : 0.0;
        case '4':
            return (l->eval() == r->eval())? 1.0 : 0.0;
        case '5':
            return (l->eval() >= r->eval())? 1.0 : 0.0;
        case '6':
            return (l->eval() <= r->eval())? 1.0 : 0.0;
    }
    return 0.0;
}

void binop::print(int level){
    print_indent(level);
    switch(nodetype){
        case '+':
        case '-':
        case '*':
        case '/':
            printf("'%c'\n",nodetype);
            break;
        case 'L':
            printf("list\n");
            break;
        case '1':
            printf("greater than\n");
            break;
        case '2':
            printf("less than\n");
            break;
        case '3':
            printf("not equal\n");
            break;
        case '4':
            printf("equal\n");
            break;
        case '5':
            printf("greater than or equal");
            break;
        case '6':
            printf("less than or equal");
            break;
    }
    level++;
    l->print(level);
    r->print(level);
}

unop::unop(int nodetype, ast *r):
ast(nodetype),
r(r){}

unop::~unop(){
    delete r;
}

double unop::eval(void){
    switch(nodetype){
        case '|':
            return fabs(r->eval());
        case 'M':
            return -r->eval();
    }
    return 0.0;
}

void unop::print(int level){
    print_indent(level);
    switch(nodetype){
        case '|':
            printf("absolute value of\n");
            break;
        case 'M':
            printf("negative of\n");
            break;
    }
    level++;
    r->print(level);
}

fncall::fncall(bifs functype, ast *l):
ast('F'),
l(l),
functype(functype){}

fncall::~fncall(){
    delete l;
}

double fncall::eval(void){
    double v = l->eval();
    switch(functype){
        case B_sqrt:
            return sqrt(v);
        case B_exp:
            return exp(v);
        case B_log:
            return log(v);
        case B_print:
            printf("= %4.4g\n", v);
            return v;
    }
    return v;
}

void fncall::print(int level){
    print_indent(level);
    switch(functype){
        case B_sqrt:
            printf("sqrt\n");
            break;
        case B_exp:
            printf("exp\n");
            break;
        case B_log:
            printf("log\n");
            break;
        case B_print:
            printf("print\n");
            break;
    }
    level++;
    l->print(level);
}

ufncall::ufncall(symbol *s, ast *l):
ast('C'),
l(l),
s(s)
{}

ufncall::~ufncall(){
    delete l;
}

double ufncall::eval(void){
    symlist *sl;
    ast *args = l;
    double *oldval;
    double *newval;
    double v;
    int nargs;
    int i;
    
    if(!s->func){
        printf("call to undefined function:%s\n",s->name.c_str());
        return 0.0;
    }
    
    /* count the number of arguments */
    sl = s->syms;
    for(nargs=0; sl; sl=sl->next)
        nargs++;
    
    oldval = new double[nargs];
    newval = new double[nargs];
    
    if(!oldval || !newval){
        printf("out of memory\n");
        if(oldval) delete [] oldval;
        if(newval) delete [] newval;
        return 0.0;
    }
    
    /* evaluate the arguments */
    for(i=0; i<nargs; i++){
        if(!args){
            printf("too few args in call to %s\n", s->name.c_str());
            delete [] oldval;
            delete [] newval;
            return 0.0;
        }
        
        if(args->nodetype == 'L'){
            binop *bop = dynamic_cast<binop*>(args);
            newval[i] = bop->l->eval();
            args = bop->r;
        }else{
            newval[i] = args->eval();
            args = NULL;
        }
    }
    
    /* save old values of dummies, assign new ones */
    sl = s->syms;
    for(i=0; i<nargs; i++){
        symbol *s = sl->sym;
        
        oldval[i] = s->value;
        s->value = newval[i];
        sl = sl->next;
    }
    
    delete [] newval;
    
    /* evaluate the function */
    v = s->func->eval();
    
    /* put the dummies back */
    sl = s->syms;
    for(i=0; i<nargs; i++){
        symbol *s = sl->sym;
        
        s->value = oldval[i];
        sl = sl->next;
    }
    
    delete [] oldval;
    
    return v;
}

void ufncall::print(int level){
    print_indent(level);
    printf("User defined function call:%s\n",s->name.c_str());
    print_indent(level);
    printf("Arguments:\n");
    level++;
    l->print(level);
}

flow::flow(int nodetype, ast *cond, ast *tl, ast *el):
ast(nodetype),
cond(cond),
tl(tl),
el(el){}

flow::~flow(){
    if(cond) delete cond;
    if(tl) delete tl;
    if(el) delete el;
}

double flow::eval(void){
    double v=0.0;
    switch(nodetype){
        case 'I':
            if( cond->eval() != 0 ){
                if(tl){
                    v = tl->eval();
                }
            }else{
                if(el){
                    v = el->eval();
                }
            }
            break;
        case 'W':
            if(tl){
                while( cond->eval() != 0 ){
                    v = tl->eval();
                }
            }
            break;
    }
    
    return v;
}

void flow::print(int level){
    print_indent(level);
    switch(nodetype){
        case 'I':
            printf("if statement\n");
            break;
        case 'W':
            printf("while statement\n");
            break;
    }
    print_indent(level);
    printf("condition:\n");
    cond->print(level+1);
    if(tl){
        print_indent(level);
        printf("true block:\n");
        tl->print(level+1);
    }
    if(el){
        print_indent(level);
        printf("else block:\n");
        el->print(level+1);
    }
}

numval::numval(double d):
ast('K'),
number(d){}

numval::~numval(){
}

double numval::eval(void){
    return number;
}

void numval::print(int level){
    print_indent(level);
    printf("number:%4.4g\n",number);
}

symref::symref(symbol *s):
ast('N'),
s(s){}

symref::~symref(){
}

double symref::eval(void){
    return s->value;
}

void symref::print(int level){
    print_indent(level);
    printf("symbol reference:%s\n",s->name.c_str());
}

symasgn::symasgn(symbol *s, ast *v):
ast('='),
s(s),
v(v){}

symasgn::~symasgn(){
    delete v;
}

double symasgn::eval(void){
    double r = v->eval();
    s->value = r;
    return r;
}

void symasgn::print(int level){
    print_indent(level);
    printf("%s =\n",s->name.c_str());
    v->print(level+1);
}

void dodef(struct symbol *name, struct symlist *syms, struct ast *stmts)
{
    if(name->syms) delete name->syms;
    if(name->func) delete name->func;
    
    name->syms = syms;
    name->func = stmts;
}

void
yyerror(const char *s, ...)
{
  va_list ap;
  va_start(ap, s);

  fprintf(stderr, "error: ");
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}

         



