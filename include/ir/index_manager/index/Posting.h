/**
* @file        Posting.h
* @author     Yingfeng Zhang
* @version     SF1 v5.0
* @brief Process the postings
*/

#ifndef POSTING_H
#define POSTING_H

#include <ir/index_manager/utility/system.h>
#include <ir/index_manager/utility/MemCache.h>
#include <ir/index_manager/utility/Utilities.h>
#include <ir/index_manager/store/IndexOutput.h>
#include <ir/index_manager/index/CompressedPostingList.h>


NS_IZENELIB_IR_BEGIN

namespace indexmanager{
class OutputDescriptor;
class InputDescriptor;

/**
*virtual base class of InMemoryPosting and OnDiskPosting
*/

class Posting
{
public:
    Posting();
    virtual ~Posting();
public:
    /**
     * Get the posting data
     * @param ppState the state of decode process
     * @param pPosing the address to store posting data
     * @param the length of pPosting,also tell us the length of actually decoded data
     * @return decoded posting count
     */
    virtual int32_t decodeNext(uint32_t* pPosting,int32_t length) = 0;

    /**
     * Get the position posting data
     * @param pPosing the address to store posting data
     * @param length length of pPosting
     */
    virtual void decodeNextPositions(uint32_t* pPosting,int32_t length) = 0;

    /**
     * Get the position posting data
     * @param pPosing the address to store posting data
     * @param pFreqs freqs array
     * @param nFreqs size of freqs array
     */
    virtual void decodeNextPositions(uint32_t* pPosting,uint32_t* pFreqs,int32_t nFreqs) = 0;

    /**
     * reset the base position which used in d-gap encoding
     */
    virtual void resetPosition() = 0;

    /**
     * reset the content of Posting list.
     */
    virtual void reset() = 0;

    /**
     * clone the object
     * @return the clone object
     */
    virtual Posting* clone() = 0;

    /**
     * get document frequency
     * @return DF value
     */
    virtual count_t docFreq()const = 0;

    /**
     * get collection's total term frequency
     * @return CTF value
     */
    virtual int64_t getCTF()const = 0;

    /**
     * set buffer for posting reading
     * @param buffer buffer for posting
     * @param nBufSize size of buffer
     * @return size of used buffer
     */
    virtual size_t setBuffer(int32_t* buffer,size_t nBufSize)
    {
        return 0;
    }

    /**
     * get buffer size of posting
     * @return buffer size
     */
    virtual size_t getBufferSize()
    {
        return 0;
    }
};

/**
*InMemoryPosting
*/

class InMemoryPosting:public Posting
{
public:
    struct DecodeState
    {
        PostingChunk* decodingDChunk;
        int32_t decodingDChunkPos;
        docid_t lastDecodedDocID;		///the latest decoded doc id
        int32_t decodedDocCount;
        PostingChunk* decodingPChunk;
        int32_t decodingPChunkPos;
        loc_t lastDecodedPos; 		///the latest decoded offset
        loc_t lastDecodedCharPos;        ///the latest decoded char offset
        int32_t decodedPosCount;
    };
public:
    InMemoryPosting();
    InMemoryPosting(MemCache* pMemCache);
    virtual ~InMemoryPosting();
public:

    /**
     * allocate new chunk
     * @param the size of chunk
     * @return the chunk object
     **/
    PostingChunk* newChunk(int32_t chunkSize);

    /**
     * Get the next chunk size
     * @param nCurSize current accumulated chunk size
     * @param as memory allocation strategy
     */
    int32_t getNextChunkSize(int32_t nCurSize);

    /**
    * update document frequency incrementally. DF will contain doc ids from other field.
    * @param docid the identifier of document
    */
    void updateDF(docid_t docid);
    /**
     * add (docid,position) pair
     * @param docid the identifier of document
     * @param location the location of term
     */
    void addLocation(docid_t docid, loc_t location);

    /**
     * Is there any chunk?
     * @return false: has allocated; true:no memory has allocated
     **/
    bool hasNoChunk()
    {
        return (pDocFreqList->pTailChunk==NULL);
    }

    /**
     * get document frequency
     * @return DF value
     */

    ///if the document frequency is required to be counted based on the total document, then use tdf
    //count_t docFreq() const {return nTDF;};
    count_t docFreq() const
    {
        return nDF;
    };

    /**
     * get collection's total term frequency
     * @return CTF value
     */
    int64_t getCTF() const
    {
        return nCTF;
    };

    /** get last added doc id */
    docid_t lastDocID()
    {
        return nLastDocID;
    }

    /**
     * write index data
     * @param pOutputDescriptor output place
     * @return	offset of posting descriptor
     */
    fileoffset_t write(OutputDescriptor* pOutputDescriptor);

    /**
     * reset the content of Posting list.
     */
    void reset();


    Posting* clone();

public:
    /**
     * Get the posting data
     * @param ppState the state of decode process
     * @param pPosing the address to store posting data
     * @param the length of pPosting,also tell us the length of actually decoded data
     * @return decoded posting count
     */
    int32_t decodeNext(uint32_t* pPosting,int32_t length);

