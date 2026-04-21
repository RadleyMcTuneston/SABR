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


    // function that displays all items being sold by the user
    void showSelling(MYSQL * db, string email) {
    cout << "<h2> Selling: </h2>";
    string query = "SELECT itemName, soldStatus, endTime FROM items WHERE userID = (SELECT userID FROM users WHERE email = '" + email + "');";
    
    mysql_query(db, query.c_str());
    MYSQL_RES* res = mysql_store_result(db);
    
    cout << "<ul>";
    while (MYSQL_ROW row = mysql_fetch_row(res)) {
        cout << "<li>" << row[0] << " - Sell Status: " << row[1] << " (Bid Ends: " << row[2] << ")</li>";
    }
    cout << "</ul>";
    mysql_free_result(res);
    
    }
    // function that displays all items that the user has bought
    void showBuying(MYSQL * db, string email) {
    cout << "<h2> Purchased: </h2>";
    string query = "SELECT i.itemName, b.bidPrice FROM items i " "JOIN bids b ON i.itemID = b.itemID " 
		   "WHERE i.endTime < NOW() AND b.bidderID = (SELECT userID FROM users WHERE email = '" + email + "') "
		   "AND b.bidPrice = (SELECT MAX(bidPrice) FROM bids WHERE itemID = i.itemID);";
    
    mysql_query(db, query.c_str());
    MYSQL_RES* res = mysql_store_result(db);
    
    cout << "<ul>";
    while (MYSQL_ROW row = mysql_fetch_row(res)) {
        cout << "<li> Purchased: " << row [0] << "</li>";
    }
    cout << "</ul>";
    mysql_free_result(res);
    
    }


    // function that displays all items that the user is bidding on(activly)
    void showCurrentBids(MYSQL * db, string email) {
    cout << "<h2> Currently Bidding: </h2>";
    string query = "SELECT i.itemID, i.itemName, MAX(b.bidPrice), " "(SELECT MAX(bidPrice) FROM bids WHERE itemID = i.itemID) AS topBid "
                   "FROM items i JOIN bids b ON i.itemID = b.itemID " "WHERE i.endTime > NOW() AND b.bidderID = (SELECT userID FROM users WHERE email = '" + email + "') "
                   "GROUP BY i.itemID;";
    
    mysql_query(db, query.c_str());
    MYSQL_RES* res = mysql_store_result(db);

    while (MYSQL_ROW row = mysql_fetch_row(res)) {
   	double myBid = atof(row[2]);
    	double maxBid = atof(row[3]);
	cout << "<ul>";
    	cout << "<li>" << row[1] << " / Your Current Bid: $" << myBid << "</li>";
	if ( myBid < maxBid) {
		cout << "<p style='color:red;'>You have been outbid. Do you want to want to increase your bid amount? (bid button)</p>";
	}
	cout << "</ul>";
    }
    
        mysql_free_result(res);
    
    }


    // function that displays all items that the user has bid on and lost
    void showLostBids(MYSQL * db, string email) {
    cout << "<h2> You let it slip away(lost bids): </h2>";
    string query = "SELECT i.itemName, (SELECT MAX(bidPrice) FROM bids WHERE itemID = i.itemID) "
                   "FROM items i JOIN bids b ON i.itemID = b.itemID "
                   "WHERE i.endTime < NOW() AND b.bidderID = (SELECT userID FROM users WHERE email = '" + email + "') "
                   "GROUP BY i.itemID HAVING MAX(b.bidPrice) < (SELECT MAX(bidPrice) FROM bids WHERE itemID = i.itemID);";
    
    mysql_query(db, query.c_str());
    MYSQL_RES* res = mysql_store_result(db);
    
    cout << "<ul>";
    while (MYSQL_ROW row = mysql_fetch_row(res)) {
        cout <<"<li>" <<row[0] << " (sold for: $" << row[1] << ") </li>";
    }
    cout << "</ul>";
    mysql_free_result(res);
    
    }
    





int main(){
    cout << "Content-Type: Text/html\r\n\r\n";
    	// userlogin for the main page to know whos account is logged in
    char* queryString = getenv("QUERY_STRING");
    string email = getForm(string(queryString), "email");
    size_t pos = email.find("%40");
            while (pos != string::npos) {
            email.replace(pos, 3, "@");
            pos = email.find("%40");
        }

    //database connection
    MYSQL* database = mysql_init(NULL);
    mysql_real_connect(database, "127.0.0.1", "sabr_user", "111111111", "sabr_db", 0, NULL, 0);
    
    string username = "";
    
    string userQuery = "SELECT username FROM users WHERE email = '" + email + "';";
    mysql_query(database, userQuery.c_str());
    MYSQL_RES* userRes = mysql_store_result(database);
    if (userRes) {
        MYSQL_ROW userRow = mysql_fetch_row(userRes);
        if (userRow){
            username = userRow[0];
        }
        mysql_free_result(userRes);
    }
    
    cout << "<html>" << endl;
    cout << "    <head>" << endl;
    cout << "        <title> " << username << " - SABR Site</title>" << endl;
    cout << "    </head>" << endl;
    cout << "" << endl;
    cout << "    <body>" << endl;
    cout << "        <center>" << endl;
    cout << "            <H1>welcome to our wonderful website, " << username << "!</H1>" << endl;
    cout << "            <H2>enjoy your stay :D</H2>" << endl;
    cout << "            <p><a href='auth.html'>logout</a></p>" << endl;
    cout << "        </center>" << endl;

    
    
    showSelling(database, email);
    showBuying(database, email);
    showCurrentBids(database, email);
    showLostBids(database, email);
    cout << "    </body>" << endl;
    cout << "</html>" << endl;


    mysql_close(database);
    return 0;
    //if your reading this, please for the love of god help me.

}
