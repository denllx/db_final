#pragma once
#include <string>
#include <iostream>
#include <memory>
using namespace std;

class Value;
using ptr_v = shared_ptr<Value>;

enum OprdType { Null, Zero, NZero, Double, Char };

constexpr int total_type = Char + 1;

class Value{
    virtual ostream& print(ostream&) const;
public:
    Value()=default;
    virtual ~Value()=default;
    virtual bool operator<(const Value&) const;
    virtual ptr_v cast_up(ptr_v) const;
    virtual OprdType oprd_type() const;
    friend ostream& operator<<(ostream&, const Value&);
    static ptr_v v_not(ptr_v);
    static ptr_v v_and(ptr_v, ptr_v);
    static ptr_v v_or(ptr_v, ptr_v);
    static ptr_v v_lt(ptr_v, ptr_v);
    static ptr_v v_gt(ptr_v, ptr_v);
    static ptr_v v_eq(ptr_v, ptr_v);
};

class DoubleValue : public Value{
    double value;
    ostream& print(ostream&) const override;
public:
    DoubleValue(double);
    bool operator<(const Value&) const override;
    ptr_v cast_up(ptr_v) const override;
    OprdType oprd_type() const override;
};

class IntValue : public Value{
    int value;
    ostream& print(ostream&) const override;
public:
    IntValue(int);
    shared_ptr<DoubleValue> to_double() const;
    bool operator<(const Value&) const override;
    ptr_v cast_up(ptr_v) const override;
    OprdType oprd_type() const override;
};

class CharValue : public Value{
    string value;
    ostream& print(ostream&) const override;
public:
    CharValue(string);
    bool operator<(const Value&) const override;
    ptr_v cast_up(ptr_v) const override;
    OprdType oprd_type() const override;
};

