#ifndef AM_TC_BUFFER_H
#define AM_TC_BUFFER_H
/**
 * @file am/tc/Buffer.h
 * @author Ian Yang
 * @date Created <2009-09-04 14:05:33>
 * @date Updated <2009-09-04 14:07:53>
 * @brief Wrapper of the memory buffer. A deleter can be specified to delegate
 * the destruction work to Buffer.
 */

#include <cstdlib>
#include <cstring>
#include <stdexcept>

#include <boost/iterator/reverse_iterator.hpp>

namespace izenelib {
namespace am {
namespace tc {

class Buffer
{
public:
    // type definitions
    typedef char           value_type;
    typedef char*          iterator;
    typedef const char*    const_iterator;
    typedef char&          reference;
    typedef const char&    const_reference;
    typedef std::size_t    size_type;
    typedef std::ptrdiff_t difference_type;
    typedef boost::reverse_iterator<iterator> reverse_iterator;
    typedef boost::reverse_iterator<const_iterator> const_reverse_iterator;

    typedef void (*deleter_type)(void*);

    Buffer()
    : data_(0), size_(0), deleter_(0)
    {}

    explicit Buffer(size_type size)
    : data_(std::malloc(size)), size_(size), deleter_(&std::free)
    {
        if (!data_)
        {
            size_ = 0;
        }
    }

    Buffer(const Buffer& rhs)
    : data_(std::malloc(rhs.size_)), size_(rhs.size_), deleter_(&std::free)
    {
        if (!data_)
        {
            size_ = 0;
            return;
        }

        std::memcpy(data_, rhs.data_, size_);
    }

    Buffer& operator=(const Buffer& rhs);

    ~Buffer()
    {
        if (deleter_)
        {
            (*deleter_)(data_);
        }
    }

    void attach(void* data,
                size_type size,
                deleter_type deleter = 0)
    {
        if (data_ != data && deleter)
        {
            (*deleter)(data_);
        }

        data_ = data;
        size_ = size;
        deleter_ = deleter;
    }

    iterator begin()
    {
        return data_;
    }
    const_iterator begin() const
    {
        return data_;
    }
    iterator end()
    {
        return data_ + size_;
    }
    const_iterator end() const
    {
        return data_ + size_;
    }
    reverse_iterator rbegin()
    {
        return reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(end());
    }
    reverse_iterator rend()
    {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(begin());
    }

    reference operator[](size_type i)
    {
        return data_[i];
    }
    const_reference operator[](size_type i)
    {
        return data_[i];
    }
    reference at(size_type i)
    {
        rangecheck_(i);
        return data_[i];
    }
    const_reference at(size_type i) const
    {
        rangecheck_(i);
        return data_[i];
    }

    reference front()
    {
        return data_[0];
    }
    const_reference front() const
    {
        return data_[0];
    }
    reference back()
    {
        return data_[size_ - 1];
    }
    const_reference back() const
    {
        return data_[size_ - 1];
    }

    size_type size() const
    {
        return size_;
    }
    bool empty()
    {
        return size_ == 0;
    }
    size_type max_size()
    {
        return size_;
    }

    void swap(Buffer& rhs)
    {
        using std::swap;
        swap(data_, rhs.data_);
        swap(size_, rhs.size_);
        swap(deleter_, rhs.deleter_);
    }

    const char* data() const
    {
        return data_;
    }
    char* data()
    {
        return data_;
    }

    void assign(char value)
    {
        if (data_)
        {
            std::memset(data_, value, size_);
        }
    }

private:
    void rangecheck_(size_type i) const
    {
        if (i >= size())
        {
            throw std::out_of_range(
                "tc::Buffer: index out of range"
            );
        }
    }

    char* data_;
    size_type size_;
    deleter_type deleter_;
};



}}} // namespace izenelib::am::tc

#endif // AM_TC_BUFFER_H
