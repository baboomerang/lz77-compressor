//==================================================================================
/* LZ77 COMPRESSOR - by baboomerang
 *  * DISCLAIMER: This "lz-77" is provided by baboomerang (the writer & provider \
 * of this software "as is" and "with all faults." baboomerang (the writer & \
 * provider of this software) makes no representations or warranties of any kind \
 * concerning the safety,suitability, lack of viruses, inaccuracies, \
 * typographical errors, or other harmful components of this "lz77-compressor". \
 * There are inherent dangers in the use of any software, and you are solely \
 * responsible for determining whether this "lz77-compressor" is compatible with \
 * your equipment and other software installed on your equipment. You are also \
 * solely responsible for the protection of your equipment and backup of your data, \
 * and baboomerang (the writer & provider of this software) will not be liable \
 * for any damages you may suffer in connection with using, modifying, or \
 * distributing this "lz77-compressor" or any part of it. */
//==================================================================================
#include <iostream>
#include <fstream>
#include <vector>

#include <assert.h>
#define assertm(exp, msg) assert(((void)msg, exp))

#ifndef LOOK_SIZE
#define LOOK_SIZE 4
#endif

#ifndef SEARCH_SIZE
#define SEARCH_SIZE 6
#endif

std::vector<char> encode(std::ifstream&);
struct Triplet chooseBest(std::vector<struct Triplet>&);

struct Triplet {
    int offset;
    int length;
    char token;
};

inline int filesize(std::ifstream &file) {
    file.seekg(0);
    file.seekg(std::ios::beg, std::ios::end);
    int size = file.tellg();
    file.seekg(0);
    return size;
}

// WARNING: USES THE ADAPTIVE ENCODING SLIDING WINDOW METHOD WHICH IS DIFFERENT THAN
// THE GENERIC SPEC. THE SEARCH BUFFER POINTER MOVES INTO THE LOOKAHEAD REGION
// FOR BETTER COMPRESSION.

// IN OTHER IMPLEMENTATIONS, THIS ONLY HAPPENS AT THE END OF THE FILE.

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <file>\n";
        return 1;
    }

    std::ifstream input{argv[1], std::ios_base::binary};
    assertm(input.is_open(), (std::string(argv[1]) + "does not exist/cannot be read."));

    std::ofstream output{std::string(argv[1]) + ".lz77", std::ios_base::binary};
    assertm(output.is_open(), ("Cannot write file" + (std::string(argv[1]) + "lz.77") \
            + "double check write permissions.\nMake sure you aren't writing to a block device!"));

    int size = filesize(input);
    std::vector<char> result = encode(input);
    output.write(result.data(), result.size());

    // OUTPUT RESULTS WILL VARY BASED ON SEARCH_SIZE AND LOOK_SIZE AND FILE SIZE
    // TOO SMALL FILE SIZE AND YOU WILL GET NEGATIVE COMPRESSION

    std::cout << "Previous size: " << size << " ";
    std::cout << "Compressed size: " << result.size() << "\n\n";
    std::cout << "Results saved to " << argv[1] << ".lz77" << "\n";
    return 0;
}

auto findMatch(std::vector<char> &window, \
        std::vector<char>::iterator itsearch, \
            std::vector<char>::iterator itlook) -> Triplet {

    int offset = 0;
    int length = 0;
    while (*itsearch == *itlook) {
        if (!offset)
            offset = std::distance(itsearch, window.end() - LOOK_SIZE);
        
        length++;
        itlook++;
        if (*itsearch != *itlook)
            itsearch++;
    }
    return Triplet{offset, length, *itlook};
}

std::vector<char> encode(std::ifstream &file) {
    std::vector<char> window;
    std::vector<char> result;

    //initial setup
    for (int x = 0; x < LOOK_SIZE; x++)
        window.push_back(file.get());

    //the main engine
    while (file) {
            std::vector<Triplet> possiblepaths;
            auto it_look = window.end() - LOOK_SIZE;

            //match token in look buffer against every search buffer item
            for (auto it_search = window.begin(); \
                        it_search < window.end() - LOOK_SIZE; it_search++) {

                Triplet match = findMatch(window, it_search, it_look);
                if (match.offset)
                    possiblepaths.push_back(match);
            }

            if (possiblepaths.empty())
                possiblepaths.push_back(Triplet{0, 0, *it_look});

            Triplet bestpath = chooseBest(possiblepaths);
            result.push_back(bestpath.offset);
            result.push_back(bestpath.length);
            result.push_back(bestpath.token);

            //shift the window n times
            for (int n = bestpath.length + 1; n > 0; n--) {
                if (!file.eof())
                    window.push_back(file.get());
                else
                    window.erase(window.begin());

                if (window.size() > SEARCH_SIZE + LOOK_SIZE)
                    window.erase(window.begin());
            }
    }

    return result;
}

struct Triplet chooseBest(std::vector<Triplet> &buffer) {
    int bestlength = 0;
    int optimaloffset = 0;

    //TODO: reverse the conditional to make it clearer for the reader

    for (auto it = buffer.begin(); it < buffer.end(); it++) {
        if (it->offset >= optimaloffset && it->length <= bestlength) {
            buffer.erase(it); //memory leak (my guess)
        } else {
            bestlength = it->length;
            optimaloffset = it->offset;
        }
    }

    return buffer[0];
}
