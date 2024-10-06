#include <iostream>
#include <cassert>
#include <cstdlib>
#include <vector>

#include "queue.h"

/**
   Compare the given queue to the given array of data elements.  Return
   true if they are all equal.
 */
bool testQueueEquality(Queue &q, std::vector<Data> const &array)
{
    if (q.size() != array.size())
        return false;

    // Accessing the head of the queue using the getHead
    QElement* current = q.getHead();
    for (unsigned i = 0; i < array.size(); ++i)
    {
        // Accessing the data of the queue using the getData
        if (!current->getData().equals(array[i]))
            return false;
        // Accessing the next element using the getNext
        current = current->getNext();
    }
    return true;
}

int main()
{
    using namespace std;

    cout << "Testing queue.\n";
    // Queue q1;

    // q1.insert(Data(1,1));
    // q1.insert(Data(3,3));
    // q1.insert(Data(5,5));

    // q1.print();

    Queue q;

    // Insert at the tail
    q.insert(Data(1, 2), q.size());
    // Insert at the tail
    q.insert(Data(3, 4), q.size());
    // Insert at the tail
    q.insert(Data(5, 6), q.size());
    // Insert at position 1
    q.insert(Data(-2, -3), 1);
    // Insert at position 3
    q.insert(Data(-4, -5), 3);

    q.print();

    vector<Data> dataVec;
    // dataVec.push_back(Data(1,1));
    // dataVec.push_back(Data(3,3));
    // dataVec.push_back(Data(5,5));

    dataVec.push_back(Data(1, 2));
    dataVec.push_back(Data(-2, -3));
    dataVec.push_back(Data(3, 4));
    dataVec.push_back(Data(-4, -5));
    dataVec.push_back(Data(5, 6));
    
    assert(testQueueEquality(q, dataVec));


    Data d44(4, 4);
    bool found = q.search(d44);
    assert(found == false);

    q.insert(d44, q.size());  // Now is (1,2), (-2,-3), (3,4), (-4,-5), (5,6), (4,4)
    found = q.search(d44);
    assert(found == true);

    // now queue is (1,2), (-2,-3), (3,4), (-4,-5), (5,6), (4,4) and
    // dataVec has (1,2), (-2,-3), (3,4), (-4,-5), (5,6). Not equal
    assert(testQueueEquality(q, dataVec) == false);

    Data temp;
    q.remove(&temp);  // now q is: (-2,-3), (3,4), (-4,-5), (5,6), (4,4)

    Data temp2(1, 2);
    assert(temp.equals(temp2));  // (1,2) == (1,2)

    Data temp3(6, 6);
    found = q.search(temp3);
    assert(found == false);
}
