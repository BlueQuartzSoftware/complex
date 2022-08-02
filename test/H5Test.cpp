
#include "complex/Utilities/Parsing/HDF5/H5.hpp"
#include "complex/Core/Application.hpp"
#include "complex/DataStructure/DataArray.hpp"
#include "complex/DataStructure/DataGroup.hpp"
#include "complex/DataStructure/DataObject.hpp"
#include "complex/DataStructure/DataStore.hpp"
#include "complex/DataStructure/DataStructure.hpp"
#include "complex/DataStructure/Geometry/EdgeGeom.hpp"
#include "complex/DataStructure/Geometry/HexahedralGeom.hpp"
#include "complex/DataStructure/Geometry/ImageGeom.hpp"
#include "complex/DataStructure/Geometry/QuadGeom.hpp"
#include "complex/DataStructure/Geometry/TriangleGeom.hpp"
#include "complex/DataStructure/Geometry/VertexGeom.hpp"
#include "complex/DataStructure/Montage/GridMontage.hpp"
#include "complex/DataStructure/ScalarData.hpp"
#include "complex/DataStructure/StringArray.hpp"
#include "complex/Utilities/DataArrayUtilities.hpp"
#include "complex/Utilities/Parsing/DREAM3D/Dream3dIO.hpp"
#include "complex/Utilities/Parsing/HDF5/H5FileReader.hpp"
#include "complex/Utilities/Parsing/HDF5/H5FileWriter.hpp"
#include "complex/Utilities/Parsing/Text/CsvParser.hpp"

#include "GeometryTestUtilities.hpp"

// This file is generated into the binary directory
#include "complex/unit_test/complex_test_dirs.hpp"

#include <catch2/catch.hpp>

#include <iostream>
#include <string>
#include <type_traits>

#define TEST_LEGACY 1

using namespace complex;
namespace fs = std::filesystem;

static_assert(std::is_same_v<hid_t, H5::IdType>, "H5::IdType must be the same type as hid_t");
static_assert(std::is_same_v<herr_t, H5::ErrorType>, "H5::ErrorType must be the same type as herr_t");
static_assert(std::is_same_v<hsize_t, H5::SizeType>, "H5::SizeType must be the same type as hsize_t");

namespace
{
namespace Constants
{
const fs::path k_DataDir = "test/data";
const StringLiteral k_LegacyFilepath = "LegacyData.dream3d";
const fs::path k_ComplexH5File = "new.h5";
} // namespace Constants

fs::path GetDataDir(const Application& app)
{
  return COMPLEX_BUILD_DIR / Constants::k_DataDir;
}

fs::path GetLegacyFilepath(const Application& app)
{
  std::string path = fmt::format("{}/test/Data/{}", unit_test::k_SourceDir.view(), Constants::k_LegacyFilepath);
  return std::filesystem::path(path);
}

fs::path GetComplexH5File(const Application& app)
{
  return GetDataDir(app) / Constants::k_ComplexH5File;
}

bool equalsf(const FloatVec3& lhs, const FloatVec3& rhs)
{
  for(usize i = 0; i < 3; i++)
  {
    float32 diff = std::abs(lhs[i] - rhs[i]);
    if(diff >= 0.0001f)
    {
      return false;
    }
  }
  return true;
}
} // namespace

