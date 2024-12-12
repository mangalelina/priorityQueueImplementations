#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "foundationPQ.h"
#include <deque>
#include <utility>
using namespace std;

// A specialized version of the priority queue ADT implemented as a pairing
// heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            explicit Node(const TYPE &val)
        : elt{ val }, child{ nullptr }, sibling{ nullptr }, parent{nullptr }
            {}

            // Description: Allows access to the element at that Node's
            // position.  There are two versions, getElt() and a dereference
            // operator, use whichever one seems more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }
            const TYPE &operator*() const { return elt; }

            // The following line allows you to access any private data
            // members of this Node class from within the PairingPQ class.
            // (ie: myNode.elt is a legal statement in PairingPQ's add_node()
            // function).
            friend PairingPQ;

        private:
            TYPE elt;
            Node *child;
            Node *sibling;
            Node *parent;
    }; // Node


    // Description: Construct an empty pairing heap with an optional
    //              comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
    BaseClass{ comp } {
        count = 0;
        root = nullptr;
    } // PairingPQ()


    // Description: Construct a pairing heap out of an iterator range with an
    //              optional comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
    BaseClass{ comp }, count{0}, root{nullptr} {
        for (auto itr = start; itr < end; itr++) {
            push(*itr);
        }
    }// PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other) : BaseClass{ other.compare } {
        root = nullptr;
        count = 0;
        if (other.root == nullptr) return;
        else {
            deque<Node *> q;
            q.push_back(other.root);
            while (!q.empty()) {
                Node* temp = q.front();
                q.pop_front();
                if (temp->sibling != nullptr) q.push_back(temp->sibling);
                if (temp->child != nullptr) q.push_back(temp->child);
                push(temp->elt);
            }
            count = other.count;
        }
    } // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    PairingPQ &operator=(const PairingPQ &rhs) {
        
        PairingPQ temp(rhs);
        std::swap(count, temp.count);
        std::swap(root, temp.root);
        return *this;
        
