#include "AbstractGeometry2D.hpp"
#include "complex/DataStructure/DataStructure.hpp"
#include "complex/Utilities/Parsing/HDF5/H5Constants.hpp"
#include "complex/Utilities/Parsing/HDF5/H5GroupReader.hpp"

using namespace complex;

AbstractGeometry2D::AbstractGeometry2D(DataStructure& ds, std::string name)
: AbstractGeometry(ds, std::move(name))
{
}

AbstractGeometry2D::AbstractGeometry2D(DataStructure& ds, std::string name, IdType importId)
: AbstractGeometry(ds, std::move(name), importId)
{
}

AbstractGeometry2D::AbstractGeometry2D(const AbstractGeometry2D& other)
: AbstractGeometry(other)
, m_VertexListId(other.m_VertexListId)
, m_EdgeListId(other.m_EdgeListId)
, m_UnsharedEdgeListId(other.m_UnsharedEdgeListId)
{
}

AbstractGeometry2D::AbstractGeometry2D(AbstractGeometry2D&& other) noexcept
: AbstractGeometry(std::move(other))
, m_VertexListId(other.m_VertexListId)
, m_EdgeListId(other.m_EdgeListId)
, m_UnsharedEdgeListId(other.m_UnsharedEdgeListId)
{
}

AbstractGeometry2D::~AbstractGeometry2D() = default;
DataObject::Type AbstractGeometry2D::getDataObjectType() const
{
  return DataObject::Type::AbstractGeometry2D;
}

void AbstractGeometry2D::resizeVertexList(usize numVertices)
{
  auto* vertices = getVertices();
  if(vertices == nullptr)
  {
    return;
  }
  vertices->getDataStore()->reshapeTuples({numVertices});
}

void AbstractGeometry2D::setVertices(const SharedVertexList* vertices)
{
  if(vertices == nullptr)
  {
    m_VertexListId.reset();
    return;
  }
  m_VertexListId = vertices->getId();
}

AbstractGeometry::SharedVertexList* AbstractGeometry2D::getVertices()
{
  DataObject* data = getDataStructure()->getData(m_VertexListId);
  return dynamic_cast<SharedVertexList*>(data);
}

const AbstractGeometry::SharedVertexList* AbstractGeometry2D::getVertices() const
{
  const DataObject* data = getDataStructure()->getData(m_VertexListId);
  return dynamic_cast<const SharedVertexList*>(data);
}

std::optional<DataObject::IdType> AbstractGeometry2D::getVerticesId() const
{
  return m_VertexListId;
}

AbstractGeometry::SharedEdgeList* AbstractGeometry2D::getEdges()
{
  return dynamic_cast<SharedEdgeList*>(getDataStructure()->getData(m_EdgeListId));
}

const AbstractGeometry::SharedEdgeList* AbstractGeometry2D::getEdges() const
{
  return dynamic_cast<const SharedEdgeList*>(getDataStructure()->getData(m_EdgeListId));
}

std::optional<DataObject::IdType> AbstractGeometry2D::getEdgesId() const
{
  return m_EdgeListId;
}

usize AbstractGeometry2D::getNumberOfEdges() const
{
  const SharedEdgeList* edges = getEdges();
  if(!edges)
  {
    return 0;
  }
  return edges->getNumberOfTuples();
}

void AbstractGeometry2D::setVertsAtEdge(usize edgeId, const usize verts[2])
{
  SharedEdgeList* edges = getEdges();
  if(!edges)
  {
    return;
  }
  (*edges)[edgeId * 2] = verts[0];
  (*edges)[edgeId * 2 + 1] = verts[1];
}

void AbstractGeometry2D::getVertsAtEdge(usize edgeId, usize verts[2]) const
{
  const SharedEdgeList* edges = getEdges();
  if(!edges)
  {
    return;
  }
  verts[0] = edges->at(edgeId * 2);
  verts[1] = edges->at(edgeId * 2 + 1);
}

void AbstractGeometry2D::deleteEdges()
{
  getDataStructure()->removeData(m_EdgeListId);
  m_EdgeListId.reset();
}

const AbstractGeometry::SharedEdgeList* AbstractGeometry2D::getUnsharedEdges() const
{
  return dynamic_cast<const SharedEdgeList*>(getDataStructure()->getData(m_UnsharedEdgeListId));
}

void AbstractGeometry2D::deleteUnsharedEdges()
{
  getDataStructure()->removeData(m_UnsharedEdgeListId);
  m_UnsharedEdgeListId.reset();
}

void AbstractGeometry2D::setEdges(const SharedEdgeList* edges)
{
  if(!edges)
  {
    m_EdgeListId.reset();
    return;
  }
  m_EdgeListId = edges->getId();
}

void AbstractGeometry2D::setUnsharedEdges(const SharedEdgeList* bEdgeList)
{
  if(!bEdgeList)
  {
    m_UnsharedEdgeListId.reset();
    return;
  }
  m_UnsharedEdgeListId = bEdgeList->getId();
}

H5::ErrorType AbstractGeometry2D::readHdf5(H5::DataStructureReader& dataStructureReader, const H5::GroupReader& groupReader, bool preflight)
{
  m_VertexListId = ReadH5DataId(groupReader, H5Constants::k_VertexListTag);
  m_EdgeListId = ReadH5DataId(groupReader, H5Constants::k_EdgeListTag);
  m_UnsharedEdgeListId = ReadH5DataId(groupReader, H5Constants::k_UnsharedEdgeListTag);

  return BaseGroup::readHdf5(dataStructureReader, groupReader, preflight);
}

H5::ErrorType AbstractGeometry2D::writeHdf5(H5::DataStructureWriter& dataStructureWriter, H5::GroupWriter& parentGroupWriter, bool importable) const
{
  auto groupWriter = parentGroupWriter.createGroupWriter(getName());
  auto errorCode = writeH5ObjectAttributes(dataStructureWriter, groupWriter, importable);
  if(errorCode < 0)
  {
    return errorCode;
  }
  errorCode = WriteH5DataId(groupWriter, m_VertexListId, H5Constants::k_VertexListTag);
  if(errorCode < 0)
  {
    return errorCode;
  }
  errorCode = WriteH5DataId(groupWriter, m_EdgeListId, H5Constants::k_EdgeListTag);
  if(errorCode < 0)
  {
    return errorCode;
  }
  errorCode = WriteH5DataId(groupWriter, m_UnsharedEdgeListId, H5Constants::k_UnsharedEdgeListTag);
  if(errorCode < 0)
  {
    return errorCode;
  }

  return errorCode;
}

void AbstractGeometry2D::checkUpdatedIdsImpl(const std::vector<std::pair<IdType, IdType>>& updatedIds)
{
  BaseGroup::checkUpdatedIdsImpl(updatedIds);

  for(const auto& updatedId : updatedIds)
  {
    if(m_VertexListId == updatedId.first)
    {
      m_VertexListId = updatedId.second;
    }

    if(m_EdgeListId == updatedId.first)
    {
      m_EdgeListId = updatedId.second;
    }

    if(m_UnsharedEdgeListId == updatedId.first)
    {
      m_UnsharedEdgeListId = updatedId.second;
    }
  }
}