#if TEST_LEGACY
TEST_CASE("Read Legacy DREAM.3D Data")
{
  Application app;
  std::filesystem::path filepath = GetLegacyFilepath(app);
  REQUIRE(exists(filepath));
  Result<DataStructure> result = DREAM3D::ImportDataStructureFromFile(filepath, true);
  COMPLEX_RESULT_REQUIRE_VALID(result);
  DataStructure ds = result.value();

  const std::string geomName = "Small IN100";
  const auto* image = ds.getDataAs<ImageGeom>(DataPath({geomName}));
  REQUIRE(image != nullptr);
  REQUIRE(equalsf(image->getOrigin(), FloatVec3(-47.0f, 0.0f, -29.0f)));
  REQUIRE(image->getDimensions() == SizeVec3(189, 201, 117));
  REQUIRE(equalsf(image->getSpacing(), FloatVec3(0.25f, 0.25f, 0.25f)));

  {
    const std::string testDCName = "DataContainer";
    DataPath testDCPath({testDCName});
    auto* testDC = ds.getDataAs<DataGroup>(testDCPath);
    REQUIRE(testDC != nullptr);

    DataPath testAMPath = testDCPath.createChildPath("AttributeMatrix");
    REQUIRE(ds.getDataAs<DataGroup>(DataPath({testAMPath})) != nullptr);

    REQUIRE(ds.getDataAs<Int8Array>(testAMPath.createChildPath("Int8")) != nullptr);
    REQUIRE(ds.getDataAs<UInt8Array>(testAMPath.createChildPath("UInt8")) != nullptr);
    REQUIRE(ds.getDataAs<Float32Array>(testAMPath.createChildPath("Float32")) != nullptr);
    REQUIRE(ds.getDataAs<Float64Array>(testAMPath.createChildPath("Float64")) != nullptr);
    REQUIRE(ds.getDataAs<BoolArray>(testAMPath.createChildPath("Bool")) != nullptr);
  }

  {
    const std::string grainData = "Grain Data";
    REQUIRE(ds.getData(DataPath({geomName, grainData})) != nullptr);
    REQUIRE(ds.getDataAs<NeighborList<int32_t>>(DataPath({geomName, grainData, "NeighborList"})) != nullptr);
    REQUIRE(ds.getDataAs<Int32Array>(DataPath({geomName, grainData, "NumElements"})) != nullptr);
    REQUIRE(ds.getDataAs<Int32Array>(DataPath({geomName, grainData, "NumNeighbors"})) != nullptr);
  }
}
#endif

DataStructure GetTestDataStructure()
{
  DataStructure ds;
  auto group = DataGroup::Create(ds, "Group");
  REQUIRE(group != nullptr);

  auto montage = GridMontage::Create(ds, "Montage", group->getId());
  REQUIRE(montage != nullptr);

  auto geom = ImageGeom::Create(ds, "Geometry", montage->getId());
  REQUIRE(geom != nullptr);

  auto scalar = ScalarData<int32>::Create(ds, "Scalar", 7, geom->getId());
  REQUIRE(scalar != nullptr);

  auto dataStore = std::make_unique<DataStore<uint8>>(std::vector<usize>{2}, std::vector<usize>{3}, 0);
  auto dataArray = DataArray<uint8>::Create(ds, "DataArray", std::move(dataStore), geom->getId());
  REQUIRE(dataArray != nullptr);

  return ds;
}

template <typename T>
DataArray<T>* CreateTestDataArray(const std::string& name, DataStructure& dataGraph, typename DataStore<T>::ShapeType tupleShape, typename DataStore<T>::ShapeType componentShape,
                                  DataObject::IdType parentId)
{
  using DataStoreType = DataStore<T>;
  using ArrayType = DataArray<T>;

  auto data_store = std::make_unique<DataStoreType>(tupleShape, componentShape, static_cast<T>(0));
  ArrayType* dataArray = ArrayType::Create(dataGraph, name, std::move(data_store), parentId);

  return dataArray;
}

DataStructure CreateDataStructure()
{
  DataStructure dataGraph;
  DataGroup* group = DataGroup::Create(dataGraph, "Small IN100");
  DataGroup* scanData = DataGroup::Create(dataGraph, "EBSD Scan Data", group->getId());

  // Create an Image Geometry grid for the Scan Data
  ImageGeom* imageGeom = ImageGeom::Create(dataGraph, "Small IN100 Grid", scanData->getId());
  imageGeom->setSpacing({0.25f, 0.25f, 0.25f});
  imageGeom->setOrigin({0.0f, 0.0f, 0.0f});
  SizeVec3 imageGeomDims = {100, 100, 100};
  imageGeom->setDimensions(imageGeomDims); // Listed from slowest to fastest (Z, Y, X)

  // Create some DataArrays; The DataStructure keeps a shared_ptr<> to the DataArray so DO NOT put
  // it into another shared_ptr<>
  usize numComponents = 1;
  std::vector<usize> tupleShape = {imageGeomDims[0], imageGeomDims[1], imageGeomDims[2]};

  Float32Array* ci_data = CreateTestDataArray<float>("Confidence Index", dataGraph, tupleShape, {numComponents}, scanData->getId());
  Int32Array* feature_ids_data = CreateTestDataArray<int32>("FeatureIds", dataGraph, tupleShape, {numComponents}, scanData->getId());
  Float32Array* iq_data = CreateTestDataArray<float>("Image Quality", dataGraph, tupleShape, {numComponents}, scanData->getId());
  Int32Array* phases_data = CreateTestDataArray<int32>("Phases", dataGraph, tupleShape, {numComponents}, scanData->getId());

  numComponents = 3;
  UInt8Array* ipf_color_data = CreateTestDataArray<uint8_t>("IPF Colors", dataGraph, tupleShape, {numComponents}, scanData->getId());

  dataGraph.setAdditionalParent(ipf_color_data->getId(), group->getId());

  // Add in another group that holds the phase data such as Laue Class, Lattice Constants, etc.
  DataGroup* phase_group = DataGroup::Create(dataGraph, "Phase Data", group->getId());
  numComponents = 1;
  usize numTuples = 2;
  Int32Array* laue_data = CreateTestDataArray<int32>("Laue Class", dataGraph, {numTuples}, {numComponents}, phase_group->getId());

  return dataGraph;
}

