#include <iostream>
#include <memory> 
#include <stdexcept>
#include <utility> 


template<typename T>
class Vector {
public:
    Vector() : size_(0), capacity_(0), data_(nullptr) {}
    explicit Vector(size_t capacity) : size_(0), capacity_(capacity), data_(allocator_.allocate(capacity)) {}

    T* begin() { return data_; }
    T* end() { return data_ + size_; }
    const T* begin() const { return data_; }
    const T* end() const { return data_ + size_; }

    T* rbegin() { return data_ + size_ - 1; }   
    T* rend() { return data_ - 1; }              
    const T* rbegin() const { return data_ + size_ - 1; }
    const T* rend() const { return data_ - 1; } 

    Vector(const Vector &other) : size_(other.size_),
        capacity_(other.capacity_), data_(allocator_.allocate(other.capacity_))
    {
        for (size_t i = 0; i < size_; ++i) {
            std::allocator_traits<std::allocator<T>>::construct(allocator_, data_ + i, other.data_[i]);
        }
    }

    ~Vector() 
    {
        clear();
        allocator_.deallocate(data_, capacity_);
    }

    T& operator[](size_t index) 
    {
        if (index >= size_) {
            throw std::out_of_range("Out of range");
        }
        return data_[index];
    }

    const T& operator[](size_t index) const 
    {
        if (index >= size_) {
            throw std::out_of_range("Out of range");
        }
        return data_[index];
    }

    size_t size() const 
    {
        return size_;
    }

    size_t max_size() const
    {
        return capacity_;
    }

    bool empty() const 
    {
        return size_ == 0;
    }

    void reserve(size_t cap)
    {
        if (cap > capacity_) {
            T* new_data = allocator_.allocate(cap);
            for (size_t i = 0; i < size_; ++i) {
                std::allocator_traits<std::allocator<T>>::construct(allocator_, new_data + i, std::move(data_[i]));
                std::allocator_traits<std::allocator<T>>::destroy(allocator_, data_ + i);
            }
            allocator_.deallocate(data_, capacity_);
            data_ = new_data;
            capacity_ = cap;
        }
    }

    void insert(size_t index, const T& value)
    {
        if (index > size_) {
            throw std::out_of_range("Out of range");
        }

        if (size_ == capacity_) {
            reserve(capacity_ > 0 ? capacity_ * 2 : 1);
        }

        for (size_t i = size_; i > index; --i) {
            std::allocator_traits<std::allocator<T>>::construct(allocator_, data_ + i, std::move(data_[i - 1]));
            std::allocator_traits<std::allocator<T>>::destroy(allocator_, data_ + i - 1);  
        }
        std::allocator_traits<std::allocator<T>>::construct(allocator_, data_ + index, value);
        ++size_;
    }

    void push_back(const T& v)
    {
        if (size_ == capacity_) {
            reserve(capacity_ > 0 ? capacity_ * 2 : 1);
        }
        std::allocator_traits<std::allocator<T>>::construct(allocator_, data_ + size_, v);
        ++size_;
    }

    void pop_back()
    {
        if (size_ > 0) {
            std::allocator_traits<std::allocator<T>>::destroy(allocator_, data_ + size_ - 1);
            --size_;
        }
    }

    void erase(size_t index)
    {
        if (index >= size_) {
            throw std::out_of_range("Out of range");
        }

        std::allocator_traits<std::allocator<T>>::destroy(allocator_, data_ + index);
        for (size_t i = index; i < size_ - 1; ++i) {
            std::allocator_traits<std::allocator<T>>::construct(allocator_, data_ + i, std::move(data_[i + 1]));
            std::allocator_traits<std::allocator<T>>::destroy(allocator_, data_ + i + 1);
        }
        --size_;
    }

    void erase(size_t first, size_t last) 
    {
        if (first >= size_ || last > size_ || first >= last) {
            throw std::out_of_range("Out of range");
        }

        size_t count = last - first;

        for (size_t i = first; i < last; ++i) {
            std::allocator_traits<std::allocator<T>>::destroy(allocator_, data_ + i);
        }

        for (size_t i = last; i < size_; ++i) {
            std::allocator_traits<std::allocator<T>>::construct(allocator_, data_ + first++, std::move(data_[i]));
            std::allocator_traits<std::allocator<T>>::destroy(allocator_, data_ + i);
        }

        size_ -= count;
    }

    void clear()
    {
        for (size_t i = 0; i < size_; ++i) {
            std::allocator_traits<std::allocator<T>>::destroy(allocator_, data_ + i);
        }
        size_ = 0;
    }

private:
    size_t size_;
    size_t capacity_;
    T* data_;
    std::allocator<T> allocator_;
};


int main() {
    Vector<int> vec;
    std::cout << "Is vector empty? " << (vec.empty() ? "Yes" : "No") << std::endl;

    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);
    std::cout << "Vector size after push_back: " << vec.size() << std::endl;
    std::cout << "Elements: ";
    for (int i : vec) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    vec.insert(1, 15);
    std::cout << "Vector size after insert: " << vec.size() << std::endl;
    std::cout << "Elements after insert: ";
    for (int i : vec) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    vec.erase(2); 
    std::cout << "Vector size after erase: " << vec.size() << std::endl;
    std::cout << "Elements after erase: ";
    for (int i : vec) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    vec.push_back(40);
    vec.push_back(50);
    std::cout << "Elements before range erase: ";
    for (int i : vec) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    
    vec.erase(1, 4);
    std::cout << "Vector size after range erase: " << vec.size() << std::endl;
    std::cout << "Elements after range erase: ";
    for (int i : vec) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    vec.clear();
    std::cout << "Is vector empty after clear? " << (vec.empty() ? "Yes" : "No") << std::endl;
    std::cout << "Final size: " << vec.size() << std::endl;

    return 0;
}