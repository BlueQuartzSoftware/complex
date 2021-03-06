#include "VertexGeom.hpp"

#include <stdexcept>

#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataStore.hpp"
#include "complex/DataStructure/DataStructure.hpp"
#include "complex/Utilities/GeometryHelpers.hpp"
#include "complex/Utilities/Parsing/HDF5/H5Constants.hpp"
#include "complex/Utilities/Parsing/HDF5/H5GroupReader.hpp"
#include "complex/Utilities/Parsing/HDF5/H5GroupWriter.hpp"

using namespace complex;

VertexGeom::VertexGeom(DataStructure& ds, std::string name)
: AbstractGeometry(ds, std::move(name))
{
}

VertexGeom::VertexGeom(DataStructure& ds, std::string name, IdType importId)
: AbstractGeometry(ds, std::move(name), importId)
{
}

// VertexGeom::VertexGeom(DataStructure& ds, std::string name, usize numVertices, bool allocate)
//: AbstractGeometry(ds, std::move(name))
//{
//}
//
// VertexGeom::VertexGeom(DataStructure& ds, std::string name, const SharedVertexList* vertices)
//: AbstractGeometry(ds, std::move(name))
//{
//}

VertexGeom::VertexGeom(const VertexGeom& other)
: AbstractGeometry(other)
, m_VertexListId(other.m_VertexListId)
, m_VertexSizesId(other.m_VertexSizesId)
{
}

VertexGeom::VertexGeom(VertexGeom&& other) noexcept
: AbstractGeometry(std::move(other))
, m_VertexListId(std::move(other.m_VertexListId))
, m_VertexSizesId(std::move(other.m_VertexSizesId))
{
}

VertexGeom::~VertexGeom() = default;
DataObject::Type VertexGeom::getDataObjectType() const
{
  return DataObject::Type::VertexGeom;
}

VertexGeom* VertexGeom::Create(DataStructure& ds, std::string name, const std::optional<IdType>& parentId)
{
  auto data = std::shared_ptr<VertexGeom>(new VertexGeom(ds, std::move(name)));
  if(!AttemptToAddObject(ds, data, parentId))
  {
    return nullptr;
  }
  return data.get();
}

VertexGeom* VertexGeom::Import(DataStructure& ds, std::string name, IdType importId, const std::optional<IdType>& parentId)
{
  auto data = std::shared_ptr<VertexGeom>(new VertexGeom(ds, std::move(name), importId));
  if(!AttemptToAddObject(ds, data, parentId))
  {
    return nullptr;
  }
  return data.get();
}

AbstractGeometry::Type VertexGeom::getGeomType() const
{
  return AbstractGeometry::Type::Vertex;
}

std::string VertexGeom::getTypeName() const
{
  return getGeometryTypeAsString();
}

DataObject* VertexGeom::shallowCopy()
{
  return new VertexGeom(*this);
}

DataObject* VertexGeom::deepCopy()
{
  return new VertexGeom(*this);
}

std::string VertexGeom::getGeometryTypeAsString() const
{
  return "VertexGeom";
}

void VertexGeom::initializeWithZeros()
{
  AbstractGeometry::SharedVertexList* vertices = getVertices();
  if(vertices == nullptr)
  {
    return;
  }
  std::fill(vertices->getDataStore()->begin(), vertices->getDataStore()->end(), 0.0f);
}

void VertexGeom::resizeVertexList(usize newNumVertices)
{
  AbstractGeometry::SharedVertexList* vertices = getVertices();
  if(vertices == nullptr)
  {
    auto dataStore = std::make_unique<DataStore<float32>>(std::vector<usize>{newNumVertices}, std::vector<usize>{3}, 0.0f);
    DataStructure* ds = getDataStructure();
    vertices = AbstractGeometry::SharedVertexList::Create(*ds, "Vertices", std::move(dataStore));
    setVertices(vertices);
  }
  else
  {
    vertices->getDataStore()->reshapeTuples({newNumVertices});
  }
}

void VertexGeom::setVertices(const SharedVertexList* vertices)
{
  if(vertices == nullptr)
  {
    m_VertexListId.reset();
    return;
  }
  m_VertexListId = vertices->getId();
}

AbstractGeometry::SharedVertexList* VertexGeom::getVertices()
{
  auto dataStructure = getDataStructure();
  if(!dataStructure)
  {
    return nullptr;
  }
  return dataStructure->getDataAs<SharedVertexList>(m_VertexListId);
}

const AbstractGeometry::SharedVertexList* VertexGeom::getVertices() const
{
  auto dataStructure = getDataStructure();
  if(!dataStructure)
  {
    return nullptr;
  }
  return dataStructure->getDataAs<SharedVertexList>(m_VertexListId);
}

std::optional<DataObject::IdType> VertexGeom::getVerticesId() const
{
  return m_VertexListId;
}

Point3D<float32> VertexGeom::getCoords(usize vertId) const
{
  auto vertices = getVertices();
  if(vertices == nullptr)
  {
    return {};
  }
  const usize offset = vertId * 3;
  Point3D<float32> coords;
  for(usize i = 0; i < 3; i++)
  {
    coords[i] = (*vertices)[offset + i];
  }
  return coords;
}

void VertexGeom::setCoords(usize vertId, const Point3D<float32>& coords)
{
  auto vertices = getVertices();
  if(vertices == nullptr)
  {
    return;
  }
  const usize offset = vertId * 3;
  for(usize i = 0; i < 3; i++)
  {
    (*vertices)[offset + i] = coords[i];
  }
}

usize VertexGeom::getNumberOfVertices() const
{
  auto vertices = getVertices();
  if(vertices == nullptr)
  {
    return 0;
  }
  return vertices->getNumberOfTuples();
}

