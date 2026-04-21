#include <iostream>
#include <string>
#include <cstdlib>
#include <memory>

using namespace std;

#include <mysql/mysql.h>


	// function that gets the standard input from the html url
    string getLogin() {
        string rawText = "";
        char* loginLength = getenv("CONTENT_LENGTH");
        if (loginLength == NULL) {
            return "";
        }
        int len = atoi(loginLength);

    for (int i = 0; i < len; i++) {
        char c;
        cin.get(c);
        rawText += c;
    }
    return rawText;
    }


	// function that finds where the password and email start
    string getForm(string rawText,string form) {

        string find = form + "=";
        size_t pos = rawText.find(find);
        if (pos == string::npos) {
            return "";
        }
        pos += find.length();

        size_t pos2 = rawText.find("&", pos);
	if (pos2 == string::npos) {
	return rawText.substr(pos);
	}

        return rawText.substr(pos, pos2 - pos);

    }




    int main() {
		//telling the code what type of file its being sent (html)
		




        string rawText = getLogin();
        string email = getForm(rawText, "email");
        string pass = getForm(rawText, "pass");
	

	//code that switched the %40 into a @ symbol for emails
	size_t pos = email.find("%40");
	while (pos != string::npos) {
    	email.replace(pos, 3, "@");
    	pos = email.find("%40");
	}


        MYSQL* database = mysql_init(NULL);
	MYSQL_RES* result = NULL;


	// checking if we successfully connected to the database
	if (!(mysql_real_connect(database, "127.0.0.1", "sabr_user", "111111111", "sabr_db", 0, NULL, 0))) {
        cout << "connection failed, make sure to cross the Ts and dot the Is" << endl;
        return 0;

    }
	string emailCheck = "SELECT * FROM users WHERE email='" + email + "';";
	//second saftey check to make sure handshake between database and input  made it
	if (mysql_query(database, emailCheck.c_str())) {
        cout << "oops";
    } else {
        result = mysql_store_result(database);
    }

	// if statement  chacking if the email exists in the database, then checking if the user password matches the email
	if(result && mysql_num_rows(result) > 0) {
		MYSQL_ROW row = mysql_fetch_row(result);
		string userpword = row[2];
	

			if(userpword == pass){
				cout << "Location: profile.cgi?email=" << email << "\r\n\r\n";
				return 0;
			}else{
				cout << "Content-Type: text/html\r\n\r\n";
				cout << "<h1>incorect password</h1>";
			}
	} else {
	//adding new user to database if email doesnt exist.
	cout << "Content-Type: text/html\r\n\r\n";
	string newUser = "INSERT INTO users (username, password, userRating, email) VALUES ('" + email + "', '" + pass + "', 0.0, '" + email + "');";
	mysql_query(database, newUser.c_str());
	cout << "<h1> you do not have a profile, so we made you one! </h1>";
	}

    return 0;
    }
	
