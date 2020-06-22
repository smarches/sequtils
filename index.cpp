#include <vector>
#include <utility>
#include <filesystem>
#include "lib/bioio.hpp"

// index object

template<size_t K>
class seq_index {
    constexpr unsigned maxKeySize{20};
    // each pair is [index of seq. in 'index' and offset inside the seq]
    using std::pair<size_t,size_t> = indexLoc;
    namespace fs = std::filesystem;
    // the main structure is a vector of vectors of indexLocs
    std::vector<std::vector<indexLoc>> kmer_index;
    // most of the memory overhead is just storing the sequences themselves
    // the FastaRecord struct is essentially a pair of strings which is move-constructable only
    std::vector<bioio::FastaRecord<std::string, std::string>> index_seqs;
public:
    seq_index() {
        static_assert(K <= maxKeySize);
        // allocate table of pointers: there are 4^K valid kmers
        kmer_index.reserve(1 << (2*K));
    }
    seq_index(const size_t initial_alloc) {
        static_assert(K <= maxKeySize);
        // allocate table of pointers: there are 4^K valid kmers
        kmer_index.reserve(1 << (2*K));
        for(size_t i=0; i < initial_alloc;++i) {
            kmer_index[i].reserve(initial_alloc);
        }
    }
    size_t populate_kmers(const std::string& seq,size_t i) {
        // i -> position of this sequence in the index
        // j -> offset into the sequence at which begins kmer value k
        kmer_index[k].push_back(std::make_pair<size_t,size_t>(i,j));
    }
    // can call this multiple times to keep adding stuff to index from multiple files
    populate_index(fs::path& p) {
        // need to convert p back to std::string for bioio:
        const auto items = bioio::read_fasta(p.string());
        // at this point, we can have one pass of parallelism for index populating,
        // so long as the mutex for updating vectors isn't too costly - it shouldn't be since
        // the chance of a 'collision' (two threads updating the same kmer value vector) should be relatively low
        for(size_t i=0; i < items.size();++i) {
            populate_kmers(e[i].sequence,i);
        }
    }
};

