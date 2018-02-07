//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License 
//					 Kratos default license: kratos/license.txt
//
//  Main author:    
//

#if !defined(KRATOS_COMPARE_ELEMENTS_AND_CONDITIONS_UTILITY_H_INCLUDED)
#define KRATOS_COMPARE_ELEMENTS_AND_CONDITIONS_UTILITY_H_INCLUDED

// System includes
#include <typeinfo>

// External includes

// Project includes
#include "includes/define.h"
#include "includes/element.h"
#include "includes/condition.h"

namespace Kratos
{
///@name Kratos Classes
///@{
// A utility for comparison of dynamic types of elements and conditions.
class CompareElementsAndConditionsUtility
{
public:
    ///@name Type Definitions
    ///@{
    ///@}
    ///@name Operations
    ///@{

    inline static bool IsSame(const Element& rLHS, const Element& rRHS)
    {
        return (typeid(rLHS) == typeid(rRHS) &&
            rLHS.GetGeometry().GetGeometryType() == rRHS.GetGeometry().GetGeometryType());
    }

    inline static bool IsSame(const Condition& rLHS, const Condition& rRHS)
    {
        return (typeid(rLHS) == typeid(rRHS) &&
            rLHS.GetGeometry().GetGeometryType() == rRHS.GetGeometry().GetGeometryType());
    }

    inline static bool IsSame(const Element* pLHS, const Element* pRHS)
    {
        return (typeid(*pLHS) == typeid(*pRHS) &&
            pLHS->GetGeometry().GetGeometryType() == pRHS->GetGeometry().GetGeometryType());
    }

    inline static bool IsSame(const Condition* pLHS, const Condition* pRHS)
    {
        return (typeid(*pLHS) == typeid(*pRHS) &&
            pLHS->GetGeometry().GetGeometryType() == pRHS->GetGeometry().GetGeometryType());
    }

    static void FindNameInRegistry(const Element& rElement, std::string& rName)
    {
        KRATOS_TRY;

        const auto& components = KratosComponents<Element>::GetComponents();
        for(auto it = components.begin(); it != components.end(); ++it) 
        {
            if (IsSame(*(it->second), rElement))
            {
                rName = it->first;
                return;
            }
        }

        KRATOS_ERROR << "Element \"" << typeid(rElement).name() << "\" not found!" << std::endl;

        KRATOS_CATCH("");
    }

    static void FindNameInRegistry(const Condition& rCondition, std::string& rName)
    {
        KRATOS_TRY;

        const auto& components = KratosComponents<Condition>::GetComponents();
        for(auto it = components.begin(); it != components.end(); ++it) 
        {
            if (IsSame(*(it->second), rCondition))
            {
                rName = it->first;
                return;
            }
        }

        KRATOS_ERROR << "Condition \"" << typeid(rCondition).name() << "\" not found!" << std::endl;

        KRATOS_CATCH("");
    }

    ///@}

private:
    ///@name Private Operations
    ///@{
    ///@}
};
///@} // Kratos Classes
} // namespace Kratos.

#endif // KRATOS_COMPARE_ELEMENTS_AND_CONDITIONS_UTILITY_H_INCLUDED defined
