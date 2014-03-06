// Copyright (c) 2013, German Neuroinformatics Node (G-Node)
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted under the terms of the BSD License. See
// LICENSE file in the root of the Project.

#ifndef NIX_DATA_ARRAY_H
#define NIX_DATA_ARRAY_H

#include <boost/multi_array.hpp>

#include <nix/base/EntityWithSources.hpp>
#include <nix/base/IDataArray.hpp>
#include <nix/Dimensions.hpp>
#include <nix/Hydra.hpp>

#include <nix/Platform.hpp>

namespace nix {


class NIXAPI DataArray : public base::EntityWithSources<base::IDataArray> {

public:

    DataArray()
        : EntityWithSources()
    {}

    DataArray(const DataArray &other)
        : EntityWithSources(other.impl_ptr)
    {
    }

    DataArray(const std::shared_ptr<base::IDataArray> &p_impl)
        : EntityWithSources(p_impl)
    {
    }

    //--------------------------------------------------
    // Element getters and setters
    //--------------------------------------------------

    /**
     * Get the label for the values stored in the DataArray.
     *
     * @return string the label
     */
    std::string label() const {
        return backend()->label();
    }

    /**
     * Set the label for the data stored.
     *
     * @param string label
     */
    void label(const std::string &label) {
        backend()->label(label);
    }

    /**
     * Get the unit of the data stored in this dataArray.
     *
     * @return string the unit.
     */
    std::string unit() const {
        return backend()->unit();
    }

    /**
     * Set the unit for the values stored in this DataArray.
     *
     * @param string the unit
     */
    void unit(const std::string &unit) {
        backend()->unit(unit);
    }

    /**
     * Returns the expansion origin of the calibration polynom.
     * This is set to 0.0 by default.
     *
     * @return double the expansion origin.
     */
    double expansionOrigin() const {
        return backend()->expansionOrigin();
    }

    /**
     * Set the expansion origin for the calibration.
     *
     * @param double the expansion origin.
     */
    void expansionOrigin(double expansion_origin) {
        backend()->expansionOrigin(expansion_origin);
    }

    /**
     * Set the polynom coefficients for the calibration. By default this is set
     * to a two element vector of [0.0, 1.0] for a linear calibration with zero offset.
     *
     * @param vector<double> the coefficients
     */
    void polynomCoefficients(std::vector<double> &polynom_coefficients) {
        backend()->polynomCoefficients(polynom_coefficients);
    }

    /**
     * Returns the polynom coefficients.
     *
     * @return vector<double> the coefficients.
     */
    std::vector<double> polynomCoefficients() const {
        return backend()->polynomCoefficients();
    }

    //--------------------------------------------------
    // Methods concerning dimensions
    //--------------------------------------------------

    /**
     * Get dimensions associated with this data array.
     *
     * The parameter "filter" is defaulted to giving back all dimensions. 
     * To use your own filter pass a lambda that accepts a "Dimension"
     * as parameter and returns a bool telling whether to get it or not.
     *
     * @param object filter function of type {@link nix::util::Filter::type}
     * @return object dimensions as a vector     
     */
    std::vector<Dimension> dimensions(util::AcceptAll<Dimension>::type filter
                                      = util::AcceptAll<Dimension>()) const
    {
        auto f = [this] (size_t i) { 
            nix::Dimension empty_dim;
            try { return getDimension(i+1); } // +1 since index starts at 1
            catch(std::exception& e) { return empty_dim; } 
        };
        return getEntities<Dimension>(f,
                                      dimensionCount(), 
                                      filter);
    }


    size_t dimensionCount() const {
        return backend()->dimensionCount();
    }


    Dimension getDimension(size_t id) const {
        return backend()->getDimension(id);
    }


    Dimension appendDimension(DimensionType type) {
        return backend()->createDimension(backend()->dimensionCount() + 1, type);
    }


    Dimension createDimension(size_t id, DimensionType type) {
        return backend()->createDimension(id, type);
    }

    bool deleteDimension(size_t id) {
        return backend()->deleteDimension(id);
    }

    //--------------------------------------------------
    // Other methods and functions
    //--------------------------------------------------

    /**
     * Output operator
     */
    friend std::ostream& operator<<(std::ostream &out, const DataArray &ent) {
        out << "DataArray: {name = " << ent.name();
        out << ", type = " << ent.type();
        out << ", id = " << ent.id() << "}";
        return out;
    }

    double applyPolynomial(std::vector<double> &coefficients, double origin, double input) const;

    //--------------------------------------------------
    // Methods concerning data access.
    //--------------------------------------------------

    void createData(DataType dtype, const NDSize &size) {
        backend()->createData(dtype, size);
    }

    template<typename T> void createData(const T &value, const NDSize &size = {});

    template<typename T> void getData(T &value) const;
    template<typename T> void setData(const T &value);

    template<typename T> void getData(T &value, const NDSize &count, const NDSize &offset) const;
    template<typename T> void getData(T &value, const NDSize &offset) const;
    template<typename T> void setData(const T &value, const NDSize &offset);

    NDSize getDataExtent() const {
        return backend()->getExtent();
    }

    void setDataExtent(const NDSize &extent) {
        backend()->setExtent(extent);
    }

    DataType getDataType(void) const {
        return backend()->getDataType();
    }

 };

template<typename T>
void DataArray::createData(const T &value, const NDSize &size)
{
    const Hydra<const T> hydra(value);
    DataType dtype = hydra.element_data_type();

    createData(dtype, size.size() ? size : hydra.shape());
}

template<typename T>
void DataArray::getData(T &value) const
{
    Hydra<T> hydra(value);

    NDSize extent = backend()->getExtent();
    hydra.resize(extent);

    DataType dtype = hydra.element_data_type();
    NDSize shape = hydra.shape();

    backend()->read(dtype, hydra.data(), shape, {});
}

template<typename T>
void DataArray::setData(const T &value)
{
    const Hydra<const T> hydra(value);

    DataType dtype = hydra.element_data_type();
    NDSize shape = hydra.shape();

    backend()->setExtent(shape);
    backend()->write(dtype, hydra.data(), shape, {});
}

template<typename T>
void DataArray::getData(T &value, const NDSize &count, const NDSize &offset) const
{
    Hydra<T> hydra(value);
    DataType dtype = hydra.element_data_type();

    hydra.resize(count);
    backend()->read(dtype, hydra.data(), count, offset);
}

template<typename T>
void DataArray::getData(T &value, const NDSize &offset) const
{
    Hydra<T> hydra(value);
    DataType dtype = hydra.element_data_type();

    NDSize count = hydra.shape();
    backend()->read(dtype, hydra.data(), count, offset);
}


template<typename T>
void DataArray::setData(const T &value, const NDSize &offset)
{
    const Hydra<const T> hydra(value);

    DataType dtype = hydra.element_data_type();
    NDSize shape = hydra.shape();

    backend()->write(dtype, hydra.data(), shape, offset);
}

} // namespace nix

#endif // NIX_DATA_ARRAY_H
