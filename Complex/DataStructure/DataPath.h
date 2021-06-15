#pragma once

#include <string>
#include <vector>

namespace Complex
{
class DataPath
{
public:
  /**
   * @brief Creates a DataPath using a vector of DataObject names.
   * @param path
   */
  DataPath(const std::vector<std::string>& path = {});

  /**
   * @brief Copy constructor
   * @param other
   */
  DataPath(const DataPath& other);

  /**
   * @brief Move constructor
   * @param other
   */
  DataPath(DataPath&& other) noexcept;

  virtual ~DataPath();

  /**
   * @brief Returns the number of items in the DataPath.
   * @return size_t
   */
  size_t getLength() const;

  /**
   * @brief Returns the path as a vector of strings.
   * @return std::vector<std::string>
   */
  std::vector<std::string> getPathVector() const;

  /**
   * @brief Returns a DataPath pointing to the target's parent.
   * @return DataPath
   */
  DataPath getParent() const;

  /**
   * @brief Creates and returns a DataPath pointing to a child with the specified name.
   * @param name
   * @return DataPath
   */
  DataPath createChildPath(const std::string& name) const;

  /**
   * @brief Creates and returns a DataPath by replacing the specified symbol with
   * the target string value.
   * @param symbol
   * @param targetName
   * @return DataPath
   */
  DataPath replace(const std::string& symbol, const std::string& targetName);

  /**
   * @brief Checks equality between two DataPaths.
   * @param rhs
   * @return bool
   */
  bool operator==(const DataPath& rhs) const;

  /**
   * @brief Checks inequality between two DataPaths.
   * @param rhs
   * @return bool
   */
  bool operator!=(const DataPath& rhs) const;

  /**
   * @brief Returns the name at the target index.
   * @param index
   * @return std::string
   */
  const std::string& operator[](size_t index) const;

  /**
   * @brief Returns a string representation using a target divider.
   * @param div = " / "
   * @return std::string
   */
  std::string toString(const std::string& div = " / ") const;

protected:
private:
  std::vector<std::string> m_Path;
};
} // namespace Complex
