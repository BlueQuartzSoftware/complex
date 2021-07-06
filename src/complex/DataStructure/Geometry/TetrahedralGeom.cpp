#include "TetrahedralGeom.hpp"

#include "complex/DataStructure/DataStructure.hpp"
#include "complex/Utilities/GeometryHelpers.hpp"

using namespace complex;

TetrahedralGeom::TetrahedralGeom(DataStructure* ds, const std::string& name)
: AbstractGeometry3D(ds, name)
{
}

TetrahedralGeom::TetrahedralGeom(DataStructure* ds, const std::string& name, size_t numTets, const std::shared_ptr<SharedVertexList>& vertices, bool allocate)
: AbstractGeometry3D(ds, name)
{
}

TetrahedralGeom::TetrahedralGeom(DataStructure* ds, const std::string& name, const std::shared_ptr<SharedTetList>& tets, const std::shared_ptr<SharedVertexList>& vertices)
: AbstractGeometry3D(ds, name)
{
}

TetrahedralGeom::TetrahedralGeom(const TetrahedralGeom& other)
: AbstractGeometry3D(other)
, m_VertexListId(other.m_VertexListId)
, m_EdgeListId(other.m_EdgeListId)
, m_UnsharedEdgeListId(other.m_UnsharedEdgeListId)
, m_TriListId(other.m_TriListId)
, m_UnsharedTriListId(other.m_UnsharedTriListId)
, m_TetListId(other.m_TetListId)
, m_TetsContainingVertId(other.m_TetsContainingVertId)
, m_TetNeighborsId(other.m_TetNeighborsId)
, m_TetCentroidsId(other.m_TetCentroidsId)
, m_TetSizesId(other.m_TetSizesId)
{
}

TetrahedralGeom::TetrahedralGeom(TetrahedralGeom&& other) noexcept
: AbstractGeometry3D(std::move(other))
, m_VertexListId(std::move(other.m_VertexListId))
, m_EdgeListId(std::move(other.m_EdgeListId))
, m_UnsharedEdgeListId(std::move(other.m_UnsharedEdgeListId))
, m_TriListId(std::move(other.m_TriListId))
, m_UnsharedTriListId(std::move(other.m_UnsharedTriListId))
, m_TetListId(std::move(other.m_TetListId))
, m_TetsContainingVertId(std::move(other.m_TetsContainingVertId))
, m_TetNeighborsId(std::move(other.m_TetNeighborsId))
, m_TetCentroidsId(std::move(other.m_TetCentroidsId))
, m_TetSizesId(std::move(other.m_TetSizesId))
{
}

TetrahedralGeom::~TetrahedralGeom() = default;

DataObject* TetrahedralGeom::shallowCopy()
{
  return new TetrahedralGeom(*this);
}

DataObject* TetrahedralGeom::deepCopy()
{
  throw std::exception();
}

std::string TetrahedralGeom::getGeometryTypeAsString() const
{
  return "TetrahedralGeom";
}

void TetrahedralGeom::resizeTriList(size_t numTris)
{
  getTriangles()->getDataStore()->resizeTuples(numTris);
}

void TetrahedralGeom::setTriangles(const SharedTriList* triangles)
{
  if(!triangles)
  {
    m_TriListId.reset();
    return;
  }
  m_TriListId = triangles->getId();
}

AbstractGeometry::SharedTriList* TetrahedralGeom::getTriangles()
{
  return dynamic_cast<SharedTriList*>(getDataStructure()->getData(m_TriListId));
}

const AbstractGeometry::SharedTriList* TetrahedralGeom::getTriangles() const
{
  return dynamic_cast<const SharedTriList*>(getDataStructure()->getData(m_TriListId));
}

void TetrahedralGeom::setVertsAtTri(size_t triId, size_t verts[3])
{
  auto triangles = getTriangles();
  if(!triangles)
  {
    return;
  }

  for(size_t i = 0; i < 3; i++)
  {
    (*triangles)[triId * 3 + i] = verts[i];
  }
}

void TetrahedralGeom::getVertsAtTri(size_t triId, size_t verts[3]) const
{
  auto triangles = getTriangles();
  if(!triangles)
  {
    return;
  }

  for(size_t i = 0; i < 3; i++)
  {
    verts[i] = triangles->at(triId * 3 + i);
  }
}

