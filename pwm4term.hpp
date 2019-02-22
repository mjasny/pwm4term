#pragma once

#include <string>
#include <map>
#include <regex>
#include <iostream>
#include <fstream>


class Pwm4Term {
    std::map<std::string, std::string> pw_db;
    
    inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }
    
    inline bool ends_with(std::string const & value, std::string const & ending) {
        if (ending.size() > value.size()) return false;
        return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
    }
    
    std::vector<std::string> split_lines(std::string &s) {
        std::vector<std::string> lines;
        
        std::istringstream iss(s);
        std::string line;
        while (std::getline(iss, line)) {
            lines.push_back(line);
        }
        
        return lines;
    }
    
    void populate_db() {
        std::string path(getenv("HOME"));
        path += "/.config/pwm4term.passwords";
        
        std::ifstream pw_file;
        pw_file.open(path);
        if (pw_file.fail()) {
            std::cerr << path << "not found." << std::endl;
            return;
        }
    
        pw_db.clear();
        for(std::string line; getline(pw_file, line);) {
            std::string hostname = line.substr(0, line.find(' '));
            std::string password = line.substr(line.find(' ')+1);
            
            pw_db[hostname] = password;
        }
        
        pw_file.close();
    }
    
    std::string get_pw_for_hostname(std::string hostname) {
        auto it = pw_db.find(hostname);
        if (it == pw_db.end())
            return {};
        return it->second;
    }
    
    std::string get_user_for_github() {
        for (auto [host, pw] : pw_db) {
            if (ends_with(host, "@github"))
                return host.substr(0, host.find('@'));
        }
        return {};
    }
    
    std::string get_pw_for_github(std::string user) {
        auto it = pw_db.find(user + "@github");
        if (it == pw_db.end())
            return {};
        return it->second;
    }
    
    
    std::string get_feed(std::string &buffer) {
        populate_db();
        rtrim(buffer);
        //std::replace(buffer.begin(), buffer.end(), '\n', '#');
        std::cout << buffer << std::endl;
        
        auto lines = split_lines(buffer);
        
        std::regex sudo_re(R"(\[sudo\] (?:password|Passwort) (?:for|für) ([A-Za-z0-9-]+):)");
        std::regex github_user_re(R"(Username for 'https?://github.com':)");
        std::regex github_pass_re(R"(Password for 'https?://([A-Za-z0-9-]+)@github.com':)");
        std::regex hostname_re(R"(([A-Za-z0-9-]+@[A-Za-z0-9-]+))");
        
        std::smatch m;
        
        auto last_line = lines.back();
        std::string response;
        /*if (std::regex_match(last_line, m, sudo_re)) {
            std::cout << "is sudo username=" << m[1] << std::endl;
        } else*/
        if (std::regex_match(last_line, m, github_user_re)) {
//             std::cout << "is github username=" << std::endl;
            response = get_user_for_github();
        } else if (std::regex_match(last_line, m, github_pass_re)) {
//             std::cout << "is github password for user=" << m[1] << std::endl;
            response = get_pw_for_github(m[1]);
        } else {
            std::string s{buffer};
            std::string hostname;
            while (std::regex_search(s, m, hostname_re)) {
                hostname = m[1].str();
//                 std::cout << "found hostname=" << hostname << std::endl;
                s = m.suffix();
            }
            response = get_pw_for_hostname(hostname);
        }
        
        if (response.size() > 0)
            return response + '\n';
        return {};
    }
    
public:
    Pwm4Term() {
        populate_db();
    }
    
    static gboolean key_handler(GtkWidget *terminal, GdkEventKey *event, gpointer user_data) {    
        if (event->keyval != GDK_KEY_F1)
            return FALSE;
        
        auto pwm4term = static_cast<Pwm4Term*>(user_data);
        
        char *text = vte_terminal_get_text(VTE_TERMINAL(terminal), NULL, NULL, NULL);
        if (!text)
            return FALSE;
        
        std::string buffer{text};
        free(text);
        
        std::string response = pwm4term->get_feed(buffer);
        
        vte_terminal_feed_child(VTE_TERMINAL(terminal), response.c_str(), response.size());        
        return TRUE;
    }
};

