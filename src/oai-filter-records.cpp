#include "prefs-common.hpp"
#include "oai-common.hpp"

const bool debug = false;

int main(int argc, char* argv[]) {
    auto recs = oai::read_records(argv[1]);
    auto pref = prefs::read_preferences(argv[2]);
    std::string minimum_date = argv[3];

    bool allow_crosspost = true, allow_replacements = true, only_accepted = false;
    std::vector<std::string> content_exclude,
                             categories_must_include, categories_exclude;

    prefs::get(pref, "content_exclude",         content_exclude);
    prefs::get(pref, "categories_must_include", categories_must_include);
    prefs::get(pref, "categories_exclude",      categories_exclude);
    prefs::get(pref, "allow_crosspost",         allow_crosspost);
    prefs::get(pref, "allow_replacements",      allow_replacements);
    prefs::get(pref, "only_accepted",           only_accepted);

    std::size_t size_before = recs.size();
    recs.erase(std::remove_if(recs.begin(), recs.end(), [&](oai::record& rec){
        // Filter by date and history.
        // ---------------
        if (only_accepted) {
            // Check if this is an accepted paper
            if (!find_word(rec.comments, "accepted")) {
                if (debug) note(rec.id, " removed because it is not an accepted publication");
                return true;
            }
        } else if (!allow_replacements) {
            // Check if this is a replacement of an already published paper
            if (!rec.updated.empty()) {
                if (debug) note(rec.id, " removed because it is a replacement");
                return true;
            }
        }

        // Check if it is too old. This can happen if:
        //  - it was present in the DB from an old poll and should be removed to avoid
        //    cluttering the DB,
        //  - it may be an internal arXiv update that we do not care about.
        std::string last_date = rec.created;
        if (!rec.updated.empty()) last_date = rec.updated;
        if (oai::date_less(last_date, minimum_date)) {
            if (debug) note(rec.id, " removed because it is too old (", last_date, ")");
            return true;
        }

        // Filter by categories
        // --------------------
        // Check if this is a cross-posting
        if (!allow_crosspost) {
            std::vector<std::string> cats = split(rec.categories, " ");
            if (cats.size() > 1) {
                if (debug) note(rec.id, " removed because it is a cross-posting (", rec.categories, ")");
                return true;
            }
        }

        // Filter out by matching
        for (const auto& w : categories_exclude) {
            if (find_word(rec.categories, w)) {
                if (debug) note(rec.id, " removed because it contains an exluded category (", w, ")");
                return true;
            }
        }

        if (!categories_must_include.empty()) {
            bool found = false;
            for (const auto& w : categories_must_include) {
                if (find_word(rec.categories, w)) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                if (debug) note(rec.id, " removed because it is in the wrong categories (", rec.categories, ")");
                return true;
            }
        }

        // Filter by keywords on content
        // -----------------------------
        for (const auto& w : content_exclude) {
            if (find_word(rec.title, w) || find_word(rec.abstract, w) ||
                find_word(rec.ref, w) || find_word(rec.comments, w)) {
                if (debug) note(rec.id, " removed because it contains an exluded keyword (", w, ")");
                return true;
            }
        }

        return false;
    }), recs.end());

    note("removed ", size_before - recs.size(), " records according to user filters");

    oai::write_records(argv[1], recs);

    return 0;
}