TEST_CASE("Image Geometry IO")
{
  Application app;

  fs::path dataDir = GetDataDir(app);

  if(!fs::exists(dataDir))
  {
    REQUIRE(fs::create_directories(dataDir));
  }

  fs::path filePath = GetDataDir(app) / "image_geometry_io.h5";

  // Write HDF5 file
  try
  {
    DataStructure ds = CreateDataStructure();

    Result<H5::FileWriter> result = H5::FileWriter::CreateFile(filePath);
    REQUIRE(result.valid());

    H5::FileWriter fileWriter = std::move(result.value());
    REQUIRE(fileWriter.isValid());

    herr_t err;
    err = ds.writeHdf5(fileWriter);
    REQUIRE(err >= 0);
  } catch(const std::exception& e)
  {
    FAIL(e.what());
  }

  // Read HDF5 file
  try
  {
    auto fileReader = H5::FileReader(filePath);
    REQUIRE(fileReader.isValid());

    herr_t err;
    auto ds = DataStructure::readFromHdf5(fileReader, err);
    REQUIRE(err >= 0);

    filePath = GetDataDir(app) / "image_geometry_io_2.h5";

    // Write DataStructure to another file
    try
    {
      Result<H5::FileWriter> result = H5::FileWriter::CreateFile(filePath);
      REQUIRE(result.valid());

      H5::FileWriter fileWriter = std::move(result.value());
      REQUIRE(fileWriter.isValid());

      err = ds.writeHdf5(fileWriter);
      REQUIRE(err >= 0);
    } catch(const std::exception& e)
    {
      FAIL(e.what());
    }

  } catch(const std::exception& e)
  {
    FAIL(e.what());
  }
}

const std::string k_TriangleGroupName = "TRIANGLE_GEOMETRY";
const std::string k_TriangleFaceName = "SharedTriList";
const std::string k_VertexListName = "SharedVertexList";
const std::string k_VertexGroupName = "VERTEX_GEOMETRY";
const std::string k_QuadGroupName = "QUAD_GEOMETRY";
const std::string k_QuadFaceName = "SharedQuadList";
const std::string k_EdgeFaceName = "SharedEdgeList";
const std::string k_EdgeGroupName = "EDGE_GEOMETRY";
const std::string k_NeighborGroupName = "NEIGHBORLIST_GROUP";

