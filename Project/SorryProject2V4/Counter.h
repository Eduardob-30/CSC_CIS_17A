/* 
 * File:   Counter.h
 * Author: eddiebrito
 *
 * Created on May 31, 2025, 12:26 PM
 */
// Counter.h
#ifndef COUNTER_H
#define COUNTER_H
/*
 * @class Counter
 * @brief A template based counter class for tracking values.
 * 
 * @tparam T The numeric type to count.
 */
template <typename T>
class Counter {
private:
    T count;
public:
    Counter() : count(0) {}
    // void add(T amt)
    // Adds a specified amount to the current count.
    void add(T amt) {
        count += amt;   
    }
    // T getCount() const
    // returns the current count.
    T getCount() const {
        return count;
    }
    // void reset()
    // resets the count to zero
    void reset() {
        count = 0;      
    }
};
#endif // COUNTER_H