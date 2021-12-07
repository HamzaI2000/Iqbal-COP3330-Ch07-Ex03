#include <iostream>

const char isConst = 'C';
const string constKey = "const";
if(s == constKey)
return Token(isConst);
 
struct Variable 
{
 string name;
 double value;
 bool isConst;
 Variable(string n, double v, bool ic) :name(n), value(v), isConst(ic) { }
};

Set_Value() was then changed to check if the variable being set was not a const.
 
//set the Variable named s to d
void set_value(string s, double d)
{
 for (int i = 0; i <= names.size(); ++i)
 {
  //allow redefinitions as long as variable isn't const
  if (names[i].name == s && names[i].isConst == false) 
  {
   names[i].value = d;
   return;
  }
 }

 error("set: undefined name ", s);
}

is_declared() and define_name() were also modified to make sure that the already-defined variable trying to be redefined is not a const variable:
 
bool is_declared(string s)
{
 for (int i = 0; i < names.size(); ++i)
 {
  if (names[i].name == s && names[i].isConst == true)
   error("Cannot reassign const variable");
  else if (names[i].name == s && names[i].isConst == false)
   return true;
 }

 return false;
}

//allow programmers to add (var,val) to variable vector
double define_name(string var, double val, bool isConst)
{
 if (is_declared(var))
  error(var, " declared twice");

 names.push_back(Variable(var, val, isConst));

 return val;
}


double declaration()
{
 Token t = ts.get();

 //is const the next word in the stream?
 bool isC;
 if (t.kind == 'C')
 {
  isC = true;
  t = ts.get();  //get the the next word in the stream for the name
 }
 else
  isC = false;

 if (t.kind != 'a') 
  error("name expected in declaration;");

 string name = t.name;
 if (is_declared(name))
 {
  cout << name + ", declared twice. Would you like to reassign? (No need to print with ';') y/n > ";
  cin.clear();
  cin.ignore(10000, '\n'); //clear the buffer
  string ans;
  getline(cin, ans);
  if (ans == "n")
   error(name, ", will not be reassigned; ");
  if (ans == "y")
  {
   cout << "(No need to print with ';') Please enter new value: ";
   int val;
   cin >> val;
   set_value(name, val);
   double d = val; //return value to print to reset calculator
   return d;

}

 }

 Token t2 = ts.get();
 if (t2.kind != '=') 
  error("= missing in declaration of ", name);

 double d = expression();
 names.push_back(Variable(name, d, isC));

 return d;
}
