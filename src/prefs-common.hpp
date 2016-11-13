#ifndef PREFS_COMMON_HPP
#define PREFS_COMMON_HPP

#include "zenxml/xml.h"
#include "common.hpp"

namespace prefs {
    std::map<std::string, std::string> read_preferences(const std::string& filename) {
        std::map<std::string, std::string> pref;

        if (std::ifstream(filename).is_open()) {
            zen::XmlDoc doc;

            try {
                doc = zen::load(filename);
            } catch (const zen::XmlError& e) {
                error("could not read the provided XML file '", filename, "'");
                throw;
            }

            const zen::XmlElement* e = &doc.root();

            if (e->getNameAs<std::string>() != "preferences") {
                throw std::runtime_error("'"+filename+"' is not a preference file");
            }

            auto p = e->getChildren("pref");
            for (auto i = p.first; i != p.second; ++i) {
                e = &(*i);

                std::string k, v;
                if (!e->getAttribute("name", k)) {
                    warning("'", filename, "' contains an ill-formed preference item (missing 'name' attribute), skipping");
                    continue;
                }

                e->getValue(v);

                pref.insert(std::make_pair(k, v));
            }
        }

        return pref;
    }

    void get(const std::map<std::string, std::string>& pref, const std::string& name, std::vector<std::string>& lst) {
        auto p = pref.find(name);
        if (p != pref.end()) {
            lst = split(p->second, ",");
            for (auto& s : lst) {
                s = trim(s);
            }

            lst.erase(std::remove_if(lst.begin(), lst.end(),
                [](const std::string& s) { return s.empty(); }), lst.end());
        } else {
            lst.clear();
        }
    }

    void get(const std::map<std::string, std::string>& pref, const std::string& name, std::string& val) {
        auto p = pref.find(name);
        if (p != pref.end()) {
            val = trim(p->second);
        } else {
            val.clear();
        }
    }

    void get(const std::map<std::string, std::string>& pref, const std::string& name, bool& val) {
        auto p = pref.find(name);
        if (p != pref.end()) {
            std::string s = tolower(trim(p->second));
            if (s == "1" || s == "true" || s == "yes") {
                val = true;
            } else {
                val = false;
            }
        } else {
            val = false;
        }
    }
}

#endif
