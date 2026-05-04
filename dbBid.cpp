#include <iostream>
#include <string>
#include <mysql/mysql.h>

using namespace std;

//this function is the one from my main.cpp but i rewrote it to handle more cases
string getForm(string postData, string field) {
    string search = field + "=";
    size_t start = postData.find(search);
    if (start == string::npos) return "";
    
    start += search.length();
    size_t end = postData.find("&", start);
    string value = postData.substr(start, end - start);

    // this changes plus signs to spaces 
    for (size_t i = 0; i < value.length(); i++) {
        if (value[i] == '+') value[i] = ' ';
    }

    // this changes %40 into @ and stuff like that
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
    int length = stoi(getenv("CONTENT_LENGTH"));
    char* data = new char[length + 1];
    cin.read(data, length);
    data[length] = '\0';
    string postData(data);

    string email = getForm(postData, "email");
    string itemID = getForm(postData, "itemID");
    string bidAmountStr = getForm(postData, "bidAmount");
    double userBid = atof(bidAmountStr.c_str());

    MYSQL* db = mysql_init(NULL);
    mysql_real_connect(db, "127.0.0.1", "sabr_user", "111111111", "sabr_db", 0, NULL, 0);

    // this gets the current highest bid on the item
    string checkQuery = "SELECT MAX(bidPrice) FROM bids WHERE itemID = " + itemID + ";";
    mysql_query(db, checkQuery.c_str());
    MYSQL_RES* res = mysql_store_result(db);
    MYSQL_ROW row = mysql_fetch_row(res);

    double currentHigh = (row[0]) ? atof(row[0]) : 0.0;
    mysql_free_result(res);

    // error message in to prevent people from bidding less than current bid
    if (userBid <= currentHigh) {
        cout << "Content-Type: text/html\r\n\r\n";
        cout << "<h2>Bid cant be less than current bid</h2>";
        cout << "<a href='profile.cgi?email=" << email << "'>Return to Profile</a>";
    } 
    // error to prevent people from bidding more than 2x the current bid
    else if (currentHigh > 0 && userBid > (currentHigh * 2)) {
        cout << "Content-Type: text/html\r\n\r\n";
        cout << "<h2>you cant bid more than 2x the current bid ammout</h2>";
        cout << "<a href='profile.cgi?email=" << email << "'>Return to Profile</a>";
    } 
    else {
        //if this activates then the bid was valid and went thru
        string insertQuery = "INSERT INTO bids (bidderID, itemID, bidPrice) VALUES ("
                             "(SELECT userID FROM users WHERE email='" + email + "'), " 
                             + itemID + ", " + bidAmountStr + ");";
        mysql_query(db, insertQuery.c_str());
        
        cout << "Location: profile.cgi?email=" << email << "\r\n\r\n";
    }

    mysql_close(db);
    delete[] data;
    return 0;
}
