#pragma once

#include <vector>

#include "complex/Plugin/AbstractPlugin.hpp"

#include "test/test2plugin_export.hpp"

class TEST2PLUGIN_EXPORT Test2Plugin : public complex::AbstractPlugin
{
public:
  static const IdType ID;

  Test2Plugin();
  ~Test2Plugin() override;

  /**
   * @brief Returns a collection of HDF5 DataStructure factories available
   * through the plugin.
   * @return std::vector<complex::IH5DataFactory*>
   */
  std::vector<complex::IH5DataFactory*> getDataFactories() const override;
};

extern "C" {
TEST2PLUGIN_EXPORT complex::AbstractPlugin* initPlugin();
}
