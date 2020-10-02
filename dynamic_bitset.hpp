#ifndef _DYNAMIC_BITSET_HPP_
#define _DYNAMIC_BITSET_HPP_

#include <vector>
#include <string>
#include <memory>

class dynamic_bitset
{
public:
    // constructor general, asignacion y copia
    dynamic_bitset();
    dynamic_bitset(int size, bool default_value);
    explicit dynamic_bitset(const char* serial);

    dynamic_bitset(const dynamic_bitset& db) = default;  
    dynamic_bitset& operator= (const dynamic_bitset& db) = default;
    dynamic_bitset(dynamic_bitset&& db) = default;
    dynamic_bitset& operator= (dynamic_bitset&& db) = default;
  

    ~dynamic_bitset() = default;

    //////////////////////////////////
    std::string to_string() const; ///
    char* serialize(int* len);     ///
    //////////////////////////////////
    
    void resize(int num_bits, bool value = false);
    void clear();
    void push_back(bool bit);

    dynamic_bitset& operator&= (const dynamic_bitset& b);
    dynamic_bitset& operator|= (const dynamic_bitset& b);
    dynamic_bitset& operator^= (const dynamic_bitset& b);
    dynamic_bitset& operator<<= (int n);
    dynamic_bitset& operator>>= (int n);

    dynamic_bitset operator<<(int n) const;
    dynamic_bitset operator>>(int n) const;

    bool operator == (const dynamic_bitset& b) const;
    bool operator != (const dynamic_bitset& b) const;
    
    dynamic_bitset& set(unsigned long n, bool val = true);
    dynamic_bitset& set();
    dynamic_bitset& reset(unsigned long n);
    dynamic_bitset& reset();
    dynamic_bitset& flip(unsigned long n);
    dynamic_bitset& flip();
    bool test(unsigned long n) const;
    bool any() const;
    bool none() const;
    dynamic_bitset operator~() const;
    int count() const;
    
    // bool& operator[](int pos);
    // bool operator[](int pos) const;
    
    unsigned long to_ulong() const;
    
    int size() const;
    bool empty() const;

    int find_first() const;
    int find_next(unsigned long pos) const;

    friend dynamic_bitset operator & (const dynamic_bitset& b1, const dynamic_bitset& b2);
    friend dynamic_bitset operator | (const dynamic_bitset& b1, const dynamic_bitset& b2);
    friend dynamic_bitset operator ^ (const dynamic_bitset& b1, const dynamic_bitset& b2);
  
private:
    unsigned short _blockSize;

    std::vector<bool> _mask;
    unsigned long _currentSize;
};








#endif //_DYNAMIC_BITSET_