size_t TetrahedralGeom::getNumberOfTris() const
{
  return getTriangles()->getTupleCount();
}

void TetrahedralGeom::resizeTetList(size_t numTets)
{
  getTriangles()->getDataStore()->resizeTuples(numTets);
}

void TetrahedralGeom::setTetrahedra(const SharedTetList* tets)
{
  if(!tets)
  {
    m_TetListId.reset();
    return;
  }
  m_TetListId = tets->getId();
}

AbstractGeometry::SharedTetList* TetrahedralGeom::getTetrahedra()
{
  return dynamic_cast<SharedTetList*>(getDataStructure()->getData(m_TetListId));
}

const AbstractGeometry::SharedTetList* TetrahedralGeom::getTetrahedra() const
{
  return dynamic_cast<const SharedTetList*>(getDataStructure()->getData(m_TetListId));
}

void TetrahedralGeom::setVertsAtTet(size_t tetId, size_t verts[4])
{
  auto tets = getTetrahedra();
  if(!tets)
  {
    return;
  }
  for(size_t i = 0; i < 4; i++)
  {
    (*tets)[tetId * 4 + i] = verts[i];
  }
}

void TetrahedralGeom::getVertsAtTet(size_t tetId, size_t verts[4]) const
{
  auto tets = getTetrahedra();
  if(!tets)
  {
    return;
  }
  for(size_t i = 0; i < 4; i++)
  {
    verts[i] = (*tets)[tetId * 4 + i];
  }
}

void TetrahedralGeom::getVertCoordsAtTet(size_t tetId, complex::Point3D<float>& vert1, complex::Point3D<float>& vert2, complex::Point3D<float>& vert3, complex::Point3D<float>& vert4) const
{
  if(!getTetrahedra())
  {
    return;
  }
  auto vertices = getVertices();
  if(!vertices)
  {
    return;
  }
  size_t verts[4];
  getVertsAtTet(tetId, verts);
  vert1 = getCoords(verts[0]);
  vert2 = getCoords(verts[1]);
  vert3 = getCoords(verts[2]);
  vert4 = getCoords(verts[3]);
}

size_t TetrahedralGeom::getNumberOfTets() const
{
  auto tets = getTetrahedra();
  if(!tets)
  {
    return 0;
  }
  return tets->getTupleCount();
}

void TetrahedralGeom::initializeWithZeros()
{
  auto vertices = getVertices();
  if(vertices)
  {
    vertices->getDataStore()->fill(0.0f);
  }
  auto tets = getTetrahedra();
  if(tets)
  {
    tets->getDataStore()->fill(0);
  }
}

size_t TetrahedralGeom::getNumberOfElements() const
{
  return getNumberOfTets();
}

AbstractGeometry::StatusCode TetrahedralGeom::findElementSizes()
{
  auto dataStore = new DataStore<float>(1, getNumberOfTets());
  auto tetSizes = getDataStructure()->createDataArray<float>("Tet Volumes", dataStore, getId());
  GeometryHelpers::Topology::FindTetVolumes<size_t>(getTetrahedra(), getVertices(), tetSizes);
  if(tetSizes == nullptr)
  {
    m_TetSizesId.reset();
    return -1;
  }
  m_TetSizesId = tetSizes->getId();
  return 1;
}

const FloatArray* TetrahedralGeom::getElementSizes() const
{
  return dynamic_cast<const FloatArray*>(getDataStructure()->getData(m_TetSizesId));
}

void TetrahedralGeom::deleteElementSizes()
{
  getDataStructure()->removeData(m_TetSizesId);
  m_TetSizesId.reset();
}

AbstractGeometry::StatusCode TetrahedralGeom::findElementsContainingVert()
{
  auto tetsContainingVert = getDataStructure()->createDynamicList<uint16_t, MeshIndexType>("Elements Containing Vert", getId());
  GeometryHelpers::Connectivity::FindElementsContainingVert<uint16_t, MeshIndexType>(getTetrahedra(), tetsContainingVert, getNumberOfVertices());
  if(tetsContainingVert == nullptr)
  {
    m_TetsContainingVertId.reset();
    return -1;
  }
  m_TetsContainingVertId = tetsContainingVert->getId();
  return 1;
}

