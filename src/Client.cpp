#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <boost/property_tree/ptree.hpp>                                        
#include <boost/property_tree/json_parser.hpp>
#include <string>
#include <vector>


using namespace std;

int read_port_text();
int read_port_data();

const string path = "configuration/config.json";

const string LOCAL_HOST = "127.0.0.1";
static constexpr int MAX_INPUT = 128;
static constexpr int MAX_OUTPUT = 4096;


int main() {
    
    int command_channel_port = read_port_text();
    int data_channel_port = read_port_data();

    int command_handler = socket(AF_INET, SOCK_STREAM, 0);
    int data_handler = socket(AF_INET, SOCK_STREAM, 0);

    if (command_handler < 0 || data_handler < 0)
        return;

    struct sockaddr_in server_command_address;
    server_command_address.sin_family = AF_INET;
    server_command_address.sin_port = htons(command_channel_port);
    if (inet_pton(AF_INET, LOCAL_HOST, &server_command_address.sin_addr) <= 0)
        return;

    struct sockaddr_in server_data_address;
    server_data_address.sin_family = AF_INET;
    server_data_address.sin_port = htons(data_channel_port);
    if (inet_pton(AF_INET, LOCAL_HOST, &server_data_address.sin_addr) <= 0)
        return;

    if (connect(command_handler, (struct sockaddr*)&server_command_address, sizeof(server_command_address)) < 0)
        return;

    if (connect(data_handler, (struct sockaddr*)&server_data_address, sizeof(server_data_address)) < 0)
        return;

    char command_output[MAX_OUTPUT] = {0};
    char data_output[MAX_OUTPUT] = {0};
    while (1) {
        cout << "Enter Command > ";
        char command[MAX_COMMAND_LENGTH];
        cin << command;
        send(command_handler, command, MAX_COMMAND_LENGTH, 0);

        memset(command_output, 0, sizeof command_output);
        recv(command_handler, command_output, sizeof(command_output), 0);
        cout << "Output: " << command_output << endl;

        // Receive data output.
        memset(data_output, 0, sizeof data_output);
        recv(data_handler, data_output, sizeof(data_output), 0);
        cout << "Output: " << data_output << endl;
    }
    return 0;
}



int read_port_text() {
    namespace pt = boost::property_tree;
    pt::ptree root_tree;                                                    
    pt::read_json(path, root_tree);
    command_channel_port = root_tree.get_child("commandChannelPort").get_value<int>();
}

int read_port_data() {
    namespace pt = boost::property_tree;
    pt::ptree root_tree;                                                    
    pt::read_json(path, root_tree);
    data_channel_port = root_tree.get_child("dataChannelPort").get_value<int>();
    return data_channel_port;
}

vector<UserInfo*> get_user_info() {
    namespace pt = boost::property_tree;

    vector<UserInfo*> users_info;
        
    
    pt::ptree root_tree;                                                    
    pt::read_json(path, root_tree);
    pt::ptree users_tree = root_tree.get_child("users");
    for (auto& item : users_tree.get_child("")) {
        string name = item.second.get<string>("user");
        string password = item.second.get<string>("password");
        bool is_admin = item.second.get<bool>("admin");
        double size = item.second.get<double>("size");
        
        users_info.push_back(new UserInfo(name, password, is_admin, size));
    }
    return users_info;

}

vector<string> get_files() {
    namespace pt = boost::property_tree;
    vector<string> files;
    pt::ptree root_tree;                                                    
    pt::read_json(path, root_tree);
    pt::ptree files_tree = root_tree.get_child("files");
    for (auto& item : files_tree.get_child("")){
        files.push_back(item.second.get_value<string>());
    }
}

