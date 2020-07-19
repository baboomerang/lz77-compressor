#include <iostream>
#include <fstream>
#include <vector>

#define LOOKAHEAD_SIZE 4
#define SEARCH_SIZE 5

inline int filesize(std::ifstream&);
inline void copy(struct Triplet&, std::vector<char>&);
std::vector<struct Triplet> chooseBest(std::vector<struct Triplet>);
char* compress(char*&, int&);

struct Triplet {
    int offset;
    int length;
    char token;
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <file>\n";
        return 1;
    }

    std::ifstream input{argv[1], std::ios_base::binary};
    int size = filesize(input);

    char* array{new char[size]};
    input.read(array, size);

    compress(array, size);
    return 0;
}

inline int filesize(std::ifstream &file) {
    file.seekg(std::ios::beg, std::ios::end);
    int size = file.tellg(); //total number of bytes in stream
    file.seekg(0);          //set pointer to beginning
    return size;
}

inline void copy(struct Triplet &in, std::vector<char> &out) {
    out.push_back(in.offset);
    out.push_back(in.length);
    out.push_back(in.token);
}

std::vector<Triplet> chooseBest(std::vector<Triplet> buffer) {
    int max = 0;
    for (auto it = buffer.begin(); it < buffer.end(); it++) {
        if (it->length > max) {
            max = it->length;
        } else {
            delete &*it;    //memory leak?
            buffer.erase(it);
        }
    }
    return buffer;
}

char* compress(char* &array, int &size) {
    static std::vector<char> result;
    std::vector<char> look;
    std::vector<char> search;
    std::vector<Triplet> paths;

    look.reserve(LOOKAHEAD_SIZE);
    search.reserve(SEARCH_SIZE);

    for (int x = 0; x < size; x++) {
        look.push_back(array[x]);

        if (look.size() > LOOKAHEAD_SIZE) {
            look.erase(look.begin());   //remove the copied token from the vector
        }

        //init vector iterators
        auto s_it = search.begin();
        auto l_it = look.begin();

        int offset = 0;
        int length = 0;
        //traverse both vectors simulaneously
        while (s_it < search.end() && l_it < look.end()) {
            if (*s_it == *l_it) {
                if (!offset)     //offset is non-zero UNTIL FIRST match
                    offset = std::distance(s_it, search.end());
                s_it++;
                l_it++;
                length++;

                if (l_it >= look.end() || s_it >= search.end())
                    break; //if either vector reach their end, its a perfect match
            } else {
                if (offset) { //case 1: finds some matches but hits a mismatch
                    paths.push_back(Triplet{offset, length, *l_it});
                    offset = 0;
                    length = 0;
                }
            }
        }

        //if it never matched or save the token because it matched once
        if (paths.empty() || offset)
            paths.push_back(Triplet{offset, length, *l_it});

        if (paths.size() > 1)
            chooseBest(paths);   //best path is largest, larger = more compression

        copy(paths[0], result);
        length = paths[0].length;

        //at this point, best path with longest length is chosen for current token
        //keep pop items from look vector into search vector until the length
        //that length CANT EVER be greater than LOOKAHEAD_SIZE



    }
    //end of file handling for lz77. both vectors need to be joined somehow
    //search vector has to expand into lookahead and compress as one

    return result.data();
}