//        
//        if (rhs.root == nullptr) {
//            root = nullptr;
//            count = 0;
//        }
//        else {
//            deque<Node *> q;
//            q.push_back(rhs.root);
//            while (!q.empty()) {
//                Node* temp = q.front();
//                if (temp != nullptr) {
//                    if (temp->sibling != nullptr) q.push_back(temp->sibling);
//                    if (temp->child != nullptr) q.push_back(temp->child);
//                    push(temp->elt);
//                    q.pop_front();
//                }
//            }
//        }
//        return *this;
    } // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        while (!empty()) {
            pop();
        }
        root = nullptr;
        count = 0;
    } // ~PairingPQ()


    // Description: Assumes that all elements inside the pairing heap are out
    //              of order and 'rebuilds' the pairing heap by fixing the
    //              pairing heap invariant.  You CANNOT delete 'old' nodes
    //              and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        if (count <= 1) return;
        
        deque<Node *> q;
        q.push_back(root);
        Node * heap = nullptr;
        while (!q.empty()) {
            Node* temp = q.front();
            q.pop_front();
            if (temp->sibling!=nullptr) q.push_back(temp->sibling);
            if (temp->child!=nullptr) q.push_back(temp->child);
            temp->parent = nullptr;
            temp->sibling = nullptr;
            temp->child = nullptr;
            if (heap == nullptr) heap = temp;
            else heap = meld(heap, temp);
        }
        root = heap;
    } // updatePriorities()


    // Description: Add a new element to the pairing heap. This is already
    //              done. You should implement push functionality entirely
    //              in the addNode() function, and this function calls
    //              addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) {
        addNode(val);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element
    //              from the pairing heap.
    // Note: We will not run tests on your code that would require it to pop
    // an element when the pairing heap is empty. Though you are welcome to
    // if you are familiar with them, you do not need to use exceptions in
    // this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        
        if (count <= 1) {
            Node* temp = root;
            delete temp;
            root = nullptr;
            count = 0;
            return;
        }
        
        deque<Node *> q;
        Node* temp = root->child;
        while (temp!=nullptr) {
            q.push_back(temp);
            temp = temp->sibling;
        }
        while (q.size() > 1) {
            Node* temp1 = q.front();
            q.pop_front();
            temp1->parent = nullptr;
            temp1->sibling = nullptr;
            
            Node* temp2 = q.front();
            q.pop_front();
            temp2->parent = nullptr;
            temp2->sibling = nullptr;
            
            Node* result;
            if (temp1 == nullptr) result = temp2;
            else if (temp2 == nullptr) result = temp1;
            else result = meld(temp1, temp2);
            q.push_back(result);
        }
        temp = root;
        delete temp;
        root = q.front();
        q.pop_front();
        count--;


    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the pairing heap. This should be a reference for speed.
    //              It MUST be const because we cannot allow it to be
    //              modified, as that might make it no longer be the most
    //              extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        return root->elt;
    } // top()


    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    virtual std::size_t size() const {
        return count;
    } // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        if (count == 0) return true;
        return false;
    } // empty()


    // Description: Updates the priority of an element already in the pairing
    //              heap by replacing the element refered to by the Node with
    //              new_value.  Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more
    //              extreme (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    void updateElt(Node* node, const TYPE &new_value) {
        if (node == nullptr || root == nullptr) return;
        
        // updated with a value smaller than it's current value, do not need to rearrange anything
        if (!this->compare(node->elt, new_value)) {
            node->elt = new_value;
            return;
        }
        // updated with a value larger than it is current value
        else {
            if (node->parent == nullptr) {
                node->elt = new_value;
                return;
            }
            //updated value <= parent's value
            if (!this->compare(node->parent->elt, new_value)) {
                node->elt = new_value;
                return;
            }
            else { // updated value > parent's value
                // leftmost child of parent - parent's child ptr needs to be updated
                if (node->parent->child == node) {
                    node->parent->child = node->sibling;
                }
                // if not the left most child, then left sibling's sibling pointer needs to be updated
                else {
                    Node* temp = node->parent->child;
                    while (temp->sibling != node) {
                        temp = temp->sibling;
                    }
                    temp->sibling = node->sibling;
                }
                // detatch subheap
                node->sibling = nullptr;
                node->parent = nullptr;
                node->elt = new_value;
                root = meld(root, node);
            }
        }
    } // updateElt()


    // Description: Add a new element to the pairing heap. Returns a Node*
    //              corresponding to the newly added element.
    // Runtime: O(1)
    // NOTE: Whenever you create a node, and thus return a Node *, you must
    //       be sure to never move or copy/delete that node in the future,
    //       until it is eliminated by the user calling pop(). Remember this
    //       when you implement updateElt() and updatePriorities().
    Node* addNode(const TYPE &val) {
        Node* n = new Node(val);
        n->parent = nullptr;
        n->sibling = nullptr;
        n->child = nullptr;
        if (count == 0) root = n;
        else root = meld(root, n);
        count++;
        return n;
    } // addNode()


private:
    std::size_t count;
    Node* root;
    
    Node* meld(Node* a, Node* b) {
        
//        if (a==nullptr && b==nullptr) return nullptr;
//        if (a==nullptr) return b;
//        if (b==nullptr) return a;
        
        // a < b, so a should become b's leftmost child
        if (this->compare(a->elt, b->elt)) {
           
            a->parent = b;
        
            if (b->child != nullptr) a->sibling = b->child;
     
            b->child = a;
   
            return b;
        }
        else {
            b->parent = a;
   
            if (a->child != nullptr) b->sibling = a->child;
       
            a->child = b;
        
            return a;
        }
    }

    // NOTE: For member variables, you are only allowed to add a "root
    //       pointer" and a "count" of the number of nodes. Anything else
    //       (such as a deque) should be declared inside of member functions
    //       as needed.
};


#endif // PAIRINGPQ_H
