#include "Value.h"
#include <cassert>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

IntValue::IntValue(int value) : value(value) {}
DoubleValue::DoubleValue(double value) : value(value) {}
CharValue::CharValue(std::string value) : value(value) {}
void DateValue::setzero(){year=0;month=0;day=0;}
bool DateValue::isValid(){
    int months[2][13]={{0,31,28,31,30,31,30,31,31,30,31,30,31},{0,31,29,31,30,31,30,31,31,30,31,30,31}};
    bool leap=false;
    if(year%400==0 || (year%4==0 && year%100!=0))leap=true;
    if(year<1001 || year>9999)return false;
    if(month<1 || month>12)return false;
    if(day<1 || day>months[leap][month])return false;
    return true;
}
DateValue::DateValue(std::string value){
    cout<<value<<endl;
    int pos1,pos2;
    if((pos1 = value.find("-",0))!=string::npos){
        if((pos2 = value.find("-",pos1+1))!=string::npos){
            year=atoi(value.substr(0,pos1).c_str());
            month=atoi(value.substr(pos1+1,pos2-pos1).c_str());
            day=atoi(value.substr(pos2+1,value.length()-pos2).c_str());
        }
    }
    if(!isValid())setzero();
}
void TimeValue::setzero(){hour=0;minute=0;second=0;positive=true;}
bool TimeValue::isValid(){
    return (hour>=0 && hour<=838 && minute>=0 && minute<=59 && second>=0 &&second<=59);
}
TimeValue::TimeValue(std::string value){
    if(value[0]=='-'){
        positive=false;
        value=value.substr(1,value.length()-1);
    }
    int pos1,pos2;
    if((pos1 = value.find(":",0))!=string::npos){
        if((pos2 = value.find(":",pos1+1))!=string::npos){
            hour=atoi(value.substr(0,pos1).c_str());
            minute=atoi(value.substr(pos1+1,pos2-pos1).c_str());
            second=atoi(value.substr(pos2+1,value.length()-pos2).c_str());
        }
    }
    if(!isValid())setzero();
}

std::shared_ptr<DoubleValue> IntValue::to_double() const {
    return std::make_shared<DoubleValue>(value);
}

std::shared_ptr<DateValue> CharValue::to_date() const {
    return std::make_shared<DateValue>(value);
}

std::shared_ptr<TimeValue> CharValue::to_time() const {
    return std::make_shared<TimeValue>(value);
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

bool DateValue::operator<(const Value &v) const {
    const DateValue& dv= static_cast<const DateValue &>(v);
    if(year<dv.year)return true;
    if(year>dv.year)return false;
    if(month<dv.month)return true;
    if(month>dv.month)return false;
    if(day<dv.day)return true;
    return false;
}

bool TimeValue::operator<(const Value &v) const {
    const TimeValue& tv= static_cast<const TimeValue &>(v);
    int f1=2*positive-1;
    int f2=2*tv.positive-1;
    if(f1*hour<f2*tv.hour)return true;
    if(f1*hour>f2*tv.hour)return false;
    if(f1*minute<f2*tv.minute)return true;
    if(f1*minute>f2*tv.minute)return false;
    if(f1*second<f2*tv.second)return true;
    return false;
}
std::ostream &Value::print(std::ostream &os) const { return os << "NULL"; }

std::ostream &IntValue::print(std::ostream &os) const { return os << value; }

std::ostream &DoubleValue::print(std::ostream &os) const {
    return os << fixed << std::setprecision(4) << value;
}

std::ostream &CharValue::print(std::ostream &os) const { return os << value; }

std::ostream &DateValue::print(std::ostream &os) const { return os << get_formated(); }

std::ostream &TimeValue::print(std::ostream &os) const { return os << get_formated(); }

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

ptr_v DateValue::cast_up(ptr_v v) const {
    if (auto p = dynamic_pointer_cast<CharValue>(v)) {
        return p->to_date();
    }
    return v;
}

ptr_v TimeValue::cast_up(ptr_v v) const {
    if (auto p = dynamic_pointer_cast<CharValue>(v)) {
        return p->to_time();
    }
    return v;
}

OprdType Value::oprd_type() const { return Null; }

OprdType DoubleValue::oprd_type() const { return Double; }

OprdType IntValue::oprd_type() const { return value ? NZero : Zero; }

OprdType CharValue::oprd_type() const { return Char; }

OprdType DateValue::oprd_type() const { return Date; }

OprdType TimeValue::oprd_type() const { return Time; }

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

ptr_v Value::v_xor(ptr_v x, ptr_v y) {
    if (x->oprd_type() == Null || y->oprd_type() == Null) {
        return std::make_shared<Value>();
    }
    bool v1= (x->oprd_type() == NZero);
    bool v2= (y->oprd_type() == NZero);
    return std::make_shared<IntValue>(v1^v2);
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
