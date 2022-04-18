

#include <vector>
#include <algorithm>

using namespace std;
class UserManager
{
    public:

        UserManager::UserManager(Configuration configuration){
            UserInfo(configuration.get_user_info())
            files(configuration.get_files())
        }

        UserManager::~UserManager() {
            for (auto u : user_info)
                delete u;
            user_info.clear();

            for (auto u : users)
                delete u;
            users.clear();
        }

        void UserManager::add_user(int command_socket, int data_socket) {
            users.push_back(new User(command_socket, data_socket));
        }

        void UserManager::remove_user(int socket) {
            for(size_t i = 0; i < users.size(); ++i) {
                if (users[i]->get_command_socket() == socket) {
                    users.erase(users.begin() + i);
                    break;
                }
            }
        }

        User* UserManager::get_user_by_socket(int socket) {
            for(size_t i = 0; i < users.size(); ++i)
                if (users[i]->get_command_socket() == socket)
                    return users[i];
            return nullptr;
        }

        UserInfo* UserManager::get_user_info_by_username(string username) {
            for(size_t i = 0; i < user_info.size(); ++i)
                if (user_info[i]->get_username() == username)
                    return user_info[i];
            return nullptr;
        }

        bool UserManager::contains_as_special_file(string filename) {
            for (size_t i = 0; i < files.size(); i++) {
                if (files[i] == filename)
                    return true;
            }
            return false;
        }


    private:
        vector<UserInfo*> user_info;
        vector<User*> users;
        vector<string> files;
};
