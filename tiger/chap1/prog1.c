#include "util.h"
#include "slp.h"

A_stm prog(void)
{
    // a := 5+3; b := (print(a, a-1), 10*a); print(b)
    return A_CompoundStm(A_AssignStm("a", A_OpExp(A_NumExp(5), A_plus, A_NumExp(3))),
                         A_CompoundStm(A_AssignStm("b",
                                       A_EseqExp(
                                           A_PrintStm(  // print a, a-1
                                                   A_PairExpList(  // a, a-1
                                                           A_IdExp("a"),
                                                           A_LastExpList(
                                                                   A_OpExp(A_IdExp("a"), A_minus, A_NumExp(1))   // a-1
                                                           )
                                                   )
                                           ),
                                           A_OpExp(A_NumExp(10), A_times, A_IdExp("a"))
                                       )
                                                  ),
                                       A_PrintStm(A_LastExpList(A_IdExp("b")))
                                      )
                        );
}



A_stm prog_prog(void)
{
// a = 5 + 3; b = (print(a, a-1), 10*a); print b;
// a = 5 + b; b = (print(a, (c=99, c), a, a-1), 3*a); print b;
//8 7 80
//85 99 85 84 255
    A_stm stm1 = prog();
    return
        A_CompoundStm( stm1,
            A_CompoundStm(A_AssignStm("a", A_OpExp(A_NumExp(5), A_plus, A_IdExp("b"))),
                          A_CompoundStm(A_AssignStm("b",
                                            A_EseqExp(A_PrintStm(
                                                        A_PairExpList(A_IdExp("a"),
                                                        A_PairExpList(A_EseqExp(A_AssignStm("c", A_NumExp(99)), A_IdExp("c")),
                                                        A_PairExpList(A_IdExp("a"),
                                                        A_LastExpList(A_OpExp(A_IdExp("a"), A_minus, A_NumExp(1))))))
                                                      ),
                                                      A_OpExp(A_NumExp(3), A_times, A_IdExp("a"))
                                            )
                                        ),
                                        A_PrintStm(A_LastExpList(A_IdExp("b")))
                          )
            )
        );
}

A_stm right_prog(void)
{
// a = 5 + 3; b = (print(a, a-1), 10*a); print b;
// a = 5 + b; b = (print(a, (c=99, c), a, a-1), 3*a); print b;
// a = (c = a+b, b-a); print (a, b, c);
//8 7 80
//85 99 85 84 255
//170 255 340
    A_stm stm1 = prog_prog();
    return
        A_CompoundStm( stm1,
            A_CompoundStm(A_AssignStm("a",
                                A_EseqExp(A_AssignStm("c", A_OpExp(A_IdExp("a"), A_plus, A_IdExp("b"))),
                                          A_OpExp(A_IdExp("b"), A_minus, A_IdExp("a"))
                                )
                          ),
                          A_PrintStm(
                                    A_PairExpList(A_IdExp("a"),
                                    A_PairExpList(A_IdExp("b"),
                                    A_LastExpList(A_IdExp("c"))))
                          )
            )
        );
}

A_stm error_prog(void)
{
// a = 5 + 3; b = (print(a, a-1), 10*a); print b;
// a = 5 + b; b = (print(a, (c=99, c), a, a-1), 3*a); print b;
//8 7 80
//85 99 85 84 255
// a = d;
    A_stm stm1 = prog_prog();
    return
        A_CompoundStm( stm1, A_AssignStm("a", A_IdExp("d")));

}
