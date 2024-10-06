#include "queue.h"
#include <iostream>
#include <cstdlib>

using namespace std;

Queue::Queue()
{
    head = 0;
    tail = 0;
    nelements = 0;
    verbose = false;
}

Queue::~Queue()
{
    for (QElement *qe = head; qe != 0;)
    {
        QElement *temp = qe;
        qe = qe->next;
        delete (temp);
    }
}

void Queue::remove(Data *d)
{
    if (size() > 0)
    {
        QElement *qe = head;
        head = head->next;
        nelements--;
        *d = qe->data;
        delete qe;
    }
    else
    {
        cerr << "Error: Queue is empty.\n";
        exit(1);
    }
}

void Queue::insert(Data d, unsigned position)
{
    if (position > size())
    {
        cerr << "Range error.\n";
        return;
    }

    QElement * el = new QElement(d);

    // Insert at the head if position is 0
    if (position == 0)
    {
        el->next = head;
        head = el;
        // Updating the tail as well if the queue was empty
        if (size() == 0)
        {
            tail = el;
        }
    }
    // Insert at the tail if position set to size of the queue
    else if (position == size())
    {
        if (size() > 0)
        {
            tail->next = el;
            tail = el;
        }
        // If the queue is empty head and tail will be same to the new element
        else
        {
            head = tail = el;
        }
    }
    // If the position was not 0 and not to the size(); insert in the middle
    else
    {
        QElement *current = head;
        for (unsigned i = 0; i < position - 1; i++)
        {
            current = current->next;
        }
        el->next = current->next;
        current->next = el;
    }
    nelements++;
}

bool Queue::search(Data otherData) const
{
    QElement *insideEl = head;
    for (int i = 0; i < nelements; i++)
    {
        if (insideEl->data.equals(otherData))
            return true;
        insideEl = insideEl->next;
    }
    return false;
}

void Queue::print() const
{
    QElement *qe = head;
    if (size() > 0)
    {
        for (unsigned i = 0; i < size(); i++)
        {
            cout << i << ":(" << qe->data.x << "," << qe->data.y << ") ";
            qe = qe->next;
        }
    }
    cout << "\n";
}

unsigned Queue::size() const
{
    return nelements;
}
