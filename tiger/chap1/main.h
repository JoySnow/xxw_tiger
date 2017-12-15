typedef struct table *Table_;
typedef struct IntAndTable *iTable_;
// Define this struct
struct table
{
    string id;
    int value;
    Table_ tail;
};

// init a such struct
Table_ Table(string id, int value, struct table *tail);

struct IntAndTable
{
    int value;
    Table_ t;
};
iTable_ iTable(int value, Table_ t);

Table_ interpExpList(A_expList e, Table_ t);
Table_ interpStm(A_stm s, Table_ t);
iTable_ interpExp(A_exp e, Table_ t);
Table_ interpExpList(A_expList e, Table_ t);

