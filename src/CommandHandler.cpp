

#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include <sys/stat.h>

using namespace std;


#define UserCommand "user"
#define PasswordCommand "pass"
#define PwdCommand "pwd"
#define MakeDirCommand "mkd"
#define DeleteCommand "dele"
#define ListDirCommand "ls"
#define ChangeWorkDirCommand "cwd"
#define RenameCommand "rename"
#define DownloadCommand "retr"
#define HelpMeCommand "help"
#define QuiteCommand "quit"


const string LOGPATH = "C:\\logs\\log.txt";

#define COMMAND 0
#define ARG1 1
#define ARG2 2
#define SUCCESS 0

class CommandHandler {
    public:
        CommandHandler(Utils utils) {
            user_manager = new UserManager();
            this->utils = utils;
        }

        UserManager* get_user_manager() {
            return user_manager;
        }

        vector<string> excute(int user_socket, char* command) {
            vector<string> command_parts = utils.extract_args(command);

            User* user = user_manager->get_user_by_socket(user_socket);
            if (user == nullptr)
                return {"500: Error", " "};

            if (command_parts[COMMAND] == UserCommand) {
                if (command_parts.size() != 2)
                    return {"501: Syntax error in parameters or arguments.", " "};
                return handle_username(command_parts[ARG1], user);
            }

            else if (command_parts[COMMAND] == PasswordCommand) {
                if (command_parts.size() != 2)
                    return {"501: Syntax error in parameters or arguments.", " "};
                return handle_password(command_parts[ARG1], user);
            }

            else if (user->get_state() != User::State::LOGGED_IN) 
                return {"332: Need account for login.", " "};

            else if (command_parts[COMMAND] == PWD_COMMAND) {
                if (command_parts.size() != 1)
                    return {"501: Syntax error in parameters or arguments.", " "};
                return handle_get_current_directory(user);
            }

            else if (command_parts[COMMAND] == MakeDirCommand) {
                if (command_parts.size() != 2)
                    return {"501: Syntax error in parameters or arguments.", " "};
                return handle_create_new_directory(command_parts[ARG1], user);
            }

            else if (command_parts[COMMAND] == DeleteCommand && command_parts[ARG1] == "-d") {
                if (command_parts.size() != 3)
                    return {"501: Syntax error in parameters or arguments.", " "};
                return handle_delete_directory(command_parts[ARG2], user);
            }

            else if (command_parts[COMMAND] == DeleteCommand && command_parts[ARG1] ==  "-f") {
                if (command_parts.size() != 3)
                    return {"501: Syntax error in parameters or arguments.", " "};
                return handle_delete_directory(command_parts[ARG2], user);   
            }

            else if (command_parts[COMMAND] == ListDirCommand) {
                if (command_parts.size() != 1)
                    return {"501: Syntax error in parameters or arguments.", " "};
                return handle_get_list_of_files(user);
            }

            else if (command_parts[COMMAND] == ChangeWorkDirCommand) {
                if (command_parts.size() != 1 && command_parts.size() != 2)
                    return {"501: Syntax error in parameters or arguments.", " "};
                return handle_change_working_directory(((command_parts.size() >= 2) ? command_parts[ARG1] : ROOT), user);
            }

            else if (command_parts[COMMAND] == RenameCommand) {
                if (command_parts.size() != 3)
                    return {"501: Syntax error in parameters or arguments.", " "};
                return handle_rename_file(command_parts[ARG1], command_parts[ARG2], user);
            }

            else if (command_parts[COMMAND] == DownloadCommand) {
                if (command_parts.size() != 2)
                    return {"501: Syntax error in parameters or arguments.", " "};
                return handle_download_file(command_parts[ARG1], user);
            }

            else if (command_parts[COMMAND] == HelpMeCommand) {
                if (command_parts.size() != 1)
                    return {"501: Syntax error in parameters or arguments.", " "};
                return handle_help();
            }

            else if (command_parts[COMMAND] == QuiteCommand) {
                if (command_parts.size() != 1)
                    return {"501: Syntax error in parameters or arguments.", " "};
                return handle_logout(user);
            }

            else
                return {"501: Syntax error in parameters or arguments.", " "};
        }

