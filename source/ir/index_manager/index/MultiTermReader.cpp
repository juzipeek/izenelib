#include <ir/index_manager/index/MultiIndexBarrelReader.h>
#include <ir/index_manager/index/MultiTermReader.h>
#include <ir/index_manager/index/MultiTermDocs.h>
#include <ir/index_manager/index/MultiTermPositions.h>
#include <ir/index_manager/index/MultiTermIterator.h>

#include <memory> // auto_ptr

using namespace izenelib::ir::indexmanager;

MultiTermReader::MultiTermReader(MultiIndexBarrelReader* pReader, collectionid_t id)
        : colID_(id)
        , pBarrelReader_(pReader)
        , pCurReader_(NULL)
{
}

MultiTermReader::~MultiTermReader(void)
{
    pBarrelReader_ = NULL;
    close();
}

void MultiTermReader::open(Directory* pDirectory,BarrelInfo* pBarrelInfo,FieldInfo* pFieldInfo)
{
}

void MultiTermReader::reopen()
{
    ReaderCache* pList = pCurReader_;
    while(pList)
    {
        pList->pTermReader_->reopen();
        pList = pList->next_;
    }
}

TermIterator* MultiTermReader::termIterator(const char* field)
{
    ReaderCache* pReaderCache = NULL;
    map<string,ReaderCache*>::iterator iter = readerCache_.find(field);
    if (iter != readerCache_.end())
    {
        pReaderCache = iter->second;
    }
    else
    {
        pReaderCache = loadReader(field);
    }
    MultiTermIterator* pTermIterator = new MultiTermIterator();
    TermIterator* pIt;
    while (pReaderCache)
    {
        pIt = pReaderCache->pTermReader_->termIterator(field);
        if (pIt)
            pTermIterator->addIterator(pIt);
        pReaderCache = pReaderCache->next_;
    }
    return pTermIterator;
}

bool MultiTermReader::seek(Term* term)
{
    bool bSuc = false;
    string field = term->getField();
    ReaderCache* pReaderCache = NULL;

    pCurReader_ = NULL;
    map<string,ReaderCache*>::iterator iter = readerCache_.find(field);
    if (iter != readerCache_.end())
    {
        pCurReader_ = pReaderCache = iter->second;
    }
    else
    {
        pCurReader_ = pReaderCache = loadReader(field.c_str());
    }
    while (pReaderCache)
    {
        bSuc = (pReaderCache->pTermReader_->seek(term) || bSuc);
        pReaderCache = pReaderCache->next_;
    }

    return bSuc;
}

TermDocFreqs* MultiTermReader::termDocFreqs()
{
    if (!pCurReader_)
        return NULL;

    MultiTermDocs* pTermDocs = new MultiTermDocs();
    bool bAdd = false;
    TermDocFreqs* pTmpTermDocs = NULL;
    ReaderCache* pList = pCurReader_;
    while (pList)
    {
        pTmpTermDocs = pList->pTermReader_->termDocFreqs();
        if (pTmpTermDocs)
        {
            pTermDocs->add(pList->pBarrelInfo_,pTmpTermDocs);
            bAdd = true;
        }
        pList = pList->next_;
    }
    if (bAdd == false)
    {
        delete pTermDocs;
        return NULL;
    }
    return pTermDocs;
}

TermPositions* MultiTermReader::termPositions()
{
    if (!pCurReader_)
        return NULL;
    MultiTermPositions* pTermPositions = new MultiTermPositions();
    bool bAdd = false;
    TermPositions* pTmpTermPositions = NULL;
    ReaderCache* pList = pCurReader_;
    while (pList)
    {
        pTmpTermPositions = pList->pTermReader_->termPositions();
        if (pTmpTermPositions)
        {
            pTermPositions->add(pList->pBarrelInfo_, pTmpTermPositions);
            bAdd = true;
        }
        pList = pList->next_;
    }
    if (bAdd == false)
    {
        delete pTermPositions;
        return NULL;
    }
    return pTermPositions;
}

freq_t MultiTermReader::docFreq(Term* term)
{
    freq_t df = 0;
    string field = term->getField();
    ReaderCache* pReaderCache = NULL;

    pCurReader_ = NULL;
    map<string,ReaderCache*>::iterator iter = readerCache_.find(field);
    if (iter != readerCache_.end())
    {
        pCurReader_ = pReaderCache = iter->second;
    }
    else
    {
        pCurReader_ = pReaderCache = loadReader(field.c_str());
    }

    while (pReaderCache)
    {
        df += pReaderCache->pTermReader_->docFreq(term);
        pReaderCache = pReaderCache->next_;
    }

    return df;
}

void MultiTermReader::close()
{
    for(map<string,ReaderCache*>::iterator iter = readerCache_.begin(); 
            iter != readerCache_.end(); ++iter)
        delete iter->second;
    pCurReader_ = NULL;
}

TermReader* MultiTermReader::clone()
{
    return new MultiTermReader(pBarrelReader_, colID_);
}

ReaderCache* MultiTermReader::loadReader(const char* field)
{
    ReaderCache* pTailList = NULL;
    ReaderCache* pPreList = NULL;
    // use auto_ptr in case of memory leak when exception is thrown in below for loop
    std::auto_ptr<ReaderCache> headListPtr;
    TermReader* pSe = NULL;

    BarrelReaderEntry* pEntry = NULL;

    for(vector<BarrelReaderEntry*>::iterator iter = pBarrelReader_->readers_.begin(); 
            iter != pBarrelReader_->readers_.end(); ++iter)	
    {
        pEntry = (*iter);
        TermReader* pTermReader = pEntry->pBarrelReader_->termReader(colID_, field);
        if(pTermReader)
            pSe =  pTermReader->clone();
        if (pSe)
        {
            pTailList = new ReaderCache(pEntry->pBarrelInfo_,pSe);
            if (headListPtr.get() == NULL)
            {
                headListPtr.reset(pTailList);
                pPreList = pTailList;
            }
            else
            {
                pPreList->next_ = pTailList;
                pPreList = pTailList;
            }
        }
    }

    ReaderCache* pHeadList = headListPtr.release();
    if (pHeadList)
        readerCache_.insert(make_pair(field, pHeadList));

    return pHeadList;
}

