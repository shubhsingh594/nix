// Copyright © 2014, German Neuroinformatics Node (G-Node)
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted under the terms of the BSD License. See
// LICENSE file in the root of the Project.
//
// Author: Christian Kellner <kellner@bio.lmu.de>

#include <nix/Value.hpp>

#include <cassert>

namespace nix {

void Value::maybe_deallocte_string() {
    if (dtype == DataType::String) {
        v_string.~basic_string();
    }
}

/**************/
/* setters */

void Value::set(bool value) {
    maybe_deallocte_string();

    dtype = DataType::Bool;
    v_bool = value;
}

void Value::set(int32_t value) {
    maybe_deallocte_string();

    dtype = DataType::Int32;
    v_int32 = value;
}

void Value::set(uint32_t value) {
    maybe_deallocte_string();

    dtype = DataType::UInt32;
    v_uint32 = value;
}

void Value::set(int64_t value) {
    maybe_deallocte_string();

    dtype = DataType::Int64;
    v_int64 = value;
}

void Value::set(uint64_t value) {
    maybe_deallocte_string();

    dtype = DataType::UInt64;
    v_uint64 = value;
}

void Value::set(double value) {
    maybe_deallocte_string();

    dtype = DataType::Double;
    v_double = value;
}

void Value::set(const std::string &value) {

    //If the active member is not a string
    //we have to inialize the string object
    if (dtype != DataType::String) {
        new (&v_string) std::string();
    }

    dtype = DataType::String;
    v_string = value;
}

/**************/
/* getters */

void Value::get(bool &value) const {
    check_argument_type(DataType::Bool);
    value = v_bool;
}

void Value::get(int32_t &value) const {
    check_argument_type(DataType::Int32);
    value = v_int32;
}

void Value::get(uint32_t &value) const {
    check_argument_type(DataType::UInt32);
    value = v_uint32;
}

void Value::get(int64_t &value) const {
    check_argument_type(DataType::Int64);
    value = v_int64;
}

void Value::get(uint64_t &value) const {
    check_argument_type(DataType::UInt64);
    value = v_uint64;
}


void Value::get(double &value) const {
    check_argument_type(DataType::Double);
    value = v_double;
}

void Value::get(std::string &value) const {
    check_argument_type(DataType::String);
    value = v_string;
}

/* swap and swap helpers */


#if 0 //set to one to check that all supported DataTypes are handled
#define CHECK_SUPOORTED_VALUES
#endif

#define DATATYPE_SUPPORT_NOT_IMPLEMENTED false

void Value::swap(Value &other) {
    using std::swap;

    swap(uncertainty, other.uncertainty);
    swap(reference, other.reference);
    swap(encoder, other.encoder);
    swap(checksum, other.checksum);

    //now for the variant members
    if (dtype == other.dtype) {
        switch(dtype) {
        case DataType::Nothing: /* nothing to do, iterally  */ break;
        case DataType::Bool:   swap(v_bool, other.v_bool);     break;
        case DataType::Int32:  swap(v_int32, other.v_int32);   break;
        case DataType::UInt32: swap(v_uint32, other.v_uint32); break;
        case DataType::Int64:  swap(v_int64, other.v_int64);   break;
        case DataType::UInt64: swap(v_uint64, other.v_uint64); break;
        case DataType::Double: swap(v_double, other.v_double); break;
        case DataType::String: swap(v_string, other.v_string); break;
#ifndef CHECK_SUPOORTED_VALUES
        default: assert(DATATYPE_SUPPORT_NOT_IMPLEMENTED);
#endif
        }

        swap(dtype, other.dtype);

    } else {

        switch(dtype) {
        case DataType::Nothing:
            assign_variant_from(other);
            other.maybe_deallocte_string();
            other.dtype = DataType::Nothing;
            other.v_bool = false;
            break;

        case DataType::Bool:   swap_helper<bool>(other);        break;
        case DataType::Int32:  swap_helper<int32_t>(other);     break;
        case DataType::UInt32: swap_helper<uint32_t>(other);    break;
        case DataType::Int64:  swap_helper<int64_t>(other);     break;
        case DataType::UInt64: swap_helper<uint64_t>(other);    break;
        case DataType::Double: swap_helper<double>(other);      break;
        case DataType::String: swap_helper<std::string>(other); break;
#ifndef CHECK_SUPOORTED_VALUES
        default: assert(DATATYPE_SUPPORT_NOT_IMPLEMENTED);
#endif
        }
    }
}

void Value::assign_variant_from(const Value &other) {
    switch (other.dtype) {
    case DataType::Nothing: set(false);          break;
    case DataType::Bool:    set(other.v_bool);   break;
    case DataType::Int32:   set(other.v_int32);  break;
    case DataType::UInt32:  set(other.v_uint32); break;
    case DataType::Int64:   set(other.v_int64);  break;
    case DataType::UInt64:  set(other.v_uint64); break;
    case DataType::Double:  set(other.v_double); break;
    case DataType::String:  set(other.v_string); break;
#ifndef CHECK_SUPOORTED_VALUES
        default: assert(DATATYPE_SUPPORT_NOT_IMPLEMENTED);
#endif
    }
}

/* operators and functions */

std::ostream& operator<<(std::ostream &out, const Value &value)
{
    out << "Value{[" << value.type() << "] ";

    switch(value.type()) {
    case DataType::Bool:
        out << value.get<bool>();
        break;

    case DataType::String:
        out << value.get<std::string>();
        break;

    case DataType::Int32:
        out << value.get<int32_t>();
        break;

    case DataType::UInt32:
        out << value.get<uint32_t>();
        break;

    case DataType::Int64:
        out << value.get<int64_t>();
        break;

    case DataType::UInt64:
        out << value.get<uint64_t>();
        break;

    case DataType::Double:
        out << value.get<double>();
        break;

    case DataType::Nothing:
        break;

    default:
        out << "IMPLEMENT ME";
        break;
    }


    out << "}";
    return out;
}

void swap(Value &a, Value &b)
{
    a.swap(b);
}

}