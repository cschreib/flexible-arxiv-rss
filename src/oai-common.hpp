#ifndef OAI_COMMON_HPP
#define OAI_COMMON_HPP

#include <vector>
#include "zenxml/xml.h"
#include "common.hpp"

namespace oai {
    struct author {
        std::string keyname, forenames;
    };

    struct record {
        std::string id, created, updated, title, categories, comments, ref, doi, abstract;
        std::vector<author> authors;
    };

    bool date_less(const std::string& d1, const std::string& d2) {
        auto sd1 = split(d1, "-");
        auto sd2 = split(d2, "-");
        if (sd1.size() != sd2.size()) {
            throw std::runtime_error("could not compare dates "+d1+" and "+d2+" because they have different format");
        }

        for (std::size_t i = 0; i < sd1.size(); ++i) {
            if (sd1[i] < sd2[i]) return true;
            if (sd1[i] > sd2[i]) return false;
        }

        return false;
    }

    std::vector<record> read_records(const std::string& filename) {
        std::vector<record> recs;

        if (std::ifstream(filename).is_open()) {
            zen::XmlDoc doc;

            try {
                doc = zen::load(filename);
            } catch (const zen::XmlError& e) {
                error("could not read the provided XML file '", filename, "'");
                throw;
            }

            const zen::XmlElement* e = &doc.root();

            if (e->getNameAs<std::string>() != "OAI-PMH") {
                throw std::runtime_error("'"+filename+"' is not an OAI request");
            }

            e = e->getChild("ListRecords");
            if (!e) {
                warning("'"+filename+"' does not contain any record");
                return recs;
            }

            auto p = e->getChildren("record");

            for (auto i = p.first; i != p.second; ++i) {
                e = &(*i);
                e = e->getChild("metadata");
                if (!e) {
                    warning("'", filename, "' contains an ill-formed record (missing 'metadata'), skipping");
                    continue;
                }
                e = e->getChild("arXiv");
                if (!e) {
                    warning("'", filename, "' contains an ill-formed record (missing 'arXiv'), skipping");
                    continue;
                }

                record r;

                auto read_value = [&](const zen::XmlElement* se, const std::string& key, std::string& val) {
                    se = se->getChild(key);
                    if (se) se->getValue(val);
                    val = replace(val, "\n", " ");
                };

                read_value(e, "id", r.id);
                read_value(e, "created", r.created);
                read_value(e, "updated", r.updated);
                read_value(e, "title", r.title);
                read_value(e, "categories", r.categories);
                read_value(e, "comments", r.comments);
                read_value(e, "journal-ref", r.ref);
                read_value(e, "doi", r.doi);
                if (r.doi.empty()) r.doi = "no DOI yet";
                read_value(e, "abstract", r.abstract);

                auto* se = e->getChild("authors");
                if (se) {
                    auto sp = se->getChildren("author");

                    for (auto si = sp.first; si != sp.second; ++si) {
                        e = &(*si);

                        author a;
                        read_value(e, "keyname", a.keyname);
                        read_value(e, "forenames", a.forenames);

                        r.authors.push_back(a);
                    }
                }

                recs.push_back(r);
            }
        }

        note("found ", recs.size(), " records in '", filename, "'");

        return recs;
    }

    void merge(std::vector<record>& in, const std::vector<record>& other) {
        auto compare_ids = [](const record& r1, const record& r2) {
            return r1.id < r2.id;
        };

        // Sort input records by ID
        std::sort(in.begin(), in.end(), compare_ids);

        for (const auto& rec : other) {
            // Use binary search to find if an element with this ID already exists
            auto p = std::lower_bound(in.begin(), in.end(), rec, compare_ids);
            if (p == in.end() || p->id != rec.id) {
                in.insert(p, rec);
            }
        }
    }

    void write_records(const std::string& filename, const std::vector<record>& recs) {
        zen::XmlDoc doc("OAI-PMH");

        zen::XmlElement* e = &doc.root();
        e = &e->addChild("ListRecords");

        for (const auto& rec : recs) {
            zen::XmlElement* se = &e->addChild("record").addChild("metadata").addChild("arXiv");
            se->addChild("id").setValue(rec.id);
            se->addChild("created").setValue(rec.created);
            se->addChild("updated").setValue(rec.updated);
            se->addChild("title").setValue(rec.title);
            se->addChild("categories").setValue(rec.categories);
            se->addChild("comments").setValue(rec.comments);
            se->addChild("journal-ref").setValue(rec.ref);
            se->addChild("doi").setValue(rec.doi);
            se->addChild("abstract").setValue(rec.abstract);
            se = &se->addChild("authors");

            for (const auto& a : rec.authors) {
                zen::XmlElement& sse = se->addChild("author");
                sse.addChild("forenames").setValue(a.forenames);
                sse.addChild("keyname").setValue(a.keyname);
            }
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
