#pragma once

#include "ComponentStore.hpp"
#include "Defines.hpp"

#include <array>
#include <bitset>
#include <cassert>
#include <memory>
#include <queue>
#include <vector>

namespace SECS
{

class EntityManager
{
public:
    EntityManager()
    {
        for(Entity i{}; i < MAX_ENT; i++)
        {
            m_availableIDs.emplace(i);
        }
    }

    Entity addEntity()
    {
        Entity entity = m_availableIDs.front();
        m_availableIDs.pop();
        m_livingEntities++;

        assert(m_livingEntities < MAX_ENT && "Max Entities reached!");

        return entity;
    }

    void killEntity(Entity entity)
    {
        assert(m_livingEntities > 0 && "There are no entities alive!");

        m_entitySignatures[entity].reset();

        m_availableIDs.push(entity);
        m_livingEntities--;

        for(auto& componentStore : m_componentStores)
        {
            if(componentStore)
            {
                componentStore->entityDied(entity);
            }
        }
    }

    template <typename T> T& addComponent(Entity entity)
    {
        ComponentID id = IDGenerator::getComponentID<T>();
        auto& storeBase = m_componentStores[id];

        // make a store for the component if one does not exist
        if(storeBase == nullptr)
        {
            m_componentStores[id] = std::make_unique<ComponentStore<T>>();
        }

        // update the entity's signature
        m_entitySignatures[entity].set(id, true);

        // add the component to the store and return it
        return getStore<T>().insert(entity, T{});
    }

    template <typename T> void removeComponent(Entity entity)
    {
        ComponentID id = IDGenerator::getComponentID<T>();
        auto& store = getStore<T>();

        assert(store && "Can't remove component, it is not being stored");

        // remove
        store.remove(entity);
        m_entitySignatures[entity].set(id, false);
    }

    template <typename T> bool hasComponent(Entity entity)
    {
        auto id = IDGenerator::getComponentID<T>();
        return m_entitySignatures[entity].test(id);
    }

    template <typename T> T& getComponent(Entity entity)
    {
        assert(hasComponent<T>(entity) && "The entity does not store the given component!");

        auto& store = getStore<T>();
        return store.get(entity);
    }

    template <typename... T> std::vector<Entity> getEntities()
    {
        bool getAll{false};
        std::vector<Entity> found;
        if(sizeof...(T) == 0)
        {
            getAll = true;
        }

        EntSignature required;
        // build the required signature
        (required.set(IDGenerator::getComponentID<T>()), ...);

        // check the entities
        for(Entity i{}; i < MAX_ENT; i++)
        {
            if(!getAll && (m_entitySignatures[i] & required) == required)
            {
                found.push_back(i);
            }
            else if(getAll && m_entitySignatures[i].any())
            {
                found.push_back(i);
            }
        }

        return found;
    }

private:
    template <typename T> ComponentStore<T>& getStore()
    {
        ComponentID id = IDGenerator::getComponentID<T>();
        auto& store = m_componentStores[id];
        auto storePtr = dynamic_cast<ComponentStore<T>*>(store.get());
        return *storePtr;
    }

    std::queue<Entity> m_availableIDs;
    Entity m_livingEntities{};

    std::array<EntSignature, MAX_ENT> m_entitySignatures;

    std::array<std::unique_ptr<ComponentStoreBase>, MAX_COMP>
        m_componentStores;
};
}
