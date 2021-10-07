#pragma once

#include "complex/DataStructure/Geometry/AbstractGeometry2D.hpp"
#include "complex/Utilities/TooltipGenerator.hpp"

#include "complex/complex_export.hpp"

namespace complex
{
/**
 * @class TriangleGeom
 * @brief
 */
class COMPLEX_EXPORT TriangleGeom : public AbstractGeometry2D
{
public:
  friend class DataStructure;

  /**
   * @brief
   * @param ds
   * @param name
   * @param parentId = {}
   * @return TriangleGeom*
   */
  static TriangleGeom* Create(DataStructure& ds, const std::string& name, const std::optional<IdType>& parentId = {});

  /**
   * @brief
   * @param other
   */
  TriangleGeom(const TriangleGeom& other);

  /**
   * @brief
   * @param other
   */
  TriangleGeom(TriangleGeom&& other) noexcept;

  virtual ~TriangleGeom();

  /**
   * @brief Returns typename of the DataObject as a std::string.
   * @return std::string
   */
  std::string getTypeName() const override;

  /**
   * @brief
   * @return DataObject*
   */
  DataObject* shallowCopy() override;

  /**
   * @brief
   * @return DataObject*
   */
  DataObject* deepCopy() override;

  /**
   * @brief
   * @return std::string
   */
  std::string getGeometryTypeAsString() const override;

  /**
   * @brief
   * @param newNumTris
   */
  void resizeTriList(usize newNumTris);

  /**
   * @brief
   * @param triangles
   */
  void setTriangles(const SharedTriList* triangles);

  /**
   * @brief
   * @return SharedTriList*
   */
  SharedTriList* getTriangles();

  /**
   * @brief
   * @return const SharedTriList*
   */
  const SharedTriList* getTriangles() const;

  /**
   * @brief
   * @param triId
   * @param verts
   */
  void setVertsAtTri(usize triId, usize verts[3]);

  /**
   * @brief
   * @param triId
   * @param verts
   */
  void getVertsAtTri(usize triId, usize verts[3]) const;

  /**
   * @brief
   * @param triId
   * @param vert1
   * @param vert2
   * @param vert3
   */
  void getVertCoordsAtTri(usize triId, Point3D<float32>& vert1, Point3D<float32>& vert2, Point3D<float32>& vert3) const;

  /**
   * @brief
   * @return usize
   */
  usize getNumberOfTris() const;

  /**
   * @brief
   */
  void initializeWithZeros() override;

  /**
   * @brief
   * @return usize
   */
  usize getNumberOfElements() const override;

  /**
   * @brief
   * @return StatusCode
   */
  StatusCode findElementSizes() override;

  /**
   * @brief
   * @return const Float32Array*
   */
  const Float32Array* getElementSizes() const override;

  /**
   * @brief
   */
  void deleteElementSizes() override;

  /**
   * @brief
   * @return StatusCode
   */
  StatusCode findElementsContainingVert() override;

  /**
   * @brief
   * @return const ElementDynamicList*
   */
  const ElementDynamicList* getElementsContainingVert() const override;

  /**
   * @brief
   */
  void deleteElementsContainingVert() override;

  /**
   * @brief
   * @return StatusCode
   */
  StatusCode findElementNeighbors() override;

  /**
   * @brief
   * @return const ElementDynamicList*
   */
  const ElementDynamicList* getElementNeighbors() const override;

  /**
   * @brief
   */
  void deleteElementNeighbors() override;

  /**
   * @brief
   * @return StatusCode
   */
  StatusCode findElementCentroids() override;

  /**
   * @brief
   * @return const Float32Array*
   */
  const Float32Array* getElementCentroids() const override;

  /**
   * @brief
   */
  void deleteElementCentroids() override;

  /**
   * @brief
   * @param pCoords
   * @return complex::Point3D<float64>
   */
  complex::Point3D<float64> getParametricCenter() const override;

  /**
   * @brief
   * @param pCoords
   * @param shape
   */
  void getShapeFunctions(const complex::Point3D<float64>& pCoords, double* shape) const override;

  /**
   * @brief
   * @param field
   * @param derivatives
   * @param observable
   */
  void findDerivatives(Float64Array* field, Float64Array* derivatives, Observable* observable) const override;

  /**
   * @brief
   * @return complex::TooltipGenerator
   */
  complex::TooltipGenerator getTooltipGenerator() const override;

  /**
   * @brief
   * @param vertId
   * @param coords
   */
  void setCoords(usize vertId, const Point3D<float32>& coords) override;

  /**
   * @brief
   * @param vertId
   */
  Point3D<float32> getCoords(usize vertId) const override;

  /**
   * @brief
   * @return usize
   */
  usize getNumberOfVertices() const override;

  /**
   * @brief
   * @return StatusCode
   */
  StatusCode findEdges() override;

  /**
   * @brief
   * @param newNumEdges
   */
  void resizeEdgeList(usize newNumEdges) override;

  /**
   * @brief
   * @param edgeId
   * @param vert1
   * @param vert2
   */
  void getVertCoordsAtEdge(usize edgeId, Point3D<float32>& vert1, Point3D<float32>& vert2) const override;

  /**
   * @brief
   * @return StatusCode
   */
  StatusCode findUnsharedEdges() override;

  /**
   * @brief
   * @return uint32
   */
  uint32 getXdmfGridType() const override;

  /**
   * @brief Reads values from HDF5
   * @param dataStructureReader
   * @param groupReader
   * @return H5::ErrorType
   */
  H5::ErrorType readHdf5(H5::DataStructureReader& dataStructureReader, const H5::GroupReader& groupReader) override;

  /**
   * @brief Writes the geometry to HDF5 using the provided parent group ID.
   * @param dataStructureWriter
   * @param parentGroupWriter
   * @return H5::ErrorType
   */
  H5::ErrorType writeHdf5(H5::DataStructureWriter& dataStructureWriter, H5::GroupWriter& parentGroupWriter) const override;

protected:
  /**
   * @brief
   * @param ds
   * @param name
   */
  TriangleGeom(DataStructure& ds, const std::string& name);

  /**
   * @brief
   * @param ds
   * @param name
   * @param numTriangles
   * @param vertices
   * @param allocate
   */
  TriangleGeom(DataStructure& ds, const std::string& name, usize numTriangles, const SharedVertexList* vertices, bool allocate);

  /**
   * @brief
   * @param ds
   * @param name
   * @param triangles
   * @param vertices
   */
  TriangleGeom(DataStructure& ds, const std::string& name, const SharedTriList* triangles, const SharedVertexList* vertices);

  /**
   * @brief
   * @param elementsContainingVert
   */
  void setElementsContainingVert(const ElementDynamicList* elementsContainingVert) override;

  /**
   * @brief
   * @param elementNeighbors
   */
  void setElementNeighbors(const ElementDynamicList* elementNeighbors) override;

  /**
   * @brief
   * @param elementCentroids
   */
  void setElementCentroids(const Float32Array* elementCentroids) override;

  /**
   * @brief
   * @param elementSizes
   */
  void setElementSizes(const Float32Array* elementSizes) override;

private:
  std::optional<DataObject::IdType> m_TriListId;
  std::optional<DataObject::IdType> m_TrianglesContainingVertId;
  std::optional<DataObject::IdType> m_TriangleNeighborsId;
  std::optional<DataObject::IdType> m_TriangleCentroidsId;
  std::optional<DataObject::IdType> m_TriangleSizesId;
};
} // namespace complex