usize VertexGeom::getNumberOfElements() const
{
  return getNumberOfVertices();
}

AbstractGeometry::StatusCode VertexGeom::findElementSizes()
{
  // Vertices are 0-dimensional (they have no getSize),
  // so simply splat 0 over the sizes array
  auto dataStore = std::make_unique<DataStore<float32>>(std::vector<usize>{getNumberOfElements()}, std::vector<usize>{1}, 0.0f);

  Float32Array* vertexSizes = DataArray<float32>::Create(*getDataStructure(), "Voxel Sizes", std::move(dataStore), getId());
  m_VertexSizesId = vertexSizes->getId();
  return 1;
}

const Float32Array* VertexGeom::getElementSizes() const
{
  return dynamic_cast<const Float32Array*>(getDataStructure()->getData(m_VertexSizesId));
}

void VertexGeom::deleteElementSizes()
{
  getDataStructure()->removeData(m_VertexSizesId);
  m_VertexSizesId.reset();
}

AbstractGeometry::StatusCode VertexGeom::findElementsContainingVert()
{
  return -1;
}

const AbstractGeometry::ElementDynamicList* VertexGeom::getElementsContainingVert() const
{
  return nullptr;
}

void VertexGeom::deleteElementsContainingVert()
{
}

AbstractGeometry::StatusCode VertexGeom::findElementNeighbors()
{
  return -1;
}

const AbstractGeometry::ElementDynamicList* VertexGeom::getElementNeighbors() const
{
  return nullptr;
}

void VertexGeom::deleteElementNeighbors()
{
}

AbstractGeometry::StatusCode VertexGeom::findElementCentroids()
{
  return -1;
}

const Float32Array* VertexGeom::getElementCentroids() const
{
  return nullptr;
}

void VertexGeom::deleteElementCentroids()
{
}

complex::Point3D<float64> VertexGeom::getParametricCenter() const
{
  return {0.0, 0.0, 0.0};
}

void VertexGeom::getShapeFunctions(const complex::Point3D<float64>& pCoords, double* shape) const
{
  (void)pCoords;

  shape[0] = 0.0;
  shape[1] = 0.0;
  shape[2] = 0.0;
}

void VertexGeom::findDerivatives(Float64Array* field, Float64Array* derivatives, Observable* observable) const
{
  // The exterior derivative of a point source is zero,
  // so simply splat 0 over the derivatives array
  derivatives->getDataStore()->fill(0.0);
}

std::string VertexGeom::getInfoString(complex::InfoStringFormat format) const
{
  if(format == InfoStringFormat::HtmlFormat)
  {
    return getTooltipGenerator().generateHTML();
  }

  return "";
}

complex::TooltipGenerator VertexGeom::getTooltipGenerator() const
{
  TooltipGenerator toolTipGen;
  toolTipGen.addTitle("Geometry Info");
  toolTipGen.addValue("Type", "VertexGeom");
  toolTipGen.addValue("Units", LengthUnitToString(getUnits()));
  toolTipGen.addValue("Number of Vertices", std::to_string(getNumberOfVertices()));

  return toolTipGen;
}

uint32 VertexGeom::getXdmfGridType() const
{
  throw std::runtime_error("");
}

void VertexGeom::setElementsContainingVert(const ElementDynamicList* elementsContainingVert)
{
}

void VertexGeom::setElementNeighbors(const ElementDynamicList* elementNeighbors)
{
}

void VertexGeom::setElementCentroids(const Float32Array* elementCentroids)
{
}

void VertexGeom::setElementSizes(const Float32Array* elementSizes)
{
  if(!elementSizes)
  {
    m_VertexSizesId.reset();
    return;
  }
  m_VertexSizesId = elementSizes->getId();
}

H5::ErrorType VertexGeom::readHdf5(H5::DataStructureReader& dataStructureReader, const H5::GroupReader& groupReader, bool preflight)
{
  m_VertexListId = ReadH5DataId(groupReader, H5Constants::k_VertexListTag);
  m_VertexSizesId = ReadH5DataId(groupReader, H5Constants::k_VertexSizesTag);

  return BaseGroup::readHdf5(dataStructureReader, groupReader, preflight);
}

H5::ErrorType VertexGeom::writeHdf5(H5::DataStructureWriter& dataStructureWriter, H5::GroupWriter& parentGroupWriter, bool importable) const
{
  auto groupWriter = parentGroupWriter.createGroupWriter(getName());
  herr_t errorCode = writeH5ObjectAttributes(dataStructureWriter, groupWriter, importable);
  if(errorCode < 0)
  {
    return errorCode;
  }

  // Write DataObject IDs
  errorCode = WriteH5DataId(groupWriter, m_VertexListId, H5Constants::k_VertexListTag);
  if(errorCode < 0)
  {
    return errorCode;
  }

  errorCode = WriteH5DataId(groupWriter, m_VertexSizesId, H5Constants::k_VertexSizesTag);
  if(errorCode < 0)
  {
    return errorCode;
  }

  return getDataMap().writeH5Group(dataStructureWriter, groupWriter);
}

void VertexGeom::checkUpdatedIdsImpl(const std::vector<std::pair<IdType, IdType>>& updatedIds)
{
  AbstractGeometry::checkUpdatedIdsImpl(updatedIds);

  for(const auto& updatedId : updatedIds)
  {
    if(m_VertexListId == updatedId.first)
    {
      m_VertexListId = updatedId.second;
    }

    if(m_VertexSizesId == updatedId.first)
    {
      m_VertexSizesId = updatedId.second;
    }
  }
}
