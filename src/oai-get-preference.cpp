#include "prefs-common.hpp"

int main(int argc, char* argv[]) {
    auto pref = prefs::read_preferences(argv[1]);
    std::string name = argv[2];

    auto p = pref.find(name);
    if (p != pref.end()) {
        print(p->second);
    }

    return 0;
}
