#ifndef RSS_COMMON_HPP
#define RSS_COMMON_HPP

#include "zenxml/xml.h"
#include "common.hpp"
#include "oai-common.hpp"

namespace rss {
    void write_records(const std::string& filename, std::vector<oai::record> recs,
        const std::string& web_host, const std::vector<std::string>& star_include) {

        if (!star_include.empty()) {
            for (auto& rec : recs) {
                if (!star_include.empty()) {
                    bool found = false;
                    for (const auto& w : star_include) {
                        if (find_word(rec.title, w) || find_word(rec.abstract, w)) {
                            found = true;
                            break;
                        }
                    }

                    if (found) {
                        rec.title = "\u2605\u2605 "+rec.title+" \u2605\u2605";
                    }
                }
            }
        }

        zen::XmlDoc doc("rss");

        zen::XmlElement* e = &doc.root();
        e->setAttribute("version", "2.0");
        e = &e->addChild("channel");

        zen::XmlElement* se;
        se = &e->addChild("title");
        se->setValue("arXiv");
        se = &e->addChild("link");
        se->setValue(web_host);
        se = &e->addChild("description");
        se->setValue("Filtered arXiv postings.");

        for (const auto& rec : recs) {
            se = &e->addChild("item");
            se->addChild("title").setValue(rec.title);
            se->addChild("link").setValue(web_host+"/arxiv/"+rec.id);
            se->addChild("description").setValue(rec.abstract);
        }

        try {
            zen::save(doc, filename, "\n", " ");
        } catch (const zen::XmlFileError& e) {
            error("could not write the XML file '", filename, "'");
            throw;
        }
    }
}

#endif