    /**
     * Get the position posting data
     * @param pPosing the address to store posting data
     * @param the length of pPosting
     * @return true:success,false: error or reach end
     */
    void decodeNextPositions(uint32_t* pPosting,int32_t length);

    /**
     * Get the position posting data
     * @param pPosing the address to store posting data
     * @param pFreqs freqs array
     * @param nFreqs size of freqs array
     */
    void decodeNextPositions(uint32_t* pPosting,uint32_t* pFreqs,int32_t nFreqs);

    /**
     * reset the base position which used in d-gap encoding
     */
    void resetPosition();

    /**
     * flush last document
     * @param bTruncTail trunc the tail of posting or not
     */
    void flushLastDoc(bool bTruncTail);
protected:
    /**
     * write document posting to output
     * @param pDOutput document posting output
     */
    void writeDPosting(IndexOutput* pDOutput);

    /**
     * write position posting to output
     * @param pPOutput position posting output
    	 */
    fileoffset_t writePPosting(IndexOutput* pPOutput);

    /**
     * write descriptors to output
     * @param pDOutput document posting output
     * @param poffset position offset
     */
    void writeDescriptor(IndexOutput* pDOutput,fileoffset_t poffset);
protected:
    MemCache* pMemCache;	/// memory cache
    count_t nDF;			///document frequency of this field
    count_t nTDF;			///term frequency of this document
    docid_t nLastDocID;	///current added doc id
    docid_t nYetAnotherLastDocID; ///last doc id served for nTDF;
    loc_t nLastLoc; 		///current added word offset
    count_t nCurTermFreq; ///current term freq
    int32_t nCTF; 			///Collection's total term frequency
    DecodeState* pDS;			///decoding state
    CompressedPostingList* pDocFreqList; /// Doc freq list
    CompressedPostingList* pLocList; 	/// Location list

    friend class PostingMerger;
public:
    static int32_t UPTIGHT_ALLOC_CHUNKSIZE;
    static int32_t UPTIGHT_ALLOC_MEMSIZE;
};

/**
*OnDiskPosting
*/


class OnDiskPosting:public Posting
{
    struct DecodeState
    {
        docid_t lastDecodedDocID;		///the latest decoded doc id
        int32_t decodedDocCount;		///decoded doc count
        loc_t lastDecodedPos; 		///the latest decoded position posting
        int32_t decodedPosCount;		///decoded position posting count
    };
public:
    OnDiskPosting();
    OnDiskPosting(InputDescriptor* pInputDescriptor,fileoffset_t poffset);
    ~OnDiskPosting();

    InputDescriptor* getInputDescriptor()
    {
        return pInputDescriptor;
    }
    /**
     * Get the posting data
     * @param pPosing the address to store posting data
     * @param the length of pPosting,also tell us the length of actually decoded data
     * @return decoded posting count
     */
    int32_t decodeNext(uint32_t* pPosting,int32_t length);

    /**
     * Get the position posting data
    	 * @param pPosing the address to store posting data
     * @param the length of pPosting
     * @return true:success,false: error or reach end
     */
    void decodeNextPositions(uint32_t* pPosting,int32_t length);

    /**
     * Get the position posting data
     * @param pPosing the address to store posting data
     * @param pFreqs freqs array
     * @param nFreqs size of freqs array
    	 */
    void decodeNextPositions(uint32_t* pPosting,uint32_t* pFreqs,int32_t nFreqs);

    /**
     * reset the base position which used in d-gap encoding
     */
    void resetPosition();

    /**
    	 * reset the content of Posting list.
     */
    void reset();

    /**
     * reset to a new posting
     * @param newOffset offset to the target posting
     */
    void reset(fileoffset_t newOffset);

    /**
     * clone the object
     * @return the clone object
     */
    Posting* clone();

    /**
     * get document frequency
     * @return DF value
     */

    ///if the document frequency is required to be counted based on the total document, then use tdf
    //count_t docFreq()const{return postingDesc.tdf;};
    count_t docFreq()const
    {
        return postingDesc.df;
    };

    /**
     * get collection's total term frequency
     * @return CTF value
     */
    int64_t getCTF()const
    {
        return postingDesc.ctf;
    };

    /**
     * set buffer for posting reading
     * @param buffer buffer for posting
     * @param nBufSize size of buffer
     * @return size of used buffer
     */
    size_t setBuffer(int32_t* buffer,size_t nBufSize);

    /**
     * get buffer size of posting
     * @return buffer size
     */
    virtual size_t getBufferSize()
    {
        return nBufSize;
    }
protected:
    PostingDescriptor postingDesc;
    ChunkDescriptor chunkDesc;
    fileoffset_t postingOffset;
    int64_t nPPostingLength;
    InputDescriptor* pInputDescriptor;
    size_t nBufSize;
    OnDiskPosting::DecodeState ds;


    friend class PostingMerger;
};


}

NS_IZENELIB_IR_END

#endif
