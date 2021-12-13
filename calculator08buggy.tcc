/*
 *  UCF COP3330 Fall 2021 Assignment 6 Solution
 *  Copyright 2021 Hamza Iqbal
 */

#include "std_lib_facilities.h"


struct Token {
    char kind;
    double value;
    string name;
    Token() : kind{0} {}
    Token(char ch) : kind{ch} {}                          
    Token(char ch, double val) : kind{ch}, value{val} {}  
    Token(char ch, string n) : kind{ch}, name{n} {}       
};

class Token_stream {
   public:
  
    Token_stream() : full{false}, buffer{0} {}
    Token get();         
    void unget(Token t) { buffer = t; full = true; }
    void ignore(char c);

private:
    bool full;
    Token buffer;
};

const char let = '#';
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';
const char sqrtfunc = 's';
const char powfunc = 'p';
const string prompt = "> ";
const string result = "= ";
// define additional constants
const string quitkey = "quit";
const string sqrtkey = "sqrt";
const string powkey = "pow";


Token Token_stream::get() {
    
    if (full)  {
        full = false;
        return buffer;
    }

    char ch;
    cin >> ch;  

    switch (ch) {
        case quit:
        case print:
        case ',':
        case '#':
        case '(':
        case ')':
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '=':
            // designates each character as its own value.
            return Token(ch);
        // decimal added in case a non integer value is used.
        case '.':
        // switch statement for integers 0 - 9.
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
            // replaces the value into the input stream.
            cin.unget();
            double val;
            // reads the input.
            cin >> val;
            return Token(number, val);
        }
    
        default:
            if (isalpha(ch)) {
                string s;
                
                s = s + ch;
              
                while (cin.get(ch) && ((isalpha(ch) || isdigit(ch) || ch == '_'))) {
                    s = s + ch;
                }
                cin.unget();
                if (s[0] == let)
                    return Token{let};  // declaration keyword 'let.'
                else if (s == quitkey)
                    return Token{name}; 
                else if (s == sqrtkey)
                    return Token{sqrtfunc}; // declaration sqrt function.
                else if (s == powkey)
                    return Token{powfunc}; // declaration pow function.

                return Token{name, s};
            }

            error("Error: The token was corrupt.");
    }
    return Token{};
}

void Token_stream::ignore(char c)
{
     // check to see if the buffer is full.
    if (full && c == buffer.kind) {
        full = false;
        return;
    }
    full = false;

    // search for input.
    char ch = 0;
    while (cin >> ch)
        if (ch == c) return;
}

struct Variable {
    string name;
    double value;
    Variable(string n, double v) : name(n), value(v) {}
};



vector<Variable> var_table;

double get_value(string s)
{
    for (int i = 0; i < var_table.size(); ++i)
        if (var_table[i].name == s) return var_table[i].value;
    error("get: undefined name ", s);
    return -1;
}

void set_value(string s, double d)
{
    for (int i = 0; i <= var_table.size(); ++i)
        if (var_table[i].name == s) {
            var_table[i].value = d;
            return;
        }
    error("set: undefined name ", s);
}

bool is_declared(string s)
{
    for (int i = 0; i < var_table.size(); ++i)
        if (var_table[i].name == s) return true;
    return false;
}


Token_stream ts;      // calls Token_stream Get() and unget();
double expression(); 
// evaluates function of t.kind == c
double eval_function(char c) {
    // for function arguments
    vector<double> func_args;

    Token t = ts.get();
    if (t.kind != '(')
        error("'(' expected after function");

    switch (c) {
        case sqrtfunc:
            func_args.push_back(expression());
            t = ts.get();
            if (t.kind != ')')
                error("')' expected after function");
            // if statement to ensure that no negative numbers are accepted under the square root.
            if (func_args[0] < 0)
                error("error: a square root of a negative number is not a real number.");
            return sqrt(func_args[0]);
        // switch statement to handle the power function.
        case powfunc:
            func_args.push_back(expression());
            t = ts.get();
            if (t.kind != ',')
                error("error: power function arguments must be input in the following format pow(2, 3)");
            func_args.push_back(expression());
            t = ts.get();
            if (t.kind != ')')
                error("')' expected after function");
            // if(args[1]-narrow_cast<int>(args[1])!=0)
            // error();
            return pow(func_args[0], func_args[1]);
        // default switch statement/
        default:
            error("error: this function does not exist.");
            return -1;
    }
}

