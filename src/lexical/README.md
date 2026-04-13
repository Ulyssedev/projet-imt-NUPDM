run make lexical to test standalone lexical analyser.

A function name must be composed of characters in {[A-Z], [a-z], [1-9], _} and must be followed by a '('
A function name can't start with a digit, otherwise the lexer will try to parse it as a real number.

A real number must only contain digits, and can contain only one dot
0 can be ommited before or after the ., for example . is the same as 0.0, 3. is 3.0 and .3 is 0.3 (just like in c)

spaces and newlines are allowed between operators, variables, real numbers, and function names 
but aren't allowed in a function name or in a real number: 
for example this is legal :
sin   (  3.4 )
but this isn't:
s in( 3.4 )
the lexer will see s, guess that it should be a function and find that it does not end wih '(' and fail with an error.


sin(3 . 4)
is legal but it will be parsed as
SIN, PARENTHESE_OUVERTE, REEL : 3.0, REEL : 0.0, REEL : 4, PARENTHESE_FERMEE
The syntaxic analyser will then error because sin takes a single real.
This behaviour can be useful if we want multiple parameter functions in the future.

function name matching is case sensitive, as such cos is recognized but COS isn't

The variable must be noted x (lower case).

treats -3.4 as 2 separed tokens : - and 3.4, one of the reason is because the lexer is not supposed to know the priority of operators, for example in :
-3^4, 3^4 must be done before the minus is applied...

can output weird things like NUMBER NUMBER
or FUNCTION FUNCTION
or NUMBER FUNCTION ....
it's the role of the syntaxic analyser to handle these situations

x() will be recognized as a function named x (and error since no function with this name exists)