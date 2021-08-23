#pragma once

#include "IPipelineNode.hpp"
#include "complex/Filter/IFilter.hpp"

namespace complex
{
class FilterHandle;

class COMPLEX_EXPORT FilterNode : public IPipelineNode
{
public:
  /**
   * @brief Attempts to construct a FilterNode based on the specified
   * FilterHandle. Returns nullptr if the corresponding filter could not be
   * created.
   * @param handle
   * @return FilterNode*
   */
  static FilterNode* Create(const FilterHandle& handle);
  
  /**
   * @brief Constructs a FilterNode with the specified filter.
   *
   * The unique_ptr is moved to the FilterNode.
   * @param filter
   */
  FilterNode(IFilter::UniquePointer&& filter);


  virtual ~FilterNode();

  /**
   * @brief Returns a pointer to the wrapped filter.
   * @return IFilter*
   */
  IFilter* getFilter() const;

  /**
   * @brief Returns a collection of the filter's parameters.
   * @return Parameters
   */
  Parameters getParameters() const;

  /**
   * @brief Returns a collection of the node's filter arguments.
   * @return Arguments
   */
  Arguments getArguments() const;

  /**
   * @brief Sets the node's filter arguments and marks the node as dirty.
   * @param args
   */
  void setArguments(const Arguments& args);

  /**
   * @brief Attempts to preflight the node using the provided DataStructure.
   * Returns true if preflighting succeeded. Otherwise, this returns false.
   * @param data
   * @return bool
   */
  bool preflight(DataStructure& data) const override;

  /**
   * @brief Attempts to execute the node using the provided DataStructure.
   * Returns true if execution succeeded. Otherwise, this returns false.
   * @param data
   * @return bool
   */
  bool execute(DataStructure& data) override;

private:
  IFilter::UniquePointer m_Filter;
  Arguments m_Arguments;
};
} // namespace complex
