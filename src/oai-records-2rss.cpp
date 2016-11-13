#include "rss-common.hpp"
#include "prefs-common.hpp"

int main(int argc, char* argv[]) {
    auto recs = oai::read_records(argv[1]);
    auto pref = prefs::read_preferences(argv[3]);

    std::vector<std::string> star_include;
    prefs::get(pref, "star_include", star_include);

    std::string host;
    prefs::get(pref, "http_host", host);
    rss::write_records(argv[2], recs, host, star_include);

    return 0;
}
