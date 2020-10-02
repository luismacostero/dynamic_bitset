#include "dynamic_bitset.hpp"
#include <cstring>
#include <sstream>
#include <iostream>

/**@
   #include "utils.hpp"
@**/
#ifdef DEBUG
#include <cassert>
#define ONLY_DBG(f) f;
#else
#define ONLY_DBG(f) ;                           
#endif
/**@ @**/



// constructor general, asignacion y copia
dynamic_bitset::dynamic_bitset() : _blockSize( sizeof(char) * 8),
				   _mask(std::vector<bool>(_blockSize, false)),
				   _currentSize(0)
{}


dynamic_bitset::dynamic_bitset(int size, bool default_value) :
    _blockSize( sizeof(char) * 8),
    _mask(std::vector<bool>(0, default_value)),
    _currentSize(0)
{    
    this->resize(size, default_value);    
}

//===============================================================================
dynamic_bitset::dynamic_bitset(const char* serial) : _blockSize( sizeof(char) * 8 )
{
    int idx=0;

       
    //recotgemos el size de los primeros 4 bytes
    unsigned long size = 0;
    size |= (serial[idx++] << 0);
    size |= (serial[idx++] << 8);
    size |= (serial[idx++] << 16);
    size |= (serial[idx++] << 24);

    _currentSize = size;

    unsigned long maskSize = _currentSize;

    if((_currentSize % _blockSize)!=0)
      maskSize += (_blockSize - (_currentSize % _blockSize)); //multiplo de blockSize
    ONLY_DBG( assert(maskSize % _blockSize == 0) );

    
    _mask = std::vector<bool>(maskSize, false);   

    const unsigned long nBytes = maskSize / _blockSize;
    
    int midx=0;
    //recogemos la mascara byte a byte
    for(unsigned long i=0; i<nBytes; i++){
	char localmask = serial[idx++];

	for(unsigned short j=0; j<8; j++)
	    _mask[midx++] = ((localmask & (1<<j))!=0) ? true : false;
    }
    
    ONLY_DBG( fprintf(stderr, "MASCARA:::: %s\n", this->to_string().c_str()) );
    ONLY_DBG( fprintf(stderr, "SIZE::: %lu\n", size) );
}


char* dynamic_bitset::serialize(int* len)
{
    auto str = new char[ 4 + _mask.size() / (sizeof(char)*8)];

    int idx=0;
    
    //primero el numero de elementos
    str[idx++] = ((_currentSize >> 0) & 0xff);
    str[idx++] = ((_currentSize >> 8) & 0xff);
    str[idx++] = ((_currentSize >> 16) & 0xff);
    str[idx++] = ((_currentSize >> 24) & 0xff);


    //por cada bloque de 8 elementos, pintamos un char
    unsigned int i=0;    
    while( i < _mask.size() ){
	char localmask = 0;
	localmask |= (_mask[i++] << 0);
	localmask |= (_mask[i++] << 1);
	localmask |= (_mask[i++] << 2);
	localmask |= (_mask[i++] << 3);
	localmask |= (_mask[i++] << 4);
	localmask |= (_mask[i++] << 5);
	localmask |= (_mask[i++] << 6);
	localmask |= (_mask[i++] << 7);
	
	str[idx++] = localmask;
    }

    *len = idx;//4 + _mask.size() / (sizeof(char)*8);
    return str;
}


std::string dynamic_bitset::to_string() const
{
    std::stringstream ss("");

    ss << '[' << _currentSize << "]-";
    
    for(int i=_currentSize-1; i>=0; i--){
	ss << (_mask[i] ? 1 : 0 );
    }

    return ss.str();
}





//===============================================================================


void dynamic_bitset::resize(int num_bits, bool value)
{
    int currentSize = _mask.size();
    if(num_bits <= currentSize){
	_currentSize = num_bits;
	return;
    }
    
    int newSize = num_bits;
    newSize += (_blockSize - (newSize % _blockSize)); //lo hacemos multiplo de _blockSize

    _mask.resize(newSize, value);
    _currentSize = num_bits;
}


void dynamic_bitset::clear()
{
    for(auto it=_mask.begin(), it2=_mask.end(); it!=it2; it++)
        *it = false;
}


void dynamic_bitset::push_back(bool bit)
{
    //is there enough space ?
    if(_currentSize == _mask.size()){
	this->resize(1, false);
    }
 
    _mask[_currentSize++] = bit;
}


dynamic_bitset& dynamic_bitset::operator&= (const dynamic_bitset& b)
{
    unsigned minsize = std::min(_currentSize, b._currentSize);
    for(unsigned int i=0; i<minsize; i++)
	_mask[i] = (_mask[i] && b._mask[i]);

    return *this;
}


dynamic_bitset& dynamic_bitset::operator|= (const dynamic_bitset& b)
{
    unsigned minsize = std::min(_currentSize, b._currentSize);
    for(unsigned int i=0; i<minsize; i++)
	_mask[i] = (_mask[i] || b._mask[i]);

    return *this;
}   
    

dynamic_bitset& dynamic_bitset::operator^= (const dynamic_bitset& b)
{
    unsigned minsize = std::min(_currentSize, b._currentSize);
    for(unsigned int i=0; i<minsize; i++){
	_mask[i] = (_mask[i] && !b._mask[i]) || (!_mask[i] && b._mask[i]);
    }

    return *this;
}


