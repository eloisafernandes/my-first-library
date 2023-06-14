#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <algorithm>        // std::copy, std::equal, std::fill
#include <cassert>          // assert()
#include <cstddef>          // std::size_t
#include <exception>        // std::out_of_range
#include <initializer_list> // std::initializer_list
#include <iostream>         // std::cout, std::endl
#include <iterator> // std::advance, std::begin(), std::end(), std::ostream_iterator
#include <limits> // std::numeric_limits<T>
#include <memory> // std::unique_ptr

/// Sequence container namespace.
namespace sc {
/// Implements tha infrastrcture to support a bidirectional iterator.
template <class T> class MyForwardIterator {
public:
  using iterator = MyForwardIterator; //!< Alias to iterator.
  // Below we have the iterator_traits common interface
  typedef std::ptrdiff_t
      difference_type;  //!< Difference type used to calculated distance between
                        //!< iterators.
  typedef T value_type; //!< Value type the iterator points to.
  typedef T *pointer;   //!< Pointer to the value type.
  typedef T &reference; //!< Reference to the value type.
  typedef const T &const_reference; //!< Reference to the value type.
  typedef std::bidirectional_iterator_tag
      iterator_category; //!< Iterator category.

  /*! Create an iterator around a raw pointer.
   * \param pt_ raw pointer to the container.
   */
  MyForwardIterator(pointer pt = nullptr) : m_ptr(pt) { /* empty */
  }

  /// Access the content the iterator points to.
  reference operator*(void) const {
    assert(m_ptr != nullptr);
    return *m_ptr;
  }

  /// Overloaded `->` operator.
  pointer operator->(void) const {
    assert(m_ptr != nullptr);
    return m_ptr;
  }

  /// Assignment operator.
  iterator &operator=(const iterator &other){
    m_ptr = other.m_ptr;
    return *this;

  }
  /// Copy constructor.
  MyForwardIterator(const iterator &other){
    m_ptr = other.m_ptr;
  }

  /// Pre-increment operator.
  iterator operator++(void) {
    ++(m_ptr);
    return *this;
  }

  /// Post-increment operator.
  iterator operator++(int) {
    iterator dummy{*this};
    ++(m_ptr);
    return dummy;
  }

  /// Pre-decrement operator.
  iterator operator--(void) {
    --(m_ptr);
    return *this;
  }

  /// Post-decrement operator.
  iterator operator--(int) {
    iterator dummy{*this};
    --(m_ptr);
    return dummy;
  }

  iterator &operator+=(difference_type offset) {
    iterator &it{*this};
    m_ptr+=offset;
    return it;
  }
  iterator &operator-=(difference_type offset) {
    iterator &it{*this};
    m_ptr-=offset;
    return it;
  }

  friend bool operator<(const iterator &ita, const iterator &itb) {
    return ita < itb ;
  }
  friend bool operator>(const iterator &ita, const iterator &itb) {
    return ita > itb;
  }
  friend bool operator>=(const iterator &ita, const iterator &itb) {
    return ita >= itb;
  }
  friend bool operator<=(const iterator &ita, const iterator &itb) {
    return ita <= itb;
  }

  friend iterator operator+(difference_type offset, iterator it) {
    iterator dummy;
    dummy.m_ptr = offset+it.m_ptr;
    return dummy;
  }
  friend iterator operator+(iterator it, difference_type offset) {
    iterator dummy;
    dummy.m_ptr = it.m_ptr+offset;
    return dummy;
  }
  friend iterator operator-(iterator it, difference_type offset) {
    iterator dummy;
    dummy.m_ptr = it.m_ptr-offset;
    return dummy;
  }

  /// Equality operator.
  bool operator==(const iterator &rhs_) const {
    return m_ptr == rhs_.m_ptr;
  }

  /// Not equality operator.
  bool operator!=(const iterator &rhs_) const {
    return !(*this == rhs_);
  }

  /// Returns the difference between two iterators.
  difference_type operator-(const iterator &rhs_) const {
    return m_ptr-rhs_.m_ptr;
  }

  /// Stream extractor operator.
  friend std::ostream &operator<<(std::ostream &os_,
                                  const MyForwardIterator &p_) {
    os_ << "[@ " << p_.m_ptr << ": " << *p_.m_ptr << " ]";
    return os_;
  }

private:
  pointer m_ptr; //!< The raw pointer.
};

/// This class implements the ADT list with dynamic array.
/*!
 * sc::vector is a sequence container that encapsulates dynamic size arrays.
 *
 * The elements are stored contiguously, which means that elements can
 * be accessed not only through iterators, but also using offsets to
 * regular pointers to elements.
 * This means that a pointer to an element of a vector may be passed to
 * any function that expects a pointer to an element of an array.
 *
 * \tparam T The type of the elements.
 */
template <typename T> class vector {
  //=== Aliases
public:
  using size_type = unsigned long; //!< The size type.
  using value_type = T;            //!< The value type.
  using pointer = value_type *; //!< Pointer to a value stored in the container.
  using reference =
      value_type &; //!< Reference to a value stored in the container.
  using const_reference = const value_type &; //!< Const reference to a value
                                              //!< stored in the container.

  using iterator =
      MyForwardIterator<value_type>; //!< The iterator, instantiated from a
                                     //!< template class.
  using const_iterator =
      MyForwardIterator<const value_type>; //!< The const_iterator,
                                           //!< instantiated from a template
                                           //!< class.

public:
  //=== [I] SPECIAL MEMBERS (6 OF THEM)
  explicit vector(size_type cp = 0) {
    m_storage = new T[cp];
    m_capacity = cp;
    m_end = cp; // Array começa vazio.
    /* for (size_type i{0}; i < m_end; ++i) {
      m_storage[i] = new T();
    } */
  }
  virtual ~vector(void) {
    if (m_storage)
      delete[] m_storage;
  }
  vector(const vector &other){
    m_capacity=other.m_capacity;
    m_end=other.m_end;
    m_storage=new T[other.m_capacity];
    for(size_type i{0};i<other.m_end;++i){
      m_storage[i]=other.m_storage[i];
    } 
  }
  vector(const std::initializer_list<T> &il) {
    m_capacity = il.size();
    m_storage = new T[m_capacity];
    m_end = m_capacity; // Array começa cheio.
    // Copy the elements from the il into the array.
    std::copy(il.begin(), il.end(), m_storage);
  }
  template <typename InputItr> 
  vector(InputItr first, InputItr last){
    size_type lenght=last-first;
    m_end = m_capacity = lenght;
    m_storage =new T[lenght];
    for(size_type i{0};i<lenght;++i) {
        m_storage[i]=*first;
        first++;
    }
  }
  vector &operator=(const vector &other){
    if (this == &other){
      return *this;
    }
    if(other.m_capacity != this->m_capacity){
      delete[] m_storage;
      this->m_storage=new T[other.m_capacity];
    }
    this->m_capacity=other.m_capacity;
    this->m_end=other.m_end;
    for(size_type i{0};i<other.m_end;++i){
      this->m_storage[i]=other.m_storage[i];
    }        
    return *this;
    
  }

  //=== [II] ITERATORS
  iterator begin(void) { return iterator{&m_storage[0]}; }
  iterator end(void){ return iterator{&m_storage[m_end]}; }
  const_iterator cbegin(void) const { 
    return const_iterator{&m_storage[0]}; 
  }
  const_iterator cend(void) const { 
    return const_iterator{&m_storage[m_end]}; 
  }
  
  

  // [III] Capacity
  size_type size(void) const { return m_end; }
  size_type capacity(void) const { return m_capacity; }
  bool empty(void) const { return m_end == 0; }
  

  // [IV] Modifiers
  void clear(void){
    m_end = 0;
  }
  void push_front(const_reference value);
  void push_back(const_reference value){
    if(m_end==m_capacity){
      if (m_capacity == 0){
        m_capacity = 1;
      }
      else{
        m_capacity *= 2;
      }
      T *new_storage= new T[m_capacity];
      for(size_type i{0};i<m_end;++i){
        new_storage[i]=m_storage[i];
      }
      delete[] m_storage;
      m_storage=new_storage;
    }
    m_storage[m_end]=value;
    ++m_end;
  }
  void pop_back(void){
    if(empty()){
      throw std::length_error("Vector está vazio!");
    }
    m_end--;
  }
  void pop_front(void);

  iterator insert(iterator pos_, const_reference value_){
    size_type distance = pos_ - m_storage;
    if (m_end == m_capacity) {
      size_type new_capacity;
      if (m_capacity == 0){
        new_capacity = 1;
      } else{
        new_capacity = m_capacity*2;  
      }
      T* new_storage = new T[new_capacity];
      for (size_type i{0}; i<distance; ++i) {
        new_storage[i] = m_storage[i];
      }
      new_storage[distance] = value_;
      for (size_type i{distance}; i < m_end; ++i) {
        new_storage[i + 1] = m_storage[i];
      }
      delete[] m_storage;
      m_storage = new_storage;
      m_capacity = new_capacity;
            
    } else {
      T* new_storage = new T[m_capacity];
      for (size_type i {0}; i<distance; ++i) {
          new_storage[i] = m_storage[i];
      }
      new_storage[distance] = value_;
      for (size_type i{distance}; i < m_end; ++i) {
        new_storage[i + 1] = m_storage[i];
      }
      delete[] m_storage;
      m_storage = new_storage;    
    }
    ++m_end;
    return pos_;
  }
  
  iterator insert(const_iterator pos_, const_reference value_){
    size_type distance = pos_ - m_storage;
    if (m_end == m_capacity) {
      size_type new_capacity;
      if (m_capacity == 0){
        new_capacity = 1;
      } else{
        new_capacity = m_capacity*2;  
      }
      T* new_storage = new T[new_capacity];
      for (size_type i{0}; i<distance; ++i) {
        new_storage[i] = m_storage[i];
      }
      new_storage[distance] = value_;
      for (size_type i{distance}; i < m_end; ++i) {
        new_storage[i + 1] = m_storage[i];
      }
      delete[] m_storage;
      m_storage = new_storage;
      m_capacity = new_capacity;
            
    } else {
      T* new_storage = new T[m_capacity];
      for (size_type i {0}; i<distance; ++i) {
          new_storage[i] = m_storage[i];
      }
      new_storage[distance] = value_;
      for (size_type i{distance}; i < m_end; ++i) {
        new_storage[i + 1] = m_storage[i];
      }
      delete[] m_storage;
      m_storage = new_storage;    
    }
    ++m_end;
    return pos_;
  }

  template <typename InputItr>
  iterator insert(iterator pos_, InputItr first_, InputItr last_){
    size_type distance = pos_ - m_storage;
    size_type lenght = last_ - first_;
    if (m_end + lenght > m_capacity) {
      size_t new_capacity;
      if (m_end + lenght >  m_capacity * 2){
        new_capacity = m_end + lenght;
      }
      else{
        new_capacity = m_capacity * 2;
      }
      T* new_storage = new T[new_capacity];
      
     
      for (size_type i{0}; i<distance; ++i) {
        new_storage[i] = m_storage[i];
      }
      //new_storage[distance] = value_;
      size_type i = distance;
      for (iterator it = first_; it != last_; ++it) {
        new_storage[i++] = *it;
      }
      for (size_type j = distance; j < m_end; ++j) {
        new_storage[i++] = m_storage[j];
      }
      delete[] m_storage;
      m_storage = new_storage;
      m_capacity = new_capacity;
            
    } else {
      for (size_type i{m_end + lenght - 1}; i > distance + lenght - 1; --i) {
          m_storage[i] = m_storage[i - lenght];
      }
      size_type i = distance;
      for (iterator it = first_; it != last_; ++it) {
        m_storage[i++] = *it;
      }    
    }
    m_end += distance;
    return m_storage+distance;
  }
  template <typename InputItr>
  iterator insert(const_iterator pos_, InputItr first_, InputItr last_){
    size_type distance = pos_ - m_storage;
    size_type lenght = last_ - first_;
    if (m_end + lenght > m_capacity) {
      size_t new_capacity;
      if (m_end + lenght >  m_capacity * 2){
        new_capacity = m_end + lenght;
      }
      else{
        new_capacity = m_capacity * 2;
      }
      T* new_storage = new T[new_capacity];
      for (size_type i{0}; i<distance; ++i) {
        new_storage[i] = m_storage[i];
      }
      //new_storage[distance] = value_;
      size_type i = distance;
      for (iterator it = first_; it != last_; ++it) {
        new_storage[i++] = *it;
      }
      for (size_type j = distance; j < m_end; ++j) {
        new_storage[i++] = m_storage[j];
      }
      delete[] m_storage;
      m_storage = new_storage;
      m_capacity = new_capacity;
            
    } else {
      for (size_type i{m_end + lenght - 1}; i > distance + lenght - 1; --i) {
          m_storage[i] = m_storage[i - lenght];
      }
      size_type i = distance;
      for (iterator it = first_; it != last_; ++it) {
        m_storage[i++] = *it;
      }    
    }
    m_end += distance;
    return m_storage+distance;
  }

  iterator insert(iterator pos_,
                  const std::initializer_list<value_type> &ilist_){
    /*size_type distance = pos_ - m_storage;
    size_type lenght = ilist_.end() - ilist_.begin();
    if (m_end + lenght > m_capacity) {
      size_t new_capacity;
      if (m_end + lenght >  m_capacity * 2){
        new_capacity = m_end + lenght;
      }
      else{
        new_capacity = m_capacity * 2;
      }
      T* new_storage = new T[new_capacity];
      for (size_type i{0}; i<distance; ++i) {
        new_storage[i] = m_storage[i];
      }
      //new_storage[distance] = value_;
      size_type i = distance;
      for (iterator it = ilist_.begin(); it != ilist_.end(); ++it) {
        new_storage[i++] = *it;
      }
      for (size_type j = distance; j < m_end; ++j) {
        new_storage[i++] = m_storage[j];
      }
      delete[] m_storage;
      m_storage = new_storage;
      m_capacity = new_capacity;
            
    } else {
      for (size_type i{m_end + lenght - 1}; i > distance + lenght - 1; --i) {
          m_storage[i] = m_storage[i - lenght];
      }
      size_type i = distance;
      for (iterator it = ilist_.begin(); it != ilist_.end(); ++it) {
        m_storage[i++] = *it;
      }    
    }
    m_end += distance;
    return m_storage+distance;*/
  }
  iterator insert(const_iterator pos_,
                  const std::initializer_list<value_type> &ilist_){
    /*size_type distance = pos_ - m_storage;
    size_type lenght = ilist_.end() - ilist_.begin();
    if (m_end + lenght > m_capacity) {
      size_t new_capacity;
      if (m_end + lenght >  m_capacity * 2){
        new_capacity = m_end + lenght;
      }
      else{
        new_capacity = m_capacity * 2;
      }
      T* new_storage = new T[new_capacity];
      for (size_type i{0}; i<distance; ++i) {
        new_storage[i] = m_storage[i];
      }
      //new_storage[distance] = value_;
      size_type i = distance;
      for (iterator it = ilist_.begin(); it != ilist_.end(); ++it) {
        new_storage[i++] = *it;
      }
      for (size_type j = distance; j < m_end; ++j) {
        new_storage[i++] = m_storage[j];
      }
      delete[] m_storage;
      m_storage = new_storage;
      m_capacity = new_capacity;
            
    } else {
      for (size_type i{m_end + lenght - 1}; i > distance + lenght - 1; --i) {
          m_storage[i] = m_storage[i - lenght];
      }
      size_type i = distance;
      for (iterator it = ilist_.begin(); it != ilist_.end(); ++it) {
        m_storage[i++] = *it;
      }    
    }
    m_end += distance;
    return m_storage+distance;*/
  }

  void reserve(size_type new_capacity){
    if(new_capacity<=m_capacity){
      return;
    }        
    T *new_storage = new T[new_capacity];
    for(size_type i{0}; i<m_end; ++i){
      new_storage[i]=m_storage[i];
    }
    delete[] m_storage;
    m_storage = new_storage;
    m_capacity = new_capacity;
  }
  void shrink_to_fit(void){
    if(m_capacity==m_end){
        return;
    }
    T *new_storage=new T[m_end];
    for(size_type i{0}; i<m_end; ++i){
        new_storage[i] = m_storage[i];
    }
    delete[] m_storage;
    m_storage=new_storage;
    m_capacity=m_end;
  }

  void assign(size_type count_, const_reference value_){
    if (count_ > m_capacity){
      T* new_storage = new T[count_];

      for (size_t i = 0; i < m_end; ++i) {
        new_storage[i] = m_storage[i];
      }
      delete[] m_storage;
      m_storage = new_storage;
      m_capacity = count_;
      m_end = count_;
      for(size_type i{0};i<count_;++i){ 
        m_storage[i]=value_;
      }
    }
    else{
      m_end = count_;
      for(size_type i{0};i<count_;++i){ 
        m_storage[i]=value_;
      }
    
    }
    
    
  }
  void assign(const std::initializer_list<T> &ilist){
    size_t count_ = ilist.end() - ilist.begin();
    if (count_ > m_capacity){
      T* new_storage = new T[count_];

      for (size_t i = 0; i < m_end; ++i) {
        new_storage[i] = m_storage[i];
      }
      delete[] m_storage;
      m_storage = new_storage;
      m_capacity = count_;
      m_end = count_;
      for (size_t i = 0; i < count_; ++i) {
          m_storage[i] = *(ilist.begin() + i);
      }
    }
    else{
      m_end = count_;
      for (size_t i = 0; i < count_; ++i) {
        m_storage[i] = *(ilist.begin() + i);
      }
    
    }

        
       
  }
  template <typename InputItr> 
  void assign(InputItr first, InputItr last){
    size_t count_ = last - first;
    if (count_ > m_capacity){
      T* new_storage = new T[count_];

      for (size_t i = 0; i < m_end; ++i) {
        new_storage[i] = m_storage[i];
      }
      delete[] m_storage;
      m_storage = new_storage;
      m_capacity = count_;
      m_end = count_;
      for (size_t i = 0; i < count_; ++i) {
            m_storage[i] = *(first + i);
        }
      }
      else{
        m_end = count_;
        for (size_t i = 0; i < count_; ++i) {
          m_storage[i] = *(first + i);
        }
      
      }
  }
  iterator erase(iterator first, iterator last);
    
    /*if (first >= last) {
      throw std::length_error("Não existe esse intervalo no vector");   
    } 
    size_type distance = first - m_storage;
    size_type lenght = last - first;
    for (size_type i{distance}; i < m_end-1; ++i){
      m_storage[i] = m_storage[i+lenght];
    }
    m_end -= lenght;
    
    return last;*/
  iterator erase(const_iterator first, const_iterator last);
    /*if (first >= last) {
      throw std::length_error("Não existe esse intervalo no vector");   
    } 
    size_type distance = first - m_storage;
    size_type lenght = last - first;
    for (size_type i{distance}; i < m_end-1; ++i){
      m_storage[i] = m_storage[i+lenght];
    }
    m_end -= lenght;*/
    

  iterator erase(const_iterator pos){
    size_type distance = pos - m_storage;
    if (distance < 0 || distance >= m_end) {
      throw std::length_error("Não existe essa posição no vector");   
    }
    for (size_type i{distance}; i<m_end - 1; ++i) {
      m_storage[i] = m_storage[i + 1];
    }
    --m_end; 
    return pos;
  }
  iterator erase(iterator pos){
    size_type distance = pos - m_storage;
    if (distance < 0 || distance >= m_end) {
      throw std::length_error("Não existe essa posição no vector");   
    }
    for (size_type i{distance}; i<m_end - 1; ++i) {
      m_storage[i] = m_storage[i + 1];
    }
    --m_end; 
    return pos;
    
  }

  // [V] Element access
  const_reference back(void) const{
    if (m_end <= 0) {
      throw std::runtime_error("Vector vazio");
    } 
    return m_storage[m_end - 1]; 
  }

  const_reference front(void) const{
    if (m_end <= 0) {
      throw std::runtime_error("Vector vazio");
    } 
    return m_storage[0]; 
  }
  reference back(void){
    if (m_end>0){
      return m_storage[m_end-1];
    }
    throw std::length_error("Vector está vazio!");

  }
  reference front(void){
    if (m_end>0){
      return m_storage[0];
    }
    throw std::length_error("Vector está vazio!");
  }
  const_reference operator[](size_type idx) const { return m_storage[idx]; }
  reference operator[](size_type idx) { return m_storage[idx]; }
  const_reference at(size_type idx) const{
    if(idx >= m_end || idx < 0){
      throw std::out_of_range("não existe essa posição!");
    }
    return m_storage[idx];
  }
  reference at(size_type idx){
    if(idx >= m_end || idx < 0){
      throw std::out_of_range("não existe essa posição!");
    }
    return m_storage[idx];
  }
  pointer data(void){
    return *m_storage;
  }
  const_reference data(void) const{
    return m_storage;
  }

  // [VII] Friend functions.
  friend std::ostream &operator<<(std::ostream &os_, const vector<T> &v_) {
    // O que eu quero imprimir???
    os_ << "{ ";
    for (auto i{0u}; i < v_.m_capacity; ++i) {
      if (i == v_.m_end)
        os_ << "| ";
      os_ << v_.m_storage[i] << " ";
    }
    os_ << "}, m_end=" << v_.m_end << ", m_capacity=" << v_.m_capacity;

    return os_;
  }
  friend void swap(vector<T> &first_, vector<T> &second_) {
    // enable ADL
    using std::swap;

    // Swap each member of the class.
    swap(first_.m_end, second_.m_end);
    swap(first_.m_capacity, second_.m_capacity);
    swap(first_.m_storage, second_.m_storage);
  }

private:
  bool full(void) const;

  size_type
      m_end; //!< The list's current size (or index past-last valid element).
  size_type m_capacity; //!< The list's storage capacity.
  T *m_storage;         //!< The list's data storage area.
};

// [VI] Operators
template <typename T> bool 
operator==(const vector<T> &vector1, const vector<T> &vector2){
  for(auto i{0}; i<vector1.size(); ++i) {
    if(vector1.at(i) != vector2.at(i)){
      return false;
    }
  }
  return true;
};
template <typename T> bool 
operator!=(const vector<T> &vector1, const vector<T> &vector2){
    for(auto i{0};i<vector1.size();++i) {
      if(vector1.at(i)!=vector2.at(i)){
        return true;
      }
    }
    return false;
}

} // namespace sc.



#endif


