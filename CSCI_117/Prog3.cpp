/* Gavin Haugen
lab 3
This lab is extending the interpreter built in the first two labs to accept the language FresnoF15.
*/

#include <sstream> //string stream
#include <cstdlib>
#include <fstream> //for streaming the file
#include <iostream> //for input and output
#include <cmath> //for the pow() function
#include <string> // for utilizing strings
#include <vector> // for using the vector of struct objects

using namespace std;

class Variable{

    public:
        char ID;
        string Type;
        string Value;

        //constructor
        Variable(char a, string b, string c)
        {
            ID = a;
            Type = b;
            Value = c;
        }
};

vector<Variable> Variable_Table;

//function declarations for lab 2
int Exp(), Term(), Exp2(int), Term2(int), Fact(), Pwr();

//function declarations for lab 3
void Declaration(), Declarations(), Statement(), Statements(), input_stmnts(), output_stmnts(), assign_stnmts(char), Run_Program(), IDList(string);
//helper functions used in the functions above
bool ValidVariableName(char), ValidType(string);
string IntToStr(int);
int StrToInt(string);
void IsInt(char);
Variable &AccessVariable(char);

//streaming the input for each expression in each seperate text file
ifstream MyData;

//this was used because I was having issues with the file keeping the data so it could be used again.
stringstream ExpressionStream;

int main(int argc, char* argv[])
{
   //These for loops builds our Variable Table in one pass so we can just add values to the ID's if found in the doucment
   //26 is used as a bound for 26 letters in the alphabet
   //the first for loop uses i+65 as an offset for the lower case letters
   //the second for loop uses i+97 as an offset for the upper case letters
   for(int i = 0; i < 26; i++)
   {
        Variable_Table.push_back(Variable(i+65,"None","None"));
   }

   for(int i = 0; i < 26; i++)
   {
        Variable_Table.push_back(Variable(i+97,"None","None"));
   }


   //opening the file to be used
   string FileName = "Prog3Data.txt";
   MyData.open(FileName.c_str());
   if(!MyData.is_open())
   {
        cerr << "Unable to open file " << FileName << endl;
   }

    Run_Program();

}


//----------------------------------------------
//----------------------------------------------
//OLD FUNCTIONS FROM PREVIOUS LAB
//----------------------------------------------
//----------------------------------------------

//returns the calculated expression from the data file
//Deals with the addition and subtraction operations
int Exp()
{
	return (Exp2(Term()) );
}

//returns the calculated Term to be included with the Expression
//Deals with the multiplication and division operations
int Term()
{
	return (Term2(Pwr()) );
}

//Checks the characters for addition or subtraction
//After it has checked for the two specific characters, it
//either adds or subtracts it from the term in the expression
//if there is one
int Exp2(int inp)
{
	int result = inp;
	char a;
	//checking if the character is not a new line
	if (!ExpressionStream.eof())
	{
		ExpressionStream.get(a);
		//if '+' then add to the rest of the calculated term
		if (a == '+')
			result = Exp2(result + Term());
		//if '-' then subtract from the rest of the calculate term
		else if (a == '-')
			result = Exp2(result - Term());
		else if (a == ')')
			ExpressionStream.putback(a);
	}
	return result;
}


//Checks the characters for the multiplication or division char
//if a isnt a new line then the function checks whether its
// '*' or '/' then computes it with the Factor of a
int Term2(int inp)
{
	int result = inp;
	char a;
	//checking if a isnt a new line
	if (!ExpressionStream.eof() )
	{
		ExpressionStream.get(a);
		//if '*' then multiply to the Factor
		if (a == '*')
			result = Term2(result * Pwr());
		//if '/' then divide by the Factor
		else if (a == '/')
			result = Term2(result / Pwr());
		//if the character is '+' or '-' then the function
		//ungets the character and moves on with the prog
		else if (a == '+' || a == '-' || a ==')')
			ExpressionStream.putback(a);
	}
	return result;
}

int Pwr(){
	int p = Fact();
	char a;

	if(!ExpressionStream.eof() ){
		ExpressionStream.get(a);

		if (a == '^')
			p = pow(p, Pwr());
		if (a == '+' || a == '-' || a == '*' || a == '/' || a == ')')
			ExpressionStream.putback(a);
	}
	return p;
}


//converts the string input into its numerical equivelance for calculation purposes
int Fact()
{
	char a = ExpressionStream.get();
	//this if branch handles the begining of the paraenthaes experession and handles it as its own seperate expression
	//to add to the result
	if (a == '('){
        int ExpTemp = Exp(); //this is creating a seperate "result" variable for the expression inside of the parenthases
        ExpressionStream.get(a);
        if (a == ')'){
        	return ExpTemp;//after Exp() is done calculating it returns that value and moves on with the expression
        }
    }
	return (atoi(&a) );
}


//----------------------------------------
//END OF OLD FUNCTIONS
//----------------------------------------


//----------------------------------------
//START OF NEW LAB3 FUNCTIONS
//-----------------------------------------

void Run_Program()
{
    string first_word; //this grabs the first word from the document
    MyData >> first_word;
    if(first_word != "program")
    {
        cout << "Expected 'program'" << endl;
        exit(1);
    }
    Declaration(); //runs the declarations
    Statement();//runs the statement
}

