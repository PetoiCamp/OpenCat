//reference: https://github.com/SloCompTech/QList

#ifndef LIB_SCP_QLIST
#define LIB_SCP_QLIST

#ifndef NULL
#define NULL 0
#endif

template<class T>
class QList
{
  private:
    typedef struct node
    {
      T item;
      node *next, *prev;
    } node;

    int len; // Size of list
    node *start, *end; // Pointers to start and end

  public:
    QList(); // Class constructor
    ~QList(); // Class destructor

    void push_back(const T i); // Push item at the back of list
    void push_front(const T i);// Push item at the front of the list
    void pop_back(); // Pops item from back
    void pop_front(); // Pops item from front
    T front(); // get item from front
    T back(); // get item from back
    int size(); // Returns size of list
    void clear(); // Clears list
    void clear(unsigned int index); // Clears list
    T get(unsigned int index); // Get item at given index
    T& at(unsigned int index); // Get item at given index

    // Array operator
    T& operator[](unsigned int index);
    const T& operator[](unsigned int index) const; // Not realy needed

    // Non - critical functions
    int length();
    int indexOf(T val);

};



// Constructor
template<class T>
QList<T>::QList()
{
  len = 0;
  start = NULL;
  end = NULL;
}

// Destructor
template<class T>
QList<T>::~QList()
{
  clear();
}

// Push at front
template<class T>
void QList<T>::push_front(const T i)
{
  node *tmp = new node;
  tmp->item = i;
  tmp->next = NULL;
  tmp->prev = NULL;

  if (start == NULL) // If list is empty
  {
    start = tmp;
    end = tmp;
  }
  else // Insert at start
  {
    tmp->next = start;
    start->prev = tmp;
    start = tmp;
  }
  len++; // Increase size counter
}

// Push at back
template<class T>
void QList<T>::push_back(const T i)
{
  node *tmp = new node;
  tmp->item = i;
  tmp->next = NULL;
  tmp->prev = NULL;

  if (end == NULL) // If list is empty
  {
    start = tmp;
    end = tmp;
  }
  else // Insert at the end
  {
    tmp->prev = end;
    end->next = tmp;
    end = tmp;
  }
  len++; // Increase size counter
}

// Pop from front
template<class T>
void QList<T>::pop_front()
{
  if (start != NULL)
  {
    node *tmp = start;
    start = start->next;
    if (start != NULL) // Re-link next item to NULL
      start->prev = NULL;
    else // List became empty so we need to clear end
      end = NULL;
    delete tmp;
    len--; // Decrease counter
  }
}

// Pop from back
template<class T>
void QList<T>::pop_back()
{
  if (end != NULL)
  {
    node *tmp = end;
    end = end->prev;
    if (end != NULL) //Re-link previous item to NULL
      end->next = NULL;
    else // List became empty so we need to clear start
      start = NULL;
    delete tmp;
    len--; // Decrease counter
  }
}

// Get item from front
template<class T>
T QList<T>::front()
{
  if (start != NULL)
    return start->item;
  //TODO: Catch error when list is empty
  else
    return NULL;
}

//Get item from back
template<class T>
T QList<T>::back()
{
  if (end != NULL)
    return end->item;
  //TODO: Catch error when list is empty
  else
      return NULL;
}

// Get size
template<class T>
int QList<T>::size()
{
  return this->len;
}

// Clear list
template<class T>
void QList<T>::clear()
{
  node *tmp = start;
  while (start != NULL)
  {
    tmp = start;
    start = start->next;
    delete tmp; // Delete item
    len--; // Decrease counter
  }
  end = NULL;
}
template<class T>
void QList<T>::clear(unsigned int index)
{
  node *tmp = start;
  for (int i = 0; i <= index && tmp != NULL; i++)
  {
    if (i == index)
    {
      if (tmp->prev != NULL)
        tmp->prev->next = tmp->next;
      else
        start = tmp->next;

      if (tmp->next != NULL)
        tmp->next->prev = tmp->prev;
      else
        end = tmp->prev;

      len--; // Decrease counter
      delete tmp; // Delete item
      break;
    }
    else
      tmp = tmp->next;
  }
}

// Get at index
template<class T>
T QList<T>::get(unsigned int index)
{
  node *tmp = start;
  for (int i = 0; i <= index && tmp != NULL; i++)
  {
    if (i == index)
      return tmp->item;
    else
      tmp = tmp->next;
  }
  //TODO: Catch error when index is out of range
  return NULL;
}

template<class T>
T& QList<T>::at(unsigned int index)
{
  node *tmp = start;
  for (int i = 0; i <= index && tmp != NULL; i++)
  {
    if (i == index)
      return tmp->item;
    else
      tmp = tmp->next;
  }
  //TODO: Catch error when index is out of range
  return NULL;
}

// Get length
template<class T>
int QList<T>::length()
{
  return this->len;
}

// Get index of value
template<class T>
int QList<T>::indexOf(T val)
{
  for (int i = 0; i < this->size(); i++)
    if (this->at(i) == val)
      return i;
  return -1;
}

// Array operators
template<class T>
T& QList<T>::operator[](unsigned int index)
{
  node *tmp = start;
  for (int i = 0; i <= index && tmp != NULL; i++)
  {
    if (i == index)
      return tmp->item;
    else
      tmp = tmp->next;
  }
  //TODO: Catch error when index is out of range
  return NULL;
}


template<class T>
const T& QList<T>::operator[](unsigned int index) const
{
  node *tmp = start;
  for (int i = 0; i <= index && tmp != NULL; i++)
  {
    if (i == index)
      return tmp->item;
    else
      tmp = tmp->next;
  }
  //TODO: Catch error when index is out of range
  return NULL;
}

#endif