const AbstractGeometry::ElementDynamicList* TetrahedralGeom::getElementsContainingVert() const
{
  return dynamic_cast<const ElementDynamicList*>(getDataStructure()->getData(m_TetsContainingVertId));
}

void TetrahedralGeom::deleteElementsContainingVert()
{
  getDataStructure()->removeData(m_TetsContainingVertId);
  m_TetsContainingVertId.reset();
}

AbstractGeometry::StatusCode TetrahedralGeom::findElementNeighbors()
{
  StatusCode err = 0;
  if(getElementsContainingVert() == nullptr)
  {
    err = findElementsContainingVert();
    if(err < 0)
    {
      return err;
    }
  }
  auto tetNeighbors = getDataStructure()->createDynamicList<uint16_t, MeshIndexType>("Tet Neighbors", getId());
  err = GeometryHelpers::Connectivity::FindElementNeighbors<uint16_t, MeshIndexType>(getTetrahedra(), getElementsContainingVert(), tetNeighbors, AbstractGeometry::Type::Tetrahedral);
  if(tetNeighbors == nullptr)
  {
    m_TetNeighborsId.reset();
    return -1;
  }
  m_TetNeighborsId = tetNeighbors->getId();
  return err;
}

const AbstractGeometry::ElementDynamicList* TetrahedralGeom::getElementNeighbors() const
{
  return dynamic_cast<const ElementDynamicList*>(getDataStructure()->getData(m_TetNeighborsId));
}

void TetrahedralGeom::deleteElementNeighbors()
{
  getDataStructure()->removeData(m_TetNeighborsId);
  m_TetNeighborsId.reset();
}

AbstractGeometry::StatusCode TetrahedralGeom::findElementCentroids()
{
  auto dataStore = new DataStore<float>(3, getNumberOfTets());
  auto tetCentroids = getDataStructure()->createDataArray<float>("Tet Centroids", dataStore, getId());
  GeometryHelpers::Topology::FindElementCentroids<size_t>(getTetrahedra(), getVertices(), tetCentroids);
  if(tetCentroids == nullptr)
  {
    m_TetCentroidsId.reset();
    return -1;
  }
  m_TetCentroidsId = tetCentroids->getId();
  return 1;
}

const FloatArray* TetrahedralGeom::getElementCentroids() const
{
  return dynamic_cast<const FloatArray*>(getDataStructure()->getData(m_TetCentroidsId));
}

void TetrahedralGeom::deleteElementCentroids()
{
  getDataStructure()->removeData(m_TetCentroidsId);
  m_TetCentroidsId.reset();
}

complex::Point3D<double> TetrahedralGeom::getParametricCenter() const
{
  return {0.25, 0.25, 0.25};
}

void TetrahedralGeom::getShapeFunctions(const complex::Point3D<double>& pCoords, double* shape) const
{
  (void)pCoords;

  // r-derivatives
  shape[0] = -1.0;
  shape[1] = 1.0;
  shape[2] = 0.0;
  shape[3] = 0.0;

  // s-derivatives
  shape[4] = -1.0;
  shape[5] = 0.0;
  shape[6] = 1.0;
  shape[7] = 0.0;

  // t-derivatives
  shape[8] = -1.0;
  shape[9] = 0.0;
  shape[10] = 0.0;
  shape[11] = 1.0;
}

void TetrahedralGeom::findDerivatives(DoubleArray* field, DoubleArray* derivatives, Observable* observable) const
{
  throw std::exception();
}

complex::TooltipGenerator TetrahedralGeom::getTooltipGenerator() const
{
  TooltipGenerator toolTipGen;

  toolTipGen.addTitle("Geometry Info");
  toolTipGen.addValue("Type", "TetrahedralGeom");
  toolTipGen.addValue("Units", LengthUnitToString(getUnits()));
  toolTipGen.addValue("Number of Tetrahedra", std::to_string(getNumberOfTets()));
  toolTipGen.addValue("Number of Vertices", std::to_string(getNumberOfVertices()));

  return toolTipGen;
}

void TetrahedralGeom::resizeVertexList(size_t numVertices)
{
  getVertices()->getDataStore()->resizeTuples(numVertices);
}

