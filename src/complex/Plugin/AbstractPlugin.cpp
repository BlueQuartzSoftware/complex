#include "AbstractPlugin.hpp"

#include <memory>

#include "complex/Filtering/AbstractFilter.hpp"

using namespace complex;

AbstractPlugin::AbstractPlugin(IdType id, const std::string& name, const std::string& description)
: m_Id(id)
, m_Name(name)
, m_Description(description)
{
}

AbstractPlugin::~AbstractPlugin() = default;

std::string AbstractPlugin::getName() const
{
  return m_Name;
}

std::string AbstractPlugin::getDescription() const
{
  return m_Description;
}

AbstractPlugin::IdType AbstractPlugin::getId() const
{
  return m_Id;
}

std::string AbstractPlugin::getVendor() const
{
  return m_Vendor;
}

void AbstractPlugin::setVendor(const std::string& vendor)
{
  m_Vendor = vendor;
}

void AbstractPlugin::addFilterHandle(const FilterHandle& addHandle)
{
  m_FilterHandles.insert(addHandle);
}

bool AbstractPlugin::containsFilterId(FilterHandle::FilterIdType uuid) const
{
  return m_InitializerMap.find(uuid) != m_InitializerMap.end();
}

std::set<FilterHandle> AbstractPlugin::getFilterHandles() const
{
  return m_FilterHandles;
}

complex::AbstractFilter* AbstractPlugin::createFilter(FilterHandle::FilterIdType id) const
{
  if(!containsFilterId(id))
  {
    return nullptr;
  }

  return m_InitializerMap.at(id)();
}

void AbstractPlugin::addFilter(FilterCreationFunc filterFunc)
{
  std::shared_ptr<AbstractFilter> filter(filterFunc());
  if(!filter)
  {
    return;
  }

  m_FilterHandles.insert(FilterHandle(filter->getName(), filter->getId(), getId()));
  m_InitializerMap[filter->getId()] = filterFunc;
}