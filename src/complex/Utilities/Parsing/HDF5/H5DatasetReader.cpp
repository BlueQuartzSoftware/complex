#include "H5DatasetReader.hpp"

#include <iostream>
#include <numeric>

#include <H5Apublic.h>

#include "complex/Utilities/Parsing/HDF5/H5.hpp"
#include "complex/Utilities/Parsing/HDF5/H5Support.hpp"

using namespace complex;

H5::DatasetReader::DatasetReader()
{
}

H5::DatasetReader::DatasetReader(H5::IdType parentId, const std::string& dataName)
: ObjectReader(parentId, dataName)
{
  m_DatasetId = H5Dopen(parentId, dataName.c_str(), H5P_DEFAULT);
}

H5::DatasetReader::~DatasetReader() = default;

void H5::DatasetReader::closeHdf5()
{
  H5Dclose(m_DatasetId);
  m_DatasetId = 0;
}

H5::IdType H5::DatasetReader::getId() const
{
  return m_DatasetId;
}

H5::IdType H5::DatasetReader::getDataspaceId() const
{
  return H5Dget_space(getId());
}

H5::Type H5::DatasetReader::getType() const
{
  return H5::getTypeFromId(getTypeId());
}

H5::IdType H5::DatasetReader::getTypeId() const
{
  return H5Dget_type(getId());
}

size_t H5::DatasetReader::getTypeSize() const
{
  return H5Tget_size(getTypeId());
}

size_t H5::DatasetReader::getNumElements() const
{
  auto typeId = getTypeId();
  size_t typeSize = H5Tget_size(typeId);
  std::vector<hsize_t> dims;
  auto dataspaceId = getDataspaceId();
  if(dataspaceId >= 0)
  {
    if(getType() == Type::string)
    {
      size_t rank = 1;
      dims.resize(rank);
      dims[0] = typeSize;
    }
    else
    {
      size_t rank = H5Sget_simple_extent_ndims(dataspaceId);
      std::vector<hsize_t> hdims(rank, 0);
      /* Get dimensions */
      auto error = H5Sget_simple_extent_dims(dataspaceId, hdims.data(), nullptr);
      if(error < 0)
      {
        std::cout << "Error Getting Attribute dims" << std::endl;
        return 0;
      }
      // Copy the dimensions into the dims vector
      dims.clear(); // Erase everything in the Vector
      dims.resize(rank);
      std::copy(hdims.cbegin(), hdims.cend(), dims.begin());
    }
  }

  hsize_t numElements = std::accumulate(dims.cbegin(), dims.cend(), static_cast<hsize_t>(1), std::multiplies<>());
  return numElements;
}

std::string H5::DatasetReader::readAsString() const
{
  if(!isValid())
  {
    return "";
  }

  std::string data;

  // Test if the string is variable length
  const hid_t typeID = H5Aget_type(getId());
  const htri_t isVariableString = H5Tis_variable_str(typeID);

  if(isVariableString == 1)
  {
    std::vector<std::string> strings;
    auto stringVec = readAsVectorOfStrings();
    if(stringVec.size() > 1 && !stringVec.empty())
    {
      std::cout << "Error Reading string dataset. There were multiple strings and the program asked for a single string." << std::endl;
      return "";
    }
    else
    {
      data.assign(strings[0]);
    }
  }
  else
  {
    hsize_t size = H5Dget_storage_size(getId());
    std::vector<char> buffer(static_cast<size_t>(size + 1), 0x00); // Allocate and Zero and array
    auto error = H5Dread(getId(), typeID, H5S_ALL, H5S_ALL, H5P_DEFAULT, buffer.data());
    if(error < 0)
    {
      std::cout << "Error Reading string dataset." << std::endl;
      return "";
    }
    else
    {
      data.append(buffer.data()); // Append the string to the given string
    }
  }

  return data;
}