void TetrahedralGeom::setVertices(const SharedVertexList* vertices)
{
  if(!vertices)
  {
    m_VertexListId.reset();
    return;
  }
  m_VertexListId = vertices->getId();
}

AbstractGeometry::SharedVertexList* TetrahedralGeom::getVertices()
{
  return dynamic_cast<SharedVertexList*>(getDataStructure()->getData(m_VertexListId));
}

const AbstractGeometry::SharedVertexList* TetrahedralGeom::getVertices() const
{
  return dynamic_cast<const SharedVertexList*>(getDataStructure()->getData(m_VertexListId));
}

void TetrahedralGeom::setCoords(size_t vertId, const complex::Point3D<float>& coords)
{
  auto vertices = getVertices();
  if(!vertices)
  {
    return;
  }
  const size_t offset = vertId * 3;
  for(size_t i = 0; i < 3; i++)
  {
    (*vertices)[offset + i] = coords[i];
  }
}

complex::Point3D<float> TetrahedralGeom::getCoords(size_t vertId) const
{
  auto vertices = getVertices();
  if(!vertices)
  {
    return {};
  }
  Point3D<float> coords;
  for(size_t i = 0; i < 3; i++)
  {
    coords[i] = vertices->at(vertId * 3 + i);
  }
  return coords;
}

size_t TetrahedralGeom::getNumberOfVertices() const
{
  auto vertices = getVertices();
  if(!vertices)
  {
    return 0;
  }
  return vertices->getTupleCount();
}

void TetrahedralGeom::resizeEdgeList(size_t numEdges)
{
  getEdges()->getDataStore()->resizeTuples(numEdges);
}

AbstractGeometry::SharedEdgeList* TetrahedralGeom::getEdges()
{
  return dynamic_cast<SharedEdgeList*>(getDataStructure()->getData(m_EdgeListId));
}

const AbstractGeometry::SharedEdgeList* TetrahedralGeom::getEdges() const
{
  return dynamic_cast<const SharedEdgeList*>(getDataStructure()->getData(m_EdgeListId));
}

void TetrahedralGeom::setVertsAtEdge(size_t edgeId, size_t verts[2])
{
  auto edges = getEdges();
  if(!edges)
  {
    return;
  }
  (*edges)[edgeId * 2] = verts[0];
  (*edges)[edgeId * 2 + 1] = verts[1];
}

void TetrahedralGeom::getVertsAtEdge(size_t edgeId, size_t verts[2]) const
{
  auto edges = getEdges();
  if(!edges)
  {
    return;
  }
  verts[0] = edges->at(edgeId * 2);
  verts[1] = edges->at(edgeId * 2 + 1);
}

void TetrahedralGeom::getVertCoordsAtEdge(size_t edgeId, complex::Point3D<float>& vert1, complex::Point3D<float>& vert2) const
{
  if(!getEdges())
  {
    return;
  }
  size_t verts[2];
  getVertsAtEdge(edgeId, verts);
  vert1 = getCoords(verts[0]);
  vert2 = getCoords(verts[1]);
}

size_t TetrahedralGeom::getNumberOfEdges() const
{
  auto edges = getEdges();
  if(!edges)
  {
    return 0;
  }
  return edges->getTupleCount();
}

AbstractGeometry::StatusCode TetrahedralGeom::findEdges()
{
  auto edgeList = createSharedEdgeList(0);
  GeometryHelpers::Connectivity::FindTetEdges<size_t>(getTetrahedra(), edgeList);
  if(edgeList == nullptr)
  {
    m_EdgeListId.reset();
    return -1;
  }
  m_EdgeListId = edgeList->getId();
  return 1;
}

void TetrahedralGeom::deleteEdges()
{
  getDataStructure()->removeData(m_EdgeListId);
  m_EdgeListId.reset();
}

AbstractGeometry::StatusCode TetrahedralGeom::findFaces()
{
  auto triList = createSharedTriList(0);
  GeometryHelpers::Connectivity::FindTetFaces<size_t>(getTetrahedra(), triList);
  if(triList == nullptr)
  {
    m_TriListId.reset();
    return -1;
  }
  m_TriListId = triList->getId();
  return 1;
}

void TetrahedralGeom::deleteFaces()
{
  getDataStructure()->removeData(m_TriListId);
  m_TriListId.reset();
}

