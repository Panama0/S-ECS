#pragma once

#include "Defines.hpp"

#include <cassert>
#include <unordered_map>

namespace SECS
{

class IDGenerator
{
public:
    template <class T> static ComponentID getComponentID()
    {
        static ComponentID id = m_componentCounter++;
        assert(id < MAX_COMP && "Max components reached!");
        return id;
    }

private:
    static ComponentID m_componentCounter;
};

class ComponentStoreBase
{
public:
    virtual ~ComponentStoreBase() = default;

    virtual void entityDied(Entity entity) = 0;
};

template <typename T> class ComponentStore : public ComponentStoreBase
{
public:
    T& insert(Entity entity, T component)
    {
        size_t newIndex = m_size;
        m_entToIndex[entity] = newIndex;
        m_IndexToEnt[newIndex] = entity;
        m_components[newIndex] = component;
        m_size++;
        return m_components[newIndex];
    }

    void remove(Entity entity)
    {
        // Copy element at end into deleted element's place to maintain density
        size_t indexOfRemovedEntity = m_entToIndex[entity];
        size_t indexOfLastElement = m_size - 1;
        m_components[indexOfRemovedEntity] = m_components[indexOfLastElement];

        // Update map to point to moved spot
        Entity entityOfLastElement = m_IndexToEnt[indexOfLastElement];
        m_entToIndex[entityOfLastElement] = indexOfRemovedEntity;
        m_IndexToEnt[indexOfRemovedEntity] = entityOfLastElement;

        m_entToIndex.erase(entity);
        m_IndexToEnt.erase(indexOfLastElement);

        m_size--;
    }

    T& get(Entity entity) { return m_components[m_entToIndex[entity]]; }

    void entityDied(Entity entity)
    {
        if(m_entToIndex.find(entity) != m_entToIndex.end())
        {
            remove(entity);
        }
    }

private:
    // packed array
    std::array<T, MAX_ENT> m_components;

    // maps to facilitate packing of component data
    std::unordered_map<Entity, size_t> m_entToIndex;
    std::unordered_map<size_t, Entity> m_IndexToEnt;

    size_t m_size;
};
}
