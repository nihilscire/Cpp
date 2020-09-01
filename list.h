#include <utility>
#include <exception>
#include <type_traits>
#include <iostream>

template<typename T>
class List
{
    static_assert( std::is_copy_constructible<T>::value, 
                    "The typename provided is not copy constructible" );
    static_assert( std::is_copy_assignable<T>::value, 
                    "The typename provided is not copy assignable" );
    static_assert( std::is_move_constructible<T>::value, 
                    "The typename provided is not move constructible" );
    static_assert( std::is_move_assignable<T>::value, 
                    "The typename provided is not move assignable" );
private:
    template<typename E>
    class ListNode;

    ListNode<T>* head{};
    ListNode<T>* tail{};
    std::size_t _size{};

public:
    class Iterator;

    List() = default;
    ~List() noexcept;

    List(const List&);
    List& operator=(List);
    List(List&&);
    List& operator=(List&&);

    Iterator begin();
    Iterator end();
    bool empty();
    std::size_t size();
    T& front();
    T& back();
    void push_front(T&);
    void push_front(T&&);
    void pop_front();
    void push_back(T&);
    void push_back(T&&);
    void pop_back();

    template<typename... Args>
    Iterator emplace(Iterator, Args&&...);

    Iterator insert(Iterator, const T&);
    Iterator erase(Iterator);
    Iterator erase(Iterator, Iterator);
    void clear();

    void reverse();

    void print();

    template<typename U>
    friend void swap(List<U>&, List<U>&);
};

template<typename T>
List<T>::~List() noexcept
{
    clear();
}

template<typename T>
List<T>::List(const List& other) 
{
    _size = 0;

    for(auto it = other.begin(); it != other.end(); ++it)
    {
        push_back(*it);
    }
}

template<typename T>
List<T>& List<T>::operator=(List other)
{
    // copy and swap idiom
    swap(*this, other);
    return *this;
}

template<typename T>
List<T>::List(List&& other) : List()
{
    swap(*this, other);
}

template<typename T>
List<T>& List<T>::operator=(List&& other)
{
    swap(*this, List(other));
    return *this;
}

template<typename T>
typename List<T>::Iterator List<T>::begin()
{
    return Iterator(head);
}

template<typename T>
typename List<T>::Iterator List<T>::end()
{
    return Iterator(nullptr);
}

template<typename T>
bool List<T>::empty()
{
    return _size == 0 || head == nullptr;
}

template<typename T>
std::size_t List<T>::size()
{
    return _size;
}

template<typename T>
T& List<T>::front()
{
    return head->data;
}

template<typename T>
T& List<T>::back()
{
    return tail->data;
}

template<typename T>
void List<T>::push_front(T& data)
{
    push_front(std::move(data));
}

template<typename T>
void List<T>::push_front(T&& data)
{
    ListNode<T>* n = new ListNode<T>(data);
    ListNode<T>* tmp = head;

    if(empty()) 
    {
        tail = n;
    }

    if(!empty())
    {
        tmp->prev = n;
    }
    
    head = n;
    n->next = tmp;

    _size++;
}

template<typename T>
void List<T>::pop_front()
{
    ListNode<T>* tmp = head;
    head = tmp->next;
    delete tmp;
    
    _size--;

    if(empty())
    {
        tail = nullptr;
    }
}

template<typename T>
void List<T>::push_back(T& data)
{
    push_back(std::move(data));
}

template<typename T>
void List<T>::push_back(T&& data)
{
    ListNode<T>* n = new ListNode<T>(data);
    ListNode<T>* tmp = tail;

    if(empty()) 
    {
        head = n;
    }

    if(!empty())
    {
        tmp->next = n;
    }

    tail = n;
    n->prev = tmp;

    _size++;
}

template<typename T>
void List<T>::pop_back()
{
    ListNode<T>* tmp = tail;
    tail = tmp->prev;
    delete tmp;

    _size--;

    if(empty())
    {
        head = nullptr;
    }
}

//template<typename... Args>
//template<typename T>
//typename List<T>::Iterator List<T>::emplace(typename List<T>::Iterator pos, Args&&... args)
//{}

