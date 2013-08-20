/**
 * An inverted index data structure consisting of
 * the following components:
 *
 *  - SegmentPool, which contains all segments.
 *  - Dictionary, which is a mapping from term to term id
 *  - Pointers, which contains Document Frequency, Head/Tail Pointers,
 *    etc.
 *  - DocumentVectors, which contains compressed document vector representation
 *    of documents
 *
 * @author Nima Asadi
 */

#ifndef IZENELIB_IR_ZAMBEZI_INVERTED_INDEX_HPP
#define IZENELIB_IR_ZAMBEZI_INVERTED_INDEX_HPP

#include "SegmentPool.hpp"
#include "Dictionary.hpp"
#include "Pointers.hpp"
#include "buffer/BufferMaps.hpp"
#include "Consts.hpp"

#include <boost/shared_ptr.hpp>
#include <iostream>
#include <vector>

NS_IZENELIB_IR_BEGIN

namespace Zambezi
{

class InvertedIndex
{
public:
    InvertedIndex(
            IndexType type,
            bool reverse, bool bloomEnabled,
            uint32_t nbHash, uint32_t bitsPerElement);

    ~InvertedIndex();

    void save(std::ostream& ostr) const;
    void load(std::istream& istr);

    bool hasValidPostingsList(uint32_t termid) const;

    void insertDoc(uint32_t docid, const std::vector<std::string>& term_list);
//  void addFilter(const std::string& filter_name, const std::vector<uint32_t>& docid_list);
    void flush();

    void retrieval(
            Algorithm algorithm,
            const std::vector<std::string>& term_list,
            uint32_t hits,
            std::vector<uint32_t>& docid_list,
            std::vector<float>& score_list) const;

private:
    IndexType type_;
    BufferMaps buffer_;
    SegmentPool pool_;
    Dictionary dictionary_;
    Pointers pointers_;
};

}

NS_IZENELIB_IR_END

#endif