double primary() {
    Token t = ts.get();
    switch (t.kind) {
        // switch statement to handle the '(' ')' expression.
        case '(': {
            double d = expression();
            t = ts.get();
            /* BUGS: incorrect 'if statement' if (t.kind != ')') error("'(' expected");
            should be 'if (t.kind != ')') error("')' expected");' and the proper changes
            were made in the below code. */
            if (t.kind != ')') error("')' expected");
            return d;
        }
        // switch statement to handle subtraction.
        case '-':
            return -primary();

        // switch statement to handle addition.
        case '+':
        {
            return primary();
        }

        case number:
            return t.value;

        case name:
            return get_value(t.name);
        case sqrtfunc:
        case powfunc:
            return eval_function(t.kind);
        // default switch statement indicating error.
        default:
            error("error: primary expected");
            return -1;
    }
}

double term()
{
    double left = primary();
    Token t = ts.get();

    while (true) {
        switch (t.kind) {
            // switch statement to handle multiplication symbol.
            case '*':
                left *= primary();
                t = ts.get();
                break;
            // switch statement to handle division symbol.
            case '/': {
                double d = primary();
                // if denominator is zero.
                if (d == 0)
                
                    error("/ : divide by zero");
                left /= d;
                t = ts.get();
                break;
            }
            default:
                ts.unget(t);
                return left;
        }
    }
}


double expression() {
    double left = term();
    Token t = ts.get();

    while (true) {
        switch (t.kind) {
            // switch statement for addition.
            case '+':
                left += term();
                t = ts.get();
                break;
            // switch statement for subtraction.
            case '-':
                left -= term();
                t = ts.get();
                break;
            default:
                ts.unget(t);
                return left;
        }
    }
}

double declaration() {
    Token t = ts.get();

    if (t.kind != 'a')
        error("name expected in declaration");
    string name = t.name;
    // if a name is repeated, print an error message saying the name is repeated.
    if (is_declared(name))
        error(name, " declared twice");
    Token t2 = ts.get();
    // if the '=' sign is not used, print an error message.
    if (t2.kind != '=')
        error("= missing in declaration of ", name);
    double d = expression();
    var_table.push_back(Variable(name, d));
    return d;
}

double statement() {
    Token t = ts.get();
    switch (t.kind) {
        case let:
            return declaration();
        default:
            ts.unget(t);
            return expression();
    }
}

void clean_up_mess() {
    ts.ignore(print);
}

void calculate() {
    while (true) try {
            cout << prompt;
            Token t = ts.get();
            while (t.kind == print) t = ts.get();
            if (t.kind == quit) return;
            ts.unget(t);
            cout << result << statement() << endl;
        } catch (runtime_error &e) {
            cerr << e.what() << endl;
            clean_up_mess();
        }
}


double define_name(string var, double val) {
    if (is_declared(var))
        error(var, " declared twice");
    var_table.push_back(Variable{var, val});
    return val;
}

int main() {
    try {
        define_name("k", 1000);
        define_name("pi", 3.14);
        define_name("e", 2.72);
        calculate();
        return 0;
    } catch (exception &e) {
        cerr << "exception: " << e.what() << endl;
        char c;
        while (cin >> c && c != ';');
        return 1;
    } catch (...) {
        cerr << "exception\n";
        char c;
        while (cin >> c && c != ';');
        return -1;
    }
}