        bool is_a_file_name(string file_name) {
            if (file_name.find("/") != string::npos)
                return false;
            return true;
        }

        bool user_has_access_to_file(string file_name, User* user) {
            if (!user_manager->contains_as_special_file(file_name))
                return true;
            else if (user->is_able_to_access())
                return true;
            return false;
        }

        vector<string> handle_username(string username, User* user) {
            if(user->get_state() != User::State::WAITING_FOR_USERNAME)
                return {"503: Bad sequence of commands.", " "};

            UserIdentityInfo* user_identity_info = user_manager->get_user_info_by_username(username);

            if (user_identity_info == nullptr)
                return {"430: Invalid username or password", " "};
            
            user->set_state(User::State::WAITING_FOR_PASSWORD);
            user->set_user_identity_info(user_identity_info);
            user->set_current_directory(ROOT);

            return {"331: User name Okay, need password.", " "};
        }

        vector<string> handle_password(string password, User* user) {
            if(user->get_state() != User::State::WAITING_FOR_PASSWORD)
                return {"503: Bad sequence of commands.", " "};

            if (user->get_user_identity_info()->get_password() != password)
                return {"430: Invalid username or password", " "};

            user->set_state(User::State::LOGGED_IN);

            log(user->get_username() +  ": " +  "logged in.");

            return {"230: User looged in, proceed. Logged out if appropriate.", " "};
        }

        vector<string> handle_get_current_directory(User* user) {
            string current_path = user->get_current_directory();
            if (current_path == ROOT)
                current_path = ".";
                
            string bash_command = "realpath " + current_path + " > file.txt";
            int status = system(bash_command.c_str());
            if (status != SUCCESS)
                return {"500: Error", " "};

            string result = read_file_to_string("file.txt");
            result.pop_back();
            status = system("rm file.txt");
            if (status != SUCCESS)
                return {"500: Error", " "};

            return {"257: " + result, " "};
        }
        vector<string> handle_create_new_directory(string dir_path, User* user) {
            string bash_command = "mkdir " + user->get_current_directory() + dir_path;
            int status = system(bash_command.c_str());
            if (status == SUCCESS) {
                string message = ": " + dir_path + " created.";
                log(user->get_username() + message);
                return {"257" + message, " "};
            }
            return {"500: Error", " "};
        }

        vector<string> handle_delete_directory(string dir_path, User* user) {
            string bash_command = "rm -r " + user->get_current_directory() + dir_path;
            int status = system(bash_command.c_str());
            if (status == SUCCESS) {
                string message =  ": " +  dir_path + " deleted.";
                log(user->get_username() + message);
                return {"250" + message, " "};
            }
            return {"500: Error", " "};
        }

        vector<string> handle_delete_file(string file_name, User* user) {
            if (!is_a_file_name(file_name))
                return {"501: Syntax error in parameters or arguments.", " "};

            if (!user_has_access_to_file(file_name, user))
                return {"550: File unavailable.", " "};

            string bash_command = "rm " + user->get_current_directory() + file_name;
            int status = system(bash_command.c_str());
            if (status == SUCCESS) {
                string message =  ": " +  file_name + " deleted.";
                log(user->get_username() + message);
                return {"250" + message, " "};
            }
            return {"500: Error", " "};
        }

        vector<string> handle_get_list_of_files(User* user) {
            string bash_command = "ls " + user->get_current_directory() + " > file.txt";
            int status = system(bash_command.c_str());
            if (status != SUCCESS)
                return {"500: Error", " "};

            string result = read_file_to_string("file.txt");
            result.pop_back();
            status = system("rm file.txt");
            if (status != SUCCESS)
                return {"500: Error", " "};

            erase_sub_str(result, "file.txt\n");
            
            return {"226: List transfer done.", result};
        }

