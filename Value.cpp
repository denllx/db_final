#include "Value.h"
#include <cassert>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

IntValue::IntValue(int value) : value(value) {}
DoubleValue::DoubleValue(double value) : value(value) {}
CharValue::CharValue(std::string value) : value(value) {}

std::shared_ptr<DoubleValue> IntValue::to_double() const {
    return std::make_shared<DoubleValue>(value);
}

bool Value::operator<(const Value &) const { assert(false); return false; }

bool IntValue::operator<(const Value &v) const {
    return value < static_cast<const IntValue &>(v).value;
}

bool DoubleValue::operator<(const Value &v) const {
    return value < static_cast<const DoubleValue &>(v).value;
}

bool CharValue::operator<(const Value &v) const {
    return value < static_cast<const CharValue &>(v).value;
}

std::ostream &Value::print(std::ostream &os) const { return os << "NULL"; }

std::ostream &IntValue::print(std::ostream &os) const { return os << value; }

std::ostream &DoubleValue::print(std::ostream &os) const {
    return os << fixed << std::setprecision(4) << value;
}

std::ostream &CharValue::print(std::ostream &os) const { return os << value; }

std::ostream &operator<<(std::ostream &os, const Value &v) {
    return v.print(os);
}

ptr_v Value::cast_up(ptr_v v) const { return v; }

ptr_v IntValue::cast_up(ptr_v v) const { return v; }

ptr_v DoubleValue::cast_up(ptr_v v) const {
    if (auto p = dynamic_pointer_cast<IntValue>(v)) {
        return p->to_double();
    }
    return v;
}

ptr_v CharValue::cast_up(ptr_v v) const { return v; }

OprdType Value::oprd_type() const { return Null; }

OprdType DoubleValue::oprd_type() const { return Double; }

OprdType IntValue::oprd_type() const { return value ? NZero : Zero; }

OprdType CharValue::oprd_type() const { return Char; }

ptr_v Value::v_not(ptr_v x) {
    if (x->oprd_type() == Null) return x;
    if (auto p = dynamic_pointer_cast<IntValue>(x)) {
        return std::make_shared<IntValue>(x->oprd_type() == Zero);
    }
    assert(false);
}

ptr_v Value::v_and(ptr_v x, ptr_v y) {
    if (x->oprd_type() == Zero || y->oprd_type() == Zero) {
        return std::make_shared<IntValue>(0);
    }
    if (x->oprd_type() == Null || y->oprd_type() == Null) {
        return std::make_shared<Value>();
    }
    return std::make_shared<IntValue>(1);
}

ptr_v Value::v_or(ptr_v x, ptr_v y) {
    if (x->oprd_type() == NZero || y->oprd_type() == NZero) {
        return std::make_shared<IntValue>(1);
    }
    if (x->oprd_type() == Null || y->oprd_type() == Null) {
        return std::make_shared<Value>();
    }
    return std::make_shared<IntValue>(0);
}

ptr_v Value::v_lt(ptr_v x, ptr_v y) {
    if (x->oprd_type() == Null) return x;
    if (y->oprd_type() == Null) return y;
    x = y->cast_up(x);
    y = x->cast_up(y);
    return std::make_shared<IntValue>(*x < *y);
}

ptr_v Value::v_gt(ptr_v x, ptr_v y) {
    if (x->oprd_type() == Null) return x;
    if (y->oprd_type() == Null) return y;
    x = y->cast_up(x);
    y = x->cast_up(y);
    return std::make_shared<IntValue>(*y < *x);
}

ptr_v Value::v_eq(ptr_v x, ptr_v y) {
    if (x->oprd_type() == Null) return x;
    if (y->oprd_type() == Null) return y;
    x = y->cast_up(x);
    y = x->cast_up(y);
    return std::make_shared<IntValue>(!(*x < *y) && !(*y < *x));
}