#include <catch2/catch.hpp>

#include "complex/Filter/Parameters.hpp"
#include "complex/Parameters/BoolParameter.hpp"
#include "complex/Parameters/NumberParameter.hpp"
#include "complex/Parameters/StringParameter.hpp"

using namespace complex;

namespace
{
constexpr StringLiteral k_FooParamKey = "FooParam";
constexpr StringLiteral k_BarParamKey = "BarParam";
constexpr StringLiteral k_BazParamKey = "BazParam";
} // namespace

TEST_CASE("ParametersTest")
{
  Parameters params;
  params.insert(std::make_unique<Int32Parameter>(k_FooParamKey, "Foo", "Test parameter", 42));
  params.insertSeparator({"Separator Name"});

  REQUIRE_FALSE(params.containsGroup(k_BarParamKey));
  params.insertLinkableParameter(std::make_unique<BoolParameter>(k_BarParamKey, "Bar", "Test parameter", false));
  REQUIRE(params.containsGroup(k_BarParamKey));

  params.insert(std::make_unique<StringParameter>(k_BazParamKey, "Baz", "Test parameter", "default"));

  params.linkParameters(k_BarParamKey, k_BazParamKey, true);

  REQUIRE(params.size() == 3);

  REQUIRE(params.getLayout().size() == 4);

  REQUIRE(std::holds_alternative<Parameters::ParameterKey>(params.getLayout().at(0)));
  REQUIRE(std::holds_alternative<Parameters::Separator>(params.getLayout().at(1)));

  REQUIRE_FALSE(params.hasGroup(k_FooParamKey));
  REQUIRE(params.hasGroup(k_BazParamKey));

  REQUIRE(params.getGroup(k_FooParamKey).empty());
  REQUIRE(params.getGroup(k_BazParamKey) == k_BarParamKey);

  const std::vector<std::string> expectedKeys = {k_FooParamKey, k_BarParamKey, k_BazParamKey};

  REQUIRE(params.getKeys() == expectedKeys);

  const std::vector<std::string> expectedGroupKeys = {k_BarParamKey};

  REQUIRE(params.getGroupKeys() == expectedGroupKeys);

  const std::vector<std::string> expectedKeyInGroup = {k_BazParamKey};

  REQUIRE(params.getKeysInGroup(k_BarParamKey) == expectedKeyInGroup);

  REQUIRE(params.isParameterActive(k_FooParamKey, {}));

  REQUIRE(params.isParameterActive(k_BazParamKey, true));
  REQUIRE_FALSE(params.isParameterActive(k_BazParamKey, false));
}
