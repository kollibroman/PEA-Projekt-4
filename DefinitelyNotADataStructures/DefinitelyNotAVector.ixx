module;
#include <algorithm>
#include <stdexcept>

export module DefinitelyNotAVector;

export template<class T>
class DefinitelyNotAVector
{
private:
    T* data;
    size_t size_{};
    size_t capacity_{};

public:
    DefinitelyNotAVector() : data(nullptr), size_(0), capacity_(0) {}

    DefinitelyNotAVector(size_t count, const T& value)
        : data(new T[count]), size_(count), capacity_(count) {
        for (size_t i = 0; i < count; i++) {
            data[i] = value;
        }
    }

    explicit DefinitelyNotAVector(size_t count)
        : data(new T[count]), size_(count), capacity_(count)
    {
        for (size_t i = 0; i < count; i++) {
            data[i] = T();
        }
    }

    ~DefinitelyNotAVector()
    {
        delete[] data;
    }

    DefinitelyNotAVector(const DefinitelyNotAVector& other)
        : data(new T[other.capacity_]), size_(other.size_), capacity_(other.capacity_)
    {
        for (size_t i = 0; i < size_; i++) {
            data[i] = other.data[i];
        }
    }

    DefinitelyNotAVector(DefinitelyNotAVector&& other) noexcept
        : data(other.data), size_(other.size_), capacity_(other.capacity_)
    {
        other.data = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    DefinitelyNotAVector& operator=(const DefinitelyNotAVector& other)
    {
        if (this != &other)
        {
            delete[] data;
            data = new T[other.capacity_];
            size_ = other.size_;
            capacity_ = other.capacity_;
            for (size_t i = 0; i < size_; i++) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    DefinitelyNotAVector& operator=(DefinitelyNotAVector&& other) noexcept
    {
        if (this != &other)
        {
            delete[] data;
            data = other.data;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    void push_back(const T& value)
    {
        if (size_ == capacity_)
        {
            resize(capacity_ == 0 ? 1 : capacity_ * 2);
        }

        data[size_++] = value;
    }

    void push_back(T&& value)
    {
        if (size_ == capacity_)
        {
            resize(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        data[size_++] = std::move(value);
    }

    template<typename... Args>
    void emplace_back(Args&&... args)
    {
        if (size_ == capacity_)
        {
            resize(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        new (&data[size_]) T(std::forward<Args>(args)...);
        size_++;
    }

    T& operator[](size_t index)
    {
        if (index >= size_)
        {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    const T& operator[](size_t index) const
    {
        if (index >= size_)
        {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    [[nodiscard]] size_t size() const
    {
        return size_;
    }

    [[nodiscard]] bool empty() const
    {
        return size_ == 0;
    }

    void resize(const size_t new_capacity)
    {
        T* new_data = new T[new_capacity];

        for (size_t i = 0; i < size_; i++)
        {
            new_data[i] = std::move(data[i]);
        }

        delete[] data;
        data = new_data;
        capacity_ = new_capacity;
    }

    void pop_back()
    {
        if (size_ > 0) {
            --size_;
        }
    }

    [[nodiscard]] bool Contains(const T& value) const
    {
        for (size_t i = 0; i < size_; ++i)
        {
            if (data[i] == value)
            {
                return true; 
            }
        }
        return false;
    }
    
    T* begin()
    {
        return data;
    }

    T* end()
    {
        return data + size_;
    }

    const T* begin() const
    {
        return data;
    }

    const T* end() const
    {
        return data + size_;
    }
};
