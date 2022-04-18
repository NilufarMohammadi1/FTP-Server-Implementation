
#include <iostream>
#include <string>
using namespace std;

class UserClass {
    public:
        UserClass(string username, string password, bool is_admin, double available_size){
            this->username = username;
            this->password = password;
            this->is_admin = is_admin;
            this->available_size = available_size;
        }
        bool login(string _username, string _password){
            return this->username == _username && this->password == _password;
        }
        bool is_admin_user() {
            return this->is_admin;
        }
        void decrease_available_size(double file_size) {
            this->available_size -= (file_size/1000);
        }
        double get_available_size(){
            return this->available_size;
        }

    private:
        string username;
        string password;
        bool is_admin;
        double available_size;
};
