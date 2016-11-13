#ifndef HTML_COMMON_HPP
#define HTML_COMMON_HPP

#include "zenxml/xml.h"
#include "common.hpp"
#include "oai-common.hpp"

namespace html {
    struct add_info_t {
        zen::XmlElement*& el;

        add_info_t(zen::XmlElement*& e) : el(e) {}

        template <typename T>
        void operator() (std::string css_id, std::string key, T&& then) {
            el = &el->addChild("div");
                el->setAttribute("class", css_id);
                el = &el->addChild("span");
                    el->setAttribute("class", "info-header");
                    el->setValue(key);
                el = el->parent();
                el = &el->addChild("span");
                    el->setAttribute("class", "info-content");
                    then();
                el = el->parent();
            el = el->parent();
        };
    };

    void write_records(const std::string& local_dir, const std::vector<oai::record> recs) {
        for (const auto& rec : recs) {
            std::string odir = local_dir+"/"+rec.id+"/";
            mkdir(odir);

            zen::XmlDoc doc("html");

            zen::XmlElement* e;

            e = &doc.root();
            e = &e->addChild("head");
                e = &e->addChild("link");
                    e->setAttribute("rel", "stylesheet");
                    e->setAttribute("href", "../arxiv.css");
                e = e->parent();

                e = &e->addChild("meta");
                    e->setAttribute("name", "viewport");
                    e->setAttribute("content", "width=device-width, initial-scale=1, user-scalable=no");
                e = e->parent();
            e = e->parent();

            e = &e->addChild("body");
                e = &e->addChild("div");
                    e->setAttribute("class", "title");
                    e->setValue(rec.title);
                e = e->parent();

                e = &e->addChild("ul");
                    e->setAttribute("class", "author-list");
                    for (std::size_t i = 0; i < rec.authors.size(); ++i) {
                        const auto& a = rec.authors[i];
                        std::string sep = ", ";
                        if (i == rec.authors.size()-1) sep = "";
                        e->addChild("li").setValue(a.forenames+" "+a.keyname+sep);
                    }
                e = e->parent();

                e = &e->addChild("div");
                    e->setAttribute("class", "info");

                    add_info_t add_info{e};

                    add_info("info-date", "Date published: ", [&]() { e->setValue(rec.created); });
                    add_info("info-update", "Last updated: ", [&]() {
                        if (rec.updated.empty()) {
                            e->setValue(rec.created);
                        } else {
                            e->setValue(rec.updated);
                        } });
                    add_info("info-comments", "Comments: ", [&]() { e->setValue(rec.comments); });
                    add_info("info-categories", "arXiv: ", [&]() {
                        e = &e->addChild("a");
                            e->setAttribute("href", "https://arxiv.org/abs/"+rec.id);
                            e->setAttribute("class", "arxiv-link");
                            e->setValue("["+rec.id+"]");
                        e = e->parent();
                        e = &e->addChild("span");
                            e->setValue(" "+rec.categories);
                        e = e->parent();
                    });
                    add_info("info-ref", "Reference: ", [&]() { e->setValue(rec.ref + " ("+rec.doi+")"); });
                e = e->parent();

                e = &e->addChild("div");
                    e->setAttribute("class", "abstract");
                    e = &e->addChild("span");
                        e->setAttribute("class", "abstract-header");
                        e->setValue("Abstract. ");
                    e = e->parent();
                    e = &e->addChild("span");
                        e->setAttribute("class", "abstract-content");
                        e->setValue(rec.abstract);
                    e = e->parent();
                e = e->parent();
            e = e->parent();

            try {
                zen::save(doc, odir+"index.html", "\n", " ");
            } catch (const zen::XmlFileError& e) {
                error("could not write the XML file '", odir, "index.html'");
                throw;
            }
        }
    }
}

#endif