void CreateVertexGeometry(DataStructure& dataGraph)
{
  DataGroup* geometryGroup = DataGroup::Create(dataGraph, k_VertexGroupName);
  using MeshIndexType = AbstractGeometry::MeshIndexType;
  auto vertexGeometry = VertexGeom::Create(dataGraph, "[Geometry] Vertex", geometryGroup->getId());

  // DataGroup* scanData = DataGroup::Create(dataGraph, "AttributeMatrix", group->getId());
  uint64 skipLines = 1;
  char delimiter = ',';

  // Create the Vertex Array with a component size of 3
  DataPath path = DataPath({k_VertexGroupName, k_VertexListName});
  std::string inputFile = fmt::format("{}/test/Data/VertexCoordinates.csv", unit_test::k_SourceDir.view());
  uint64 vertexCount = CsvParser::LineCount(inputFile) - skipLines;
  REQUIRE(vertexCount == 144);
  complex::Result result = complex::CreateArray<float>(dataGraph, {vertexCount}, {3}, path, IDataAction::Mode::Execute);
  REQUIRE(result.valid());
  auto vertexArray = complex::ArrayFromPath<float>(dataGraph, path);
  CsvParser::ReadFile<float, float>(inputFile, *vertexArray, skipLines, delimiter);
  vertexGeometry->setVertices(vertexArray);
  REQUIRE(vertexGeometry->getNumberOfVertices() == 144);

  // Now create some "Cell" data for the Vertex Geometry
  std::vector<usize> tupleShape = {vertexGeometry->getNumberOfVertices()};
  usize numComponents = 1;
  Int16Array* ci_data = CreateTestDataArray<int16_t>("Area", dataGraph, tupleShape, {numComponents}, geometryGroup->getId());
  Float32Array* power_data = CreateTestDataArray<float>("Power", dataGraph, tupleShape, {numComponents}, geometryGroup->getId());
  UInt32Array* laserTTL_data = CreateTestDataArray<uint32>("LaserTTL", dataGraph, tupleShape, {numComponents}, geometryGroup->getId());
  for(usize i = 0; i < vertexGeometry->getNumberOfVertices(); i++)
  {
    ci_data->getDataStore()->setValue(i, static_cast<int16_t>(i * 10));
    power_data->getDataStore()->setValue(i, static_cast<float>(i * 2.345f));
    laserTTL_data->getDataStore()->setValue(i, static_cast<uint32>(i * 3421));
  }
}

void CreateTriangleGeometry(DataStructure& dataGraph)
{
  // Create a Triangle Geometry
  DataGroup* geometryGroup = DataGroup::Create(dataGraph, k_TriangleGroupName);
  using MeshIndexType = AbstractGeometry::MeshIndexType;
  auto triangleGeom = TriangleGeom::Create(dataGraph, "[Geometry] Triangle", geometryGroup->getId());

  // Create a Path in the DataStructure to place the geometry
  DataPath path = DataPath({k_TriangleGroupName, k_TriangleFaceName});
  std::string inputFile = fmt::format("{}/test/Data/TriangleConnectivity.csv", unit_test::k_SourceDir.view());
  uint64 skipLines = 1;
  char delimiter = ',';
  uint64 faceCount = CsvParser::LineCount(inputFile) - skipLines;
  REQUIRE(faceCount == 242);
  // Create the default DataArray that will hold the FaceList and Vertices. We
  // size these to 1 because the Csv parser will resize them to the appropriate number of typles
  complex::Result result = complex::CreateArray<MeshIndexType>(dataGraph, {faceCount}, {3}, path, IDataAction::Mode::Execute);
  REQUIRE(result.valid());
  auto dataArray = complex::ArrayFromPath<MeshIndexType>(dataGraph, path);
  CsvParser::ReadFile<MeshIndexType, MeshIndexType>(inputFile, *dataArray, skipLines, delimiter);
  triangleGeom->setFaces(dataArray);

  // Create the Vertex Array with a component size of 3
  path = DataPath({k_TriangleGroupName, k_VertexListName});
  inputFile = fmt::format("{}/test/Data/VertexCoordinates.csv", unit_test::k_SourceDir.view());
  uint64 vertexCount = CsvParser::LineCount(inputFile) - skipLines;
  REQUIRE(vertexCount == 144);
  result = complex::CreateArray<float>(dataGraph, {vertexCount}, {3}, path, IDataAction::Mode::Execute);
  REQUIRE(result.valid());
  auto vertexArray = complex::ArrayFromPath<float>(dataGraph, path);
  CsvParser::ReadFile<float, float>(inputFile, *vertexArray, skipLines, delimiter);
  triangleGeom->setVertices(vertexArray);
}

