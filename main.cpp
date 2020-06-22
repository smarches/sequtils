#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <variant>
#include <unordered_map>
#include <string_view>
#include <filesystem>
#include "lib/bioio.hpp"
#include "lib/json.hpp"

// the json object/class
using json = nlohmann::json;

void usage(bool err) {
    if(err) std::cout << "Incorrect number of arguments!\n";
    std::cout << "Usage: bv <config.json>\n";
    exit(err ? EXIT_FAILURE : EXIT_SUCCESS);
}

void fail(const std::string& reason) {
    std::cout << reason << '\n';
    exit(EXIT_FAILURE);
}

// rmbr link lib -lstdc++fs
// need a 'string-like' concept here
template<class T>
bool file_exists(const T& s) {
    // create a path and normalize it:
    std::filesystem::path cfg(s);
    return std::filesystem::exists(cfg);
}

// config fields may be bool, signed/unsigned int, or float
// using std::variant as a convenient generic container
// so that the config is just a map between tag and value
void validate_config(const json& config) {
    const std::array<std::string,2> fileArgs{"read_file","index_file"};
    const std::array<std::string,3> boolArgs{"output_unaligned","output_merged","output_meta"};
    // can also have other type args (unsigned int)
    std::stringstream reason;
    for(const auto e : fileArgs) {
        if(! config.contains(e)) {
            reason << "Config must have value " << e << ".";
            fail(reason.str());
        }
        const auto filename = config.get<std::string>(e);
        if (!file_exists(filename)) {
            reason << "File " << filename << "(config " << e << ") not found!";
            fail(reason.str());
        }
    }
    for(const auto e: boolArgs) {
        if( ! config[e].is_boolean()) {
            reason << "Config value " << e << " must be boolean (true or false)";
            fail(reason.str());
        }
    }
}


int main(int argc, const char* argv[]) {
    // read in JSON config as first arg:
    if(argc != 1) usage(true);
    const std::string a1(argv[1]);
    if(a1 == "-h" || a1 == "--help") {
        usage(false); // user intentionally asked for help, not an error
    }
    // check that file exists:
    if(! file_exists(a1) ) {
        std::cerr << "Could not find config file: " << a1 << '\n';
        exit(EXIT_FAILURE);
    }
    // parse config
    json config;
    std::ifstream fh(a1);
    fh >> config;
    // if the config is valid, there's no need to parse it into another object, we just keep the nice
    // JSON representation with all its convenience methods
    validate_config(config);

    // call entrypoint for actual work

    return 0;
}