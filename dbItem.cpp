#include <iostream>
#include <string>
#include <mysql/mysql.h>
#include <cstdlib>

using namespace std;

// this is the same code from my main.cpp but remade to handle more cases
string getForm(string postData, string field) {
    string search = field + "=";
    size_t start = postData.find(search);
    if (start == string::npos) return "";
    
    start += search.length();
    size_t end = postData.find("&", start);
    string value = postData.substr(start, end - start);

    //  converts plus signs to spaces
    for (size_t i = 0; i < value.length(); i++) {
        if (value[i] == '+') value[i] = ' ';
    }

    // changes %40 to @ and stuff like that
    string decoded = "";
    for (size_t i = 0; i < value.length(); i++) {
        if (value[i] == '%' && i + 2 < value.length()) {
            string hex = value.substr(i + 1, 2);
            decoded += (char)strtol(hex.c_str(), NULL, 16);
            i += 2;
        } else {
            decoded += value[i];
        }
    }
    return decoded;
}

int main() {
    // again most of this is just stuff from my main thats rewritten, this  just gets post data from the browser
    const char* contentLengthChar = getenv("CONTENT_LENGTH");
    if (!contentLengthChar) {
        cout << "Content-Type: text/html\r\n\r\n";
        cout << "No form data found.";
        return 0;
    }
    
    

    int length = stoi(contentLengthChar);
    char* data = new char[length + 1];
    cin.read(data, length);
    data[length] = '\0';
    string postData(data);

    // runs getform to get raw text
    string email = getForm(postData, "email");
    string itemName = getForm(postData, "itemName");
    string itemDescription = getForm(postData, "itemDescription");
    string itemCondition = getForm(postData, "itemCondition");
    string shippingPrice = getForm(postData, "shippingPrice");

    // connection to database
    MYSQL* db = mysql_init(NULL);
    if (!mysql_real_connect(db, "127.0.0.1", "sabr_user", "111111111", "sabr_db", 0, NULL, 0)) {
        return 0; 
    }

    // name of value self explanitory
    string query = "INSERT INTO items (userID, itemName, itemDescription, itemCondition, soldStatus, shippingPrice, endTime) "
                   "VALUES ("
                   "(SELECT userID FROM users WHERE email='" + email + "'), "
                   "'" + itemName + "', "
                   "'" + itemDescription + "', "
                   "'" + itemCondition + "', " 
                   "'AVAILABLE', "
                   "" + shippingPrice + ", "
                   "DATE_ADD(NOW(), INTERVAL 7 DAY));";

    
    mysql_query(db, query.c_str());


    // this puts the user back to there profile to see the item they just posted
    cout << "Location: profile.cgi?email=" << email << "\r\n\r\n";

    //free memory he aint do nothin i stg
    mysql_close(db);
    delete[] data;
    return 0;
}