void CreateQuadGeometry(DataStructure& dataGraph)
{
  // Create a Triangle Geometry
  DataGroup* geometryGroup = DataGroup::Create(dataGraph, k_QuadGroupName);
  using MeshIndexType = AbstractGeometry::MeshIndexType;
  auto geometry = QuadGeom::Create(dataGraph, "[Geometry] Quad", geometryGroup->getId());

  // Create a Path in the DataStructure to place the geometry
  DataPath path = DataPath({k_QuadGroupName, k_QuadFaceName});
  std::string inputFile = fmt::format("{}/test/Data/QuadConnectivity.csv", unit_test::k_SourceDir.view());
  uint64 skipLines = 1;
  char delimiter = ',';
  uint64 faceCount = CsvParser::LineCount(inputFile) - skipLines;
  REQUIRE(faceCount == 121);
  // Create the default DataArray that will hold the FaceList and Vertices. We
  // size these to 1 because the Csv parser will resize them to the appropriate number of typles
  complex::Result result = complex::CreateArray<MeshIndexType>(dataGraph, {faceCount}, {4}, path, IDataAction::Mode::Execute);
  REQUIRE(result.valid());
  auto dataArray = complex::ArrayFromPath<MeshIndexType>(dataGraph, path);
  CsvParser::ReadFile<MeshIndexType, MeshIndexType>(inputFile, *dataArray, skipLines, delimiter);
  geometry->setFaces(dataArray);

  // Create the Vertex Array with a component size of 3
  path = DataPath({k_QuadGroupName, k_VertexListName});
  inputFile = fmt::format("{}/test/Data/VertexCoordinates.csv", unit_test::k_SourceDir.view());
  uint64 vertexCount = CsvParser::LineCount(inputFile) - skipLines;
  REQUIRE(vertexCount == 144);
  result = complex::CreateArray<float>(dataGraph, {vertexCount}, {3}, path, IDataAction::Mode::Execute);
  REQUIRE(result.valid());
  auto vertexArray = complex::ArrayFromPath<float>(dataGraph, path);
  CsvParser::ReadFile<float, float>(inputFile, *vertexArray, skipLines, delimiter);
  geometry->setVertices(vertexArray);
}

void CreateEdgeGeometry(DataStructure& dataGraph)
{
  // Create a Triangle Geometry
  DataGroup* geometryGroup = DataGroup::Create(dataGraph, k_EdgeGroupName);
  using MeshIndexType = AbstractGeometry::MeshIndexType;
  auto geometry = EdgeGeom::Create(dataGraph, "[Geometry] Edge", geometryGroup->getId());

  // Create a Path in the DataStructure to place the geometry
  DataPath path = DataPath({k_EdgeGroupName, k_EdgeFaceName});
  std::string inputFile = fmt::format("{}/test/Data/EdgeConnectivity.csv", unit_test::k_SourceDir.view());
  uint64 skipLines = 1;
  char delimiter = ',';
  uint64 faceCount = CsvParser::LineCount(inputFile) - skipLines;
  REQUIRE(faceCount == 264);
  // Create the default DataArray that will hold the FaceList and Vertices. We
  // size these to 1 because the Csv parser will resize them to the appropriate number of typles
  complex::Result result = complex::CreateArray<MeshIndexType>(dataGraph, {faceCount}, {2}, path, IDataAction::Mode::Execute);
  REQUIRE(result.valid());
  auto dataArray = complex::ArrayFromPath<MeshIndexType>(dataGraph, path);
  CsvParser::ReadFile<MeshIndexType, MeshIndexType>(inputFile, *dataArray, skipLines, delimiter);
  geometry->setEdges(dataArray);

  // Create the Vertex Array with a component size of 3
  path = DataPath({k_EdgeGroupName, k_VertexListName});
  inputFile = fmt::format("{}/test/Data/VertexCoordinates.csv", unit_test::k_SourceDir.view());
  uint64 vertexCount = CsvParser::LineCount(inputFile) - skipLines;
  REQUIRE(vertexCount == 144);
  result = complex::CreateArray<float>(dataGraph, {vertexCount}, {3}, path, IDataAction::Mode::Execute);
  REQUIRE(result.valid());
  auto vertexArray = complex::ArrayFromPath<float>(dataGraph, path);
  CsvParser::ReadFile<float, float>(inputFile, *vertexArray, skipLines, delimiter);
  geometry->setVertices(vertexArray);
}

