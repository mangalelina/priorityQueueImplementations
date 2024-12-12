#ifndef BINARYPQ_H
#define BINARYPQ_H
#include <algorithm>
#include "FoundationPQ.h"

// A specialized version of the priority queue ADT implemented as a binary heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class BinaryPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Description: Constructs an empty PQ with an optional comparison functor.
    // Runtime: O(1)
    explicit BinaryPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {} // BinaryPQ


    // Description: Constructs a PQ out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    BinaryPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
    BaseClass{ comp }, data{start, end} {
        updatePriorities();
    } // BinaryPQ


    // Description: Destructor doesn't need any code, the data vector will
    //              be destroyed automatically.
    virtual ~BinaryPQ() {
    } // ~BinaryPQ()


    // Description: Assumes that all elements inside the heap are out of order and
    //              'rebuilds' the heap by fixing the heap invariant.
    // Runtime: O(n)
    virtual void updatePriorities() {
        if (size() <= 1) return;
        int i = static_cast<int>(size()-1);
        //std::cout << "max index is " << i << "\n";
        while (i >= 0) {
            //std::cout << "i is " << i << "\n";
            if (2*i < int(size())) {
                //std::cout << "not a leaf node\n";
                fixDown(static_cast<std::size_t>(i));
            }
            i--;
        }
    } // updatePriorities()


    // Description: Adds a new element to the PQ.
    // Runtime: O(log(n))
    virtual void push(const TYPE &val) {
        data.push_back(val);
        fixUp(data.size() - 1);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element
    //              from the PQ.
    // Note: We will not run tests on your code that would require it to pop
    // an element when the PQ is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: O(log(n))
    virtual void pop() {
        swap(data[0], data[size()-1]);
        data.pop_back();
        if (data.size() > 1) {
            fixDown(0);
        }
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the PQ. This should be a reference for speed. It MUST
    //              be const because we cannot allow it to be modified, as
    //              that might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        return data[0];
    } // top()


    // Description: Get the number of elements in the PQ.
    // Runtime: O(1)
    virtual std::size_t size() const {
        return data.size();
    } // size()


    // Description: Return true if the PQ is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        if (size() == 0) return true;
        return false;
    } // empty()


private:
    std::vector<TYPE> data;


    
//    bool isLeaf(std::size_t i) { return ((2*i >= size()));}
    
    void fixUp(std::size_t k) {
        while (k > 0 && this->compare(data[(k-1)/2], data[k])) {
            swap(data[k], data[(k-1)/2]);
            k = (k-1)/2;
        }
    } // fixUp()
    
    void fixDown (std::size_t k) {
        while (((2*k)+1) <= size() - 1) {
            std::size_t j = (2*k) + 1;
            if (j < size()-1) {
                if (this->compare(data[j], data[j+1])) {
                    j++;
                }
            }
            if (!this->compare(data[k], data[j])) {
                break;
            }
            swap(data[k], data[j]);
            k = j;
        }
    } // fixDown()
    
}; // BinaryPQ


#endif // BINARYPQ_H