        vector<string> handle_change_working_directory(string dir_path, User* user) {
            string check_validity_command = "realpath " + dir_path + " > file.txt";
            int status1 = system(check_validity_command.c_str());
            int status2 = system("rm file.txt");
            if (status1 != SUCCESS || status2 != SUCCESS)
                return {"500: Error", " "};

            if(dir_path == "")
                user->set_current_directory("");
            else
                user->set_current_directory(user->get_current_directory() + dir_path + "/");

            return {"250: Successful change.", " "};
        }

        vector<string> handle_rename_file(string old_name, string new_name, User* user) {
            if (!is_a_file_name(old_name) || !is_a_file_name(new_name))
                return {"501: Syntax error in parameters or arguments.", " "};

            if (!user_has_access_to_file(old_name, user))
                return {"550: File unavailable.", " "};

            string bash_command = "mv " + user->get_current_directory() + old_name + " " +
                    user->get_current_directory() + new_name;
            int status = system(bash_command.c_str());
            if (status == SUCCESS)
                return {"250: Successful change.", " "};
            return {"500: Error", " "};
        }

        vector<string> handle_download_file(string file_name, User* user) {
            if (!is_a_file_name(file_name))
                return {"501: Syntax error in parameters or arguments.", " "};
                
            if (!user_has_access_to_file(file_name, user))
                return {"550: File unavailable.", " "};

            string file_path = user->get_current_directory() + file_name;
            string size_command = "stat -c%s " + file_path + " > " + "size.txt";
            int status = system(size_command.c_str());
            if (status != SUCCESS)
                return {"500: Error", " "};
            
            double file_size = read_file_to_double("size.txt");
            status = system("rm size.txt");
            if (status != SUCCESS)
                return {"500: Error", " "};

            if (user -> is_able_to_download(file_size) == false)
                return {"425:Can't open data connection.", " "};

            string bash_command = "cp " + file_path + " file.txt";
            status = system(bash_command.c_str());
            if (status != SUCCESS)
                return {"500: Error", " "};

            string result = read_file_to_string("file.txt");
            status = system("rm file.txt");
            if (status != SUCCESS)
                return {"500: Error", " "};

            user->decrease_available_size(file_size);

            string message =  ": " +  file_name + " downloaded.";
            log(user->get_username() + message);

            return {"226: Successful Download.", result};
        }


                
        static void log(string message) {
            fstream log_file;
            log_file.open(LOGPATH, std::fstream::in | std::fstream::out | std::fstream::app);
            
            auto curr = std::chrono::system_clock::now();
            std::time_t curr_time = std::chrono::system_clock::to_time_t(curr);
            
            log_file << std::ctime(&curr_time);
            log_file << message << endl;
            log_file.close();
        }


        vector<string> handle_help() {
            string info = "214\n";
            info +=  "USER [name], It is used for user authentication.\n"; 
            info += "PASS [password], It is used for user authentication.\n";
            info += "PWD, Return the current working directory.\n";
            info += "MKD [path], It is usede to create a new directory.\n";
            info += "DELE [flag] [path], Its argument is used to specify the file/directory's path. It flag is used to specify whether a file (-f) or a directory (-d) will be removed. It is usede to remove a file or directory.\n";
            info += "LS. Return the list of files/directories in the current working directory.\n";
            info += "CWD [path], It is used to change the current working directory.\n";
            info += "RENAME [from] [to], It is used to change A file's name.\n";
            info += "RETR [name], It is used to download a file.\n";
            info += "HELP, It is used to display information about builtin commands.\n";
            info += "QUIT, It is used to sign out from the server.\n";
            return {info, " "};
        }

        vector<string> handle_logout(User* user) {    
            if (user->get_state() != User::State::LOGGED_IN)
                return {"500: Error", " "};

            user->set_state(User::State::WAITING_FOR_USERNAME);

            log(user->get_username() +  ": " + "logged out.");

            return {"221: Successful Quit.", " "};
        }

    private:
        UserManager* user_manager;
};