void CreateNeighborList(DataStructure& dataStructure)
{
  const usize numItems = 50;

  auto* neighborGroup = DataGroup::Create(dataStructure, k_NeighborGroupName);
  auto* neighborGroup2 = DataGroup::Create(dataStructure, k_NeighborGroupName + "2");
  auto* neighborList = NeighborList<int64>::Create(dataStructure, "NeighborList", numItems, neighborGroup->getId());
  for(usize i = 0; i < numItems; i++)
  {
    neighborList->addEntry(i + 1, i);
  }
  dataStructure.setAdditionalParent(neighborList->getId(), neighborGroup2->getId());
}

void CreateArrayTypes(DataStructure& dataStructure)
{
  const std::vector<usize> tupleShape = {2};
  const std::vector<usize> componentShape = {1};

  auto* boolArray = DataArray<bool>::CreateWithStore<DataStore<bool>>(dataStructure, "BoolArray", tupleShape, componentShape);
  AbstractDataStore<bool>& boolStore = boolArray->getDataStoreRef();
  boolStore[0] = false;
  boolStore[1] = true;

  DataArray<int8>::CreateWithStore<DataStore<int8>>(dataStructure, "Int8Array", tupleShape, componentShape);
  DataArray<int16>::CreateWithStore<DataStore<int16>>(dataStructure, "Int16Array", tupleShape, componentShape);
  DataArray<int32>::CreateWithStore<DataStore<int32>>(dataStructure, "Int32Array", tupleShape, componentShape);
  DataArray<int64>::CreateWithStore<DataStore<int64>>(dataStructure, "Int64Array", tupleShape, componentShape);

  DataArray<uint8>::CreateWithStore<DataStore<uint8>>(dataStructure, "UInt8Array", tupleShape, componentShape);
  DataArray<uint16>::CreateWithStore<DataStore<uint16>>(dataStructure, "UInt16Array", tupleShape, componentShape);
  DataArray<uint32>::CreateWithStore<DataStore<uint32>>(dataStructure, "UInt32Array", tupleShape, componentShape);
  DataArray<uint64>::CreateWithStore<DataStore<uint64>>(dataStructure, "UInt64Array", tupleShape, componentShape);

  DataArray<float32>::CreateWithStore<DataStore<float32>>(dataStructure, "Float32Array", tupleShape, componentShape);
  DataArray<float64>::CreateWithStore<DataStore<float64>>(dataStructure, "Float64Array", tupleShape, componentShape);

  StringArray::CreateWithValues(dataStructure, "StringArray", {"Foo", "Bar", "Bazz"});
}

//------------------------------------------------------------------------------
DataStructure CreateNodeBasedGeometries()
{
  DataStructure dataGraph;

  CreateVertexGeometry(dataGraph);
  CreateTriangleGeometry(dataGraph);
  CreateQuadGeometry(dataGraph);
  CreateEdgeGeometry(dataGraph);

  return dataGraph;
}

struct VertexGeomData
{
  std::optional<DataObject::IdType> verticesId;

  bool operator==(const VertexGeomData& rhs) const
  {
    return verticesId == rhs.verticesId;
  }
};

struct EdgeGeomData
{
  std::optional<DataObject::IdType> verticesId;
  std::optional<DataObject::IdType> edgesId;

  bool operator==(const EdgeGeomData& rhs) const
  {
    return (verticesId == rhs.verticesId) && (edgesId == rhs.edgesId);
  }
};

struct TriangleGeomData
{
  std::optional<DataObject::IdType> verticesId;
  std::optional<DataObject::IdType> edgesId;
  std::optional<DataObject::IdType> trianglesId;

  bool operator==(const TriangleGeomData& rhs) const
  {
    return (verticesId == rhs.verticesId) && (edgesId == rhs.edgesId) && (trianglesId == rhs.trianglesId);
  }
};

struct QuadGeomData
{
  std::optional<DataObject::IdType> verticesId;
  std::optional<DataObject::IdType> edgesId;
  std::optional<DataObject::IdType> quadsId;

  bool operator==(const QuadGeomData& rhs) const
  {
    return (verticesId == rhs.verticesId) && (edgesId == rhs.edgesId) && (quadsId == rhs.quadsId);
  }
};

struct NodeBasedGeomData
{
  VertexGeomData vertexData;
  EdgeGeomData edgeData;
  TriangleGeomData triangleData;
  QuadGeomData quadData;