void Declaration()
{
    string word; //again, grabs the word
    MyData >> word; //iterates through the file
    if(word == "begin")
    {
        return; //if the declarations are done, then you just return
    }
    else if(ValidType(word))
    {
        IDList(word); //else you run the ID List if valid type
    }

    Declaration(); //recurse recurse recurse!
}

//This helper function checks if the variable name is valid for our language then takes the appropriate action and recurses until there is nothing left to check
void IDList(string type)
{
    char letter; //grabs the word
    MyData >> letter;
    if(letter == ',') //checks for a comma, if its there then it keeps going through the document
    {
        IDList(type);
        return;
    }

    else if(letter == ';') //finds a semicolon so it ends the recursion
    {
        return;
    }

    else if(ValidVariableName(letter))// checking to see if its a valid variable name in our language
    {
        AccessVariable(letter).Type = type;
        //accesses our variable in the vector and uses it for later recursion
    }

    IDList(type); //recurse recurse recurse!
}



void Statement()
{
    string word;
    MyData >> word; //Grabs the word
    if(word == "end")
    {
        return;
    }

    else if(word.size() == 1)//case for grabbing a variable
    {
        char varletter;
        MyData >> varletter;
        if(varletter == '=')
        {
            assign_stnmts(word[0]); //assignment statement
        }
        else
        {
            cout << "Invalid Statement\n"; //checks for invalid statement
            exit(1);
        }
    }
    //checking for different parts of the statement
    else if(word == "input")
    {
        input_stmnts();
    }

    else if(word == "output")
    {
        output_stmnts();
    }

    else
    {
        cout << "Invalid Statement\n";
        exit(1);
    }

    Statement(); //recurse!
}

void assign_stnmts(char ID)
{

   //First we check if we can store numbers
   if(AccessVariable(ID).Type != "var")
   {
        cout << "Type Mismatch\n";
        exit(1);
   }

   string tempWord;
   MyData >> tempWord;
   if((tempWord[0] + 0 >= 48 && tempWord[0] + 0 <= 57) || tempWord[0] == '('){}
   else
   {
        cout << "This is not an expression\n";
        exit(1);
   }

   ExpressionStream.clear();
   ExpressionStream.str(string());
   ExpressionStream.str(tempWord);
   int temp = Exp();

   AccessVariable(ID).Value = IntToStr(temp);

}

void input_stmnts()
{

    char var, semicolon;
    MyData >> var; // Grabs the variable
    MyData >> semicolon; // grabs the semicolon for check
    ValidVariableName(var); //checking to see if valid variable name
    if(semicolon != ';')
    {
        cout << "Expected a ';'\n";
        exit(1);
    }

    cout << "Please enter a value for " << AccessVariable(var).ID << endl;
    cin >> AccessVariable(var).Value; //input the value for the variable
    if(AccessVariable(var).Type == "var")
    {
        IsInt(var); //checking the var to see if it really is an integer
    }

}

//Checks value for integer type
void IsInt(char var)
{
    //Comparing the Var value to its string -> int -> string conversion
    if(AccessVariable(var).Value != IntToStr(StrToInt(AccessVariable(var).Value)))
    {
        cout << "Type Mismatch Error\n";
        exit(1);
    }
}

//Converts Int to Str
string IntToStr(int input)
{
    string result = "";
    while(input > 0)
    {
        result.push_back((input % 10)+48);
        input/=10;
    }

    string result2;
    for(int i = result.size()-1; i >= 0; i--)
    {
        result2.push_back(result[i]);
    }
    return result2;
}

int StrToInt(string input)
{
    int result = 0;
    for(int i = 0; i <input.size(); i++)
    {
        result += (input[i] - 48);
        result *= 10;
    }
    result /= 10;
    return result;
}


//Outputs the Variable value unless a semicolon is not found
void output_stmnts()
{
        char var, semicolon; /* Used for parsing through the document looking for different things
                                in this example we are looking for the var and semicolon*/
        MyData >> var;
        MyData >> semicolon;

        if(semicolon != ';')
        {
            cout << "Expected a ';'\n";
            exit(1);
        }

        cout << AccessVariable(var).Value <<endl;

}

//Accessing the Variables
Variable &AccessVariable(char input)
{
    if(ValidVariableName(input)) //Checking to see if the name of Variable is valid
    {
        if(input+0 <= 90)
        {
            return Variable_Table[input - 65]; //Offset from Uppercase
        }

        return Variable_Table[input-71]; //Offset from Lowercase
    }
}

//Checks the ID too see if its a valid type name
bool ValidType(string input)
{
    if(input == "var") //var is the only valid type name in lang
    {
        return true;
    }

    cout << "This is an invalid variable type " << input << endl; exit(1);
}

//Checks the ID to see if its a valid variable name
bool ValidVariableName(char input)
{
    //Checking to see if in ASCII range, if so it returns true
    if((input + 0 >= 65 && input + 0 <= 90) || (input + 0 >= 97 && input + 0 <= 122))
    {
        return true;
    }
    else
    {
    //if not then display this error
    cout << "Invalid variable ID\n";
    exit(1);
    }
}
