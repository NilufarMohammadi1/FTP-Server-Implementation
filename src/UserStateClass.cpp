

#include <iostream>
#include <string>


using namespace std;

#define ROOT ""

class UserStateClass {
    public:
        enum State {
            WAITING_FOR_USERNAME,
            WAITING_FOR_PASSWORD,
            LOGGED_IN,
        };

        UserStateClass(int command_socket, int data_socket){
            command_socket(command_socket);
            data_socket(data_socket);
            state(WAITING_FOR_USERNAME);
            current_directory(ROOT);
            user_identity_info(nullptr) ;
        }

        int get_command_socket() {
            return command_socket;
        }

        int get_data_socket() {
            return data_socket;
        }

        State get_state() {
            return state;
        }

        string get_username() {
            return user_identity_info->get_username();
        }

        string get_current_directory() {
            return current_directory;
        }

        UserIdentityInfo* get_user_identity_info() {
            return user_identity_info;
        }

        void set_state(State _state) {
            state = _state;
        }

        void set_user_identity_info(UserIdentityInfo* _user_identity_info) {
            user_identity_info = _user_identity_info;
        }

        void User::set_current_directory(string path) {
            current_directory = path;
        }

        bool User::is_able_to_download(double file_size) {
            if (user_identity_info->get_available_size() >= file_size/1000)
                return true;
            return false;
        }

        void decrease_available_size(double file_size) {
            user_identity_info->decrease_available_size(file_size);
        }

        bool is_able_to_access() {
            return user_identity_info->is_admin_user();
        }

    private:
        int command_socket;
        int data_socket;
        State state;
        string current_directory;
        UserIdentityInfo* user_identity_info;
};
