#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<map>
#include<stack>

using namespace std;

class CToken
{
public:
    string tokenValue;
    string tokenType;
};

class CLex
{

private:

    //const int numStates = 11;
    int rows = 128;
    int col = 11;
    vector<vector<int>> DFA;

    ifstream lex;


public:


    CLex(string lexname)
    {
        DFA.resize(rows, vector<int>(col));
        ifstream dfafile("DFA.txt");
        if (!dfafile)
        {
            cout << "DFA file not opening" << endl;
        }


        for (int i = 0; i < 128; i++)
        {
            for (int j = 0; j < col; j++)
            {
                dfafile >> DFA[i][j];

            }
        }
        dfafile.close();
         
        lex.open(lexname);
        if (!lex)
        {
            cout << "Lex file not opening" << endl;
        }

    }

    bool GetToken(CToken& ctoken)
    {

        int CurrentState = 0;
        int PrevState = 0;
        ctoken.tokenType.clear();
        ctoken.tokenValue.clear();
        char ch;
        while (!lex.eof())
        {
            ch = lex.get();
            int acsii = static_cast<int>(ch);
            PrevState = CurrentState;

            if (ch == -1)
            {
                break;
            }

            CurrentState = DFA[acsii][CurrentState];
            if (CurrentState != 0 && CurrentState != 55 && CurrentState != 99)
            {
                ctoken.tokenValue += acsii;
            }
            else if (CurrentState == 55 && acsii != 32)
            {
                lex.unget();
            }
            if (CurrentState == 55 || CurrentState == 10)
            {
                if (PrevState == 1)
                {
                    ctoken.tokenType = "word";
                }
                else if (PrevState == 2)
                {
                    ctoken.tokenType = "integer";
                }
                else if (PrevState == 9 || PrevState == 4)
                {
                    ctoken.tokenType = "real";
                }
                else if (PrevState == 10 || PrevState == 0)
                {
                    ctoken.tokenType = "special";
                }

                return true;
            }
            else if (CurrentState == 99)
            {
                cout << " lex error at: " << ctoken.tokenValue << endl;
                return false;
            }

        }

        if (PrevState == 1 || PrevState == 2 || PrevState == 4 || PrevState == 9 || PrevState == 10 || PrevState == 55)
        {
            if (PrevState == 1)
            {
                ctoken.tokenType = "word";
            }
            else if (PrevState == 2)
            {
                ctoken.tokenType = "integer";
            }
            else if (PrevState == 4 || PrevState == 9)
            {
                ctoken.tokenType = "real";
            }
            else if (PrevState == 10 || PrevState == 55)
            {
                ctoken.tokenType = "special";
            }

        }
        else
        {
            if (lex.eof())
            {
                return false;
            }
            cout << "Lex error at " << ctoken.tokenValue << endl;
            return false;
        }
        return true;


    }


};

class CParcer
{
    
    CLex clex;
    CToken ctoken;

private:
    map<string, int> productions;
    map<string, int> keys;
    vector<vector<char>> opc;
    int DToken;
    stack<string> thestack;
    int rows = 14;
    int col = 14;
	
	

public:
     

	CParcer(string prod): clex(prod)
	{
        ifstream prodfile("productions.txt");
        string value, key;
        for (int i = 0; i < 13; i++)
        {
            prodfile >> value;
            string str = value.substr(0, 2);
            int strint = stoi(str);
            key = value.substr(2, 12);
            productions[key] = strint;
            


        }
        prodfile.close();

        ifstream keysfile("key.txt");
        string k;
        int v;
        for (int i = 0; i < 14; i++)
        {
            if (keysfile >> v)
            {
                if (keysfile >> k)
                {
                    keys[k] = v;
                }
            }
        }
        keysfile.close();

        opc.resize(rows, vector<char>(col)); 
        ifstream opcfile("opc.txt");
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < col; j++)
            {
                opcfile >> opc[i][j];

            }
        }

        //opc.resize(rows, vector<char>(col));
        //ifstream opcfile("opc.txt");
        //for (int i = 0; i < rows; i++) {
        //    string line;
        //    getline(opcfile, line); // Read entire line
        //    for (int j = 0; j < col; j++) {
        //        opc[i][j] = line[j]; // Assign each character
        //    }
        //}

        opcfile.close();



	}


    void Parce()
    {

        DToken = clex.GetToken(ctoken);
        int column;
        if (ctoken.tokenType == "special")
        {
            column = keys[ctoken.tokenValue];
        }
        else
        {
            column = keys[ctoken.tokenType];

        }
        int Row;
        char Action;
        int size;
        string padcol;
        string production;
        int RC;

        while (true)
        {
            if (thestack.empty())
            {
                Row = 14;

            }
            else
            {
                size = thestack.top().size();
                Row = stoi(thestack.top().substr(size - 2, 2));
            }
            Action = opc[Row-1][column-1];
            switch (Action)
            {
            case'<':
                padcol = to_string(column);
                if (padcol.size() < 2)
                {
                    padcol = '0' + padcol;
                }
                production = padcol;
                thestack.push(padcol);
                DToken = clex.GetToken(ctoken);
                if (ctoken.tokenType == "special")
                {
                    column = keys[ctoken.tokenValue];
                }
                else if(DToken == 0)
                {

                    column = 14;

                }
                else if (ctoken.tokenType != "special")
                {
                    column = keys[ctoken.tokenType];
                }
                
                
                break;
            case'=':
                padcol = to_string(column);
                if (padcol.size() < 2)
                {
                    padcol = '0' + padcol;
                }
                thestack.top() += padcol;
                DToken = clex.GetToken(ctoken);
                if (ctoken.tokenType == "special")
                {
                    column = keys[ctoken.tokenValue];
                }
                else if (DToken == 0)
                {

                    column = 14;

                }
                else if (ctoken.tokenType != "special")
                {
                    column = keys[ctoken.tokenType];
                }

                break;
            case'#':
                cout << "Bad code";
                return;
            case'A':
                if (thestack.size() == 1 && thestack.top().size() == 2) { cout << "Good code"; return;} 
                else
                {
                    cout << "Bad code";
                    return;
                }
            case '>':
                production = thestack.top();
                thestack.pop();
                while (production.size() < 12)  // pad the production with zeros
                {
                    production += "00";
                }
                RC = productions[production];

                if (thestack.empty())
                {
                    Row = 14;
                }
                else
                {
                    size = thestack.top().size();
                    Row = stoi(thestack.top().substr(size - 2, 2));
                }
                Action = opc[Row-1][RC-1];
                switch (Action)
                {
                case'<':
                    padcol = to_string(RC);
                    if (padcol.size() < 2)
                    {
                        padcol = '0' + padcol;
                    }
                    production = padcol;
                    thestack.push(padcol);
                    break;
                case'=':
                    padcol = to_string(RC);
                    if (padcol.size() < 2)
                    {
                        padcol = '0' + padcol;
                    }
                    thestack.top() += padcol;
                }
                
            }

        }



    }

};

int main()
{

	CParcer p1("code.txt");

    p1.Parce();


}


