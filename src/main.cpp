#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <zlib.h>
#include <vector>

using namespace std;

// function for reading and returning compressed data as a string
string compressed_file_data(filesystem::path &path) {

    ifstream compressed_file(path, ios::binary);
    if(!compressed_file.is_open()) {
        throw runtime_error("couldn't read the file " + path.string());
    }
    return string((istreambuf_iterator<char>(compressed_file)), istreambuf_iterator<char>());
}

string decompress_file_data(string &compressed_data) {
    uLongf uncompressed_size = compressed_data.size() *10;
    vector<char> uncompressed_buffer(uncompressed_size);

    int result = uncompress((Bytef*)uncompressed_buffer.data(), &uncompressed_size,
    (const Bytef*)compressed_data.data(), compressed_data.size());

    if (result != Z_OK) {
        throw runtime_error("Failed to decompress data using zlib.");
    }

    return string(uncompressed_buffer.begin(), uncompressed_buffer.begin() + uncompressed_size);
}

void cat_file(const string &hash) {
    string hash_prefix = hash.substr(0,2);
    string file_name = hash.substr(2);
    filesystem::path object_path = filesystem::path(".git")/ "objects" / hash_prefix / file_name;

    try {
        string compressed_data = compressed_file_data(object_path);
        string data = decompress_file_data(compressed_data);

        int nullbyte_position = data.find('\0');
        if (nullbyte_position != string::npos) {
            string file_content = data.substr(nullbyte_position + 1);
            cout << file_content;
        }
    } catch (const exception error) {
        cerr << error.what() << "\n";
    }
}

// argument count and real argument value 
int main(int argc, char *argv[])
{
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // You can use print statements as follows for debugging, they'll be visible when running tests.
    std::cerr << "Logs from your program will appear here!\n";

    // Uncomment this block to pass the first stage
    //
    if (argc < 2) {
        std::cerr << "No command provided.\n";
        return EXIT_FAILURE;
    }
    //
    std::string command = argv[1];
    
    if (command == "init") {
        try {
            std::filesystem::create_directory(".git");
            std::filesystem::create_directory(".git/objects");
            std::filesystem::create_directory(".git/refs");
    
            std::ofstream headFile(".git/HEAD");
            if (headFile.is_open()) {
                headFile << "ref: refs/heads/main\n";
                headFile.close();
            } else {
                std::cerr << "Failed to create .git/HEAD file.\n";
                return EXIT_FAILURE;
            }
    
            std::cout << "Initialized git directory\n";
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << e.what() << '\n';
            return EXIT_FAILURE;
        }
    }else if(command == "cat-file") {
        if(argc >=4 && string(argv[2]) == "-p") {
            cat_file(argv[3]);
        }

    } else {
        std::cerr << "Unknown command " << command << '\n';
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
