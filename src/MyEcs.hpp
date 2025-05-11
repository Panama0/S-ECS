#pragma once

#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <queue>
#include <unordered_map>

#define MAX_COMP 32
#define MAX_ENT 256

using Entity = uint32_t;
using EntSignature = std::bitset<MAX_COMP>;

using ComponentID = std::uint8_t;

// for component ID
int s_componentCounter{};
template <class T> int getID()
{
    static int s_componentId = s_componentCounter++;
    return s_componentId;
}

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
class EntityManager
{
public:
    EntityManager()
    {
        for(uint32_t i{}; i < MAX_ENT; i++)
        {
            m_availableIDs.emplace(i);
        }
    }

    Entity addEntity()
    {
        Entity entity = m_availableIDs.front();
        m_availableIDs.pop();
        m_livingEntities++;

        return entity;
    }

    void killEntity(Entity entity)
    {
        m_entitySignatures[entity].reset();

        m_availableIDs.push(entity);
        m_livingEntities--;
    }

    template <typename T> ComponentStore<T>* getComponentStore()
    {
        ComponentID id = getID<T>();
        return dynamic_cast<ComponentStore<T>*>(m_componentStores[id].get());
    }

    template <typename T> T& addComponent(Entity entity)
    {
        // make a store for the component if one does not exist
        ComponentID id = getID<T>();
        auto& store = m_componentStores[id];
        if(store == nullptr)
        {
            store = std::make_unique<ComponentStore<T>>();
        }

        // update the entity's signature
        m_entitySignatures[entity].set(id, true);

        // add the component to the store and return it
        auto actualStore = dynamic_cast<ComponentStore<T>*>(store.get());

        return actualStore->insert(entity, T{});
    }

    template <typename T> void removeComponent(Entity entity)
    {
        ComponentID id = getID<T>();
        auto& store = m_componentStores[id];
        assert(store && "Can't remove component, it is not being stored");

        auto actualStore = dynamic_cast<ComponentStore<T>*>(store.get());

        // remove
        actualStore->remove(entity);
        m_entitySignatures[entity].set(id, false);
    }

private:
    std::queue<Entity> m_availableIDs;
    uint32_t m_livingEntities{};

    std::array<EntSignature, MAX_ENT> m_entitySignatures;

    std::array<std::unique_ptr<ComponentStoreBase>, MAX_COMP>
        m_componentStores;
};
