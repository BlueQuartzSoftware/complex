#pragma once

#include <random>

#include "ComplexCore/ComplexCore_export.hpp"

#include "complex/Common/StringLiteral.hpp"
#include "complex/Common/Types.hpp"
#include "complex/DataStructure/DataArray.hpp"
#include "complex/Filter/FilterTraits.hpp"
#include "complex/Filter/IFilter.hpp"

namespace complex
{
class COMPLEXCORE_EXPORT ScalarSegmentFeatures : public IFilter
{
public:
  using SeedGenerator = std::mt19937_64;
  using Int64Distribution = std::uniform_int_distribution<int64>;

  ScalarSegmentFeatures() = default;
  ~ScalarSegmentFeatures() noexcept override = default;

  ScalarSegmentFeatures(const ScalarSegmentFeatures&) = delete;
  ScalarSegmentFeatures(ScalarSegmentFeatures&&) noexcept = delete;

  ScalarSegmentFeatures& operator=(const ScalarSegmentFeatures&) = delete;
  ScalarSegmentFeatures& operator=(ScalarSegmentFeatures&&) noexcept = delete;

  // Parameter Keys
  static inline constexpr StringLiteral k_DataPath_Key = "scalar_segment_path";

  /**
   * @brief Returns the filter's name.
   * @return std::string
   */
  std::string name() const override;

  /**
   * @brief Returns the C++ classname of this filter.
   * @return std::string
   */
  std::string className() const override;

  /**
   * @brief Returns the filter's Uuid.
   * @return Uuid
   */
  Uuid uuid() const override;

  /**
   * @brief Returns the human-readable filter name.
   * @return std::string
   */
  std::string humanName() const override;

  /**
   * @brief Returns the filter's parameters.
   * @return Parameters
   */
  Parameters parameters() const override;

  /**
   * @brief Creates a copy of the filter.
   * @return UniquePointer
   */
  UniquePointer clone() const override;

protected:
  static bool validateNmberOfTuples(const DataStructure& dataStructure, const std::vector<DataPath>& dataArrayPaths);

  /**
   * @brief
   * @param featureIds
   * @param totalPoints
   * @param totalFeatures
   * @param distribution
   * @param generator
   */
  void randomizeFeatureIds(Int32Array* featureIdsArray, int64_t totalPoints, int64_t totalFeatures, Int64Distribution& distribution);

  /**
   * @brief
   * @param featureIds
   * @param gnum
   * @param nextSeed
   * @return int64
   */
  int64 getSeed(Int32Array* featureIds, int32 gnum, int64 nextSeed, bool useGoodVoxels, const UInt8Array* goodVoxels);

  /**
   * @brief
   * @param featureIdsArray
   * @param useGoodVoxels
   * @param goodVoxelsArray
   * @param referencepoint
   * @param neighborpoint
   * @param gnum
   * @return bool
   */
  bool determineGrouping(const Int32Array* featureIdsArray, bool useGoodVoxels, const UInt8Array* goodVoxelsArray, int64 referencepoint, int64 neighborpoint, int32 gnum);

  /**
   * @brief
   * @param distribution
   * @param rangeMin
   * @param rangeMax
   * @return SeedGenerator
   */
  SeedGenerator initializeVoxelSeedGenerator(Int64Distribution& distribution, const int64 rangeMin, const int64 rangeMax);

  /**
   * @brief
   * @param data
   * @param args
   * @param messageHandler
   * @return PreflightResult
   */
  PreflightResult preflightImpl(const DataStructure& data, const Arguments& args, const MessageHandler& messageHandler) const override;

  /**
   * @brief
   * @param data
   * @param args
   * @param pipelineNode
   * @param messageHandler
   * @return Result<>
   */
  Result<> executeImpl(DataStructure& data, const Arguments& args, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler) const override;
};
} // namespace complex

COMPLEX_DEF_FILTER_TRAITS(complex, ScalarSegmentFeatures, "e4da1b19-507d-4494-8abd-f517f1ed0651");
