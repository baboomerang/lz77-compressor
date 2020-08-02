#include <iostream>
#include <fstream>
#include <vector>

#include <assert.h>
#define assertm(exp, msg) assert(((void)msg, exp))

#ifndef LOOK_SIZE
#define LOOK_SIZE 6
#endif

#ifndef SEARCH_SIZE
#define SEARCH_SIZE 7
#endif

/* LZ77 COMPRESSOR - by baboomerang
 *  * DISCLAIMER: This "lz-77" is provided by baboomerang (the writer & provider \
 * of this software "as is" and "with all faults." baboomerang (the writer & \
 * provider of this software) makes no representations or warranties of any kind \
 * concerning the safety,suitability, lack of viruses, inaccuracies, \
 * typographical errors, or other harmful components of this "lz77-compressor". \
 * There are inherent dangers in the use of any software, and you are solely \
 * responsible for determining whether this "lz77-compressor" is compatible with \
 * your equipment and other software installed on your equipment. You are also 
 * solely responsible for the protection of your equipment and backup of your data, \
 * and baboomerang (the writer & provider of this software) will not be liable \
 * for any damages you may suffer in connection with using, modifying, or \
 * distributing this "lz77-compressor" or any part of it.
 */

std::vector<char> encode(std::ifstream&);
struct Triplet chooseBest(std::vector<struct Triplet>&);

struct Triplet {
    int offset;
    int length;
    char token;
};

inline int filesize(std::ifstream &file) {
    file.seekg(std::ios::beg, std::ios::end);
    int size = file.tellg();
    file.seekg(0);
    return size;
}

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

    std::vector<char> result = encode(input);
    output.write(result.data(), result.size());

    std::cout << "Previous size: " << filesize(input) << " ";
    std::cout << "Compressed size: " << result.size() << "\n";
    std::cout << "Results saved to " << argv[1] << ".lz77" << "\n";
    return 0;
}

auto findMatch(std::vector<char> &window, \
        std::vector<char>::reverse_iterator itsearch, \
            std::vector<char>::iterator itlook) -> Triplet {
    int offset = 0;
    int length = 0;

    while (*itsearch == *itlook) {
        if (!offset)
            offset = std::distance(window.rend() - LOOK_SIZE, itsearch);
        
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
    for (int x = 0; x < 5; x++)
        window.push_back(file.get());

    while (file) {
            //  all iterators are relative to the end ]
            //          it    lookit                  |
            //  [][][][][i]    [v][][][][x] <---------]
            //
            //  btw, x.rend() == x.end() - 1;
            //  x.rend() is a reverse iterator
            //  x.end() is a forward iterator

            std::vector<Triplet> possiblepaths;
            auto it_look = window.end() - LOOK_SIZE;

            for (auto it_search = window.rend() - LOOK_SIZE; \
                                    it_search > window.rbegin(); it_search--) {

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
            int n = bestpath.length;
            do {
                window.push_back(file.get());
                n--;
            } while (n);
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