template<typename T>
typename List<T>::Iterator List<T>::insert(typename List<T>::Iterator pos, const T& data)
{
    ListNode<T>* n = new ListNode<T>(data);
    _size++;
    
    if(pos->curr == head)
    {
        n->next = head;
        head->prev = n;
        head = n;

        pos->curr = n;
        return pos;
    }

    if(pos->curr == nullptr)
    {
        tail->next = n;
        n->prev = tail;
        tail = n;

        pos->curr = n;
        return pos;
    }

    ListNode<T>* prev = pos->curr->prev;
    prev->next = n;
    n->prev = prev;
    n->next = pos->curr;
    pos->curr->prev = n;

    pos->curr = n;
    return pos;
}

template<typename T>
typename List<T>::Iterator List<T>::erase(typename List<T>::Iterator pos)
{
    ListNode<T> *prev = pos->curr->prev; 
    ListNode<T> *next = pos->curr->next;

    _size--;

    if(pos->curr == tail)
    {
        prev->next = nullptr;
        tail = prev;
        delete pos->curr;

        pos->curr = nullptr;
        return pos;
    }

    if(pos->curr == head)
    {
        head = next;
    }
    else
    {
        prev->next = next;
    }

    next->prev = prev;
    delete pos->curr;

    pos->curr = next;
    return pos;

}

template<typename T>
typename List<T>::Iterator List<T>::erase(typename List<T>::Iterator first, typename List<T>::Iterator last)
{
    while(first!=last)
    {
        first = erase(first);
    }

    return first;
}

template<typename T>
void List<T>::clear()
{
    if(empty()) return;

    ListNode<T>* curr = head;

    while(head)
    {
        head = head->next;
        delete curr;
        curr = head;
    }

    _size = 0;
}

template<typename T>
void List<T>::reverse()
{
    ListNode<T>* curr = tail;

    while(curr != nullptr)
    {
        std::swap(curr->next, curr->prev);
        curr = curr->next;
    }
}

template<typename T>
void List<T>::print()
{
    for(auto it = begin(); it != end(); ++it)
    {
        std::cout << it->data << ' ';
    }
    std::cout << '\n';
}

template<typename T>
void swap(List<T>& lhs, List<T>& rhs)
{
    std::swap(lhs.head, rhs.head);
    std::swap(lhs.tail, rhs.tail);
    std::swap(lhs._size, rhs._size);
}


//**********************************
//List::ListNode
//**********************************
template<typename T>
template<typename E>
class List<T>::ListNode
{
private:
public:
    T data{};
    ListNode* next{};
    ListNode* prev{};

    ListNode() = default;
    ListNode(const T& d) : data(d) {}
    ListNode(const ListNode&) = delete;
    ListNode& operator=(const ListNode&) = delete;
    ListNode(ListNode&&) = delete;
    ListNode& operator=(ListNode&&) = delete;
};


//**********************************
//List::Iterator
//**********************************
template<typename T>
class List<T>::Iterator
{
private:
    ListNode<T>* curr;
public:
    Iterator() noexcept : curr{} {};
    ~Iterator() = default;
    Iterator(ListNode<T>* ptr) noexcept : curr{ptr} {}
    
    Iterator(const Iterator& other) noexcept : curr{other.curr} {}

    Iterator& operator=(const Iterator& rhs)
    { 
        curr = rhs.curr; 
        return *this; 
    }

    //Iterator(Iterator&&) = delete;
    //Iterator& operator=(Iterator&&) = delete;

    friend void swap(Iterator& lhs, Iterator& rhs) 
    { 
        std::swap(lhs.curr, rhs.curr); 
    }

    T& operator*() const 
    { 
        if(curr) 
            return *curr;

        throw std::terminate();
    }

    Iterator& operator++()
    {
        if(curr)
        {
            curr = curr->next;
        }

        return *this;
    }

    Iterator operator++(int)
    {
        Iterator it = *this;
        ++*this;
        return it;
    }
    
    Iterator& operator--()
    {
        if(curr)
        {
            curr = curr->prev;
        }

        return *this;
    }
    
    Iterator operator--(int)
    {
        Iterator it = *this;
        --*this;
        return it;
    }
    
    bool operator==(const Iterator& rhs) const 
    {
        return this->curr == rhs.curr;
    }
    
    bool operator!=(const Iterator& rhs) const 
    {
        return !this->operator!=(rhs);
    }

    ListNode<T>* operator->()
    {
        return curr;
    }
};
