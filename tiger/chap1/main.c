#include <stdio.h>

#include "util.h"
#include "slp.h"
#include "prog1.h"
#include "main.h"

int ID_VALID = 1;


Table_ Table(string id, int value, Table_ tail)
{
    Table_ t = checked_malloc(sizeof(*t));
    t->id=id;
    t->value=value;
    t->tail=tail;
    return t;
}

iTable_ iTable(int value, Table_ t)
{
    iTable_ it = checked_malloc(sizeof(*it));
    it->value = value;
    it->t = t;
    return it;
}

int exp_helper(A_expList list)
{
    switch(list->kind)
    {
    case A_pairExpList :
        return 1 + exp_helper(list->u.pair.tail);
    case A_lastExpList :
        return 1;
    default:
        return 0;
    }
}

int maxargs(A_stm prog)
{
    switch (prog->kind)
    {
    case A_compoundStm :
    {
        int left = maxargs(prog->u.compound.stm1);
        int right = maxargs(prog->u.compound.stm2);
        return (left > right)? left : right;
    }
    case A_assignStm:
    {
        A_exp exp = prog->u.assign.exp;
        if (exp->kind == A_eseqExp)
        {
            return maxargs(exp->u.eseq.stm);
        }
        else
        {
            return 0;
        }
    }
    case A_printStm:
    {
        A_expList list = prog->u.print.exps;
        return exp_helper(list);
    }
    default :
        return 0;
    }
}


int lookup(Table_ t, string key)
{
    if (t->id == key)
    {
        return t->value;
    }
    else
    {
        if (t->tail)
        {
            return lookup(t->tail, key);
        }
        else
        {
            ID_VALID = 0;
            return 1;
        }
    }
}

//int insert(Table_ t, string key, )
//{
//    if (t->id == key)
//    {
//        return t->value
//    }
//    else
//    {
//        return lookup(t->tail, key)
//    }
//}

Table_ update(string id, int value, Table_ t)
{
    return Table(id, value, t);
}


Table_ interpStm(A_stm s, Table_ t)
{
    switch (s->kind)
    {
    case A_compoundStm :
    {
        t = interpStm( s->u.compound.stm1 , t);
        t = interpStm( s->u.compound.stm2, t);
        return t;
    }
    case A_assignStm :
    {
        iTable_ exp = interpExp( s->u.assign.exp , t);
        return update(s->u.assign.id, exp->value, exp->t);
    }
    case A_printStm :
    {
        return interpExpList(s->u.print.exps, t);
    }
	default :
        return t;
    }
}


iTable_ interpExp(A_exp e, Table_ t)
{
    switch (e->kind)
    {
    case A_idExp :
    {
        int value = lookup(t, e->u.id);
        if (!ID_VALID)
        {
            printf("[Error] Identifier %s does not exist!\n", e->u.id);
            ID_VALID = 1;
        }
        return iTable(value, t);
    }
    case A_numExp :
    {
        return iTable(e->u.num, t);
    }
    case A_opExp :
    {
        iTable_ l = interpExp(e->u.op.left, t);
        iTable_ r = interpExp(e->u.op.right, l->t);
        iTable_ result = iTable(0, r->t);
        //A_binop op = e->u.op.oper;
        //TODO: ??? why this switch always run into each cases???
        switch (e->u.op.oper)
        {
            case A_plus:{
                result->value = l->value + r->value;
                break;
            }
            case A_minus:{
                result->value = l->value - r->value;
                break;
            }
            case A_times:{
                result->value = l->value * r->value;
                break;
            }
            case A_div:
            {
                if (r->value != 0)
                {
                    result->value = l->value / r->value;
                }
                else
                {
                    printf("[Error] Zero is divided by %d!\n", l->value);
                }
                break;
            }
        }
        return result;
    }
    case A_eseqExp :
    {
        t = interpStm(e->u.eseq.stm, t);
        return interpExp(e->u.eseq.exp, t);
    }
    default :
        return iTable(0, t);
    }
}

/*
 Since ExpList can only be derived by PrintStm,
 Stm -> print ( ExpList )  <==(PrintStm)
 ExpList -> Exp, ExpList   <==(PairExpStm)
 ExpList -> Exp            <==(LastExpStm)

 So at last, Stm -> print (Exp, Exp, ...) (1~n)
 no matter how much Exp have been dirived,
 just need to print value for each exp in PairExpStm & LastExpStm level.
 And when interpres, also need to carry the Table list,
 Exp -> ( Stm, Exp )  <==(EseqExp)
 since Exp can drive to Stm.
*/

Table_ interpExpList(A_expList e, Table_ t)
{
    switch (e->kind)
    {
    case A_pairExpList:
    {
        iTable_ it = interpExp(e->u.pair.head, t);
        printf("%d ", it->value);
        t = interpExpList(e->u.pair.tail, it->t);
        return t;
    }
    case A_lastExpList:
    {
        iTable_ it = interpExp(e->u.last, t);
        printf("%d ", it->value);
        return it->t;
    }
    default:
        return t;
    }
}

void interp(A_stm prog)
{
    interpStm(prog, NULL);
}


/*
 This Table is used to cache the id-value pairs that given by AssignStm.
 When to add item: in AssignStm;
 When to use the cache: in IdExp.

 Why use iTable?
 A Exp need to store the value of this exp.
*/

int main()
{
    printf(">> Prog Section:\n");
    A_stm p = prog();
    printf("the maximum number of arguments of any print statement is %d\n", maxargs(p));
    interp(p);

    printf(">> prog_prog section:\n");
    A_stm pp = prog_prog();
    printf("the maximum number of arguments of any print statement is %d\n",maxargs(pp));
    interp(pp);

    printf(">> Right Prog Section:\n");
    A_stm rp = right_prog();
    printf("the maximum number of arguments of any print statement is %d\n", maxargs(rp));
    interp(rp);

    printf(">> error prog section:\n");
    A_stm ep = error_prog();
    printf("the maximum number of arguments of any print statement is %d\n",maxargs(ep));
    interp(ep);
    return 0;
}
