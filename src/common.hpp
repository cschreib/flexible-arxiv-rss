#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <cstring>

template<typename T>
void print(const T& t) {
    std::cout << t << std::endl;
}

template<typename T, typename ... Args>
void print(const T& t, Args&&... args) {
    std::cout << t;
    print(std::forward<Args>(args)...);
}

template<typename ... Args>
void error(Args&& ... args) {
    print("error: ", std::forward<Args>(args)...);
}

template<typename ... Args>
void warning(Args&& ... args) {
    print("warning: ", std::forward<Args>(args)...);
}

template<typename ... Args>
void note(Args&& ... args) {
    print("note: ", std::forward<Args>(args)...);
}

inline std::string trim(std::string s, const std::string& chars = " \t\n\r") {
    std::size_t spos = s.find_first_of(chars);
    if (spos == 0) {
        std::size_t epos = s.find_first_not_of(chars);
        if (epos == s.npos) return "";
        s = s.substr(epos);
    }

    spos = s.find_last_of(chars);
    if (spos == s.size()-1) {
        std::size_t epos = s.find_last_not_of(chars);
        s = s.erase(epos+1, s.size() - epos+1);
    }

    return s;
}

inline std::string toupper(std::string s) {
    for (auto& c : s) {
        c = ::toupper(c);
    }
    return s;
}

inline std::string tolower(std::string s) {
    for (auto& c : s) {
        c = ::tolower(c);
    }
    return s;
}

inline std::string replace(std::string s, const std::string& pattern, const std::string& rep) {
    auto p = s.find(pattern);
    while (p != s.npos) {
        s.replace(p, pattern.size(), rep);
        p = s.find(pattern, p+rep.size());
    }

    return s;
}

inline bool start_with(const std::string& s, const std::string& pattern) {
    if (s.size() < pattern.size()) return false;
    for (std::size_t i = 0; i < pattern.size(); ++i) {
        if (s[i] != pattern[i]) return false;
    }

    return true;
}

inline bool end_with(const std::string& s, const std::string& pattern) {
    if (s.size() < pattern.size()) return false;
    for (std::size_t i = 1; i <= pattern.size(); ++i) {
        if (s[s.size()-i] != pattern[pattern.size()-i]) return false;
    }

    return true;
}

std::size_t length(const std::string& s) {
    return s.size();
}

std::size_t length(const char* s) {
    return strlen(s);
}

template<typename T>
std::vector<std::string> split(const std::string& ts, const T& pattern) {
    std::vector<std::string> ret;
    std::size_t p = 0, op = 0;
    while ((p = ts.find(pattern, op)) != ts.npos) {
        ret.push_back(ts.substr(op, p - op));
        op = p + length(pattern);
    }

    ret.push_back(ts.substr(op));
    return ret;
}

const std::string nonword = " ,.!?;:+-*/^_\\()[]{}<>|~\t\n\r\"\'";

bool find_word(std::string s, std::string w) {
    s = tolower(s);
    w = tolower(w);

    auto p = s.find(w);
    while (p != s.npos) {
        // Check word boundaries
        if ((p == 0                 || nonword.find_first_of(s[p-1])        != nonword.npos) &&
            (p+w.size() == s.size() || nonword.find_first_of(s[p+w.size()]) != nonword.npos)) {
            return true;
        }
        p = s.find(w, p+1);
    }

    return false;
}

inline void mkdir(const std::string& path) {
    if (path.empty()) return;
    std::vector<std::string> dirs = split(path, "/");
    std::string tmp;
    for (auto& d : dirs) {
        if (d.empty()) continue;
        if (!tmp.empty() || start_with(path, "/")) tmp += "/";
        tmp += d;
        bool res = (::mkdir(tmp.c_str(), 0775) == 0) || (errno == EEXIST);
        if (!res) throw std::runtime_error("cannot create the directory '"+tmp+"'");
    }
}

#endif
