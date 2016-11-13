#include "oai-common.hpp"

int main(int argc, char* argv[]) {
    auto new_recs = oai::read_records(argv[1]);
    auto old_recs = oai::read_records(argv[2]);
    oai::merge(old_recs, new_recs);

    note("merged two files into ", old_recs.size(), " records");
    oai::write_records(argv[3], old_recs);

    return 0;
}
