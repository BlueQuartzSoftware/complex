#include "DataGroupFactory.hpp"

#include "complex/DataStructure/DataGroup.hpp"
#include "complex/Utilities/Parsing/HDF5/H5DataStructureReader.hpp"
#include "complex/Utilities/Parsing/HDF5/H5GroupReader.hpp"

using namespace complex;

DataGroupFactory::DataGroupFactory()
: IH5DataFactory()
{
}

DataGroupFactory::~DataGroupFactory() = default;

std::string DataGroupFactory::getDataTypeName() const
{
  return "DataGroup";
}

H5::ErrorType DataGroupFactory::readDataStructureGroup(H5::DataStructureReader& dataStructureReader, const H5::GroupReader& groupReader, const std::optional<DataObject::IdType>& parentId)
{
  auto name = groupReader.getName();
  auto group = DataGroup::Create(dataStructureReader.getDataStructure(), name, parentId);
  return group->readHdf5(dataStructureReader, groupReader);
}

//------------------------------------------------------------------------------
H5::ErrorType DataGroupFactory::readDataStructureDataset(H5::DataStructureReader& dataStructureReader, const H5::DatasetReader& datasetReader, const std::optional<DataObject::IdType>& parentId)
{
  return -1;
}