dynamic_bitset& dynamic_bitset::operator<<= (int n)
{
    int initial_size = _currentSize;
    std::vector<bool> cp (_mask);
    
    this->resize(_currentSize+n);

    for(int i=0; i<n; i++)
	_mask[i] = false;

    
    for(int i=0; i<initial_size; i++)
	_mask[i+n] = cp[i];

    return *this;    
}



dynamic_bitset& dynamic_bitset::operator>>= (int n)
{
    std::vector<bool> cp(_mask);
    for(int i=_currentSize-1; i>=n; i--)
	_mask[i-n] = cp[i];

    for(int i=0; i<n; i++)
	_mask[_currentSize-n-i] = false;

    return *this;
}


dynamic_bitset dynamic_bitset::operator<<(int n) const
{
    dynamic_bitset bt (*this);
    bt <<= n;
    return bt;
}


dynamic_bitset dynamic_bitset::operator>>(int n) const
{
    dynamic_bitset bt (*this);
    bt >>= n;
    return bt;
}
    
dynamic_bitset& dynamic_bitset::set(unsigned long n, bool val)
{
    if(n >= _currentSize)
	resize(n+1, false);
    _mask[n] = val;

    return *this;
}


dynamic_bitset& dynamic_bitset::set()
{
    for(unsigned int i =0; i<_currentSize; i++)
	_mask[i] = true;

    return *this;
}


dynamic_bitset& dynamic_bitset::reset(unsigned long n)
{
    set(n, false);

    return *this;
}


dynamic_bitset& dynamic_bitset::reset()
{
    clear();

    return *this;
}


dynamic_bitset& dynamic_bitset::flip(unsigned long n)
{
    if(n >= _currentSize)
	resize(n+1, false);
    _mask[n] = !_mask[n];

    return *this;
}


dynamic_bitset& dynamic_bitset::flip()
{
    for(unsigned long i=0; i<_currentSize; i++)
	_mask[i] =!_mask[i];

    return *this;    
}


bool dynamic_bitset::test(unsigned long n) const
{
    if(n >= _currentSize)
	return false;
    else
	return _mask[n];
}

bool dynamic_bitset::any() const
{
    for(auto it=_mask.begin(), it2 = _mask.end(); it!=it2; it++)
	if( *it ) return true;

    return false;
}

bool dynamic_bitset::none() const
{
    return !any();
}


dynamic_bitset dynamic_bitset::operator~() const
{
    dynamic_bitset bt (*this);
    bt.flip();
    return bt;        
}


int dynamic_bitset::count() const
{
    int count = 0;

    for(unsigned long i=0; i<_currentSize; i++)
	if(_mask[i]) count ++;
    
    return count;
}


// bool& dynamic_bitset::operator[](int pos)
// {
// }


// bool dynamic_bitset::operator[](int pos) const
// {
// }
unsigned long dynamic_bitset::to_ulong() const
{
    int size = std::min(sizeof(unsigned long)*8, _currentSize);
    
    unsigned long mask=0;
    for(int i=0; i<size; i++){
	if(_mask[i])
	    mask |= (1<<i);
    }

    return mask;
}

    
int dynamic_bitset::size() const
{
    return _currentSize;
}


bool dynamic_bitset::empty() const
{
    return _currentSize == 0;
}


int dynamic_bitset::find_first() const
{
    for(unsigned long i=0; i<_currentSize; i++)
	if(_mask[i]) return i;

    return -1;
}


int dynamic_bitset::find_next(unsigned long pos) const
{
    for(unsigned long i=pos+1; i<_currentSize; i++)
	if(_mask[i]) return i;

    return -1;
}


bool dynamic_bitset::operator == (const dynamic_bitset& b) const
{
    if(_currentSize != b._currentSize)
	return false;

    for(unsigned long i=0; i<_currentSize; i++)
	if(_mask[i] != b._mask[i]) return false;
	
    return true;
}


bool dynamic_bitset::operator != (const dynamic_bitset& b) const
{
    return !(*this == b);   
}


dynamic_bitset operator & (const dynamic_bitset& b1, const dynamic_bitset& b2)
{
    dynamic_bitset bt;
    if(b1._currentSize > b2._currentSize){
	bt = b1;
	bt &= b2;
    } else {	
	bt = b2;
	bt &= b1;
    }

    return bt;
}

	
dynamic_bitset operator | (const dynamic_bitset& b1, const dynamic_bitset& b2)
{
    dynamic_bitset bt;
    if(b1._currentSize > b2._currentSize){
	bt = dynamic_bitset(b1);
	bt |= b2;
    } else {	
	bt = dynamic_bitset(b2);
	bt |= b1;
    }

    return bt;
}


dynamic_bitset operator ^ (const dynamic_bitset& b1, const dynamic_bitset& b2)
{
    dynamic_bitset bt;
    if(b1._currentSize > b2._currentSize){
	bt = dynamic_bitset(b1);
	bt ^= b2;
    } else {	
	bt = dynamic_bitset(b2);
	bt ^= b1;
    }

    return bt;
}