std::vector<std::string> H5::DatasetReader::readAsVectorOfStrings() const
{
  if(!isValid())
  {
    return {};
  }

  std::vector<std::string> strings;

  hid_t typeID = getTypeId();
  if(typeID >= 0)
  {
    hsize_t dims[1] = {0};
    /*
     * Get dataspace and allocate memory for read buffer.
     */
    hid_t dataspaceID = getDataspaceId();
    int nDims = H5Sget_simple_extent_dims(dataspaceID, dims, nullptr);
    if(nDims != 1)
    {
      // H5Sclose(dataspaceID);
      // H5Tclose(typeID);
      std::cout << "H5DatasetReader.cpp::readVectorOfStrings(" << __LINE__ << ") Number of dims should be 1 but it was " << nDims << ". Returning early. Is your data file correct?" << std::endl;
      return {};
    }

    std::vector<char*> rData(dims[0], nullptr);

    /*
     * Create the memory datatype.
     */
    hid_t memtype = H5Tcopy(H5T_C_S1);
    herr_t status = H5Tset_size(memtype, H5T_VARIABLE);

    H5T_cset_t characterSet = H5Tget_cset(typeID);
    status = H5Tset_cset(memtype, characterSet);

    /*
     * Read the data.
     */
    status = H5Dread(getId(), memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, rData.data());
    if(status < 0)
    {
      status = H5Dvlen_reclaim(memtype, dataspaceID, H5P_DEFAULT, rData.data());
      // H5Sclose(dataspaceID);
      // H5Tclose(typeID);
      H5Tclose(memtype);
      std::cout << "H5DatasetReader.cpp::readVectorOfStrings(" << __LINE__ << ") Error reading Dataset at locationID (" << getParentId() << ") with object name (" << getName() << ")" << std::endl;
      return {};
    }
    /*
     * copy the data into the vector of strings
     */
    strings.resize(dims[0]);
    for(size_t i = 0; i < dims[0]; i++)
    {
      // printf("%s[%d]: %s\n", "VlenStrings", i, rData[i].p);
      strings[i] = std::string(rData[i]);
    }
    /*
     * Close and release resources.  Note that H5Dvlen_reclaim works
     * for variable-length strings as well as variable-length arrays.
     * Also note that we must still free the array of pointers stored
     * in rData, as H5Tvlen_reclaim only frees the data these point to.
     */
    status = H5Dvlen_reclaim(memtype, dataspaceID, H5P_DEFAULT, rData.data());
  }

  return strings;
}

template <typename T>
std::vector<T> H5::DatasetReader::readAsVector() const
{
  if(!isValid())
  {
    return {};
  }

  hid_t dataType = H5::Support::HdfTypeForPrimitive<T>();
  if(dataType == -1)
  {
    return {};
  }

  std::vector<T> data;

  auto spaceId = getDataspaceId();
  if(spaceId > 0)
  {
    int32_t rank = H5Sget_simple_extent_ndims(spaceId);
    if(rank > 0)
    {
      std::vector<hsize_t> dims(rank, 0); // Allocate enough room for the dims
      auto error = H5Sget_simple_extent_dims(spaceId, dims.data(), nullptr);
      hsize_t numElements = std::accumulate(dims.cbegin(), dims.cend(), static_cast<hsize_t>(1), std::multiplies<>());
      // std::cout << "NumElements: " << numElements << std::endl;
      // Resize the vector
      data.resize(numElements);
      error = H5Dread(getId(), dataType, H5S_ALL, H5S_ALL, H5P_DEFAULT, data.data());
      if(error < 0)
      {
        std::cout << "Error Reading Data.'" << getName() << "'" << std::endl;
      }
    }
    // auto error = H5Sclose(spaceId);
    // if(error < 0)
    //{
    //  std::cout << "Error Closing Data Space" << std::endl;
    //}
  }
  else
  {
    std::cout << "Error Opening SpaceID" << std::endl;
  }

  return data;
}

// declare readAsVector
template std::vector<int8_t> H5::DatasetReader::readAsVector<int8_t>() const;
template std::vector<int16_t> H5::DatasetReader::readAsVector<int16_t>() const;
template std::vector<int32_t> H5::DatasetReader::readAsVector<int32_t>() const;
template std::vector<int64_t> H5::DatasetReader::readAsVector<int64_t>() const;
template std::vector<uint8_t> H5::DatasetReader::readAsVector<uint8_t>() const;
template std::vector<uint16_t> H5::DatasetReader::readAsVector<uint16_t>() const;
template std::vector<uint32_t> H5::DatasetReader::readAsVector<uint32_t>() const;
template std::vector<uint64_t> H5::DatasetReader::readAsVector<uint64_t>() const;
template std::vector<float> H5::DatasetReader::readAsVector<float>() const;
template std::vector<double> H5::DatasetReader::readAsVector<double>() const;