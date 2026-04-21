
CXX = g++
CXXFLAGS = -Wall
LDFLAGS = -L/usr/lib64/mysql -lmysqlclient -Wl,-rpath,/usr/lib64/mysql

TARGET1 = login.cgi
TARGET2 = profile.cgi

SRC1 = main.cpp
SRC2 = profile.cpp

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(SRC1)
	$(CXX) $(CXXFLAGS) $(SRC1) -o $(TARGET1) $(LDFLAGS)
	chmod 755 $(TARGET1)

$(TARGET2): $(SRC2)
	$(CXX) $(CXXFLAGS) $(SRC2) -o $(TARGET2) $(LDFLAGS)
	chmod 755 $(TARGET2)

clean:
	rm -f $(TARGET1) $(TARGET2)