  bool operator==(const NodeBasedGeomData& rhs) const
  {
    bool vertexCheck = (vertexData == rhs.vertexData);
    bool edgeCheck = (edgeData == rhs.edgeData);
    bool triangleCheck = (triangleData == rhs.triangleData);
    bool quadCheck = (quadData == rhs.quadData);
    return vertexCheck && edgeCheck && triangleCheck && quadCheck;
  }
};

NodeBasedGeomData getNodeGeomData(const DataStructure& dataStructure)
{
  NodeBasedGeomData nodeData;

  DataPath vertexPath({k_VertexGroupName, "[Geometry] Vertex"});
  auto* vertexGeom = dataStructure.getDataAs<VertexGeom>(vertexPath);
  REQUIRE(vertexGeom != nullptr);
  nodeData.vertexData.verticesId = vertexGeom->getVerticesId();

  DataPath edgePath({k_EdgeGroupName, "[Geometry] Edge"});
  auto* edgeGeom = dataStructure.getDataAs<EdgeGeom>(edgePath);
  REQUIRE(edgeGeom != nullptr);
  nodeData.edgeData.verticesId = edgeGeom->getVerticesId();
  nodeData.edgeData.edgesId = edgeGeom->getEdgesId();

  DataPath trianglePath({k_TriangleGroupName, "[Geometry] Triangle"});
  auto* triangleGeom = dataStructure.getDataAs<TriangleGeom>(trianglePath);
  REQUIRE(triangleGeom != nullptr);
  nodeData.triangleData.verticesId = triangleGeom->getVerticesId();
  nodeData.triangleData.edgesId = triangleGeom->getEdgesId();
  nodeData.triangleData.trianglesId = triangleGeom->getFacesId();

  DataPath quadPath({k_QuadGroupName, "[Geometry] Quad"});
  auto* quadGeom = dataStructure.getDataAs<QuadGeom>(quadPath);
  REQUIRE(quadGeom != nullptr);
  nodeData.quadData.verticesId = quadGeom->getVerticesId();
  nodeData.quadData.edgesId = quadGeom->getEdgesId();
  nodeData.quadData.quadsId = quadGeom->getFacesId();

  return nodeData;
}

void checkNodeGeomData(const DataStructure& dataStructure, const NodeBasedGeomData& nodeData)
{
  NodeBasedGeomData newNodeData = getNodeGeomData(dataStructure);
  REQUIRE(newNodeData == nodeData);
}

TEST_CASE("Node Based Geometry IO")
{
  Application app;

  fs::path dataDir = GetDataDir(app);

  if(!fs::exists(dataDir))
  {
    REQUIRE(fs::create_directories(dataDir));
  }

  fs::path filePath = GetDataDir(app) / "NodeGeometryTest.dream3d";

  std::string filePathString = filePath.string();

  NodeBasedGeomData nodeData;

  // Write HDF5 file
  try
  {
    DataStructure ds = CreateNodeBasedGeometries();
    nodeData = getNodeGeomData(ds);
    Result<H5::FileWriter> result = H5::FileWriter::CreateFile(filePathString);
    REQUIRE(result.valid());

    H5::FileWriter fileWriter = std::move(result.value());
    REQUIRE(fileWriter.isValid());

    herr_t err;
    err = ds.writeHdf5(fileWriter);
    REQUIRE(err >= 0);
  } catch(const std::exception& e)
  {
    FAIL(e.what());
  }

  // Read HDF5 file
  try
  {
    H5::FileReader fileReader(filePathString);
    REQUIRE(fileReader.isValid());

    herr_t err;
    auto ds = DataStructure::readFromHdf5(fileReader, err);
    REQUIRE(err >= 0);
    checkNodeGeomData(ds, nodeData);
  } catch(const std::exception& e)
  {
    FAIL(e.what());
  }
}

