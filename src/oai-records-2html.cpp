#include "html-common.hpp"

int main(int argc, char* argv[]) {
    auto recs = oai::read_records(argv[1]);
    html::write_records(argv[2], recs);

    return 0;
}
