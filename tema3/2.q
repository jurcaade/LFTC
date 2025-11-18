//defVar ::= VAR ID COLON baseType SEMICOLON
var a: int;
var b: real;
a = 5;
b = 3.14;

//defFunc ::= FUNCTION ID LPAR funcParams? RPAR COLON baseType defVar* block END
function f(): int
    var x: int;
    x = 5;
    return x;
end

//block ::= instr+
if (a < b)
    a = a + 1;
else
    a = a - 1;
end

//expr 
z = suma(a, b);

//factor
5;
3.14;
"hello";
x;