TEST_CASE("NeighborList IO")
{
  Application app;

  fs::path dataDir = GetDataDir(app);

  if(!fs::exists(dataDir))
  {
    REQUIRE(fs::create_directories(dataDir));
  }

  fs::path filePath = GetDataDir(app) / "NeighborListTest.dream3d";

  std::string filePathString = filePath.string();

  // Write HDF5 file
  try
  {
    DataStructure ds;
    CreateNeighborList(ds);
    Result<H5::FileWriter> result = H5::FileWriter::CreateFile(filePathString);
    REQUIRE(result.valid());

    H5::FileWriter fileWriter = std::move(result.value());
    REQUIRE(fileWriter.isValid());

    herr_t err;
    err = ds.writeHdf5(fileWriter);
    REQUIRE(err >= 0);
  } catch(const std::exception& e)
  {
    FAIL(e.what());
  }

  // Read HDF5 file
  try
  {
    H5::FileReader fileReader(filePathString);
    REQUIRE(fileReader.isValid());

    herr_t err;
    auto ds = DataStructure::readFromHdf5(fileReader, err);
    REQUIRE(err >= 0);

    // auto neighborList = ds.getDataAs<NeighborList<int64>>(DataPath({k_NeighborGroupName, "NeighborList"}));
    auto neighborList = ds.getData(DataPath({k_NeighborGroupName, "NeighborList"}));
    REQUIRE(neighborList != nullptr);
  } catch(const std::exception& e)
  {
    FAIL(e.what());
  }
}

TEST_CASE("DataArray<bool> IO")
{
  Application app;

  fs::path dataDir = GetDataDir(app);

  if(!fs::exists(dataDir))
  {
    REQUIRE(fs::create_directories(dataDir));
  }

  fs::path filePath = GetDataDir(app) / "BoolArrayTest.dream3d";

  std::string filePathString = filePath.string();

  // Write HDF5 file
  try
  {
    DataStructure ds;
    CreateArrayTypes(ds);
    Result<H5::FileWriter> result = H5::FileWriter::CreateFile(filePathString);
    REQUIRE(result.valid());

    H5::FileWriter fileWriter = std::move(result.value());
    REQUIRE(fileWriter.isValid());

    herr_t err;
    err = ds.writeHdf5(fileWriter);
    REQUIRE(err >= 0);
  } catch(const std::exception& e)
  {
    FAIL(e.what());
  }

  // Read HDF5 file
  try
  {
    H5::FileReader fileReader(filePathString);
    REQUIRE(fileReader.isValid());

    herr_t err;
    auto ds = DataStructure::readFromHdf5(fileReader, err);
    REQUIRE(err >= 0);

    REQUIRE(ds.getDataAs<DataArray<int8>>(DataPath({"Int8Array"})) != nullptr);
    REQUIRE(ds.getDataAs<DataArray<int16>>(DataPath({"Int16Array"})) != nullptr);
    REQUIRE(ds.getDataAs<DataArray<int32>>(DataPath({"Int32Array"})) != nullptr);
    REQUIRE(ds.getDataAs<DataArray<int64>>(DataPath({"Int64Array"})) != nullptr);
    REQUIRE(ds.getDataAs<DataArray<uint8>>(DataPath({"UInt8Array"})) != nullptr);
    REQUIRE(ds.getDataAs<DataArray<uint16>>(DataPath({"UInt16Array"})) != nullptr);
    REQUIRE(ds.getDataAs<DataArray<uint32>>(DataPath({"UInt32Array"})) != nullptr);
    REQUIRE(ds.getDataAs<DataArray<uint64>>(DataPath({"UInt64Array"})) != nullptr);
    REQUIRE(ds.getDataAs<DataArray<float32>>(DataPath({"Float32Array"})) != nullptr);
    REQUIRE(ds.getDataAs<DataArray<float64>>(DataPath({"Float64Array"})) != nullptr);
    REQUIRE(ds.getDataAs<StringArray>(DataPath({"StringArray"})) != nullptr);

    BoolArray* boolArray = ds.getDataAs<BoolArray>(DataPath({"BoolArray"}));
    REQUIRE(boolArray != nullptr);
    AbstractDataStore<bool>& boolStore = boolArray->getDataStoreRef();
    REQUIRE(boolStore[0] == false);
    REQUIRE(boolStore[1] == true);

    StringArray* stringArray = ds.getDataAs<StringArray>(DataPath({"StringArray"}));
    REQUIRE(stringArray != nullptr);
    REQUIRE(stringArray->values() == std::vector<std::string>{"Foo", "Bar", "Bazz"});
  } catch(const std::exception& e)
  {
    FAIL(e.what());
  }
}