AbstractGeometry::StatusCode TetrahedralGeom::findUnsharedEdges()
{
  auto dataStore = new DataStore<MeshIndexType>(2, 0);
  auto unsharedEdgeList = getDataStructure()->createDataArray<MeshIndexType>("Unshared Edge List", dataStore, getId());
  GeometryHelpers::Connectivity::FindUnsharedTetEdges<size_t>(getTetrahedra(), unsharedEdgeList);
  if(unsharedEdgeList == nullptr)
  {
    m_UnsharedEdgeListId.reset();
    return -1;
  }
  m_UnsharedEdgeListId = unsharedEdgeList->getId();
  return 1;
}

const AbstractGeometry::SharedEdgeList* TetrahedralGeom::getUnsharedEdges() const
{
  return dynamic_cast<const SharedEdgeList*>(getDataStructure()->getData(m_UnsharedEdgeListId));
}

void TetrahedralGeom::deleteUnsharedEdges()
{
  getDataStructure()->removeData(m_UnsharedEdgeListId);
  m_UnsharedEdgeListId.reset();
}

AbstractGeometry::StatusCode TetrahedralGeom::findUnsharedFaces()
{
  auto dataStore = new DataStore<MeshIndexType>(3, 0);
  auto unsharedTriList = getDataStructure()->createDataArray<MeshIndexType>("Unshared Face List", dataStore, getId());
  GeometryHelpers::Connectivity::FindUnsharedTetFaces<size_t>(getTetrahedra(), unsharedTriList);
  if(unsharedTriList == nullptr)
  {
    m_UnsharedTriListId.reset();
    return -1;
  }
  m_UnsharedTriListId = unsharedTriList->getId();
  return 1;
}

const AbstractGeometry::SharedEdgeList* TetrahedralGeom::getUnsharedFaces() const
{
  return dynamic_cast<const SharedEdgeList*>(getDataStructure()->getData(m_UnsharedTriListId));
}

void TetrahedralGeom::deleteUnsharedFaces()
{
  getDataStructure()->removeData(m_UnsharedTriListId);
  m_UnsharedTriListId.reset();
}

void TetrahedralGeom::setElementSizes(const FloatArray* elementSizes)
{
  if(!elementSizes)
  {
    m_TetSizesId.reset();
    return;
  }
  m_TetSizesId = elementSizes->getId();
}

uint32_t TetrahedralGeom::getXdmfGridType() const
{
  throw std::exception();
}

H5::ErrorType TetrahedralGeom::generateXdmfText(std::ostream& out, const std::string& hdfFileName) const
{
  throw std::exception();
}

H5::ErrorType TetrahedralGeom::readFromXdmfText(std::istream& in, const std::string& hdfFileName)
{
  throw std::exception();
}

void TetrahedralGeom::setElementsContainingVert(const ElementDynamicList* elementsContainingVert)
{
  if(!elementsContainingVert)
  {
    m_TetsContainingVertId.reset();
    return;
  }
  m_TetsContainingVertId = elementsContainingVert->getId();
}

void TetrahedralGeom::setElementNeighbors(const ElementDynamicList* elementNeighbors)
{
  if(!elementNeighbors)
  {
    m_TetNeighborsId.reset();
    return;
  }
  m_TetNeighborsId = elementNeighbors->getId();
}

void TetrahedralGeom::setElementCentroids(const FloatArray* elementCentroids)
{
  if(!elementCentroids)
  {
    m_TetCentroidsId.reset();
    return;
  }
  m_TetCentroidsId = elementCentroids->getId();
}

void TetrahedralGeom::setEdges(const SharedEdgeList* edges)
{
  if(!edges)
  {
    m_EdgeListId.reset();
    return;
  }
  m_EdgeListId = edges->getId();
}

void TetrahedralGeom::setUnsharedEdges(const SharedEdgeList* bEdgeList)
{
  if(!bEdgeList)
  {
    m_UnsharedEdgeListId.reset();
    return;
  }
  m_UnsharedEdgeListId = bEdgeList->getId();
}

void TetrahedralGeom::setUnsharedFaces(const SharedFaceList* bFaceList)
{
  if(!bFaceList)
  {
    m_UnsharedTriListId.reset();
    return;
  }
  m_UnsharedTriListId = bFaceList->getId();
}
